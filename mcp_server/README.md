# UMT MCP Server (PC-side)

PC 侧 MCP 服务端：把 Android/C++ 设备端（UMT，Unreal Memory Tools）的 43 条原生命令
通过 **43 个 MCP 工具**暴露给 AI agent，实现 UE4/UE5 内存读取、引擎探测、SDK 转储与
ptrace 远程调用。

- 基于官方 Python MCP SDK（`mcp` 包，FastMCP），stdio 传输
- 设备通信：TCP + NDJSON，`adb forward tcp:35515 tcp:35515` 隧道
- 协议：HELLO → 直接命令调用（无需认证）→ 心跳 → 严格串行一问一答
- 工具名对外为 **camelCase**（如 `readMemory`），设备端命令为 **UPPER_SNAKE**
  （如 `MEMORY_READ`），由 `tools.py` 自动映射，启动时 `self_check()` 校验

## 架构分层

```
AI (MCP client)
   │  camelCase 工具名，snake_case 参数
   ▼
server.py               FastMCP 挂载 43 个工具 + 4 个资源 + instructions
src/umt_mcp/tools.py    工具 → 设备端命令名/参数键映射 + self_check()
src/umt_mcp/bridge.py   socket 桥接：连接 / 握手 / 分帧 / 心跳判活 / 重连 / 串行化
src/umt_mcp/protocol.py 帧常量、错误码、协议层/执行层错误分层
   ▼  TCP + NDJSON（127.0.0.1:35515）
设备端 CommandServer (C++, 只监听回环) → 43 条命令 → UMT 引擎语义层
```

## 前置条件

1. 手机 adb 连接：`adb devices`
2. 端口转发（设备端只监听回环）：`adb forward tcp:35515 tcp:35515`

设备端不要求 Token。PC 侧收到 `HELLO` 并确认协议版本一致后即可直接调用命令。

## Layout

```
mcp_server/
  server.py               # FastMCP 入口：工具挂载 + 资源路由 + 启动自检
  pyproject.toml          # 项目元数据 + 依赖 + pytest 配置
  src/umt_mcp/
    protocol.py           # wire protocol：帧常量 / 错误码全表 / 错误分层
    bridge.py             # 唯一碰 socket 的地方：握手/心跳/重连/RLock 串行
    tools.py              # 43 个 MCP 工具 + DEVICE_PARAMS 参数契约 + self_check()
    instructions.py       # Server Instructions（工作流/铁律/排障顺序）
    resources.py          # 4 个资源：umt://protocol / capabilities / config / process
    adb.py                # adb forward 前置检查（幂等）
    disassemble.py        # PC 侧 capstone 封装（可选）
    config.py             # 连接/超时/体积/安全常量
  tests/
    test_bridge.py        # mock 设备端：握手/心跳/错误分层（14 个 socket 用例暂 skip）
    test_protocol.py      # 帧编解码/错误分层/常量对齐（全过）
```

## Install

```powershell
# one-shot: create venv and install
& "C:\Users\52334\.workbuddy\binaries\python\versions\3.13.12\python.exe" `
    -m venv .venv
.\.venv\Scripts\python.exe -m pip install -e .
```

## Run (stdio)

```powershell
.\.venv\Scripts\python.exe -m server
# or
.\.venv\Scripts\python.exe server.py
```

启动时先执行 `tools.self_check()`：校验工具推导出的命令真实存在、下发参数键与设备端
一致、设备端命令无漏暴露、工具总数 ≤ 45。自检不通过直接退出并打印问题清单。

## MCP 可用能力（43 个工具）

工具按 9 组分类；详细参数、默认值、示例见
[`docs/MCP可用工具文档.md`](../docs/MCP可用工具文档.md)。

### A 连接与能力

| 工具 | 设备命令 | 说明 |
|---|---|---|
| `ping` | `PING` | 连通性检查，返回 build / 协议版本 |
| `getCapabilities` | `GET_CAPABILITIES` | 列出设备端支持的命令与 ptrace 可用性 |

### B 进程

| 工具 | 设备命令 | 说明 |
|---|---|---|
| `listProcesses` | `LIST_PROCESSES` | 枚举 UE 进程（可选 dedicatedOnly） |
| `selectProcess` | `SELECT_PROCESS` | 选中目标进程（切进程后旧探针失效） |
| `attach` | `ATTACH` | 建立内存访问上下文（必须先 selectProcess） |

### C 流程与产出

| 工具 | 设备命令 | 说明 |
|---|---|---|
| `startProbe` | `START_PROBE` | 启动引擎探测（几乎所有引擎语义命令的前置） |
| `getProbeStatus` | `GET_PROBE_STATUS` | 探测进度轮询 |
| `getProbeResults` | `GET_PROBE_RESULTS` | 取探测偏移表与结构体布局 |
| `startDump` | `START_DUMP` | 启动 SDK 转储（先清空输出目录） |
| `getDumpStatus` | `GET_DUMP_STATUS` | 转储进度轮询 |
| `dumpSdk` | `DUMP_SDK` | 一键 probe + dump，返回 jobId |
| `dumpUnrealLibrary` | `DUMP_UNREAL_LIBRARY` | 单独转储 libUE4.so 本体 |
| `getLogs` | `GET_LOGS` | 拉取设备端日志（环形缓冲 1500 行） |
| `listOutputFiles` | `LIST_OUTPUT_FILES` | 列出产物文件（大文件走 adb pull） |
| `readOutputFile` | `READ_OUTPUT_FILE` | 读小文件内容 |
| `cancelJob` | `CANCEL_JOB` | 取消当前重活（无 jobId 参数） |
| `applyProbeOverrides` | `APPLY_PROBE_OVERRIDES` | 手动指定偏移覆盖探测（startProbe 前） |

### D 内存原语

| 工具 | 设备命令 | 说明 |
|---|---|---|
| `readMemory` | `MEMORY_READ` | 读内存返回 hex（size 上限 4096） |
| `readMemoryValue` | `MEMORY_READ_VALUE` | 按类型读值（bool/i8…/f64/ptr64） |
| `readString` | `READ_STRING` | 读字符串（wide=true 按 UTF-16/FString） |
| `writeMemory` | `WRITE_MEMORY` | 写内存 🔴 需 confirm_dangerous=true |
| `scanPattern` | `SCAN_PATTERN` | IDA 风格 pattern 扫描（重活） |
| `listModules` | `LIST_MODULES` | 列出已加载模块 |
| `resolveSymbol` | `RESOLVE_SYMBOL` | 符号名解析地址（strip 过的 ELF 会失败） |

### E 理解层

| 工具 | 设备命令 | 说明 |
|---|---|---|
| `decodeAdrl` | `DECODE_ADRL` | 解码 ADRP/ADR+LDR 指令序列 |
| `disassemble` | `DISASSEMBLE` | ARM64 反汇编（设备端自研解码器） |

### G 引擎语义

| 工具 | 设备命令 | 说明 |
|---|---|---|
| `detectUeVersion` | `DETECT_UE_VERSION` | 探测 UE4/UE5 版本 |
| `sampleGnames` | `SAMPLE_GNAMES` | 抽样读 GNames（快，优先） |
| `scanGnames` | `SCAN_GNAMES` | 全量扫 GNames 🔴 重活，可能冻结设备端 UI |
| `sampleObjects` | `SAMPLE_OBJECTS` | 抽样读 GUObjectArray（快，优先） |
| `scanObjects` | `SCAN_OBJECTS` | 全量扫 GUObjectArray 🔴 重活 |
| `searchClasses` | `SEARCH_CLASSES` | 按名字模糊搜 UClass |
| `describeClass` | `DESCRIBE_CLASS` | 取 UClass 完整定义（递归父类字段） |
| `inspectObject` | `INSPECT_OBJECT` | 读 UObject 实例字段值（容器只回摘要） |

### H 高层用例

| 工具 | 设备命令 | 说明 |
|---|---|---|
| `locateEngineGlobals` | `LOCATE_ENGINE_GLOBALS` | 一站式定位 GWorld/GEngine/GNames/GUObjectArray |
| `analyzeClass` | `ANALYZE_CLASS` | 深度分析单个类 🔴 重活 |

### I 复合操作

| 工具 | 设备命令 | 说明 |
|---|---|---|
| `followPointerChain` | `FOLLOW_POINTER_CHAIN` | 偏移链逐级解引用（负数=数组下标） |
| `scanCandidates` | `SCAN_CANDIDATES` | 扫描可分析候选（默认 50 条） |

### F 远程调用（ptrace）🔴

| 工具 | 设备命令 | 说明 |
|---|---|---|
| `beginAttachSession` | `BEGIN_ATTACH_SESSION` | 开启 ptrace 会话（有状态，单会话） |
| `endAttachSession` | `END_ATTACH_SESSION` | 结束会话（用完必须调用） |
| `callRemoteFunction` | `CALL_REMOTE_FUNCTION` | 会话内远程调用 🔴 需 confirm |
| `callRemoteFunctionBatch` | `CALL_REMOTE_FUNCTION_BATCH` | 无状态批量远程调用（主推）🔴 |
| `allocScratch` | `ALLOC_SCRATCH` | 目标进程内 mmap 缓冲（无释放命令）🔴 |

## MCP 资源

| 资源 | 内容 |
|---|---|
| `umt://protocol` | 协议要点速查（传输/帧/握手/心跳/错误分层/高频坑） |
| `umt://capabilities` | 当前设备端能力清单 |
| `umt://config` | PC 侧配置摘要（不含 token） |
| `umt://process` | 目标进程信息（当前为占位实现） |

## 通用约定

- **地址**：一律 `"0x..."` 字符串，不要用数字。
- **参数名**：MCP 侧 snake_case（如 `confirm_dangerous`），下发设备端自动转 camelCase。
- **危险操作安全门**：`writeMemory` / `callRemoteFunction` / `callRemoteFunctionBatch` /
  `allocScratch` 必须显式 `confirm_dangerous=true`，否则直接拒绝。
- **响应体积**：单工具响应 ≤ 4K token；大文件一律 `adb pull`，禁止整读。
- **超时**：命令硬超时 120s；重活用 `getProbeStatus` / `getDumpStatus` 轮询，不要长超时硬等。

## 错误分层

| 层级 | 含义 | 处理 |
|---|---|---|
| 协议层（`E_PROTOCOL_MISMATCH` / `E_UNKNOWN_CMD` / `E_BAD_ARGS`） | 调用方式错误 | 修改调用方式或升级其中一端 |
| 执行层（`E_READ_FAILED` / `E_PROBE_FAILED` / `E_NOT_READY` / `E_TIMEOUT` / `E_PTRACE_FAILED`） | 设备端执行失败 | 排障或换策略 |
| 连接/超时 | 断线 / 假死 / 等待超时 | 检查 adb forward、重试 |

## Test

```powershell
.\.venv\Scripts\python.exe -m pytest tests/ -v
```

当前：`42 passed`。`test_bridge.py` 的真实 socket 传输层用例已全部启用，覆盖无 Token
直连、协议版本校验、心跳判活、错误分层与端到端工具调用。

## Known limitations

1. **严格串行**：设备协议"发一收一"，所有工具在 RLock 下排队；长调用阻塞后续所有工具。
2. **重活冻结**：`scanGnames` / `scanObjects` / `scanPattern` / `searchClasses` /
   `locateEngineGlobals` / `analyzeClass` 在主线程同步执行，期间设备端 UI 可能冻结
   （设备端心跳由 socket 线程发，PC 侧不会误判假死）。
3. `cancelJob` 无 jobId；`allocScratch` 无释放命令；`endAttachSession` 未透出 sessionId。
4. 传输层 socket 测试尚跳过；真机端到端验证未跑。
