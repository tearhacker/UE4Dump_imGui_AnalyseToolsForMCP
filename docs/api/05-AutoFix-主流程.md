# L4 分析层 API · AutoFix 主流程（RunFixup 编排）

> **源码**：`src/AutoFix/AutoFixOffsets.hpp`（14 行）/ `AutoFixOffsets.cpp`（3185 行）
> **定位**：AutoFix 的总编排。基于真实内存布局校验并修补 `UE_Offsets` 偏移表。`startProbe` 的最后一环。
> **已精读范围**：hpp 全文（14 行）+ cpp `1-120`（辅助函数）+ `3110-3185`（RunFixup 全文）。中间 30 个 `Fixup*` 函数体是同类"锚点比对推断"模式（已在 07 文档展示），未逐行精读，但**完整清单与依赖关系已从 RunFixup 调用序列提取**。

---

## 0. 全局约定

1. **唯一入口 `RunFixup`**（hpp `:13`），必须在 `InitUEVars()` 成功之后调用（hpp `:8-9` 注释明示）。
2. **安全降级**（hpp `:11-12`）：任一锚点找不到 → **保留版本预设值并继续**，不阻断。
3. **依赖 `UEWrappers` 全局**（`Offsets()`/`Vars()`/`Objects()` 三个访问器 `:29-31`）。
4. **失败不抛异常**：`RunFixup` 返回 bool，日志 `LOGI/LOGE/LOGW` 输出过程。

---

## 1. `RunFixup(IGameProfile *profile)`　`:3116-3184`（主流程）

```
1. 前置校验：Offsets()/Objects()/Vars() 任一空 → "UEWrappers not initialized" 返回 false
2. ApplyDefaultsOnly()                     ← 先套版本默认偏移
3. 快照 before = *off
4. 依赖链推断（见 §2）：
   nameOk  = FixupNamePrivate()            ← 根锚点（FName 解码的基础）
   classOk = nameOk ? FixupClassPrivate() : false
   ...其余 28 项全部 if (nameOk) 才执行（共 30 项调用：29 个 `Fixup*` + 1 个 `Apply*`）...
   funcOk  = nameOk ? FixupUFunctionFunc() : false
5. nameOk 时：Invalidate + EnsureResolved(PropertyOffsets) + Warmup(StructLayout)
6. DumpAutoFoundOffsets(before, *off)      ← 打印前后对比
7. success = nameOk && classOk && superOk && funcOk
```

### 关键：`nameOk` 是依赖链的根

`FixupNamePrivate()`（推断 `UObject.NamePrivate`）失败 → **后续 29 项全部跳过**（注释 `:3132-3133`：后续修复都依赖 NamePrivate 解码 FName）。

**成功判定四锚点**（`:3181`）：`nameOk && classOk && superOk && funcOk` —— 即使 30 项里大部分成功，只要这四个关键锚点有一个失败，`RunFixup` 仍返回 false。

---

## 2. 30 项推断清单（29 个 `Fixup*` + 1 个 `Apply*`，按 RunFixup 调用顺序）

| Fixup | 推断的偏移 | 依赖 |
|---|---|---|
| `FixupNamePrivate` | `UObject.NamePrivate` | **根** |
| `FixupClassPrivate` | `UObject.ClassPrivate` | nameOk |
| `FixupFNameSettings` | FName Size/ComparisonIndex/Number/CasePreserving/OutlineNumber | nameOk |
| `ApplyFNameDependentFieldFixups` | FName 相关字段批量 | nameOk |
| `FixupObjectFlags` | `UObject.ObjectFlags` | nameOk |
| `FixupInternalIndex` | `UObject.InternalIndex` | nameOk |
| `FixupOuterPrivate` | `UObject.OuterPrivate` | nameOk |
| `FixupSuperStruct` | `UStruct.SuperStruct` | nameOk |
| `FixupStructSize` | `UStruct.PropertiesSize` | nameOk |
| `FixupMinAlignment` | `UStruct.MinAlignment` | nameOk |
| `FixupClassDefaultObject` | `UClass.ClassDefaultObject` | nameOk |
| `FixupImplementedInterfaces` | `UClass.ImplementedInterfaces` | nameOk |
| `Fixup_UClassCastFlags` | `UClass.CastFlags` | nameOk |
| `Fixup_UClassClassFlags` | `UClass.ClassFlags` | nameOk |
| `FixupLevelActors` | `ULevel.Actors` | nameOk |
| `FixupDataTableRowMap` | `UDataTable.RowMap` | nameOk |
| `FixupChildren` | `UStruct.Children` | nameOk |
| `FixupUFieldNext` | `UField.Next` | nameOk（UE<4.25 用，UE>=4.25 自适应） |
| `FixupChildProperties` | `UStruct.ChildProperties` | nameOk |
| `FixupFFieldName` | `FField.NamePrivate` | nameOk（FField 三件套①） |
| `FixupFFieldClassName` | `FField.ClassPrivate` | nameOk（三件套②） |
| `FixupFFieldNext` | `FField.Next` | nameOk（三件套③） |
| `FixupFFieldClass` | `FFieldClass.Name` | nameOk |
| `FixupFFieldOwner` | `FField.Owner` | nameOk |
| `FixupFFieldClassSuperClass` | `FFieldClass.SuperClass` | nameOk |
| `FixupFFieldClassCastFlags` | `FFieldClass.CastFlags` | nameOk |
| `FixupFPropertyLayout` | FProperty 五字段（ArrayDim/ElementSize/PropertyFlags/Offset_Internal/Size） | nameOk |
| `FixupUEnumNames` | `UEnum.Names` | nameOk |
| `FixupUFunctionLayout` | UFunction（EFunctionFlags/NumParams/ParamSize） | nameOk |
| `FixupUFunctionFunc` | `UFunction.Func` | nameOk |

**FField 三件套顺序**（注释 `:3154-3156`）：Name → Class → Next，因 Class 验证依赖 ChildProperties 链遍历、Next 验证依赖 NamePrivate 已知。

---

## 3. 关键辅助（`1-120`）

| 函数 | 作用 | 行号 |
|---|---|---|
| `Offsets()/Vars()/Objects()` | UEWrappers 三访问器 | `:29-31` |
| `ReadPtr/ReadI32/ReadI16/ReadI8/ReadU32/ReadU64` | 类型化读内存 | `:40-68` |
| `IsPlausibleArrayCount(count, max, cap=0x1000)` | 数组计数合理性 | `:70-73` |
| `IsObjectAByClassName(obj, name, depth=16)` | 沿 Super 链判类名 | `:75-90` |
| `FindClassByFullName/ByName` / `FindStructByName` | FindObject 封装 | `:92-105` |
| `ReadNameById` / `ReadNameAt` / `ReadObjectName` | 名字解析 | `:107-121` |

**扫描范围常量**（`:24-27`）：`kNameScanStart=0x08` / `kNameScanEnd=0x7C`（NamePrivate 候选扫描区间）、`kStrictSampleCap=0x40` / `kLooseSampleCap=0x400`（采样上限）。

---

## 4. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | `nameOk` 是根，失败则 30 项全跳过 | 单点失败大面积降级 | 诊断先看 NamePrivate 是否推断成功 |
| 2 | 成功判定只看四锚点 | 30 项部分成功也可能返回 false | 看 `DumpAutoFoundOffsets` 前后对比 |
| 3 | `DumpAutoFoundOffsets` 打印前后对比 | 诊断利器 | `getLogs` 可见，MCP 应透出 |
| 4 | `ApplyDefaultsOnly` 先套预设 | 推断前偏移已是预设值 | 推断是"覆盖"不是"从零" |
| 5 | 依赖 UEWrappers 未初始化 | 早退返回 false | `startProbe` 前先 `InitUEVars` |
| 6 | 3185 行，RunFixup 是慢操作 | 探测总耗时大头 | 标注「中/慢」档，waitMs 覆盖 |

---

## 5. 本次未精读范围

| 范围 | 内容 |
|---|---|
| `:122-3109` | 30 个 `Fixup*` 函数体 + `ApplyDefaultsOnly` + `DumpAutoFoundOffsets` 实现 |

这些函数体是同类"锚点比对 + 采样打分"模式（与 07 文档 `ResolveOffsets` 同构），清单与依赖关系已从 RunFixup 调用序列完整提取。MCP 实现到 `applyProbeOverrides` 需要精确偏移推断逻辑时，再逐函数精读。
