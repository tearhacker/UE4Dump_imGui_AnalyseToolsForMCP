# L4 分析层 API · AutoFixPropertyOffsets（属性子偏移推断）

> **源码**：`src/AutoFix/PropertyOffsetFinder.hpp`（7 行）/ `PropertyOffsetFinder.cpp`（483 行）
> **定位**：通过已知锚点类，推断 13 项属性子类的子指针偏移。AutoFix 的核心一环，`describeClass`/`analyzeClass` 类型解析的偏移来源。
> **已精读范围**：hpp 全文 + cpp 全文（1–483）。

---

## 0. 全局约定

1. **前置条件（`ResolveOffsets` `:320-322`）**：`!IsUsingFNamePool || !UStruct.ChildProperties || !FProperty.Size` 直接 return。**只有 FNamePool + FField 体系（UE4.25+）才做推断**；GNames 老版本用预设值，不进本层。
2. **副作用**：`ResolveOffsets` 直接写 `offsets->XXX.XXX`（改偏移表），与 `BootstrapCoreObjectArrayOffsets` 同类副作用。推断结果覆盖版本预设值。
3. **缓存**：`EnsureResolved` 按 `gCachedGeneration`（`:20`）+ `gResolved`（`:21`）缓存，切进程后 `Init()` 递增代数自动失效；`Invalidate()`（`:467-471`）手动清。
4. **失败保持预设**：任一项推断失败（锚点类不存在）→ 该偏移保持版本预设值，**不影响其他项**。

---

## 1. 公开接口

| 接口 | 签名 | 作用 |
|---|---|---|
| 确保已推断 | `void EnsureResolved()` | 按代数缓存，首次或代际变化时跑 `ResolveOffsets()` |
| 失效 | `void Invalidate()` | 清缓存，下次 `EnsureResolved` 重跑 |

---

## 2. 查找辅助（全部全量线性扫描）

| 函数 | 行为 | 行号 |
|---|---|---|
| `FindClassByName(name)` | `ForEachObjectOfClass(UE_UClass)` 找短名匹配，返回空对象若未找到 | `:48-64` |
| `FindStructByName(name)` | 同上（跳过 UFunction） | `:66-84` |
| `FindEnumByName(name)` | 同上 | `:86-102` |
| `FindFunctionByName(name)` | 同上 | `:104-120` |
| `FindMember(owner, memberName, propertyClassName, includeSupers=true)` | 沿 `GetChildProperties` 找成员，**可选沿 Super 链最多 64 层**（`:129`） | `:127-147` |

⚠️ **这 4 个 `Find*ByName` 都是 O(n) 全量扫描**（对象数组可能几十万）。`ResolveOffsets` 会调用约 15 次 → 属「中 <30s」档。MCP 侧若要暴露"按名找类"，应复用这套而不是再写一套（但要注意性能）。

---

## 3. 偏移扫描算法

### `FindPointerOffset(infos, minOffset, maxOffset, minRequired)`　`:163-186`

指针偏移对齐扫描：从 `AlignPointerOffset(minOffset)` 起、步进 8，找**所有 anchor 的 `field地址+offset` 读出的值 == expected** 的第一个 offset。`matches == validPairs` 才接受。

### `FindByteOffset(infos, minOffset, maxOffset, minRequired)`　`:188-211`

字节版（步进 1），逻辑同上。

### `FindFirstPropertyPointerOffset(field, min, max)`　`:213-225`

找**第一个「值看起来像属性指针」的偏移**（`IsPropertyPointer`：值 ≥0x10000 且类名含 "Property"）。

### `FindMapPropertyBaseOffset(field, min, max)`　`:227-241`

Map 专用：找 `offset` 与 `offset+8` **两个位置都是属性指针**的偏移（KeyProp + ValueProp 连续）。

### `FindBoolPropertyBaseOffset(offsets)`　`:243-310` —— 最难的一项

1. **锚点法**：`Engine.bIsOverridingSelectedColor`（掩码 0xFF）、`Engine.bEnableOnScreenDebugMessagesDisplay`（0x02）、`PlayerController.bAutoManageActiveCameraTarget`（0xFF），`FindByteOffset` 找 fieldMask 位置，`>=3` 则 `return fieldMaskOffset - 3`（反推 Base）
2. **全量评分法**（锚点失败才走）：收集 128 个 BoolProperty（`:274`），对每个 `base` 偏移打分——`fieldSize ∈ {1,2,4,8}` 且 `byteOffset < fieldSize` 且 `byteMask` 合法且 `fieldMask` 是 bool 掩码，`bestScore >= 4` 才接受

---

## 4. 13 项推断清单（`ResolveOffsets` `:318-464`）

| # | 推断项 | 锚点 | 方法 | 行号 |
|---|---|---|---|---|
| 1 | `ObjectProperty.PropertyClass` | Controller.PlayerState / Controller.Pawn | FindPointerOffset | `:328-339` |
| 2 | `ObjectProperty.PropertyClass` | World.PersistentLevel → Level | FindPointerOffset | `:341-348` |
| 3 | `StructProperty.Struct` | TwoVectors.v1 / v2 → Vector | FindPointerOffset | `:350-362` |
| 4 | `ByteProperty.Enum` | CollisionResponseContainer.GameTraceChannel1/2 | FindPointerOffset | `:364-374` |
| 5 | `BoolProperty.Base` | Engine 三锚点 + 全量评分 | FindBoolPropertyBaseOffset | `:376-377` |
| 6 | `EnumProperty.UnderlayingProp/Enum` | ActorComponent.CreationMethod | FindPointerOffset → `-8`/`+0` | `:379-392` |
| 7 | `EnumProperty.UnderlayingProp/Enum` | Pawn.AutoPossessAI | 同上 | `:394-407` |
| 8 | `DelegateProperty.SignatureFunction` | K2_GetTimerElapsedTimeDelegate.Delegate | FindPointerOffset | `:409-420` |
| 9 | `ArrayProperty.Inner` | GameViewportClient.DebugProperties | FindFirstPropertyPointerOffset | `:422-427` |
| 10 | `SetProperty.ElementProp` | LevelCollection.Levels | FindFirstPropertyPointerOffset | `:429-434` |
| 11 | `MapProperty.KeyProp/ValueProp` | UserDefinedEnum.DisplayNameMap | FindMapPropertyBaseOffset → `+0`/`+8` | `:436-444` |
| 12 | `ClassProperty.MetaClass` | **推导** = `ObjectProperty.PropertyClass + 8` | 非扫描 | `:446-447` |
| 13 | `InterfaceProperty.InterfaceClass` | **推导** = `ObjectProperty.PropertyClass` | 非扫描 | `:448-449` |

### 关键点

1. **#12/#13 是推导不是扫描**：依赖 #1/#2 先拿到 `ObjectProperty.PropertyClass`，否则两者保持 0。
2. **EnumProperty 成对**：`UnderlayingProp = enumOffset - 8`，`Enum = enumOffset`（`>=8` 才接受，`:386-390`）。
3. **锚点类名硬编码**（Engine / Controller / Pawn / World / TwoVectors / CollisionResponseContainer / UserDefinedEnum 等）。**游戏改类名则该推断静默失败**，保持预设值。
4. **`LogFound` 打印**（`:451-463`）：每个推断项命中时 `LOGI("[AutoFix] Xxx=0x...")`——MCP `getLogs` 可见，可作探测成功度诊断。

---

## 5. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | 前置条件（FNamePool+FField+FProperty.Size）不满足直接跳过 | 老版本不推断 | 老版本用预设值，无需推断 |
| 2 | 锚点类名硬编码，游戏改类名静默失败 | 偏移保持预设值 | `getProbeResults` 如实报告"未推断" |
| 3 | ClassProperty/InterfaceProperty 依赖 ObjectProperty 先成功 | 主项失败则两项连坐 | 诊断时看依赖链 |
| 4 | `Find*ByName` O(n) 全量扫描，ResolveOffsets 调 15 次 | 慢 | 标注「中」档，缓存结果 |
| 5 | 写 `offsets` 副作用 | 覆盖预设值 | 与 Bootstrap 同类，报告修正后值 |
| 6 | `FindBoolPropertyBaseOffset` 全量评分需 128 bool 字段 | 字段少则失败 | bestScore>=4 兜底 |

---

## 6. 本次未精读范围

无。hpp + cpp 已全文精读。
