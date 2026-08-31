# UMT MCP 通信协议（设备端 wire protocol）

> **定位**：设备端（C++ / nlohmann/json）与 PC 侧（Python / json）之间**唯一的对齐事实**。
> 两端任一改动必须先改本文档。MVP 阶段只实现 §6 中标注 **[MVP]** 的 3 个命令。
>
> 上游依据：《开发必读架构 v1.2》§2.1（协议细节与安全修正）、《MCP工具功能规格》附录（编码约定/错误分层）。

---

## 1. 传输层

| 项 | 规定 |
|---|---|
| 传输 | TCP |
| 监听地址 | **设备端 `bind("127.0.0.1", 35515)`** —— 🔴 **绝不 `0.0.0.0`** |
| 暴露方式 | 仅经 `adb forward tcp:35515 tcp:35515` |
| PC 侧连接 | `connect("127.0.0.1", 35515)` —— **不连设备局域网 IP** |
| 编码 | UTF-8 |
| 帧格式 | **NDJSON（换行分隔 JSON）**：每条消息一个 JSON 对象，以 `\n` 结尾 |

🔴 **安全理由**：UMT 以 root 运行、提供内存读写接口。bind `0.0.0.0` 等于局域网任意设备可读写目标游戏内存。

> 不采用源项目的 `OK <len>\n<body>` 文本协议——那套是手写解析，是 v1.0 安全/兼容问题的根因之一（架构 §5.0）。

---

## 2. 连接生命周期

```
设备端 listen(127.0.0.1:35515)
   ↓
PC 侧 connect
   ↓
设备端主动发 HELLO（协议/构建/能力）
   ↓
PC 侧校验 protocol   ──不匹配──> 明确报错并 close()
   ↓ 通过
直接请求-响应（无需认证，严格串行，一问一答）
   ↓
断开 → PC 侧指数退避重连（1/2/4…≤30s）→ 重连后重新校验 HELLO
```

🔴 **重连后所有 `sessionId` 立即失效**（扫描候选集、类索引、job 全部作废，架构 v1.2 issue #5）。

---

## 3. 消息格式

### 3.1 HELLO（设备端 → PC，连接建立后立即发送）

```json
{"type":"hello","protocol":1,"build":"1.0.0","capabilities":["PING","LIST_PROCESSES","GET_LOGS"]}
```

> 设备端不要求 Token，也不存在 AUTH 帧。PC 侧确认 `protocol` 一致后可直接发送请求。

| 字段 | 含义 |
|---|---|
| `protocol` | 协议版本，当前 `1`。**不匹配 PC 侧必须明确报错并断开**，不让后续命令一路失败 |
| `build` | UMT 构建版本，供 `getCapabilities` 暴露与排障 |
| `capabilities` | 设备端当前支持的命令列表（ptrace 不可用时不含 F 组命令） |

### 3.2 请求（PC → 设备端）

```json
{"id":1,"cmd":"MEMORY_READ","args":{"address":"0x7a8b9c0000","size":256}}
```

| 字段 | 说明 |
|---|---|
| `id` | 请求序号，**单调递增**。v1 协议是严格串行，id 用于校验响应配对（乱序即协议错误） |
| `cmd` | 原生命令名，**大写下划线**（与 MCP 工具名 camelCase 是两层，不混） |
| `args` | 命令参数对象，缺省为空对象 `{}` |

### 3.3 响应成功

```json
{"id":1,"ok":true,"data":{"address":"0x7a8b9c0000","size":256,"hex":"7f454c46..."}}
```

### 3.4 响应失败

```json
{"id":1,"ok":false,"error":{"code":"E_READ_FAILED","msg":"vm_rpm_ptr failed at 0x...","detail":{}}}
```

### 3.5 长任务（未完成，返回 job）

```json
{"id":1,"ok":true,"data":{"jobId":"j-123","status":"running","progress":0.42,"etaSeconds":18,"suggestedWaitMs":15000}}
```

🔴 **轮询响应必须带 `progress` + `etaSeconds` + `suggestedWaitMs`**（v1.2 issue #2/#13），AI 按建议等待，避免盲目短轮询白烧 token。

### 3.6 长轮询（长任务接近完成时，设备端可 hold 连接）

设备端在 `waitMs` 窗口内**可不立即返回**，hold 到任务完成或 `waitMs` 上限。把 N 次空轮询压成 1 次带结果的返回。

### 3.7 日志推送（设备端 → PC，可选）

```json
{"type":"log","level":"E","message":"[Bootstrap] 通用方式搜索 GUObject 失败"}
```

用于 MCP `logging` capability 透传；同时保留 `GET_LOGS` 工具做兜底。

### 3.8 心跳帧（设备端 → PC，🔴 防卡死必需）

```json
{"type":"heartbeat","ts":1785312000,"busy":true,"activeJobId":"j-123"}
```

| 字段 | 含义 |
|---|---|
| `ts` | 设备端时间戳（秒） |
| `busy` | 是否正在执行重活 |
| `activeJobId` | 当前活跃 job（无则省略） |

**发送规则**：设备端每 `HEARTBEAT_INTERVAL`（默认 **2 秒**）发一帧，**无论是否有请求在处理**。

**PC 侧判活**：
- 超过 `HEARTBEAT_TIMEOUT`（默认 **10 秒**，即 5 个周期）未收到心跳 → 判定设备端**进程假死**，
  断开并重连；此时若重活在跑，重连后该 job 作废（`E_CANCELLED` 语义）

🔴 **心跳存在的意义**：让 PC 侧能区分「**在算**（心跳正常，busy=true）」与「**死了**（无心跳）」。
没有心跳，AI 只能盲等，最终触发重试风暴。

### 3.9 单命令硬超时（🔴 防卡死必需）

- 设备端对**每个命令**设执行时长上限 `CMD_TIMEOUT`（默认 **120 秒**，重活可放宽）
- 超时：中断执行（若支持取消点）→ 回 `E_TIMEOUT` + 当前 progress
- **绝不允许命令无限执行** —— 这是"工具调用卡死"的根因

---

## 4. 编码约定

沿用《MCP工具功能规格》附录：

1. **地址一律用字符串 `"0x..."`**，不用 number —— 避免精度丢失与进制歧义
2. **字符串参数 UTF-8 hex 传输**（防引号/空格/中文/分隔符冲突）—— 例如 `anchorNames` 传 hex
3. **类型化值用 `struct` 小端编码**（`readMemoryValue` 的返回值）

---

## 5. 错误码表

| 错误码 | 含义 | PC 侧行为 |
|---|---|---|
| `E_PROTOCOL_MISMATCH` | HELLO 的 protocol 与 PC 侧不匹配 | 断开，明确报错，不重试 |
| `E_BAD_JSON` | JSON 解析失败 | 协议错误 |
| `E_UNKNOWN_CMD` | 未知命令名 | 协议错误 |
| `E_BAD_ARGS` | 参数 schema 校验失败 | 协议错误 |
| `E_NOT_ATTACHED` | 会话内命令但未 attach | 执行失败 |
| `E_READ_FAILED` | 读内存失败 | 执行失败 |
| `E_WRITE_FAILED` | 写内存失败（含映射不可写被拒） | 执行失败 |
| `E_PROBE_FAILED` | 探测失败（附 `UEVarsInitStatus` 字符串） | 执行失败 |
| `E_CANCELLED` | 任务被 `cancelJob` 取消 | 执行失败 |
| `E_TIMEOUT` | 命令执行超过设备端硬超时上限 | 执行失败（**PC 侧视为"设备端可能异常"，触发心跳探活**） |
| `E_INTERNAL` | 设备端内部异常 | 执行失败 |
| `E_NOT_READY` | 前置条件未满足（如 probe 未完成、进程已切换需重 probe） | 执行失败 |
| `E_NOT_FOUND` | 符号 / 资源 / 进程未找到 | 执行失败 |
| `E_DECODE_FAILED` | 指令解码失败（如 ADRL 解码无目标地址） | 执行失败 |
| `E_PTRACE_FAILED` | ptrace attach/detach 失败，或远程调用异常（F 组）。含 ptrace 不可用 / 权限不足 / 目标进程已退出 / `mmap` 返回 `MAP_FAILED` | 执行失败 |
| `E_MAP_STALE` | 调用绑定的 maps revision 已变化 | 执行失败，重新 `listModules` / 扫描 |
| `E_SESSION_STALE` | 候选或搜索 session 已过期、进程已切换 | 执行失败，重新生成候选 |

### 错误分层（架构 §2.1 修正 2）

| 类型 | 返回方式 |
|---|---|
| **协议层错误**（`E_BAD_JSON` / `E_UNKNOWN_CMD` / `E_BAD_ARGS`） | PC 侧转为 **JSON-RPC `error`** |
| **执行层失败**（`E_READ_FAILED` / `E_PROBE_FAILED` / …） | PC 侧转为 **`isError: true` 的 tool result** |

两者绝不可混为一谈，否则客户端无法区分"我调错了"和"设备端失败了"。

---

## 6. 命令表（MCP 工具 → 原生命令）

### A 连接与能力

| MCP 工具 | 原生命令 | 级别 | MVP |
|---|---|---|---|
| `ping` | `PING` | 快 | **[MVP]** |
| `getCapabilities` | `GET_CAPABILITIES` | 快 | **[MVP]** |

### B 进程

| MCP 工具 | 原生命令 | 级别 | MVP |
|---|---|---|---|
| `listProcesses` | `LIST_PROCESSES` | 快 | **[MVP]** |
| `selectProcess` | `SELECT_PROCESS` | 重 | |

### C 流程与产出

| MCP 工具 | 原生命令 | 级别 |
|---|---|---|
| `startProbe` | `START_PROBE` | 重 |
| `getProbeStatus` | `GET_PROBE_STATUS` | 快 |
| `getProbeResults` | `GET_PROBE_RESULTS` | 快 |
| `startDump` | `START_DUMP` | 重 |
| `getDumpStatus` | `GET_DUMP_STATUS` | 快 |
| `dumpUnrealLibrary` | `DUMP_UNREAL_LIBRARY` | 重 |
| `listOutputFiles` | `LIST_OUTPUT_FILES` | 快 |
| `readOutputFile` | `READ_OUTPUT_FILE` | 快 |
| `getLogs` | `GET_LOGS` | 快 | **[MVP]** |
| `cancelJob` | `CANCEL_JOB` | 快 |
| `applyProbeOverrides` | `APPLY_PROBE_OVERRIDES` | 快 |

### D 内存原语

| MCP 工具 | 原生命令 | 级别 |
|---|---|---|
| `readMemory` | `MEMORY_READ` | 快（size≤4096） |
| `writeMemory` | `MEMORY_WRITE` | 重（需校验映射可写） |
| `readMemoryValue` | `MEMORY_READ_VALUE` | 快 |
| `scanPattern` | `SCAN_PATTERN` | 重 |
| `searchMemory` | `SEARCH_MEMORY` | 重 |
| `findReferences` | `FIND_REFERENCES` | 重 |
| `listModules` | `LIST_MODULES` | 快 |
| `resolveSymbol` | `RESOLVE_SYMBOL` | 快（符号查找 O(1)） |
| `readString` | `READ_STRING` | 快 |

### E 理解层

| MCP 工具 | 原生命令 | 级别 |
|---|---|---|
| `disassemble` | **（PC 侧，无设备端命令）** | PC 侧 capstone |
| `decodeAdrl` | `DECODE_ADRL` | 快 |

### F 远程调用（ptrace）

| MCP 工具 | 原生命令 | 级别 |
|---|---|---|
| `callRemoteFunctionBatch` | `CALL_REMOTE_FUNCTION_BATCH` | 重（**主推，无状态原子**） |
| `beginAttachSession` | `BEGIN_ATTACH_SESSION` | 重（需四重兜底） |
| `endAttachSession` | `END_ATTACH_SESSION` | 快 |
| `callRemoteFunction` | `CALL_REMOTE_FUNCTION` | 重 |
| `allocScratch` | `ALLOC_SCRATCH` | 重 |

### G 引擎语义

| MCP 工具 | 原生命令 | 级别 |
|---|---|---|
| `detectUEVersion` | `DETECT_UE_VERSION` | 快 |
| `scanGNames` | `SCAN_GNAMES` | **重**（分钟级） |
| `sampleGNames` | `SAMPLE_GNAMES` | 快 |
| `scanObjects` | `SCAN_OBJECTS` | **重**（分钟级） |
| `sampleObjects` | `SAMPLE_OBJECTS` | 快 |
| `searchClasses` | `SEARCH_CLASSES` | 重 |
| `describeClass` | `DESCRIBE_CLASS` | 快 |
| `inspectObject` | `INSPECT_OBJECT` | 快 |

### H 高层用例

| MCP 工具 | 原生命令 | 级别 |
|---|---|---|
| `locateEngineGlobals` | `LOCATE_ENGINE_GLOBALS` | 重 |
| `dumpSDK` | `DUMP_SDK` | 重 |
| `analyzeClass` | `ANALYZE_CLASS` | 重 |

### I 复合操作

| MCP 工具 | 原生命令 | 级别 |
|---|---|---|
| `followPointerChain` | `FOLLOW_POINTER_CHAIN` | 快 |
| `scanCandidates` | `SCAN_CANDIDATES` | 重 |

---

## 7. 命令分级（🔴 架构 v1.2 issue #8）

**快命令**（在命令线程**直接执行**，不进 `gWorkerThread`）：
`PING` / `GET_CAPABILITIES` / `GET_LOGS` / `GET_PROBE_STATUS` / `GET_DUMP_STATUS` / `LIST_PROCESSES` / `MEMORY_READ`(size≤4096) / `LIST_OUTPUT_FILES` / `RESOLVE_SYMBOL` / `SAMPLE_*` / `DESCRIBE_CLASS` / `INSPECT_OBJECT` / `DECODE_ADRL` / `FOLLOW_POINTER_CHAIN` / `CANCEL_JOB` / `END_ATTACH_SESSION` / `READ_*`

**重活**（投递 `gWorkerThread`）：
`SCAN_GNAMES` / `SCAN_OBJECTS` / `SCAN_PATTERN` / `SEARCH_MEMORY` / `FIND_REFERENCES` / `SCAN_CANDIDATES` / `START_PROBE` / `START_DUMP` / `DUMP_SDK` / `DUMP_UNREAL_LIBRARY` / `LOCATE_ENGINE_GLOBALS` / `ANALYZE_CLASS` / `SEARCH_CLASSES` / `SELECT_PROCESS` / `MEMORY_WRITE` / F 组（除 `END_ATTACH_SESSION`）

🔴 **不分级会怎样**：UMT 是单 worker 线程，分钟级扫描期间 `PING`/`GET_LOGS` 全部排队 → AI 误判掉线 → 触发重试风暴。

**两类命令以 `DumpUiState` mutex 保护的数据为界**，避免竞态（架构 §4.1）。

---

## 8. 参数约定

| 参数 | 默认 | 上限 | 依据 |
|---|---|---|---|
| `waitMs` | 5000 | 60000 | 短等优先，超时才返 jobId |
| `size`（`MEMORY_READ`） | — | **4096** | 单响应 ≤4K token 硬约束 |
| `maxLines`（`GET_LOGS`） | 50 | 1500 | 设备端环形缓冲上限 |
| `maxCandidates` | 50 | 2000 | 单响应 ≤4K token |
| `brief` | false | — | 🔴 **响应分级**（见 §8.2），省 token |
| `cursor` | 空 | — | 分页游标，续读上一页 |

### 8.1 Attach-only 诊断与地址语义

`LIST_MODULES`、`SCAN_PATTERN`、`SEARCH_MEMORY`、`FIND_REFERENCES`、
`RESOLVE_SYMBOL`、`DUMP_UNREAL_LIBRARY`、候选模式的 `SCAN_GNAMES` /
`SCAN_OBJECTS` 只要求 `SELECT_PROCESS + ATTACH`，不得检查 Probe success。

全局变量响应统一区分：`slotAddress` 是模块内槽位，`valueAddress` 是槽位内容，
`moduleOffset` 相对模块基址，`indirection` 是解引用层数。搜索和候选响应必须携带
`mapRevision`；候选 session 与 `APPLY_PROBE_OVERRIDES` 的 revision 不一致时返回
`E_MAP_STALE`，进程或 session 不一致时返回 `E_SESSION_STALE`。

`processStartTime` 接受非负整数或十进制字符串；响应统一允许字符串表示，避免 64 位值
经过 JSON 客户端时丢失精度。大范围扫描传 `async:true` 后立即返回 `jobId`，完成结果在
`GET_DUMP_STATUS.jobs` 最新对应项的 `result` 中。设备端同一时间只运行一个 Probe、Dump
或分析 job；并发启动、切换进程、重新 attach、注入 override 均返回 `E_NOT_READY`。

### 8.2 常量（超时与心跳）

| 常量 | 默认 | 含义 |
|---|---|---|
| `HEARTBEAT_INTERVAL` | 2s | 设备端心跳间隔 |
| `HEARTBEAT_TIMEOUT` | 10s | PC 侧判活阈值（约 5 个周期） |
| `CMD_TIMEOUT` | 120s | 🔴 单命令硬超时上限，绝不无限执行 |
| `RECONNECT_BASE/MAX` | 1s / 30s | 指数退避重连 |

### 8.3 响应分级（`brief` 参数）—— 🔴 省 token 关键

**问题**：同一份数据，AI 有时只要"有几个、大致是什么"，有时要逐条细节。
全量返回 = 每次都烧满 token。

**方案**：列表类命令统一支持 `brief`：

| 模式 | 返回内容 | 典型体积 |
|---|---|---|
| `brief:true` | 只给**统计 + 前 N 条摘要**（如候选数、前 3 条地址与命中数） | 极小（<200 token） |
| `brief:false`（默认） | 完整列表（仍受 §8 上限约束 + 分页） | 中等 |

**AI 用法**：先 `brief:true` 看全貌 → 判断有价值 → 再用 `cursor` 分页取细节。
把"一次吃全"变成"先看后取"。

### 8.4 数据通道：大文件绕开协议 🔴

**NDJSON 不适合传大文件**——hex 编码膨胀 2 倍，JSON 解析开销大，且会撑爆响应上限。

| 数据类型 | 通道 | 机制 |
|---|---|---|
| 命令 / 扫描结果 / 内存读 / 日志 | **控制通道**（NDJSON over socket） | 分页 + `brief` + 上限 |
| **产物文件（`SDK_Classes.hpp` 等几十 MB）** | **数据通道（`adb pull`）** | 🔴 **不走 socket** |

**数据通道流程**：
1. 设备端 dump 产物本就落盘在 `/sdcard/UnrealMemoryTools/<pkg>/`
2. PC 侧用 **`adb pull`** 直拉到本地缓存目录
3. MCP 工具通过 `listOutputFiles` / `searchClasses` / `describeClass` **服务端过滤**读取，
   只回 AI 要的那一个类的定义

🔴 **铁律**：**绝不把几十 MB 的 SDK 文件塞进协议**，也**绝不整文件读进上下文**
（架构 §2.9 已定：大文件一律走服务端过滤）。

### 8.5 流式分块（分块响应的帧格式）

当单个响应确实超过一帧容量时（如 `readOutputFile` 大块文本），分多帧返回：

```json
{"id":1,"ok":true,"seq":0,"total":3,"data":{"chunk":"..."}}
{"id":1,"ok":true,"seq":1,"total":3,"data":{"chunk":"..."}}
{"id":1,"ok":true,"seq":2,"total":3,"data":{"chunk":"..."}}
```

- 同一 `id`，`seq` 从 0 递增（幂等：可按 seq 重传单帧）
- `total` 为总帧数，最后一帧 `seq == total-1`
- PC 侧按 `id` 重组后再交给工具层

---

## 9. 严格串行（🔴 单对单前提下的最优设计）

- **设备协议是"发一收一"**，不是全双工
- **本项目单对单（一个客户端 ↔ 一个服务端，只有一个手机调试），并发数恒为 1**，
  因此严格串行是**最优解而非折中**——为不存在的并发加 `reqId`/连接池属过度设计
- PC 侧 `bridge` 用 `RLock` 串行化所有设备调用
- MCP 允许并行 tool calls，但 PC 侧排队顺序执行（正确，延迟可忽略——
  真正的耗时在设备端扫描，不在协议层）
- 若将来真的出现多客户端（当前明确不存在），再加 `reqId` 做请求-响应匹配

---

## 10. 契约样例（tests/contract/）

两端测试共用的黄金样例（MVP 三个命令各一）：

```jsonc
// ping.request
{"id":1,"cmd":"PING","args":{}}
// ping.response
{"id":1,"ok":true,"data":{"build":"1.0.0","protocol":1,"connected":true}}

// list_processes.request
{"id":2,"cmd":"LIST_PROCESSES","args":{"dedicatedOnly":false}}
// list_processes.response
{"id":2,"ok":true,"data":{"processes":[{"pid":8350,"package":"com.tencent.letsgo","profileName":"自动识别 (UE4/UE5 通用)","dedicated":false}]}}

// get_logs.request
{"id":3,"cmd":"GET_LOGS","args":{"sinceIndex":0,"maxLines":50}}
// get_logs.response
{"id":3,"ok":true,"data":{"lines":[{"index":0,"level":"E","timestamp":"","message":"[Bootstrap] 通用方式搜索 GUObject 失败"}],"totalLines":1}}

// hello（收到并确认 protocol 后即可直接发送上面的请求）
{"type":"hello","protocol":1,"build":"1.0.0","capabilities":["PING","LIST_PROCESSES","GET_LOGS"]}
```
