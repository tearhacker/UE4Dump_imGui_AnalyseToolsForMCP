# UMT MCP Server (PC-side)

PC 侧 MCP 服务端：把 Android/C++ 设备端（UMT，Unreal Memory Tools）的 43 条原生命令
通过 **43 个 MCP 工具**暴露给 AI agent，实现 UE4/UE5 内存读取、引擎探测、SDK 转储与
ptrace 远程调用。

## 当前实现状态

| 项目 | 当前状态 |
|---|---|
| 服务版本 | `0.1.0` |
| wire protocol | `1` |
| MCP 工具 | `43` 个，`tools.self_check()` 返回空问题列表 |
| MCP 资源 | `4` 个 |
| 默认地址 | `127.0.0.1:35515` |
| 认证 | 无 Token、无 AUTH；HELLO 版本校验后直接调用 |
| ADB | 启动预热、连接前检查、失败重试时强制刷新 forward |
| 自动化测试 | `51 passed`（2026-08-31） |

- 基于官方 Python MCP SDK（`mcp` 包，FastMCP），stdio 传输
- 设备通信：TCP + NDJSON；MCP 自动建立并维护 `adb forward tcp:35515 tcp:35515` 隧道
- 协议：HELLO → 直接命令调用（无需认证）→ 心跳 → 严格串行一问一答
- 工具名对外为 **camelCase**（如 `readMemory`），设备端命令为 **UPPER_SNAKE**
  （如 `MEMORY_READ`），由 `tools.py` 自动映射，启动时 `self_check()` 校验

## 架构分层

```
AI (MCP client)
   │  camelCase 工具名，snake_case 参数
   ▼
server.py               设置 adb/port、预热 forward、自检、挂载 FastMCP
src/umt_mcp/adb.py      ADB 检测 / 幂等 forward / 失败强制刷新
src/umt_mcp/tools.py    工具 → 设备端命令名/参数键映射 + self_check()
src/umt_mcp/bridge.py   连接前自动 forward / HELLO / 心跳 / 重连 / 串行化
src/umt_mcp/protocol.py 帧常量、错误码、协议层/执行层错误分层
   ▼  TCP + NDJSON（127.0.0.1:35515）
设备端 CommandServer (C++, 只监听回环) → 43 条命令 → UMT 引擎语义层
```

## 前置条件

1. Python `>=3.10`，且运行解释器已安装本项目依赖。
2. Android Platform Tools 可用；当前配置使用 `C:/Program Files/platform-tools/adb.exe`。
3. 手机已连接并授权 USB 调试，设备端 UMT 正在运行。
4. 当前自动选择依赖 ADB 的默认设备；同时连接多台设备时应只保留目标设备在线。

无需人工执行 `adb forward`。MCP 启动、首次连接和断线重连都会自动建立或刷新隧道。
设备端不要求 Token；PC 侧收到 `HELLO` 并确认协议版本一致后即可直接调用命令。

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
    adb.py                # adb 检测 + 自动端口转发 + 隧道自愈（线程安全、幂等）
    disassemble.py        # PC 侧 capstone 封装（可选）
    config.py             # 连接/超时/体积/安全常量
  tests/
    test_adb.py           # 自动转发：新建/复用/强制刷新/无设备
    test_bridge.py        # mock 设备端：自动转发/握手/心跳/重连/错误分层
    test_protocol.py      # 帧编解码/错误分层/常量对齐（全过）
```

## Install

```powershell
Set-Location "D:\泪心安卓领域基本盘技术\ue4ImGuiAutoWorkingMcpBytear\mcp_server"

# 安装到客户端 JSON 当前指定的解释器
& "C:\Program Files\Python310\python.exe" -m pip install -e .

# 开发/测试环境已经存在时可直接使用
& ".\.venv\Scripts\python.exe" -m pip install -e .
```

## Run (stdio)

```powershell
& "C:\Program Files\Python310\python.exe" .\server.py `
    --port 35515 `
    --adb "C:\Program Files\platform-tools\adb.exe"
```

`stdout` 专用于 MCP stdio 协议，运行日志只写 `stderr`。启动顺序是：解析参数 → 设置 ADB
路径和端口 → 尝试预热自动 forward → 执行 `tools.self_check()` → 启动 FastMCP。ADB 预热失败
不会终止 MCP；首次工具调用会继续自动重试。

`tools.self_check()` 校验工具推导出的命令真实存在、下发参数键与设备端一致、设备端命令
无漏暴露、工具总数不超过 45。自检不通过时服务直接退出并打印问题清单。

## 客户端配置

仓库内当前配置文件：[`客户端AI工具配置MCP文件.json`](./客户端AI工具配置MCP文件.json)

```json
{
  "mcpServers": {
    "ue4dump-mcp": {
      "command": "C:/Program Files/Python310/python.exe",
      "args": [
        "D:/泪心安卓领域基本盘技术/ue4ImGuiAutoWorkingMcpBytear/mcp_server/server.py",
        "--port",
        "35515",
        "--adb",
        "C:/Program Files/platform-tools/adb.exe"
      ]
    }
  }
}
```

配置中不需要 `env` 或 `UMT_TOKEN`。代码或配置更新后重启 MCP 客户端，使其重新拉起
`server.py` 进程。

## 自动连接时序

1. `server.py` 启动时调用 `adb.setup(35515)`，自动启动 ADB daemon、检查在线设备并预热转发。
2. 生产 bridge 第一次创建时读取运行期 `config.DEFAULT_PORT`，不使用导入时缓存端口。
3. 每次 socket 连接前再次调用 `adb.setup(port, force=False)`；已有精确转发时直接复用。
4. socket 首次失败后，下一次尝试使用 `force=True` 重新下发 forward，修复陈旧或丢失隧道。
5. 收到设备端 `HELLO` 后只校验 protocol 版本，随后直接发送命令，不发送认证帧。
6. USB 晚插入、ADB 重启或转发表丢失时，后续 MCP 调用重复上述流程，无需人工补命令。

Windows 下 ADB 子进程使用 `CREATE_NO_WINDOW`，自动连接过程不会弹出命令窗口。

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
| `umt://process` | bridge 连接、build、能力集与心跳摘要；选中进程仍由设备端维护 |

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
| 连接/超时 | 断线 / 假死 / 等待超时 | bridge 自动刷新 adb forward 并重试 |

## Test

```powershell
& ".\.venv\Scripts\python.exe" -m pytest tests -q
```

最新实测输出：

```text
...................................................                      [100%]
51 passed in 5.63s
```

- `test_adb.py`：自动转发新建、精确复用、强制刷新、相似端口排除、无在线设备。
- `test_bridge.py`：连接前 forward、重连刷新、运行期端口、无 Token、HELLO、心跳、错误分层、mock 工具调用。
- `test_protocol.py`：NDJSON 编解码、1 MB 帧上限、错误映射和配置常量。
- `tools.self_check()`：43 个工具与 43 条设备命令一一覆盖，当前结果为 `[]`。

本次验证环境中 Python `3.10.5`、ADB `1.0.41` 可用；执行验证时没有在线 Android 设备，
因此真机 forward 和设备命令调用仍需在手机连接、授权且 UMT 运行时完成。

## Known limitations

1. **严格串行**：设备协议"发一收一"，所有工具在 RLock 下排队；长调用阻塞后续所有工具。
2. **重活冻结**：`scanGnames` / `scanObjects` / `scanPattern` / `searchClasses` /
   `locateEngineGlobals` / `analyzeClass` 在主线程同步执行，期间设备端 UI 可能冻结
   （设备端心跳由 socket 线程发，PC 侧不会误判假死）。
3. `cancelJob` 无 jobId；`allocScratch` 无释放命令；`endAttachSession` 未透出 sessionId。
4. `umt://process` 只反映 bridge 状态，不保存设备端当前选中进程的 package/pid。
5. 自动 forward 尚未提供 `--serial` 参数；多台 ADB 设备同时在线时 ADB 会拒绝默认目标选择。
6. 真机连接取决于手机已连接、USB 调试已授权且设备端 UMT 正在运行。
