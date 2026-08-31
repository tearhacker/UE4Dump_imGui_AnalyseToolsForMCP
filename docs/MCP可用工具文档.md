# UMT MCP 可用工具文档

> 本文档覆盖本项目 MCP 服务端（`mcp_server`）当前**已注册并可调用**的全部 43 个 MCP 工具，
> 与设备端（`Alltear_UnrealMemoryToolsNewMcp`）实际注册的 43 条命令一一对应。
> 内容以 `mcp_server/src/umt_mcp/tools.py` 与设备端 `src/executable.cpp` 的真实实现为准。

- 文档版本：1.0（2026-08-31）
- 协议版本：1（`config.PROTOCOL_VERSION`）
- 服务端：Python `FastMCP`（stdio），工具名对外为 **camelCase**
- 设备端：Android/C++ 命令服务，`127.0.0.1:35515`，经 `adb forward` 暴露

---

## 1. 架构与命名

```
AI (MCP client)
   │  camelCase 工具名，snake_case 参数
   ▼
mcp_server (Python, stdio)
   │  TCP + NDJSON + HELLO/AUTH + 心跳
   ▼
设备端 CommandServer (C++, 127.0.0.1:35515)
   │  UPPER_SNAKE 命令
   ▼
UMT 引擎语义层（GNames / GUObjectArray / SDK Dump / ptrace）
```

- **工具名**：MCP 工具名是 camelCase（如 `readMemory`），设备端命令是 UPPER_SNAKE（如 `MEMORY_READ`），两层不同名，由 PC 侧自动映射。
- **参数名**：MCP 参数保持 Python 侧 snake_case（如 `confirm_dangerous`），下发到设备端时自动转为设备端读取的 camelCase 键（如 `confirmDangerous`）。
- **启动自检**：服务启动时执行 `self_check()`，校验命令名存在、参数键无拼写错误、设备端命令无漏暴露、工具总数 ≤ 45。自检不通过直接退出。

---

## 2. 前置条件

```bash
# 1. 手机 adb 连接
adb devices

# 2. 端口转发（设备端只监听回环）
adb forward tcp:35515 tcp:35515

# 3. 取设备端一次性 token（UMT logcat 输出，设备端每次启动随机生成）
export UMT_TOKEN=<logcat 中的 token>

# 4. 启动 MCP 服务
python server.py
```

- token 只从环境变量 `UMT_TOKEN` 读取，不进代码、不进仓库。
- 设备端重启后 token 会变，必须重新取。
- 连接失败的错误信息会提示 adb forward 排障步骤。

---

## 3. 通用约定

### 3.1 地址格式

- 一律使用 `"0x..."` 字符串，**不要用数字**。
- 示例：`readMemory(address="0x7a3b4000", size=64)`

### 3.2 valueType 枚举（readMemoryValue）

`bool` / `i8` / `u8` / `i16` / `u16` / `i32` / `u32` / `i64` / `u64` / `f32` / `f64` / `ptr32` / `ptr64`

### 3.3 远程调用参数语法（F 组）

args 中每个元素都是**字符串**：
- `"0x..."` → 按地址处理
- `"i:123"` → 按立即数处理
- 最多 8 个参数（aarch64 寄存器上限）

`returnKind` 枚举：`ptr` / `i32` / `i64` / `f32` / `void`

### 3.4 confirmDangerous 安全门

以下危险操作默认拒绝，必须显式传 `confirm_dangerous=true`：

| 工具 | 风险 |
|---|---|
| `writeMemory` | 写目标进程内存 |
| `callRemoteFunction` / `callRemoteFunctionBatch` | 在目标进程内执行代码 |
| `allocScratch` | 在目标进程内 mmap 分配内存 |

### 3.5 参数缺省

- Python 侧为 `None` 的参数一律**不下发**，由设备端使用自己的默认值。
- 响应统一为格式化 JSON 字符串（`json.dumps(indent=2)`）。

### 3.6 体积约束

- 单工具响应 ≤ 4K token（硬约束）。
- `readMemory` size 上限 **4096**。
- `getLogs` 默认 50 行。
- 大文件（几十 MB 的 SDK 产物）一律走 `adb pull`，**禁止**用 `readOutputFile` 整读进上下文。

---

## 4. 工具总表（43 个）

| 分组 | MCP 工具名 | 设备端命令 | 一句话说明 |
|---|---|---|---|
| A 连接与能力 | `ping` | `PING` | 连通性检查 |
| A 连接与能力 | `getCapabilities` | `GET_CAPABILITIES` | 列出设备端支持的命令与 ptrace 可用性 |
| B 进程 | `listProcesses` | `LIST_PROCESSES` | 枚举 UE 进程 |
| B 进程 | `selectProcess` | `SELECT_PROCESS` | 选中目标进程 |
| B 进程 | `attach` | `ATTACH` | 建立内存访问上下文 |
| C 流程与产出 | `startProbe` | `START_PROBE` | 启动引擎探测（前置） |
| C 流程与产出 | `getProbeStatus` | `GET_PROBE_STATUS` | 查询探测进度 |
| C 流程与产出 | `getProbeResults` | `GET_PROBE_RESULTS` | 取探测偏移表与结构体布局 |
| C 流程与产出 | `startDump` | `START_DUMP` | 启动 SDK 转储 |
| C 流程与产出 | `getDumpStatus` | `GET_DUMP_STATUS` | 查询转储进度 |
| C 流程与产出 | `dumpSdk` | `DUMP_SDK` | 一键 probe + dump（返回 jobId） |
| C 流程与产出 | `dumpUnrealLibrary` | `DUMP_UNREAL_LIBRARY` | 单独转储 libUE4.so 本体 |
| C 流程与产出 | `getLogs` | `GET_LOGS` | 拉取设备端日志（环形缓冲） |
| C 流程与产出 | `listOutputFiles` | `LIST_OUTPUT_FILES` | 列出产物文件 |
| C 流程与产出 | `readOutputFile` | `READ_OUTPUT_FILE` | 读小文件内容 |
| C 流程与产出 | `cancelJob` | `CANCEL_JOB` | 取消当前重活 |
| C 流程与产出 | `applyProbeOverrides` | `APPLY_PROBE_OVERRIDES` | 手动指定偏移覆盖探测 |
| D 内存原语 | `readMemory` | `MEMORY_READ` | 读内存返回 hex |
| D 内存原语 | `readMemoryValue` | `MEMORY_READ_VALUE` | 按类型读一个值 |
| D 内存原语 | `readString` | `READ_STRING` | 读字符串（可 UTF-16） |
| D 内存原语 | `writeMemory` | `WRITE_MEMORY` | 写内存 🔴 |
| D 内存原语 | `scanPattern` | `SCAN_PATTERN` | IDA 风格 pattern 扫描 |
| D 内存原语 | `listModules` | `LIST_MODULES` | 列出已加载模块 |
| D 内存原语 | `resolveSymbol` | `RESOLVE_SYMBOL` | 符号名解析地址 |
| E 理解层 | `decodeAdrl` | `DECODE_ADRL` | 解码 ADRP/ADR+LDR 序列 |
| E 理解层 | `disassemble` | `DISASSEMBLE` | ARM64 反汇编 |
| G 引擎语义 | `detectUeVersion` | `DETECT_UE_VERSION` | 探测 UE4/UE5 版本 |
| G 引擎语义 | `sampleGnames` | `SAMPLE_GNAMES` | 抽样读 GNames（快） |
| G 引擎语义 | `scanGnames` | `SCAN_GNAMES` | 全量扫 GNames（重） |
| G 引擎语义 | `sampleObjects` | `SAMPLE_OBJECTS` | 抽样读 GUObjectArray（快） |
| G 引擎语义 | `scanObjects` | `SCAN_OBJECTS` | 全量扫 GUObjectArray（重） |
| G 引擎语义 | `searchClasses` | `SEARCH_CLASSES` | 按名字模糊搜 UClass |
| G 引擎语义 | `describeClass` | `DESCRIBE_CLASS` | 取 UClass 完整定义 |
| G 引擎语义 | `inspectObject` | `INSPECT_OBJECT` | 读 UObject 实例字段值 |
| H 高层用例 | `locateEngineGlobals` | `LOCATE_ENGINE_GLOBALS` | 一站式定位引擎全局变量 |
| H 高层用例 | `analyzeClass` | `ANALYZE_CLASS` | 深度分析单个类 |
| I 复合操作 | `followPointerChain` | `FOLLOW_POINTER_CHAIN` | 偏移链逐级解引用 |
| I 复合操作 | `scanCandidates` | `SCAN_CANDIDATES` | 扫描可分析候选 |
| F 远程调用 | `beginAttachSession` | `BEGIN_ATTACH_SESSION` | 开启 ptrace 会话 |
| F 远程调用 | `endAttachSession` | `END_ATTACH_SESSION` | 结束 ptrace 会话 |
| F 远程调用 | `callRemoteFunction` | `CALL_REMOTE_FUNCTION` | 会话内远程调用 |
| F 远程调用 | `callRemoteFunctionBatch` | `CALL_REMOTE_FUNCTION_BATCH` | 无状态批量远程调用 |
| F 远程调用 | `allocScratch` | `ALLOC_SCRATCH` | 目标进程内分配缓冲 |

---

## 5. 工具详解

### 5.1 A 组：连接与能力

#### ping
```json
{ "tool": "ping" }
```
连通性检查。返回设备端 `build` / 协议版本 / 连接状态。排障第一步。

#### getCapabilities
```json
{ "tool": "getCapabilities" }
```
列出设备端当前支持的全部命令，以及 ptrace 是否可用。F 组（远程调用）在 ptrace 不可用时不会出现在返回清单中。

---

### 5.2 B 组：进程管理

#### listProcesses
```json
{
  "tool": "listProcesses",
  "dedicatedOnly": false
}
```
- `dedicatedOnly`（可选 bool）：`true` 只返回已适配的专属游戏（profile 精确匹配）；默认 `false` 额外包含 /proc 扫描到的通用 UE4/UE5 进程。

#### selectProcess
```json
{ "tool": "selectProcess", "package": "com.tencent.letsgo", "pid": 12345 }
```
- `package`（必填 str）：包名。
- `pid`（可选 int）：pid；不传则按包名解析。
- 🔴 切进程会让旧探针结果立即失效，之后必须重新 `startProbe`。

#### attach
```json
{ "tool": "attach" }
```
对当前已选中的进程建立内存访问上下文。
- 🔴 **不接受 pid** —— 设备端从 `SELECT_PROCESS` 的选中项取目标，**必须先 selectProcess**。

---

### 5.3 C 组：流程与产出

#### startProbe
```json
{ "tool": "startProbe" }
```
启动引擎探测（定位 GNames / GUObjectArray / 偏移表）。几乎所有引擎语义命令的前置条件。
- 🔴 必须先 `selectProcess`。
- 🔴 本命令立即返回 `started`，不支持 waitMs 短等，需用 `getProbeStatus` 轮询。

#### getProbeStatus
```json
{ "tool": "getProbeStatus" }
```
查询探测进度（phase / percent / 是否完成）。`startProbe` 之后轮询。

#### getProbeResults
```json
{ "tool": "getProbeResults" }
```
取探测得到的偏移表与结构体布局。需探测完成后调用。

#### startDump
```json
{ "tool": "startDump" }
```
启动 SDK 转储。需先完成 `startProbe`。
- 🔴 会先清空设备端输出目录（`/sdcard/UnrealMemoryTools/<pkg>/`）。

#### getDumpStatus
```json
{ "tool": "getDumpStatus" }
```
查询转储进度。`startDump` / `dumpSdk` 之后轮询。

#### dumpSdk
```json
{ "tool": "dumpSdk", "waitMs": 5000 }
```
- `waitMs`（可选 int）：短等上限。
一键跑完 probe + dump 并产出完整 SDK。返回 `jobId`，可用 `getDumpStatus` 或 `cancelJob` 跟踪/取消。

#### dumpUnrealLibrary
```json
{ "tool": "dumpUnrealLibrary" }
```
单独转储 `libUE4.so` / `libUnreal.so` 本体。抓不到符号时用它拿原始 so，再在 PC 侧离线分析。

#### getLogs
```json
{ "tool": "getLogs", "sinceIndex": 120, "maxLines": 50 }
```
- `sinceIndex`（可选 int）：增量拉取游标，避免重复读。
- `maxLines`（可选 int）：默认 50。
设备端日志为环形缓冲，上限 1500 行，超出丢最旧。

#### listOutputFiles
```json
{ "tool": "listOutputFiles", "package": "com.tencent.letsgo" }
```
- `package`（可选 str）：指定包名。
列出设备端 `/sdcard/UnrealMemoryTools` 下已产出的文件。🔴 大文件（`SDK_Classes.hpp` 等）用 `adb pull` 拉，不要走 `readOutputFile` 整读。

#### readOutputFile
```json
{ "tool": "readOutputFile", "filename": "mcp_analysis.json", "package": "com.tencent.letsgo" }
```
- `filename`（必填 str）。
- `package`（可选 str）。
🔴 只用于小文件；几十 MB 的 SDK 文件一律走 `adb pull`。

#### cancelJob
```json
{ "tool": "cancelJob" }
```
取消当前正在跑的重活。
- 🔴 设备端不接受 jobId —— 只能取消**当前那一个**任务。

#### applyProbeOverrides
```json
{
  "tool": "applyProbeOverrides",
  "overrides": { "GNames": "0x7a3b4000" }
}
```
- `overrides`（必填 dict[str, str]）：key 为偏移名，value 为 `"0x..."` 字符串。
手动指定偏移地址覆盖自动探测结果。🔴 必须在 `startProbe` 之前调用。

---

### 5.4 D 组：内存原语

#### readMemory
```json
{ "tool": "readMemory", "address": "0x7a3b4000", "size": 64 }
```
- `address`（必填 str，`"0x..."`）。
- `size`（必填 int，上限 **4096**）。
读目标进程内存，返回 hex。

#### readMemoryValue
```json
{ "tool": "readMemoryValue", "address": "0x7a3b4000", "valueType": "u32" }
```
- `valueType`（必填 str）：见 §3.2 枚举。
按类型读一个内存值。

#### readString
```json
{ "tool": "readString", "address": "0x7a3b4000", "maxLength": 256, "wide": true }
```
- `maxLength`（可选 int）。
- `wide`（可选 bool）：`true` 按 UTF-16 读（FString）。
读内存中的字符串。

#### writeMemory
```json
{
  "tool": "writeMemory",
  "address": "0x7a3b4000",
  "hexBytes": "9090",
  "confirm_dangerous": true
}
```
- `hexBytes`（必填 str，hex 字节串）。
- `confirm_dangerous`（必填 true）🔴。
写目标进程内存。写前建议先用 `scanPattern` / `disassemble` 确认地址，并用 `listModules` 确认区间可写。写错会崩游戏。

#### scanPattern
```json
{
  "tool": "scanPattern",
  "pattern": "FF DD ? 99 CC ? 00",
  "module": "libUE4.so",
  "maxResults": 100
}
```
- `pattern`（必填 str，IDA 风格，`?` 为通配）。
- `module`（可选 str）：限定模块；不指定扫整个进程。
- `maxResults`（可选 int）。
🔴 耗时较长（可能扫最多 512MB）。

#### listModules
```json
{ "tool": "listModules" }
```
列出目标进程已加载的模块（基址 / 大小 / 路径）。

#### resolveSymbol
```json
{ "tool": "resolveSymbol", "symbol": "GWorld" }
```
- `symbol`（必填 str）。
先查动态符号表，失败回退调试符号。🔴 strip 过的 ELF 查不到符号，此时回退 `scanPattern`。

---

### 5.5 E 组：理解层

#### decodeAdrl
```json
{ "tool": "decodeAdrl", "address": "0x7a3b4000" }
```
解码 ARM64 ADRP/ADR+LDR 指令序列，算出它引用的绝对地址。

#### disassemble
```json
{ "tool": "disassemble", "address": "0x7a3b4000", "count": 16 }
```
- `count`（可选 int）。
反汇编指定地址处的 ARM64 指令。走设备端自研解码器（不依赖 capstone），未识别指令返回 `"???"`。

---

### 5.6 G 组：引擎语义

#### detectUeVersion
```json
{ "tool": "detectUeVersion" }
```
探测目标进程的 UE 版本（UE4.x / UE5.x）。

#### sampleGnames
```json
{ "tool": "sampleGnames", "startIndex": 0, "count": 32 }
```
- `startIndex`（可选 int）。
- `count`（可选 int）。
抽样读 GNames/FNamePool 条目。快命令，先看样本再决定要不要全扫。

#### scanGnames
```json
{ "tool": "scanGnames" }
```
全量扫描 GNames。
- 🔴 重活。当前设备端 isFast 分级未接线，执行期间可能冻结设备端 UI；PC 侧 10s 无心跳会判定假死断开。
- 能用 `sampleGnames` 解决就别跑全扫。

#### sampleObjects
```json
{ "tool": "sampleObjects", "startIndex": 0, "count": 32 }
```
抽样读 GUObjectArray 对象。快命令。

#### scanObjects
```json
{ "tool": "scanObjects" }
```
全量扫描 GUObjectArray。🔴 重活，冻结风险同 `scanGnames`，优先 `sampleObjects`。

#### searchClasses
```json
{ "tool": "searchClasses", "query": "Player", "maxResults": 50 }
```
- `query`（必填 str）：名字模糊搜索，支持子串匹配。
- `maxResults`（可选 int）。
按名字模糊搜索 UClass。

#### describeClass
```json
{ "tool": "describeClass", "className": "APlayerController" }
```
- `className`（必填 str）。
取 UClass 完整定义：字段（**递归父类**）、函数签名、Size / 对齐 / CDO。

#### inspectObject
```json
{ "tool": "inspectObject", "address": "0x7a3b4000" }
```
读 UObject 实例的运行时字段值。🔴 Array/Set/Map 只回 Count/Max/Data 摘要，不展开元素。

---

### 5.7 H 组：高层用例

#### locateEngineGlobals
```json
{ "tool": "locateEngineGlobals" }
```
一站式定位引擎全局变量（GWorld / GEngine / GNames / GUObjectArray）。多步编排，较慢。

#### analyzeClass
```json
{ "tool": "analyzeClass", "className": "APlayerController" }
```
- `className`（必填 str）。
深度分析单个类（内部会跑 probe + dump）。🔴 重活，耗时不可控，用 `getDumpStatus` 跟踪进度。

---

### 5.8 I 组：复合操作

#### followPointerChain
```json
{
  "tool": "followPointerChain",
  "baseAddress": "0x7a3b4000",
  "offsets": ["0x10", "0x28", "-1"]
}
```
- `offsets`（必填 list[str]）：偏移链，支持负数索引表示数组下标（如 `-1` 表示第 1 个元素）。
按偏移链逐级解引用：base → +off[0] → +off[1] → ...

#### scanCandidates
```json
{
  "tool": "scanCandidates",
  "region": "libUE4.so",
  "alignment": 4,
  "maxCandidates": 50
}
```
- `region`（可选 str）。
- `alignment`（可选 int）。
- `maxCandidates`（可选 int，默认 50）。
扫描并列出可分析的目标候选（进程 / 模块 / 关键地址）。

---

### 5.9 F 组：远程调用（ptrace）🔴

#### beginAttachSession
```json
{ "tool": "beginAttachSession", "maxHoldMs": 30000 }
```
- `maxHoldMs`（可选 int）。
开启 ptrace 附着会话（**有状态**）。同一目标进程只允许一个会话，重复开启会被拒。
- 推荐优先用 `callRemoteFunctionBatch`（无状态，不会泄漏）。

#### endAttachSession
```json
{ "tool": "endAttachSession" }
```
结束当前 ptrace 会话。🔴 **用完必须调用**，否则目标进程会一直被挂住。

#### callRemoteFunction
```json
{
  "tool": "callRemoteFunction",
  "sessionId": "abc123",
  "address": "0x7a3b4000",
  "args": ["0x1000", "i:1"],
  "returnKind": "ptr",
  "confirm_dangerous": true
}
```
- `sessionId`（必填 str）：来自 `beginAttachSession`。
- `address`（必填 str）。
- `args`（可选 list[str]，最多 8 个）：语法见 §3.3。
- `returnKind`（可选，默认 `ptr`）。
- `confirm_dangerous`（必填 true）🔴。
在当前 ptrace 会话内远程调用一个函数（需先 `beginAttachSession`）。

#### callRemoteFunctionBatch
```json
{
  "tool": "callRemoteFunctionBatch",
  "address": "0x7a3b4000",
  "argSets": [["i:0"], ["i:1"], ["i:2"]],
  "returnKind": "i32",
  "confirm_dangerous": true
}
```
- `address`（必填 str）。
- `argSets`（必填 list[list[str]]）：每组一次调用。
- `returnKind`（可选，默认 `ptr`）。
- `confirm_dangerous`（必填 true）🔴。
批量远程调用**同一个**函数（无状态，主推）。内部 attach → N 次调用 → detach。
- 🔴 与 attach 会话互斥：若已有活动会话，设备端会拒绝，需先 `endAttachSession`。
- 返回陷阱靠 SIGSEGV/SIGILL 判定，无法区分"陷阱正常触发"与"被调函数自己崩了"——每个结果都附 `rawHex`，必须自行校验返回值。

#### allocScratch
```json
{ "tool": "allocScratch", "size": 4096, "confirm_dangerous": true }
```
- `size`（必填 int）。
在目标进程内 mmap 一块可读写内存（供远程调用当缓冲区）。
- 🔴 **无对应的释放命令**，分配的内存随目标进程生命周期存在。

---

## 6. 典型工作流

### 6.1 标准 SDK 产出流程

```text
ping
  → listProcesses
  → selectProcess(package)
  → attach
  → startProbe → getProbeStatus(轮询至完成) → getProbeResults
  → dumpSdk(waitMs) 或 startDump → getDumpStatus(轮询)
  → listOutputFiles → adb pull 拉取大文件
```

### 6.2 类结构分析

```text
selectProcess → attach → startProbe → getProbeStatus
  → searchClasses("Player")
  → describeClass("APlayerController")
  → inspectObject(实例地址)
  → analyzeClass("APlayerController")  // 深度分析，用 getDumpStatus 跟踪
```

### 6.3 内存定位

```text
listModules → resolveSymbol("GWorld")        // 有符号时
  或 scanPattern("FF DD ? 99 CC ? 00")       // 无符号时
  → readMemory / readMemoryValue / readString
  → followPointerChain(base, offsets)        // 逐级解引用
```

### 6.4 远程调用（危险，谨慎）

```text
推荐：callRemoteFunctionBatch(address, argSets)   // 无状态，用完即 detach
备选：beginAttachSession → callRemoteFunction → endAttachSession  // 有状态，必须收尾
辅助：allocScratch(size) 分配参数缓冲区
```

---

## 7. 错误处理

错误分两层，错误信息会明确标注层级：

| 层级 | 错误码示例 | 含义 | 处理 |
|---|---|---|---|
| 协议层 `[协议层错误]` | `E_BAD_TOKEN` / `E_PROTOCOL_MISMATCH` / `E_UNKNOWN_CMD` / `E_BAD_ARGS` / `E_BAD_JSON` | 调用方式错误 | 改调用方式 / 取新 token |
| 执行层 | `E_READ_FAILED` / `E_PROBE_FAILED` / `E_NOT_READY` / `E_TIMEOUT` / `E_PTRACE_FAILED` / `E_CANCELLED` | 设备端执行失败 | 排障或换策略 |
| 连接/超时 `[连接/超时]` | — | 断线 / 假死 / 等待超时 | 检查 adb forward、重试 |

- 连接失败：错误信息包含 adb forward 排障提示。
- 假死判定：设备端超过 10s 无心跳判定进程假死，自动断开，下次调用自动重连；若有重活在跑，该 job 已作废。
- 命令硬超时 120s：超时返回 `E_TIMEOUT`。
- 等待响应超时：提示"重活在跑时需轮询 GET_*_STATUS，不要用长超时硬等"。

---

## 8. 已知限制（使用前必读）

1. **严格串行**：设备协议是"发一收一"，所有工具调用在一把 RLock 下排队，不支持并发；长调用会阻塞后续所有工具。
2. **重活冻结风险**：`scanGnames` / `scanObjects` / `scanPattern` / `searchClasses` / `locateEngineGlobals` / `analyzeClass` 在主线程同步执行，期间设备端 UI 可能冻结（设备端心跳仍会由 socket 线程发送）。
3. **`cancelJob` 无 jobId 参数**：只能取消当前任务。
4. **`allocScratch` 无释放命令**。
5. **`endAttachSession` 未暴露 sessionId 参数**：设备端支持但 PC 侧工具未透出，仅结束当前会话。
6. **响应体积硬约束 4K token**：超大结果被截断或要求分页，大文件一律 adb pull。
7. **`describeClass` 的 Array/Set/Map 字段只回摘要**，不展开元素。
8. **strip 过的 ELF 无符号**：`resolveSymbol` 会失败，回退 `scanPattern`。
