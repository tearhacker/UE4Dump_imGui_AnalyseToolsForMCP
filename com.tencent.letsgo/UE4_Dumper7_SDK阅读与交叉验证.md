# UE4 Dumper-7 SDK 阅读与交叉验证

> 分析对象：UE 4.26.2 Dumper-7 CppSDK 导出  
> 分析范围：`Feature_ChestPVE`、`Feature_Chest`、`MoeGameCore` 相关类型  
> 文档日期：2026-08-06  
> 说明：本文只讨论静态结构分析和自有测试构建中的验证方法，不提供对第三方线上进程进行数值篡改的实现。

---

## 1. 先认识导出目录

Dumper-7 将 UE 反射信息拆分到多类文件。分析一个功能时，不应只查看某一个字段，也不能把生成的包装函数当成游戏真实实现。

| 文件 | 主要内容 | 能确认什么 | 不能确认什么 |
|---|---|---|---|
| `SDK.hpp` | 所有模块头文件的聚合入口 | SDK 包含哪些模块 | 具体业务逻辑 |
| `*_classes.hpp` | 类继承、成员、偏移、大小、函数声明 | 对象静态布局与反射函数列表 | Native 函数内部算法 |
| `*_structs.hpp` | 结构体和枚举 | 字段相对偏移、枚举底层类型 | 字段运行时语义是否与命名一致 |
| `*_parameters.hpp` | `ProcessEvent` 参数结构 | 参数顺序、类型和总大小 | 函数由客户端还是服务端裁决 |
| `*_functions.cpp` | Dumper 生成的调用包装 | 如何通过 `UFunction` 调用反射函数 | 原始 Native 函数体 |
| `SDK_Offset.hpp` | 属性和函数 RVA 汇总 | 同一二进制版本内的相对位置 | 新版本仍然有效 |
| `Logs.txt` | Dump 时解析出的地址 | 本次运行的模块基址和全局对象位置 | 下次运行的绝对地址 |

### 1.1 推荐搜索顺序

分析一个名称，例如 `TakePropDamage`，建议按以下顺序查找：

```powershell
rg -n "TakePropDamage" CppSDK/SDK
rg -n "class ANPCBase|struct NPCBase_TakePropDamage" CppSDK/SDK
rg -n "Damage|TakeDamage" CppSDK/SDK/Feature_ChestPVE_*.hpp
```

阅读顺序为：

1. 在模块文件中确定功能属于哪个 Package。
2. 在 `classes.hpp` 中确认声明类及其父类。
3. 在 `structs.hpp` 中展开所有值类型成员。
4. 在 `parameters.hpp` 中确认函数参数布局。
5. 在 `functions.cpp` 中确认它只是包装函数，还是能得到额外函数标记。
6. 回到父类继续向上追踪，避免把继承成员误认为当前类成员。

---

## 2. 如何阅读类大小注释

以下注释来自 `UMoeTakeDamageEffect`：

```cpp
// Class MoeGameCore.MoeTakeDamageEffect
// 0x00B0 (0x01A8 - 0x00F8)
class UMoeTakeDamageEffect final : public UMoeEffect
```

含义如下：

| 数值 | 含义 |
|---|---|
| `0x00F8` | 父类 `UMoeEffect` 的大小 |
| `0x00B0` | 当前子类新增的大小 |
| `0x01A8` | `UMoeTakeDamageEffect` 的最终大小 |

成员后的注释格式通常为：

```cpp
float ChangeOtherHPValue; // 0x019C(0x0004)(Edit, ...)
```

其中：

- `0x019C` 是成员相对于对象起始地址的偏移。
- `0x0004` 是成员大小。
- `Edit`、`Net` 等是 UE 属性标记，不是 C++ 访问权限。
- `NativeAccessSpecifierPublic/Protected/Private` 才表示 Dumper 推断出的原始访问级别。

`Pad_XXX` 是 Dumper 为未知区域、对齐空隙或未识别属性生成的占位数组。不能仅凭 `Pad` 判断该区域没有业务数据。

---

## 3. 常见属性标记的意义

| 标记 | 一般含义 | 分析时的注意点 |
|---|---|---|
| `Net` | 属性参与网络复制 | 不代表客户端写入会被服务端接受 |
| `RepNotify` | 复制更新时调用通知函数 | 通常能找到对应的 `OnRep_*` |
| `Transient` | 临时运行数据，不持久化 | 常见于缓存、统计和过程状态 |
| `Edit` | 可在编辑器或配置中编辑 | 不等于运行时权威值 |
| `DisableEditOnInstance` | 实例级编辑受限 | 更可能是类默认配置或资产配置 |
| `BlueprintVisible` | 蓝图可见 | 不代表蓝图可写 |
| `BlueprintReadOnly` | 蓝图只读 | Native 代码仍可能修改 |
| `ZeroConstructor` | 可安全零初始化 | 只是类型特征，不是业务语义 |
| `IsPlainOldData` | POD 类型 | 可帮助判断内存布局，不代表值可信 |
| `ExportObject` / `InstancedReference` | 指向归属当前对象的子对象 | 需要继续解引用并验证对象类型 |

特别注意：网络复制通常是权威端向非权威端同步。仅看到 `Net` 或 `RepNotify`，不能推出客户端能够决定最终值。

---

## 4. 样例一：ANPCBase 与怪物参数

`MoeGameCore_classes.hpp` 中的声明为：

```cpp
// 0x00D0 (0x0660 - 0x0590)
class ANPCBase : public AMoeCharacter
{
    FMonsterAIParam AIParam;                    // 0x05A0(0x0020)
    UMoeMonsterAttributeComponent* NPCAttributeComponent; // 0x0620(0x0008)
    int32 MonsterLevel;                         // 0x0638(0x0004)
    int64 MonsterID;                            // 0x0640(0x0008)
};
```

这里可以直接得出：

1. `ANPCBase` 继承自 `AMoeCharacter`。
2. 父类大小为 `0x590`，当前类最终大小为 `0x660`。
3. `AIParam` 是一个内嵌的 `FMonsterAIParam`，不是指针。
4. `NPCAttributeComponent` 才是一个 8 字节对象指针。
5. `MonsterLevel` 是 `int32`。
6. `MonsterID` 是 `int64`。

继续在 `MoeGameCore_structs.hpp` 中展开 `FMonsterAIParam`：

```cpp
struct FMonsterAIParam final
{
    int64 MonsterTypeID;   // 0x0000
    int32 monsterUid;      // 0x0008
    int32 Score;           // 0x000C
    int32 Status;          // 0x0010
    ENPCStateEnum BehaveMode; // 0x0014
    int32 SideId;          // 0x0018
}; // Size: 0x20
```

因此，在自有测试构建中表达成员位置时，应把它理解为：

```text
ANPCBase.AIParam.BehaveMode
= ANPCBase 对象起始位置 + 0x05A0 + 0x14
```

这里没有第二次指针解引用。把内嵌结构体误写成指针，会读取完全错误的地址。

---

## 5. 样例二：伤害效果对象

`UMoeTakeDamageEffect` 的关键静态布局为：

```cpp
class UMoeTakeDamageEffect final : public UMoeEffect
{
    FVector ImpactLocation;                     // 0x00F4
    FVector HitDirection;                       // 0x0100
    FHitResult HitResult;                       // 0x010C
    ESGWeaponType PropWeaponType;               // 0x0198
    EMoePropDamageType MatchingWeaponType;      // 0x0199
    float ChangeOtherHPValue;                   // 0x019C
    ABaseMoeGameCharacter* DamageCauser;        // 0x01A0
}; // Size: 0x01A8
```

从 Dump 可以确认：

- `ChangeOtherHPValue` 是 `float`，静态偏移为 `0x19C`。
- 它属于 `UMoeTakeDamageEffect`，不是 `ANPCBase` 的成员。
- `DamageCauser` 是对象指针。
- `HitResult` 是内嵌结构体，占用 `0x8C` 字节。

从 Dump 不能确认：

- `ChangeOtherHPValue` 是否为最终伤害。
- 该对象是否只是配置模板或运行时副本。
- 伤害是否会在服务端重新计算。
- 是否还有属性克制、减伤、护盾、难度倍率等后续步骤。

字段名只能作为搜索入口，不能替代数据流分析。

---

## 6. 样例三：TakePropDamage 函数

类声明中存在：

```cpp
void TakePropDamage(float Damage, const FVector& ImpactLocation);
```

对应的参数结构为：

```cpp
struct NPCBase_TakePropDamage final
{
    float Damage;            // 0x0000(0x0004)
    FVector ImpactLocation;  // 0x0004(0x000C)
}; // Size: 0x10
```

生成的 `functions.cpp` 包装大致执行：

```cpp
UFunction* Func = Class->GetFunction("NPCBase", "TakePropDamage");
Params::NPCBase_TakePropDamage Parms{};
Parms.Damage = Damage;
Parms.ImpactLocation = ImpactLocation;
UObject::ProcessEvent(Func, &Parms);
```

这只能说明该函数可通过 UE 反射调用。包装代码不是原始函数体，看不到函数内部如何处理伤害。

本次导出给出的函数标记为：

```text
Native, Public, HasDefaults
```

没有直接显示 `Net`、`NetServer`、`Server` 或 `Reliable`。因此不能从该包装得出“调用它会向服务端发送伤害”的结论。要判断权威路径，需要自有服务端源码、调用栈、网络日志或测试构建中的服务器遥测。

---

## 7. 样例四：FChestPVE_DamageInfo

`Feature_ChestPVE_structs.hpp` 中存在：

```cpp
struct FChestPVE_DamageInfo final
{
    int64 SourceID;  // 0x0000, Transient
    int64 TargetId;  // 0x0008, Transient
    int32 Damage;    // 0x0010, Transient
    bool IsKilled;   // 0x0014, Transient
    int32 SkillId;   // 0x0018, Transient
    int32 ItemId;    // 0x001C, Transient
}; // Size: 0x20
```

所有字段均带有 `Transient`，这更像一次伤害事件、统计记录或临时上报数据。不能因为它有 `Damage` 字段，就认定它是伤害计算源。

判断一个结构的角色时，应搜索它被哪些类持有、以数组还是单值保存、是否被函数作为输入/输出参数使用，以及是否存在 RPC 或上报函数。

---

## 8. ChestPVE 继承关系校正

原分析文档将 `AChestPVEGameCharacter` 画在怪物继承链下方，但 Dump 不支持这个结论。

实际可确认的两个分支是：

```text
ABaseMoeGameCharacter
  -> AChestPVEGameCharacter
```

以及：

```text
AMoeInGameMonster
  -> AChestMonsterBase
```

其中：

- `AChestPVEGameCharacter` 位于 `Feature_ChestPVE_classes.hpp`，大小为 `0xD50`。
- `AChestMonsterBase` 位于 `Feature_Chest_classes.hpp`，继承 `AMoeInGameMonster`。
- 蓝图类名包含 `BP_ChestPVE_Monster_`，不代表其原生父类名一定包含 `ChestPVEGameCharacter`。

要确认某个蓝图 Actor 的真实继承关系，应沿它的 `UClass::SuperStruct` 链逐级记录，而不是只根据类名猜测。

---

## 9. 对原《游戏dump基础分析》的校正结果

| 原结论 | Dump 中的证据 | 校正 |
|---|---|---|
| `ANPCBase.AIParam` 是指针 | `FMonsterAIParam AIParam // 0x05A0(0x20)` | 它是内嵌结构体 |
| `AIParam.monsterUid` 是 `int64` | `int32 monsterUid // 0x08` | 类型应为 `int32` |
| `ANPCBase.MonsterID` 是 `int32` | `int64 MonsterID // 0x0640` | 类型应为 `int64` |
| `AChestPVEGameCharacter` 继承自怪物类 | 声明为 `public ABaseMoeGameCharacter` | 它是另一条角色继承分支 |
| `functions.cpp` 展示伤害实现 | 内容仅组装参数并调用 `ProcessEvent` | 只能视为反射调用包装 |
| `Net/RepNotify` 说明客户端修改可生效 | 标记只描述复制属性 | 不能证明客户端具有权威性 |

---

## 10. 静态结论的可信度分级

建议为每条结论标注可信度，避免把推测写成事实。

### A 级：Dump 可直接证明

- 类继承声明。
- 成员静态类型。
- 成员相对偏移。
- 参数结构布局。
- 反射属性与函数标记。

### B 级：需要多文件交叉证明

- 某字段属于配置、状态还是缓存。
- 某个组件管理哪些对象。
- 某个事件结构用于统计还是结算。
- 某个蓝图类大致属于哪个业务类别。

### C 级：必须在自有测试构建验证

- 某字段是否参与最终伤害。
- 客户端与服务端谁具有权威性。
- 修改后是否会被复制覆盖。
- 函数是否经过服务器校验。
- 不同玩法和版本是否共用同一逻辑。

原文中“服务器校验弱”“修改实际生效”等表述属于 C 级结论，不能仅由当前 CppSDK 静态导出证明。

---

## 11. 自有项目中的验证清单

在有源码和授权测试环境时，可以按以下方式验证静态结论：

1. 用 `sizeof` 和 `offsetof` 对照生成布局。
2. 为伤害请求、服务端重算值和最终结算值分别记录日志。
3. 在客户端与服务端输出相同事件 ID，建立完整数据链。
4. 检查 RPC 声明、Validation、权限角色和调用方向。
5. 对异常值、负数、NaN、Inf 和超大倍率执行边界测试。
6. 确认复制更新是否覆盖客户端本地状态。
7. 比较 PIE、Dedicated Server 和移动端 Shipping 构建差异。
8. 每次版本更新重新生成 SDK，并按类名、大小、偏移生成差异报告。

最终安全结论应建立在服务端日志和源码数据流之上，而不是建立在单个字段名称或单次客户端观察之上。

---

## 12. 本次分析引用位置

- `CppSDK/SDK/MoeGameCore_classes.hpp:4341`：`ANPCBase`
- `CppSDK/SDK/MoeGameCore_structs.hpp:8200`：`FMonsterAIParam`
- `CppSDK/SDK/MoeGameCore_classes.hpp:8124`：`UMoeTakeDamageEffect`
- `CppSDK/SDK/MoeGameCore_parameters.hpp:7502`：`NPCBase_TakePropDamage`
- `CppSDK/SDK/MoeGameCore_functions.cpp:18792`：`TakePropDamage` 反射包装
- `CppSDK/SDK/Feature_ChestPVE_structs.hpp:526`：`FChestPVE_DamageInfo`
- `CppSDK/SDK/Feature_ChestPVE_classes.hpp:2601`：`AChestPVEGameCharacter`
- `CppSDK/SDK/Feature_Chest_classes.hpp:2133`：`AChestMonsterBase`
- `com.tencent.letsgo/Logs.txt:13`：`FNamePool` Dump 记录
- `com.tencent.letsgo/Logs.txt:21`：`GUObjectArray` Dump 记录
- `com.tencent.letsgo/Logs.txt:35`：`GWorld` Dump 记录

