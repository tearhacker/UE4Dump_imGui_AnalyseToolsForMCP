# L1/L7 底层库 API · KittyMemoryEx（读写 / 扫描 / 校验 / ptrace）

> **源码**：`KittyMemoryEx/*.{hpp,cpp}`（约 7000 行，含 zip.cpp 2190）
> **定位**：第三方底层库（上游 AndUEDumper 同源）。MCP 的 D 组原语 / F 组 ptrace / `resolveSymbol` / `writeMemory` 校验的直接底座。
> **已精读范围**：**用到的 5 个头文件全文**（KittyMemoryMgr.hpp 133 / KittyScanner.hpp 327 / KittyPtrValidator.hpp 211 / KittyTrace.hpp 123 / KittyMemOp.hpp 51）。cpp 实现未逐行（第三方库，接口契约已足够，`findSymbol`/`callFunctionFrom` 是零调用接线点）。

---

## 0. 全局约定

1. **双内存后端**（`KittyMemOp.hpp:6-11`）：`EK_MEM_OP_SYSCALL`（`process_vm_readv/writev`）vs `EK_MEM_OP_IO`（`/proc/pid/mem`）。`initialize` 可指定，SYSCALL 失败可回退 IO（`executable.cpp:655-656`）。
2. **`ElfScanner::isValid()` 要求符号表非空**（`KittyScanner.hpp:170-173`，7 字段全非零含 `_symbolTable`）——**strip 掉的 ELF 会 `isValid()==false`**。这是"符号优先策略"失效的根本原因（架构文档 §2.5 已预警）。
3. **两处「存在但 src/ 零调用」**（MCP 化最划算的接线点）：
   - `ElfScanner::findSymbol`（`KittyScanner.hpp:211`）→ `resolveSymbol` 工具
   - `KittyTraceMgr::callFunctionFrom`（`KittyTrace.cpp:140`）→ `callRemoteFunctionBatch` 工具

---

## 1. `KittyMemoryMgr`（入口单例，`KittyMemoryMgr.hpp:27-134`）

| 接口 | 签名 | 作用 |
|---|---|---|
| 初始化 | `bool initialize(pid_t, EKittyMemOP, bool initMemPatch)` | 选后端 + 是否初始化补丁/备份 |
| 读内存 | `size_t readMem(uintptr_t, void *, size_t)` | **返回实际读到的字节数**（≠len 即失败） |
| 写内存 | `size_t writeMem(uintptr_t, void *, size_t)` | 返回实际写入字节数 |
| 读字符串 | `std::string readMemStr(uintptr_t, size_t maxLen)` | |
| 写字符串 | `bool writeMemStr(uintptr_t, std::string)` | |
| 校验 ELF | `bool isValidELF(uintptr_t elfBase)` | |
| 找内存 ELF | `ElfScanner findMemElf(const std::string &elfName)` | 按名找 libUE4.so/libUnreal.so |
| 找 zip 内 ELF | `ElfScanner findMemElfInZip(zip, elfName)` | 拆分包 |
| linker solist | `ElfScanner findMemElfFromLinker(elfName)` | 去 header 场景 |
| 转储 ELF | `bool dumpMemELF(const ElfScanner &, const std::string &)` | `dumpUnrealLibrary` 底座 |
| 转储范围 | `bool dumpMemRange(start, end, path)` | |
| 子管理器 | `memPatch` / `memBackup` / `memScanner` / `elfScanner` / `trace` / `linkerScanner` | 成员（`:38-47`） |

⚠️ **`readMem`/`writeMem` 返回"实际字节数"**，不是 bool——MCP 封装 `readMemory`/`writeMemory` 时要用 `== len` 判断成功（`UEMemory::vm_rpm_ptr` 已这么封装）。

---

## 2. `KittyScannerMgr` + `ElfScanner`（`KittyScanner.hpp`）

### `KittyScannerMgr`（`:11-113`）—— pattern 扫描

`findBytesAll/First`、`findHexAll/First`、**`findIdaPatternAll/First`**、`findDataAll/First`。**全部返回地址（First 返回 uintptr_t，All 返回 vector）**。

### `ElfScanner`（`:134-223`）—— 符号定位

| 接口 | 作用 | 行号 |
|---|---|---|
| `isValid()` | 7 字段全非零（含符号表） | `:170-173` |
| `base()` / `end()` / `segments()` / `bssSegments()` | ELF 基址/段 | `:177-218` |
| `symbols()` | 动态符号（DT_SYMTAB）→ `unordered_map<name, addr>` | `:206` |
| `dsymbols()` | 调试符号（SHT_SYMTAB 磁盘） | `:209` |
| **`findSymbol(name)`** | 🔧 **零调用**，符号优先策略的底座 | `:211` |
| `findDebugSymbol(name)` | 调试符号查找 | `:212` |

⚠️ **符号查找语义**：`findSymbol` 走动态符号表（`DT_SYMTAB`），`findDebugSymbol` 走磁盘调试段（`SHT_SYMTAB`）。`resolveSymbol` 工具应两个都试。

---

## 3. `KittyPtrValidator`（`KittyPtrValidator.hpp`）—— 写内存校验

| 接口 | 作用 |
|---|---|
| `setPID(pid)` | 切进程（清缓存 + 换 pid） |
| `setUseCache(bool)` | 关缓存时清 regions |
| `refreshRegionCache()` | 重读 /proc/pid/maps |
| `isPtrReadable(ptr, len=8)` | 映射区间含可读 |
| **`isPtrWritable(ptr, len=8)`** | **`writeMemory` 写前校验（架构 §2.8）** |
| `isPtrExecutable(ptr, len=8)` | |
| `isPtrInAddressSpace(ptr)` | |

⚠️ **默认 `len = sizeof(void*)`（8 字节）**：`isPtrReadable(addr)` 只校验 8 字节。MCP 的 `readMemory(address, size)` 传大 size 时，必须用带 `len` 参数的重载校验**整个区间**，否则越界读会崩。

⚠️ **双平台实现**：`__APPLE__`（mach）vs Android（`/proc/pid/maps`）。本项目 Android 分支 `setPID` 才可用（`:157-162`）。

---

## 4. `KittyTraceMgr`（`KittyTrace.hpp`）—— ptrace 远程调用

| 接口 | 作用 | 行号 |
|---|---|---|
| `Attach()` / `Detach()` / `Cont()` | PTRACE_ATTACH/DETACH/CONT | `:61-71` |
| `isAttached()` | 检查 TracerPid | `:53-56` |
| `getRegs()` / `setRegs()` | 寄存器读写 | `:84-89` |
| **`callFunctionFrom(caller, funcAddr, nargs, ...)`** | 🔧 **零引用**，远程调用核心 | `:114` |
| `callFunction<Args...>(funcAddr, nargs, ...)` | 转发到 callFunctionFrom（用 defaultCaller） | `:119-123` |

⚠️ **`callFunctionFrom` 参数是 `callerAddress`（伪造返回地址）+ `functionAddress` + `nargs` + 可变参数**。MCP 的 `callRemoteFunctionBatch` 要封装成：attach → N 次 callFunctionFrom → detach（架构 §2.6 无状态原子操作）。

⚠️ **返回陷阱无法区分"陷阱触发"与"函数崩了"**（架构 §F 组已预警）：`callFunctionFrom` 靠 SIGSEGV/SIGILL 判断返回，MCP 侧必须校验返回值。

---

## 5. `IKittyMemOp`（`KittyMemOp.hpp`）—— 双后端抽象

| 类 | 后端 | 备注 |
|---|---|---|
| `IKittyMemOp` | 抽象基类 | `init(pid)` / `Read` / `Write` / `ReadStr` / `WriteStr` |
| `KittyMemSys` | `process_vm_readv/writev` | 首选（快） |
| `KittyMemIO` | `/proc/pid/mem` | 回退（兼容性好） |

---

## 6. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | `findSymbol`/`callFunctionFrom` 零调用 | 能力闲置 | 接线（最划算） |
| 2 | `ElfScanner::isValid` 要求符号表非空 | strip ELF 判无效 | 符号失败回退 pattern |
| 3 | `readMem` 返回实际字节数非 bool | 误判成功 | 用 `== len` |
| 4 | `isPtrReadable` 默认只查 8 字节 | 大 size 越界读 | 带 len 校验整个区间 |
| 5 | `isPtrWritable` 是 writeMemory 前置 | 不校验直接写崩游戏 | 写前强制校验 |
| 6 | 双后端 SYSCALL/IO | 后端差异 | initialize 失败回退 |
| 7 | ptrace 返回陷阱难区分 | 误判调用结果 | 校验返回值 |

---

## 7. 本次未精读范围

| 范围 | 内容 |
|---|---|
| 各 `.cpp` 实现 | `KittyScanner.cpp`(1022)/`KittyTrace.cpp`(346)/`KittyPtrValidator.cpp`(269)/`KittyMemOp.cpp`(256)/`KittyMemoryMgr.cpp`(339)/`KittyMemoryEx.cpp`(286) |
| `zip.cpp`(2190) / `Deps/` | zip 解包 + 依赖 |

这些是第三方库实现（上游同源），接口契约已从 hpp 完整提取。`findSymbol`/`callFunctionFrom` 接线时，若需确认内部实现细节（如符号解析算法、ptrace 寄存器布局），再读对应 cpp。
