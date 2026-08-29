# L4 分析层 API · AutoFix 函数解析（虚表 + 原生函数）

> **源码**：`src/AutoFix/VirtualFunctionResolver.{hpp,cpp}`（23 + 239 行）/ `NativeFunctionResolver.{hpp,cpp}`（15 + 377 行）
> **定位**：解析 UFunction 的「exec 桩 → 虚表槽位 → 真实实现地址」三地址关系。`analyzeClass` / `dumpSDK` 函数签名的核心，`script.json` 的 `Functions:[{Address,Name}]` 来源。
> **已精读范围**：两个模块的 hpp + cpp 全部全文精读。

---

## 0. 全局约定

1. **两个 namespace**：`AutoFixVTable`（虚表槽位解析）+ `AutoFixNativeFunctions`（原生函数三地址编排，依赖前者）。
2. **全部按代数缓存**：两个模块都用 `cachedGeneration` 比对 `UEWrappers::GetInitGeneration()`，切进程自动失效。
3. **失败返回 0**：所有地址/偏移解析失败返回 0（`kNotFound`），不抛异常。
4. **`funcFlags` 被忽略**（`NativeFunctionResolver.cpp:341` `(void)funcFlags;`）——主入口的参数目前**没有实际用途**，MCP 侧调用可传任意值。

---

## 1. `AutoFixVTable` 四接口

| 接口 | 签名 | 作用 | 行号 |
|---|---|---|---|
| 找虚表调用槽位 | `uintptr_t FindVTableCallOffset(uintptr_t functionAddress)` | exec 桩内间接调用的 vtable slot 字节偏移，0=非虚 | `:58-174` |
| 找直接分支目标 | `uintptr_t FindDirectBranchCallTarget(uintptr_t functionAddress)` | 桩内第一个不自环的 B/BL 绝对目标，0=无 | `:176-225` |
| 槽位转索引 | `int OffsetToIndex(uintptr_t off)` | `off%8==0 ? off/8 : -1`（inline） | hpp `:16-19` |
| 解析虚表函数 | `uintptr_t ResolveVTableFunction(uintptr_t objectAddress, uintptr_t vtableOffset)` | 读对象 vtable → slot 处函数指针 | `:227-238` |

### 1.1 `FindVTableCallOffset` —— 寄存器状态机（`:58-174`）

**这是本项目最精密的汇编模拟器**，追踪 4 种寄存器语义：

```
RegKind: Unknown / ThisPtr / VTable / VTableSlotAddress / VirtualFunction
初始: regs[0] = ThisPtr（x0 是 this）
扫描上限 192 条指令（:85），遇 RET / 读失败 break
```

| 指令 | 状态传播 |
|---|---|
| `BLR Xn` / `BR Xn` | reg 是 VirtualFunction 且 offset 合理 → **命中，返回 offset**；BLR 后清 caller-saved |
| `BL` | 清 caller-saved（x0-x18） |
| `MOV Xd, Xm` | `regs[d] = regs[m]` |
| `ADD Rd, Rn, #imm` | VTable/VTableSlotAddress → 传播为 VTableSlotAddress(off+imm)；ThisPtr+0 → ThisPtr |
| `LDR Rt, [Rn, #imm]` | ThisPtr+0 → VTable；VTable → VirtualFunction(off)；VTableSlotAddress → VirtualFunction(base+off) |

**合理槽位校验**（`IsReasonableVTableOffset` `:35-38`）：`offset != 0 && 8 对齐 && < 0x40000`。

⚠️ **vtable 指针固定 offset 0**（`GetVftOffset` 返回 0 `:51-55`）——假设标准 C++ 对象布局。非标准布局会误判。

### 1.2 `FindDirectBranchCallTarget`（`:176-225`）

扫描 192 条指令，找第一个「不自环」的 B/BL：`target` 不在 `[functionAddress, functionAddress+0x400)` 内、`!= functionAddress`、`>= 0x10000`、4 对齐。**BL 比 B 可靠**（`:214-215` 命中 BL 立即 break）。

### 1.3 `ResolveVTableFunction`（`:227-238`）

`objectAddress` 或 vtableOffset 非法 → 0；读 `objectAddress+0` 得 vtable 基址 → `isPtrReadable` 校验 → 读 `vtable + offset` 得函数指针。

---

## 2. `AutoFixNativeFunctions` 主入口

### `NativeFunctionExportInfo ResolveNativeFunctionInfo(uintptr_t ownerClassAddr, uintptr_t funcPtr, uint32_t funcFlags)`　`:339-376`

**三地址产物**（`NativeFunctionExportInfo`，hpp `:7-12`）：

| 字段 | 含义 | 计算 |
|---|---|---|
| `FuncOffset` | exec 桩的 RVA | `funcPtr - baseAddr`（`funcPtr > baseAddr` 才算，`:349-350`） |
| `VTableOffset` | 虚表槽位字节偏移 | `FindVTableCallOffset(funcPtr)`，0=非虚函数 |
| `RealOffset` | **真正实现的 RVA** | 见下 |

**RealOffset 的三种来源**：
1. `funcPtr <= baseAddr` 或非 4 对齐 → 直接返回（info 全 0 或只有 FuncOffset）
2. **非虚函数**（VTableOffset==0）→ `FindDirectBranchCallTarget` 目标 RVA；无目标则 `RealOffset = FuncOffset`（`:356-363`）
3. **虚函数** → `FindContextObjectForClass` 拿 context 对象 → `ResolveVTableFunction`；失败则走接口 vtable 解析（`:366-373`）

---

## 3. 缓存体系与关键算法

### 3.1 六个 static 缓存（`:22-28`）

| 缓存 | 键 → 值 |
|---|---|
| `gDiscoveredDefaultObjectOffset` | 已发现的 CDO 偏移（跨类复用） |
| `gScannedDefaultObjectCache` | classAddr → CDO 对象地址 |
| `gClassObjectCache` | classAddr → ClassObjectCandidate |
| `gInterfaceCheckCache` | interfaceAddr → (classAddr → bool) |
| `gInterfaceImplementerCache` | interfaceAddr → 实现者类列表 |
| `gResolvedInterfaceVTableCache` | interfaceAddr → (vtableOffset → 真实地址) |

全部 `ResetCachesIfNeeded()`（`:30-43`）按代数统一清。

### 3.2 CDO 扫描（`FindDefaultObjectByScanningClass` `:92-144`）

在 `classAddr + [0x40, 0x400)` 逐 8 字节扫，找「是此类对象 + 有原生 vtable + 是默认对象（`Outer==class` 或名字 `Default__` 前缀）」的对象。`gDiscoveredDefaultObjectOffset` 让后续类直接复用该偏移（`:114-122`）。

### 3.3 接口实现者发现（`GetInterfaceImplementerClasses` `:278-308`）

`ForEachObject` 全量扫，`IsA(UClass)` 且 `ClassMayImplementInterface` → 收集。**`ClassMayImplementInterface`（`:249-276`）** 在 `classAddr + [0x40, 0x500)` 扫 TArray，用 `ArrayContainsInterfaceClass`（`:224-247`）匹配接口类地址（条目大小试 0x10/0x18/0x20 三种）。

### 3.4 接口 vtable 解析（`ResolveInterfaceVTableFunction` `:310-336`）

遍历所有实现者类 → 每个拿 context 对象 → `AutoFixVTable::ResolveVTableFunction` → 取第一个落在模块内的真实地址。

---

## 4. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | `funcFlags` 参数被忽略 | 传错也不报错 | 可传 0，语义未实现 |
| 2 | vtable 指针固定 offset 0 | 非标准布局误判 | 已知假设，文档标注 |
| 3 | `FindVTableCallOffset` 上限 192 条指令 | 超长桩漏判 | 视为"非虚"，走直接分支 |
| 4 | 接口解析要遍历所有实现者（ForEachObject 全量） | 慢 | 标注「中/慢」档，缓存结果 |
| 5 | 全部按代数缓存，切进程失效 | 串档 | 切进程 Init 自动失效 |
| 6 | `RealOffset` 三来源优先级 | 判断错函数归属 | 理解 FuncOffset vs VTableOffset vs RealOffset 语义 |
| 7 | 依赖 `IsModuleAddress`（address >= baseAddr） | 非模块地址被过滤 | 外部函数无法解析，属预期 |

---

## 5. 本次未精读范围

无。两个模块 hpp + cpp 已全文精读。
