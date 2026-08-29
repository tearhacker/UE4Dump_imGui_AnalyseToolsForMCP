# L5 批处理层 API · UE_UPackage（SDK 产物生成）

> **源码**：`src/UPackageGenerator.hpp`（84 行）/ `UPackageGenerator.cpp`（678 行）
> **定位**：把 UObject 转成 C++ SDK 文本（SDK_Classes/Structs/Enums/Offset）。`dumpSDK` 产物格式的唯一来源。
> **已精读范围**：hpp 全文 + cpp 全文（1–678）。

---

## 0. 全局约定

1. **`UE_UPackage` 一个实例对应一个包**（`GatherUObjects` 里按 `GetPackageObject()` 分组）。
2. **三容器**：`Classes`（UClass）/ `Structures`（UScriptStruct）/ `Enums`（UEnum）。
3. **产物格式**（4 种输出）：`AppendStructsToBuffer`（SDK_Classes/Structs）、`AppendEnumsToBuffer`（SDK_Enums）、`AppendOffsetsToBuffer`（SDK_Offset）、`AppendToBuffer(s)`（聚合/分流）。
4. **依赖 AutoFix**：`GenerateStruct` 用 `AutoFixStructLayout`（布局），`AppendStructsToBuffer` 用 `AutoFixNativeFunctions`（虚表注释）。

---

## 1. `Process()`　`:481-540`

1. 先按 `GetOuter()` 把 UFunction 归组到 `functionsByOuter`（`:485-496`）
2. 遍历对象：UClass → `GenerateStruct` + `mergeOuterFunctions`；UScriptStruct 同理；UEnum → `GenerateEnum`
3. `mergeOuterFunctions`（`:498-521`）：把 Outer 归组的函数合并进对应 Struct，**按 FullName 去重**

---

## 2. `GenerateStruct`（`:128-335`）

### 关键分支 `useAutoLayout`

```cpp
const auto layoutFields = AutoFixStructLayout::GetSortedFields(object);
const bool useAutoLayout = !layoutFields.empty();   // ← 决定走新版还是旧版
```

- **useAutoLayout = true**：用 `AutoFixStructLayout` 的排序字段 + 布局信息（含位域、对齐、padding 复用）
- **false**：回退旧版 `GetChildProperties()` 链 + `GetChildren()` 链

**结构体头**（`:137-161`）：
- `UseExplicitAlignment` → `alignas(0x...)`
- `Inherited = GetOwnMemberStart(object)`；`Size = GetDisplayStructSize(object)`
- 有父类 → `: SuperCppName`
- `UsePack = HasReusedTrailingPadding`（父类尾 padding 复用 → `#pragma pack(push, 0x1)`）

### 位域 bool 的 magic 判断（`:184`）

```cpp
if (type.first == UEPropertyType::BoolProperty && *(uint32_t *)type.second.data() != 'loob')
```

⚠️ **`'loob'` 是 `"bool"` 的 4 字节小端整数**（0x6C6F6F62）。当属性类型是 BoolProperty 但**类型名不是 "bool"**（即自定义位域 bool），才走位域逻辑。这是判断"是不是原生 bool"的巧妙写法，MCP 复刻时别照抄成字符串比较。

### 位域生成（`:199-213` / `:242-270`）

用 `GetFieldMask()` 数前导 0（`zeros`）和连续 1（`ones`），生成 `Name : ones` 位域声明 + `BitPad` 填充。

### 函数列表

- 旧版：`GetChildren()` 里 `IsA<UE_UFunction>()` 收集（`:297-305`）
- 新版：同样走 `GetChildren()`（`:317-328`）

---

## 3. `GenerateFunction`（`:51-126`）

- 参数从 `GetChildProperties()`（FField）优先，**空则回退 `GetChildren()`（UField）**（`:100-116`）
- 参数修饰（`:73-97`）：
  - `CPF_ReturnParm` 或名字 "ReturnValue" → 设 `CppName = type + " " + name`（返回值）
  - `CPF_ConstParm` → `const `
  - `ArrayDim > 1` → `Type* Name`
  - `CPF_OutParm` → `Type& Name`
- 无返回值 → `CppName = "void " + name`（`:122-125`）

---

## 4. `GenerateEnum`（`:337-404`）

- 读 `GetNames()`（`TArray<TPair<FName,int64>>`），`pairSize = Align(FName.Size) + 8`
- 名字取最后一个 `:` 之后（去命名空间）
- **`isUninitializedEnum`（`:365-376`）**：值非严格递增 → **全部重置为 0..n-1**（枚举未初始化时兜底）
- 类型推断（`:389-396`）：按 max 值选 `uint8_t/uint16_t/uint32_t/uint64_t`

---

## 5. 产物输出格式

### `AppendStructsToBuffer`（`:406-459`）—— SDK_Classes/Structs

每个结构体：
```
// Object: <FullName>
// Size: 0x{Size} (Inherited: 0x{Inherited})
struct {CppName} [ : Super] {
    {Type} {Name}; // 0x{Offset}(0x{Size})[, Mask(...)]
    ...
    // Object: <func FullName>
    // Flags: [...]
    // Offset: 0x{funcOffset}
    void/type FuncName(params); // VTableIndex: N (Offset: 0x.., Real: 0x..)
};
```

⚠️ **虚表注释**（`:437-449`）：每个函数调 `ResolveNativeFunctionInfo` 拿 `VTableOffset`/`RealOffset`，非零则注释 `VTableIndex`（slot/8）。**这是 SDK_Classes.hpp 里 `VTableIndex` 注释的来源**。

### `AppendEnumsToBuffer`（`:461-479`）

```
enum class {Name} : {type} { A = 0, B = 1, ... };
```

### `AppendOffsetsToBuffer`（`:619-655`）—— SDK_Offset.hpp

每个结构体生成 `namespace {CppIdent} { constexpr uintptr_t MemberName = 0x...; namespace Functions {...} }`，供 `SDKOffset::ClassName::MemberName` 引用。

### `MakeCppIdentifier`（`:600-617`）

非 `[A-Za-z0-9_]` → `_`；首字符数字 → 前插 `_`；空 → `"_"`。

---

## 6. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | `useAutoLayout` 决定新旧两条生成路径 | 行为分叉 | 依赖 AutoFixStructLayout 是否产出 |
| 2 | 位域判断 `'loob'` 是 "bool" 小端整数 | 照抄成字符串比较错 | 理解 0x6C6F6F62 语义 |
| 3 | `isUninitializedEnum` 重置枚举值为 0..n-1 | 未初始化枚举值被改写 | 产物枚举值可能是"伪造的" |
| 4 | 函数参数 ChildProperties 优先、Children 回退 | 版本差异 | UE4.25+ 走 FField |
| 5 | `SDK_Classes.hpp` 可达几十 MB | 整文件读爆上下文 | `searchClasses`/`describeClass` 服务端过滤 |
| 6 | 函数按 Outer 归组再合并 | 漏合或重复 | 按 FullName 去重 |
| 7 | 依赖 `ResolveNativeFunctionInfo`（慢） | 每函数一次 | 转储慢，标注「慢」档 |

---

## 7. 本次未精读范围

无。hpp + cpp 已全文精读。
