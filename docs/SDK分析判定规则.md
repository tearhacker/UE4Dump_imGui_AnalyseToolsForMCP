# SDK 分析判定规则

> **来源**：`com.tencent.letsgo`（元梦之星 UE4.26.2）真实 dump 产物与四份人工分析文档的沉淀：
> - `UE4_Dumper7_SDK阅读与交叉验证.md`（方法论 + 人工错误校正）
> - `游戏dump基础分析.md`（指针链 / GNames 解析 / 系统结构）
> - `探灵实录武器内存功能分析.md`（字段标记语义 / 服务端权威判定）
> - `探灵实录武器内存功能针对性修复方案.md`（错误清单与纠错过程）
>
> **用途**：MCP Server Instructions 的素材库。AI 分析 SDK 时必须遵守这里的判定纪律。
> 真实规模参照：458,752 对象 / 320 包 / 10,754 类 / 6,186 结构 / 2,865 枚举 / 48,868 函数。

---

## 1. 产物文件能确认什么、不能确认什么

| 文件 | 能确认 | 不能确认 |
|---|---|---|
| `Offsets.hpp` | 本次运行的模块基址、全局对象位置、偏移表 | 下次运行的绝对地址 |
| `SDK_Classes.hpp` | 类继承、成员偏移大小、函数声明 | Native 函数内部算法 |
| `SDK_Structs.hpp` | 字段相对偏移、枚举底层类型 | 字段运行时语义与命名是否一致 |
| `script.json` | 函数 RVA 清单（`Functions:[{Address,Name}]`） | 函数实现 |
| `Logs.txt` | FNamePool/GUObjectArray/GEngine/GWorld 定位结果 | 长期有效性 |

**分析一个功能的阅读顺序**（`searchClasses`→`describeClass` 工具链的设计依据）：
确定所属 Package → `classes.hpp` 确认声明类与父类 → `structs.hpp` 展开值类型成员 →
确认函数参数布局 → 确认是包装还是实现 → **回溯父类，避免把继承成员当成当前类成员**。

## 2. FNamePool 解析参数与自校验（UE 4.26 实测）

| 参数 | 值 |
|---|---|
| Stride | 2（offset × Stride） |
| BlocksBit | 16（blockIdx = nameIndex >> 16） |
| BlocksOff | 0x40 |
| Header | `uint16`：`bIsWide:1 \| ProbeHash:5 \| Len:10`（len = hdr >> 6，wide = hdr & 1） |
| 字符串 | Header 后 +2 |

```
block = [NamePool + 0x40 + blockIdx * 8]
entry = block + (nameIndex & 0xFFFF) * 2
```

**自校验**：读 `GUObjectArray[1]` 的名字应为 `"Object"`（下标 0 是 `/Script/CoreUObject`，
1-4 依次为 Object / Engine / Subsystem / GameInstance）。
**GUObjectArray 为分块数组**：每块 65,536 元素，`FUObjectItem` 大小 0x18。

## 3. FName 条目粘连是真实现象（不要在设备端清洗）

`com.tencent.letsgo/Logs.txt` 实测：

```
GetNameByID(0): None
GetNameByID(1): neBytePropertyIntProperty      ← 粘连
GetNameByID(2): BytePropertyIntProperty        ← 粘连
GetNameByID(3): ByteProperty                   ← 干净
GetNameByID(4): tePropertyIntProperty          ← 粘连
```

相邻条目被连读是常见噪声。**AI 判定名字表时必须容忍**：部分条目粘连但序列里
能辨认出 `None/ByteProperty/IntProperty/...` 引擎名序列，仍可判定候选正确。

## 4. 类大小与成员注释格式

```cpp
// Class MoeGameCore.MoeTakeDamageEffect
// 0x00B0 (0x01A8 - 0x00F8)        ← 新增大小 (最终大小 - 父类大小)
class UMoeTakeDamageEffect final : public UMoeEffect
{
    float ChangeOtherHPValue;       // 0x019C(0x0004)   ← 相对偏移(成员大小)
};
```

`Pad_XXX` 是对齐/未识别区域的占位——**不能凭 Pad 断定该区域没有业务数据**。

## 5. 🔴 铁律：内嵌结构体 vs 指针

```cpp
FMonsterAIParam AIParam;                        // 0x05A0(0x0020) ← 内嵌，无二次解引用
UMoeMonsterAttributeComponent* NPCAttributeComponent; // 0x0620(0x0008) ← 指针，需解引用
```

真实教训：把内嵌结构体当指针，读出来的是完全错误的地址。
`AIParam.BehaveMode = 对象起始 + 0x05A0 + 0x14`，**没有中间解引用**。
→ `analyzeClass` 必须输出 `fieldKinds`（inline_struct / pointer / value），
`inspectObject` 据此决定是否二次解引用。

## 6. 属性标记语义

| 标记 | 含义 | 注意 |
|---|---|---|
| `Net` | 参与网络复制 | **不代表客户端写入会被服务端接受** |
| `RepNotify` | 复制更新时回调 OnRep_* | 通常有对应 `OnRep_*` 函数 |
| `Transient` | 临时数据不持久化 | 常见于缓存/统计/事件记录，**有 Damage 字段≠伤害计算源** |
| `Edit` / `DisableEditOnInstance` | 编辑器可编辑 / 实例级受限 | 不等于运行时权威值 |
| `BlueprintVisible` / `BlueprintReadOnly` | 蓝图可见 / 只读 | Native 代码仍可修改 |
| `ExportObject` / `InstancedReference` | 归属当前对象的子对象 | 需继续解引用并验证类型 |

**字段名只能作为搜索入口，不能替代数据流分析。**

## 7. 服务端权威性判定（静态可做的部分）

- 带 `Net`/`RepNotify` 的字段是**服务端同步字段**，客户端改动会被复制覆盖（PvP 下严格，PVE 下部分宽松——**这只 能实测确认，属 C 级结论**）
- 名字含 `Svr*`（如 `SvrCheckFlag` / `SvrCheckData_LastShootTime`）→ **服务端校验点，动它们必然触发校验**
- `Server_*` 前缀函数 → 服务端 RPC，操作要过服务端
- 生命值类常走 Watcher 模式（`OnHealthChangedFromWatcher`）从服务端推送

## 8. 🔴 结论可信度分级（AI 输出必须带）

| 级别 | 定义 | 例 |
|---|---|---|
| **A** | Dump 可直接证明 | 继承声明、成员类型、偏移、参数布局、属性标记 |
| **B** | 需多文件交叉证明 | 字段属于配置/状态/缓存、组件管理哪些对象、结构用于统计还是结算 |
| **C** | 必须运行时实测 | 字段是否参与最终计算、客户端/服务端谁权威、修改是否被覆盖 |

**禁止把 B/C 级推测写成事实。** "服务器校验弱""修改实际生效"是 C 级结论。

## 9. 继承链校正方法

**蓝图类名不代表原生父类**（`BP_ChestPVE_Monster_` 的父类不一定含 Monster）。
确认真实继承必须沿 `UClass::SuperStruct` 链逐级记录——UMT 的 `UE_UStruct::GetSuper()` + `ForEachObjectOfClass` 可直接做，这就是 `analyzeClass` 输出继承链的依据。

## 10. 指针链的真实样例（`followPointerChain` 用例）

```
GWorld = [Base + 0x11FCAC30]
  +0x1F0  → OwningGameInstance
  +0x38   → LocalPlayers[0]        ← 数组元素步进
  +0x00   → PlayerController
  +0x30   → Pawn
  +0x300  → selfEntity
```

6 级解引用、含一次数组下标。**Actor 遍历**：
`Level = [GWorld + 0x30]`，`Actors TArray @ Level + 0x98`（Data/Num/Max）。

## 11. 护栏参数的真实参照值

| 项 | 参照值 | 出处 |
|---|---|---|
| 指针有效范围 | `0x10000000 ≤ addr < 0x10000000000` | 人工分析采用值 |
| Actor 数量上限 | `0 < Num < 4096` | 同上 |
| 短期缓存 | 道具 3s/32 个、怪物每帧/128 | 同上 |
| 永久缓存 | GName 解析结果 | 同上 |
| `ProcessEventIdx` 实测 | **68**（UE 4.26.2） | Logs.txt——UMT 硬编码区间 `(50,100)` 的下边界附近，**区间外游戏会定位失败** |

## 12. UMT 产物与 Dumper-7 产物的差距

Dumper-7 另有 `*_parameters.hpp`（ProcessEvent 参数结构）与 `*_functions.cpp`（反射调用包装）。
**UMT 缺 parameters 类产物**——AI 要分析函数参数布局时只能在线读 UFunction。
→ `describeClass` 的函数部分应输出参数字段布局（`PropertyOffsetFinder` 已能推 UFunction 布局），弥补差距。

## 13. 真实错误案例库（人工分析犯过、AI 也会犯）

| 错误 | 真相 | 防范 |
|---|---|---|
| `AIParam` 当指针 | 内嵌结构体 | 输出 fieldKinds（§5） |
| `monsterUid` 当 int64 | 实为 int32 | 类型以 Dump 为准 |
| `MonsterID` 当 int32 | 实为 int64 | 同上 |
| 继承链按类名猜 | 沿 SuperStruct 逐级确认 | §9 |
| `functions.cpp` 当实现 | 只是 ProcessEvent 包装 | §1 |
| `Net` 标记推出"客户端可改" | 标记只描述复制 | §7 |

**共性**：结论超出证据范围。→ 一切输出带分级（§8）、带证据引用。
