# L4 分析层 API · AutoFixStructLayout（结构体布局推断）

> **源码**：`src/AutoFix/StructLayout.hpp`（39 行）/ `StructLayout.cpp`（630 行）
> **定位**：推断结构体 Size / Alignment / 位域布局 / 字段排序。`describeClass` / `analyzeClass` 的字段偏移与大小直接来源。
> **已精读范围**：hpp 全文 + cpp 全文（1–630）。

---

## 0. 全局约定

1. **全量预热 + O(1) 查表**：`BuildLayoutCache()` 一次性遍历所有 UStruct 建表（`gStructInfos` map），之后查询 O(1)。`Warmup()` 可在探测阶段显式预热。
2. **按代数缓存失效**：`gCachedGeneration` 比对 `UEWrappers::GetInitGeneration()`（`:510-518`）——切进程后 `Init()` 递增代数自动失效；也可手动 `Invalidate()`（`:625-629`）。
3. **防御上限**：`kMaxFieldCount=2048`、`kMaxStructSize=0x400000`（4MB）、`kMaxArrayDim=0x10000`（`:15-17`）。超限字段/大小被丢弃或归零，防脏数据打爆。
4. **空对象返回空/零**：`GetStructLayoutInfo` 对空 strct 返回 `StructLayoutInfo{}`（全零），`GetSortedFields` 返回空 vector。

---

## 1. 数据结构

### `StructLayoutInfo`（hpp `:10-19`）

| 字段 | 默认 | 含义 |
|---|---|---|
| `LastMemberEnd` | 0 | 最后一个成员结束偏移（`offset + elementSize × arrayDim` 的最大值） |
| `Size` | 0 | 结构体大小（含父类） |
| `AlignedSize` | 0 | `Size` 按 `Alignment` 对齐后 |
| `Alignment` | 1 | 对齐值 |
| `UseExplicitAlignment` | false | 是否用了显式 MinAlignment（`minAlignment > highestMemberAlignment`） |
| `HasReusedTrailingPadding` | false | **父类尾 padding 被子类复用**（UE 布局经典陷阱） |
| `IsFinal` | true | 是否叶子结构体（被继承则置 false） |

### `FieldLayoutInfo`（hpp `:21-31`）

| 字段 | 含义 |
|---|---|
| `Field` | `UE_FProperty` 属性对象 |
| `Offset` | 成员偏移 |
| `ElementSize` | 元素大小 |
| `ArrayDim` | 数组维度 |
| `TotalSize` | `ElementSize × ArrayDim` |
| `IsBitfield` | 是否位域 bool |
| `BitIndex` | 位索引（`byteOffset×8 + bit`，非位域 = `0xFF`） |
| `FieldMask` | 位掩码 |

---

## 2. 四个公开接口

### `const StructLayoutInfo &GetStructLayoutInfo(const UE_UStruct &strct)`　`:536-548`

- `BuildLayoutCache()` 后查 `gStructInfos`；**未命中则 `BuildSingleInfo` 单算并 emplace**
- 空 strct 返回全零的 `StructLayoutInfo{}`

### `std::vector<FieldLayoutInfo> GetSortedFields(const UE_UStruct &strct)`　`:550-600`

- 遍历 `GetChildProperties()` 链表，`offset < 0` 跳过，`ElementSize<=0 || TotalSize<=0` 跳过
- **排序规则**（stable_sort，`:588-597`）：按 `Offset` 升序 → 位域排后 → 位域内按 `BitIndex` 升序 → 地址兜底
- ⚠️ **只遍历当前层 ChildProperties，不含父类字段**——完整字段列表要沿 `GetSuper()` 逐级合并（`analyzeClass` 需自行递归）

### `int32_t GetOwnMemberStart(const UE_UStruct &strct)`　`:602-610`

```cpp
if (!super) return 0;
return superInfo.HasReusedTrailingPadding ? superInfo.Size : superInfo.AlignedSize;
```

**核心语义**：子类自己的成员从父类的哪个偏移开始。若父类尾 padding 被复用，则从父类**未对齐的 Size** 开始；否则从 **AlignedSize** 开始。这是判断"父类结束点"的关键，`analyzeClass` 划分自有字段 vs 继承字段就靠它。

### `int32_t GetDisplayStructSize(const UE_UStruct &strct)`　`:612-618`

`AlignedSize > 0 ? AlignedSize : strct.GetSize()`。

---

## 3. 构建流程（`BuildLayoutCache` `:520-533`，5 阶段）

```
EnsureCacheFresh   → 按代数失效，代际变化 ResetCaches
CollectStructs     → ForEachObjectOfClass(UE_UStruct) 收集所有结构体（跳过 UFunction）
BuildBasicInfos    → 每结构体 BuildSingleInfo
FixAlignmentsFromSupers → 沿 Super 链修正对齐（父对齐 >= 子对齐）
FixSizesAndFinalFlags   → 修正大小 + 标记 IsFinal + 复用尾 padding
```

### `BuildSingleInfo`（`:337-392`）

1. **接口结构体特判**（`:343-350`）：`Alignment=1, Size=0, AlignedSize=0`（接口无内存布局）
2. 读 `minAlignment`，遍历成员算 `highestMemberAlignment`
3. **类 + 有父类 + 对齐 < 指针** → 强制 `Alignment = sizeof(void*)`（`:375-378`）
4. 否则 `Alignment = max(minAlignment, highestMemberAlignment)`，且 `UseExplicitAlignment = minAlignment > highestMemberAlignment`
5. `Size = RawStructSize(strct)`，为 0 且有父类则回退 `RawStructSize(super)`

---

## 4. 关键算法

### 4.1 对齐推断（`ReadStructMinAlignment` `:67-99`）

读 `UStruct.MinAlignment`，**双候选偏移**（显式偏移 + 推断偏移 `PropertiesSize+4`），**32 位/16 位都试**，`NormalizeAlignment` 校验（非 2 的幂或 >0x100 → 1）。

### 4.2 属性对齐/大小查表（MCP `analyzeClass` 的核心）

**`KnownPropertyAlignment`（`:163-185`）**：类名 → 对齐
- `Bool/Byte/Int8` → 1；`Int16/UInt16` → 2；`Int/Float/Name` → 4
- `Int64/UInt64/Double/Object/Class/Array/Map/Set/Str/Text/Delegate/Interface...` → `sizeof(void*)`（8）

**`KnownPropertyElementSize`（`:219-240`）**：类名 → 大小
- `Bool/Byte/Int8` → 1；`Int16/UInt16` → 2；`Int/Float` → 4
- `Int64/Double/Name/Object/Class/Interface` → 8
- `Str/Array/Delegate` → 0x10；`Text` → 0x18；**`Map/Set` → 0x50**

⚠️ **这些是 64 位引擎的硬编码兜底值**。MCP 侧若要复刻 `analyzeClass` 的"内嵌结构体 vs 指针"判断，这套表是依据——`StructProperty` 内嵌（递归取 size），`ObjectProperty`/指针类走 8 字节。

### 4.3 位域 bool（`GetBoolBitIndex` `:136-150`）

`FieldMask` 为单 bit（`fieldMask == 1<<bit`）才是位域，`BitIndex = byteOffset×8 + bit`；`0xFF` 或非 2 的幂 → 普通 bool（返回 0xFF）。

### 4.4 接口判定（`IsInterfaceStruct` `:293-316`）

沿 `GetSuper()` 链找 `Class CoreUObject.Interface`，**带 `seen` 集合防环**（`:306-315`），有环立即 break。

### 4.5 尾 padding 复用（`FixSizesAndFinalFlags` `:489-499`）

子类成员 `lowestOffset` 落在父类 `AlignedSize` 之前 → 父类 `HasReusedTrailingPadding=true`，`Size` 收缩到 `lowestOffset`。**这是 UE 编译器复用父类尾 padding 的实证判定**。

---

## 5. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | `GetSortedFields` 只含当前层，不含父类 | 漏掉继承字段 | `analyzeClass` 沿 `GetSuper()` 递归合并 |
| 2 | `HasReusedTrailingPadding` 决定子类成员起点 | 起点算错整个布局错 | 用 `GetOwnMemberStart`，别手动算 |
| 3 | 接口结构体 `Size=0` | 误判"无布局" | 接口单独处理 |
| 4 | `Map/Set=0x50` 等硬编码大小 | 非 64 位引擎错 | 仅兜底，优先读真实 `ElementSize` |
| 5 | 按代数缓存，切进程不失效会串档 | 拿到上个进程布局 | 切进程 `Init()` 或 `Invalidate()` |
| 6 | 字段上限 2048 / 大小 4MB | 超限静默丢弃 | 超大结构体标注"截断" |
| 7 | 位域 bool 的 `BitIndex` 非位域是 0xFF | 当有效 bit 用错 | 先判 `IsBitfield` |

---

## 6. 本次未精读范围

无。hpp + cpp 已全文精读。
