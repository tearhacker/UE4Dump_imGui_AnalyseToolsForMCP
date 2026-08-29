# L2 配置层 API · UEOffsets（偏移表 / 版本默认值）

> **源码**：`src/UE/UEOffsets.hpp`（315 行）/ `UEOffsets.cpp`（826 行）
> **定位**：偏移表定义 + 9 档版本默认值 + `UEVars` 运行时结果。`detectUEVersion` / `describeClass` / `analyzeClass` 的数据底座。
> **已精读范围**：hpp 全文 + cpp 全文（1–826）。

---

## 0. 全局约定

1. **`kMAX_UENAME_BUFFER = 0xff`（255）**（hpp `:8`）：FName 名字缓冲区上限，所有名字解析共用。
2. **`UE_Offsets` 构造即 `memset(this, 0, sizeof(...))`**（hpp `:12-15`）——**所有偏移默认 0**。
3. **偏移为 0 = 该版本无此字段**（不是"偏移是 0"）。这是全项目最核心的空值语义，MCP 侧 `describeClass` 输出时遇到 0 字段要标注"不支持/未识别"。
4. **`UE_Pointers` 同样 `memset` 清零**（hpp `:194-197`）。

---

## 1. `UE_Offsets` 结构（hpp `:10-190`）

**30 个子结构**，是「读某个 UE 类字段时用哪个偏移」的总表。全表如下（`toString` 会逐项导出成 `Offsets.hpp` 产物）：

| 子结构 | 字段 | 含义 |
|---|---|---|
| `Config` | `isUsingCasePreservingName` / `IsUsingFNamePool` / `isUsingOutlineNumberName` | 三个 bool 配置（决定名字表布局） |
| `FName` | `ComparisonIndex` / `DisplayIndex` / `Number` / `Size` | FName 结构布局 |
| `FNameEntry` | `Index` / `Name` / `GetIsWide(fn)` | FNameEntry（GNames 模式） |
| `FNamePool` | `Stride` / `BlocksBit` / `BlocksOff` | FNamePool 布局（UE4.21+） |
| `FNamePoolEntry` | `Header` / `GetIsWide(fn)` / `GetLength(fn)` | FNamePool 条目头（**含两个 std::function 回调**） |
| `FUObjectArray` | `ObjObjects` | GUObjectArray → TUObjectArray 偏移 |
| `TUObjectArray` | `Objects` / `NumElements` / `NumElementsPerChunk` | 对象数组布局 |
| `FUObjectItem` | `Object` / `Size` | 单对象条目 |
| `UObject` | `ObjectFlags` / `InternalIndex` / `ClassPrivate` / `NamePrivate` / `OuterPrivate` | UObject 五字段 |
| `UField` | `Next` | UField 链 |
| `UEnum` | `Names` | 枚举名列表 |
| `UStruct` | `SuperStruct` / `Children` / `ChildProperties` / `PropertiesSize` / `MinAlignment` | 结构体五字段 |
| `UClass` | `ClassDefaultObject` / `ImplementedInterfaces` / `CastFlags` / `ClassFlags` | 类四字段 |
| `UScriptStruct` | `StructFlags` | 脚本结构体 flags |
| `UFunction` | `EFunctionFlags` / `NumParams` / `ParamSize` / `Func` | 函数四字段 |
| `UProperty` | `ArrayDim` / `ElementSize` / `PropertyFlags` / `Offset_Internal` / `Size` | 属性五字段 |
| `FField` | `ClassPrivate` / `Owner` / `Next` / `NamePrivate` / `FlagsPrivate` | FField（UE4.25+）五字段 |
| `FFieldClass` | `Name` / `SuperClass` / `CastFlags` | FFieldClass 三字段 |
| `FProperty` | `ArrayDim` / `ElementSize` / `PropertyFlags` / `Offset_Internal` / `Size` | FProperty（UE4.25+）五字段 |
| `ObjectProperty` | `PropertyClass` | → `FProperty.Size` |
| `StructProperty` | `Struct` | → `FProperty.Size` |
| `ByteProperty` | `Enum` | → `FProperty.Size` |
| `BoolProperty` | `Base` | → `FProperty.Size` |
| `EnumProperty` | `UnderlayingProp` / `Enum` | → `Size` / `Size+8` |
| `DelegateProperty` | `SignatureFunction` | → `FProperty.Size` |
| `ArrayProperty` | `Inner` | → `FProperty.Size` |
| `SetProperty` | `ElementProp` | → `FProperty.Size` |
| `MapProperty` | `KeyProp` / `ValueProp` | → `Size` / `Size+8` |
| `ClassProperty` | `MetaClass` | → `Size+8` |
| `InterfaceProperty` | `InterfaceClass` | → `FProperty.Size` |
| `ULevel` | `Actors` | 关卡 Actor 列表 |
| `UDataTable` | `RowMap` | 数据表行映射 |

### 关键点

1. **`UProperty.Size` 是「子类数据区起始偏移」，不是属性大小**（03 文档 §9.1 已证）。这里再次印证：`InitPropertySpecificDefaults` 把全部属性子类偏移指向 `propertyTail`（`FProperty.Size` 优先，否则 `UProperty.Size`）。
2. **两个 std::function 回调**（`FNameEntry.GetIsWide`、`FNamePoolEntry.GetIsWide/GetLength`）不是偏移值，是**名字表解析函数**。MCP 侧若要复刻 FName 解析，必须照抄这些 lambda 逻辑（见 §3 各档）。

---

## 2. `UE_Pointers` 结构（hpp `:192-212`）

12 个运行时定位指针（`UEVars` 的「指针版」），`ToString()` 导出成 `UEPointers` namespace：

`Names / UObjectArray / ObjObjects / Engine / World / Matrix / Physx / FrameCount / StaticFindObject / NativeAndroidApp / ProcessEvent / ProcessEventIdx`

⚠️ **`Engine`/`World` 字段存在但 `UEVars` 结构里没有对应**——`UE_Pointers` 是 Dumper 产物用的展示结构，与运行时 `UEVars` 字段不完全一致（`UEVars` 缺 Engine/World，多了 GUObjectsArrayPtr/ObjObjectsPtr）。

---

## 3. `UE_DefaultOffsets` 9 档版本偏移表（cpp `:357-767`）

### 3.0 `kGetFNameSize(bCasePreserving, bOutlineNumber)`（hpp `:216-224`）

```
fNameSize = outline ? 4 : 8;   // int32 vs 2×int32
if (casePreserving) fNameSize += 4;
```

### 3.1 9 档继承链（每档 `static` + `once` 标志，**只算一次**）

| 档 | 版本 | 继承 | 关键差异 |
|---|---|---|---|
| `UE4_00_17` | 4.0–4.17 | 基 | GNames 模式；`FNameEntry.GetIsWide = index&1`；`FNamePool` 全 0 |
| `UE4_18_19` | 4.18–4.19 | 继承 00_17 | 改 `NumParams`/`Offset_Internal`/`Size` |
| `UE4_20` | 4.20 | 继承 18_19 | `NumElementsPerChunk = 65*1024` |
| `UE4_21` | 4.21 | 继承 4_20 | **`IsUsingFNamePool = true`**；`NumElementsPerChunk = 64*1024` |
| `UE4_22` | 4.22 | 继承 4_21 | `FNameEntry.Index=8`；`UStruct.SuperStruct` 加 `FStructBaseChain` |
| `UE4_23_24` | 4.23–4.24 | 基（重写） | FNamePool 全套：`Stride = case?4:2`、`BlocksBit=16`、`BlocksOff=0x40`（Android LP64） |
| `UE4_25_27` | 4.25–4.27 | 继承 23_24 | 加 `ChildProperties`（FField）；`FField`/`FFieldClass`/`FProperty`；**`UProperty` 全清零** |
| `UE5_00_02` | 5.0–5.2 | 基（重写） | OutlineNumber 支持；`FField.Next` 加 `FFieldVariant` |
| `UE5_03` | 5.3+ | 继承 5_00_02 | `FField.Next` 改 `ClassPrivate+16` |

### 3.2 关键版本差异（MCP `detectUEVersion` 要暴露的）

1. **GNames → FNamePool 分界在 UE4.21**：`UE4_20` 及以下是 GNames（`IsUsingFNamePool=false`），`UE4_21` 起 FNamePool。
2. **FField 体系分界在 UE4.25**：`UE4_23_24` 及以下用 UField 链（`Children`），`UE4_25_27` 起用 FField 链（`ChildProperties`），且**把 `UProperty` 五个字段清零**（`:637-641`）——MCP 侧看到 `UProperty.* == 0` 说明走 FField 体系，要读 `FProperty.*`。
3. **`FNamePool.BlocksOff` 平台相关**（`:547-556`/`:672-680`）：iOS `0xD0`、Android LP64 `0x40`、32 位 `0x30`。
4. **`FNamePoolEntry.GetLength` 依赖 stride**（`:562-565`）：`stride==2 → header>>6`，`stride==4 → header>>1`。这是 FName 长度解析的关键，MCP 复刻时不能写死。

---

## 4. `UE_Offsets::ToString()` / `UE_Pointers::ToString()`（cpp `:20-355`）

用一组宏（`kOUT_NS_MEMBER_P/I/B`）把偏移表序列化成**可直接 `#include` 的 C++ 头**：

```cpp
namespace UEOffsets {
    namespace Config { constexpr bool IsUsingFNamePool = true; ... }
    namespace FName { constexpr uintptr_t ComparisonIndex = 0x0; ... }
    ...
}
```

**这是 `Offsets.hpp` 产物的生成逻辑**（Dumper 转储时写盘）。注意：
- 指针字段用 `(void*)(uintptr_t(...))` 强转打印（`:18`），**避免 char* 走 ostream 字符串路径**
- `FNameEntry` 只有 `Index`/`Name`（GetIsWide 回调不导出，因为 lambda 无法序列化）

---

## 5. `UEVars` 的缓存与诊断（cpp `:769-826`）

### `std::string UEVars::GetNameByID(int32_t id) const`　`:769-788`

```cpp
static std::unordered_map<int32_t, std::string> namesCachedMap;  // ← static！
static std::mutex namesCachedMtx;
// 命中缓存直接返回；未命中调 pGetNameByID 后写入缓存
// pGetNameByID 为空 → 返回 "pGetNameByID_IS_NULL"
```

⚠️ **`namesCachedMap` 是 static，跨进程不清理**。切进程后若同名 id 命中旧缓存，会拿到上一个进程的名字。MCP 侧切进程后必须重新 `InitUEVars`（会重建 `pGetNameByID`，但 **static 缓存本身不失效**——这是隐患，见陷阱 2）。

### `static std::string UEVars::InitStatusToStr(UEVarsInitStatus s)`　`:790-826`

14 个枚举值 → 字符串。**`getProbeStatus.lastError` / 探测失败日志的唯一文本来源**。

⚠️ 两个小坑：
1. **`ARCH_NOT_SUPPORTED` 输出 `"ERROR_ARCH_NOT_SUPPORTED"`**（`:801`）——枚举名无 `ERROR_` 前缀，字符串有，不一致。
2. default 分支返回 `"UNKNOWN"`（`:825`）。

---

## 6. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | 偏移 0 = 版本不支持，不是偏移 0 | 误判字段在偏移 0 | `describeClass` 显式标注"不支持" |
| 2 | **9 档偏移表 static + once，参数变化不重算** | 同版本函数第二次传不同参数返回旧缓存 | DetectVersion 只调一次，避免重复调用 |
| 3 | `UEVars::GetNameByID` 的 static 缓存跨进程不清理 | 切进程名字串档 | 切进程强制重建（需清缓存，见下） |
| 4 | `UProperty.Size` 是子类数据区偏移 | 当属性大小用全错 | 用 `ElementSize` 取大小 |
| 5 | UE4.25 起 `UProperty.*` 清零 | 误读旧字段 | 版本 ≥4.25 走 `FProperty.*` |
| 6 | FNamePool 解析依赖 stride/blocksOff/GetLength | 写死必错 | 复刻 lambda 逻辑 |
| 7 | `pGetNameByID` 空返回 `"pGetNameByID_IS_NULL"` | 当真实名字用 | 视为"未初始化"哨兵 |
| 8 | `InitStatusToStr` 的 `ARCH_NOT_SUPPORTED` 命名不一致 | 字符串匹配错 | 认准字符串值 |

---

## 7. 本次未精读范围

无。hpp + cpp 已全文精读。`UE_DefaultOffsets` 各档的逐字段偏移值已在上表归纳，具体数值可直接查 cpp `:380-766`。
