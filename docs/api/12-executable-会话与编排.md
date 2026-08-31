# L0 会话层 API · executable（进程枚举 / 状态机 / 编排 / 主循环 / MCP 命令注册）

> **源码**：`src/executable.cpp`（**3596 行**）
> **定位**：整个工具的会话层与编排层，**兼作 MCP 命令的注册中心**。
> **已精读范围**：`:1-1195`（匿名 namespace：全局配置 / 数据结构 / 状态机 / 编排函数）+ `:1202-2722`（`SetupMcpCommands`，43 条命令）+ `:3513-3596`（main 全文）。
> **未精读**：`:2728-3512`（`RenderAutoUEDumpPanel` 及全部 ImGui UI 渲染，MCP 不依赖）。
>
> ⚠️ **行号引用约定**（2026-08-31 踩坑后确立）：
> - §1–§4（编排层，`:1-1195`）行号稳定，可放心引用。
> - §5（命令注册区 `:1202+`）与 main 区 **变动极频繁**——该文件每增删一条命令就整体位移。
>   本节行号仅供当前快照参考，**引用具体命令一律用命令名**：
>   `grep -n 'Register("XXX"' src/executable.cpp`。
>   本次核对过程中，仅因补充一段注释就造成过两次位移（+4、+3）。

---

## 0. 全局约定

1. **全局配置**（`:67-68`）：`kOutputDirectory = "/sdcard/UnrealMemoryTools"`、`kMaxLogLines = 1500`。
2. **8 个内置 profile**（`UE_Games` `:52-61`）：ArenaBreakout / DeltaForce / Farlight / Shuisha / Valorant / NRC / PUBGMHD / PUBG，**全部已 include 且已注册**。
   ⚠️ **文件名 `Sfps2.hpp` 里装的类叫 `ShuishaProfile`**（`Sfps2.hpp:6`）——源码存在**文件名与类名不一致**，审查时别误判成"未注册"。
   真正未接入的是 **`Lineage2.hpp`**（`Lineage2Profile`，既未 include 也未被 `UE_Games` 注册）。
3. **全部业务在匿名 namespace**（`:50` 起）→ MCP 命令以 **handler 内联注册**的方式调用，未做外迁（见 §7）。
4. **单 worker 线程模型**（`gWorkerThread` `:154`）：重活投递 worker，快速状态在主线程读 `gDumpUiState`（mutex 保护）。

---

## 1. 核心数据结构（全部匿名 namespace）

| 结构 | 字段 | 行号 |
|---|---|---|
| `AutoProcessCandidate` | `pid` / `package` / `profileName` / `dedicated` | `:78-84` |
| `ProbeOffsetEntry` | `name` / `value` / `relative` / `found` | `:86-92` |
| `StructFieldEntry` | `name` / `type` / `offset` / `found` / `description` | `:94-101` |
| `StructGroup` | `name` / `fields[]` | `:103-107` |
| `ProbeResult` | `valid`/`success`/`pid`/`package`/`profileName`/`dedicated`/`baseAddress`/`offsets[]`/`structGroups[]`/`profile`/`autoProfileOwner` | `:109-122` |
| `DumpUiState` | `mutex` + `logLines[]` + `phase` + `activePackage` + `resultPath` + `lastError` + 三套 running/finished/success（probe/dump/soDump）+ `objectsPercent`/`dumpPercent` + `probeOffsets`/`probeStructGroups` | `:124-149` |

**全局单例**（`:151-159`）：`gDumpUiState` / `gCandidates` / `gSelectedIndex` / `gWorkerThread` / `gProbeResult` / `gCancelRequested` / `gOverrideMutex` / `gProbeOverrides`。

### 1.1 `JobRegistry`（`:161-208`）

轻量长任务账本：`JobEntry{jobId,type,createdAt,running,progress,lastError}` + `genJobId()` / `StartJob(type)` / `FinishJob(id,ok,err)` / `UpdateJobProgress(id,pct)`，`gJobMutex` 保护。

⚠️ **当前只有 `DUMP_SDK`（`:2397`）用了 jobId**。`START_PROBE` / `START_DUMP` / `DUMP_UNREAL_LIBRARY` 仍是"发起即返回 + 轮询 `GET_PROBE_STATUS`/`GET_DUMP_STATUS`"模式，**未接入 jobId**。接入时直接调 `StartJob`/`FinishJob` 即可。

---

## 2. 进程枚举（→ `LIST_PROCESSES`）

### `FindAutoProcessCandidates()`　`:256`

两条来源：
1. **profile 匹配**：遍历 `UE_Games`（`:52-61`），`GetPIDsOf(appId)` → `dedicated=true`
2. **/proc 扫描**：遍历数字目录，`HasUnrealLib(pid)`（maps 含 `libUE4.so`/`libUnreal.so`）→ `dedicated=false`，profileName = "自动识别 (UE4/UE5 通用)"

**排序**：`dedicated` 优先，再按包名字典序。**去重**靠 `unordered_map<package, candidate>`。

### `HasUnrealLib(pid_t pid)`　`:244`

`getAllMaps(pid)` 找 maps 含 `libUE4.so` / `libUnreal.so`。

### `RefreshCandidates()`　`:435`

重新枚举 + 保持原选中包名（`gSelectedIndex` 复位后按 package 找回）；**进程列表变化时调 `InvalidateProbeReuse`**（`:442` 附近）。

---

## 3. 日志与状态机（→ `GET_LOGS` / `GET_PROBE_STATUS` / `GET_DUMP_STATUS`）

### 日志环形缓冲

`PushUiLog(char, const std::string&)` `:317`；`LoggerSink(char, const char*)` `:329`。
`Logger::SetSink(LoggerSink)`（main `:3512`）→ 所有 `LOGI/LOGE` 进 `gDumpUiState.logLines`，**上限 1500 行，超出删最旧**。格式 `<level>: <msg>`。

### 状态机函数

| 函数 | 作用 | 行号 |
|---|---|---|
| `SetDumpPhase(phase)` | 设当前阶段 | `:334` |
| `SetProgress(objects, dump)` | 设进度（-1 不覆盖） | `:340` |
| `BeginProbeState(candidate)` | 探针开始（清 log/offset/struct） | `:349` |
| `FinishProbeState(success, offsets, groups, err)` | 探针结束 | `:392` |
| `InvalidateProbeReuse(reason)` | 跨进程作废旧探针 | `:372` |
| `BeginDumpState(package)` / `FinishDumpState(...)` | 转储状态 | `:405` / `:419` |

**`GET_PROBE_STATUS.phase/percent` 的数据源就是 `DumpUiState.phase` / `objectsPercent`**；`GET_PROBE_RESULTS.offsets/structs` 就是 `probeOffsets` / `probeStructGroups`。

---

## 4. 编排函数（→ `START_PROBE` / `START_DUMP` / `DUMP_UNREAL_LIBRARY`）

### `ExecuteProbe(const AutoProcessCandidate)`　`:734`

```
BeginProbeState
→ kMgr.initialize(pid, EK_MEM_OP_SYSCALL) 失败回退 EK_MEM_OP_IO
→ 匹配专用 profile
→ probeDumper.Init(matchedProfile)，失败回退 AutoFixProfile
→ CollectProbeOffsets + CollectStructGroups
→ FinishProbeState(true, ...)
```

⚠️ **profile 三级回退**：专用 profile → AutoFixProfile 兜底 → 都失败 `ERROR_PROBE_INIT_FAILED`。

### `ExecuteDump(const AutoProcessCandidate)`　`:872`

```
BeginDumpState
→ 校验 gProbeResult.valid/success/profile（缺 → ERROR_NO_PROBE_RESULT）
→ 校验 package+pid 一致（否则 ERROR_PROBE_MISMATCH）
→ delete_directory + mkdir_recursive
→ uEDumper.Init(profile, reuse=true)
→ Dump → SaveDumpBuffers → FinishDumpState(true, dumpGameDir, ...)
```

⚠️ **`delete_directory` 先清空输出目录**——`DUMP_SDK` 会清掉上次产物。

### `ExecuteDumpUnrealLib(const AutoProcessCandidate)`　`:984`

独立 soDump 状态机（`soDumpRunning/Finished/Success/Path`），`kMgr.dumpMemELF(ue_elf, path)` 转储 `libUE4.so`/`libUnreal.so`。

### 编排辅助

| 函数 | 行号 |
|---|---|
| `SaveDumpBuffers` | `:478` |
| `CollectProbeOffsets` | `:505` |
| `CollectStructGroups` | `:539` |

### UI 触发器（投递 worker）

`StartProbeSelected` `:1070` / `StartDumpAfterProbe` `:1082` / `StartDumpUnrealLib` `:1099`——**都是 `join()` 旧线程再 `std::thread(...)` 起新线程**。MCP 命令走各自的 handler，不经过这三个。

---

## 5. MCP 命令服务（**已落地**）

> 本节取代旧版"MCP 化的两处接入点（建议）"。两个接入点均已实现，不再是待办。

### 5.1 注册中心 `SetupMcpCommands()`　`:1202`

注册 **43 条**命令到 `UmtMcp::CommandDispatcher::Register(cmd, handler, isFast)`，函数体至 `:2722`。
命令层源码在 `src/mcp/`：`Protocol.hpp` / `CommandQueue.*` / `CommandDispatcher.*` / `CommandServer.*` / `MemoryHelpers.*` / `Arm64Disasm.*` / `PtraceSession.*`。

### 5.2 main() 接入点　`:3513`

```cpp
int main() {
    setbuf 全关;
    Logger::SetSink(LoggerSink);                    // :3519
    RefreshCandidates();                            // :3520
    {   // ---- MCP 命令服务（服务端，bind 127.0.0.1:27185）----
        static UmtMcp::CommandQueue mcpQueue;       // :3524
        UmtMcp::CommandDispatcher::BindQueue(&mcpQueue);  // :3525
        SetupMcpCommands();                         // :3526
        UmtMcp::CommandServer::Start(kDefaultPort, &mcpQueue, kUEDUMPER_VERSION);  // :3528
    }
    // VULKAN 图形后端 + ANativeWindow + 渲染初始化
    while (flag) {
        drawBegin(); ... graphics->NewFrame();
        Layout_tick_UI(&flag);                      // :3580
        UmtMcp::CommandDispatcher::PollOnce();      // :3582  ← 每帧 poll 一次
        graphics->EndFrame();
    }
    UmtMcp::CommandServer::Stop();                  // :3586
    if (gWorkerThread.joinable()) gWorkerThread.join();
    ...
}
```

### 5.3 线程模型（**铁律**）

| 角色 | 线程 | 约束 |
|---|---|---|
| `CommandServer` | **独立线程** | 🔴 **严禁调用任何 UMT 能力**（`kMgr` / `UEWrappers` / `gDumpUiState` 非线程安全）。只做 socket I/O + 入队/出队 + 心跳 |
| `CommandDispatcher::PollOnce()` | **主线程**（渲染循环内 `:3582`） | 命令在此执行，**与 UI 操作天然串行，故既有代码无需加锁** |
| 重活 | `gWorkerThread` | ⚠️ **当前没有任何命令 handler 用它**，见 §5.3.1 |

### 5.3.1 🔴 `isFast` 是**死参数**——快/重分级尚未实现

2026-08-31 全量核对确认：

```cpp
// src/mcp/CommandDispatcher.cpp
:12  std::unordered_map<std::string, bool> CommandDispatcher::fastFlags_;   // 声明
:29  fastFlags_[cmd] = isFast;                                              // 只写
     // ← 全项目再无任何读取点
```

`Register(cmd, handler, isFast)` 把标志存进 `fastFlags_`，但 `PollOnce()` **从不读它**。
`PollOnce()` 对所有命令一视同仁：**取请求 → 当场同步执行 handler → 回响应**。

**结论：43 条命令全部在主线程同步执行，`isFast` 目前只是注释，改它没有任何运行时效果。**
（`SCAN_PATTERN` 标了 `false` 并附注释「扫描可能慢，标为重活」，但同样不会被投到 worker。）

#### 由此产生的真实风险（🔴 2026-08-31 二次核对修正）

耗时命令会**同步占满主线程**：

```
handler 同步跑几秒~几十秒
  → ImGui 渲染循环停 → **设备端界面冻住**（用户可见）
  → 若超过 kCommandTimeoutSec(120s)，CommandServer 直接返 E_TIMEOUT，结果丢弃
```

⚠️ **但心跳不会停，PC 侧不会误判假死。**
心跳由 `CommandServer` **线程**在 `HandleFrame` 的等待循环里发
（`CommandServer.cpp:255`），与主线程是否繁忙**无关**。
（本文件此前写过「心跳停 → PC 侧 10s 判超时」，**该说法是错的，已删除**。）

#### 哪些命令真慢 —— 必须看实现，不能照协议文档的字面描述

🔴 **重要纠正**：协议文档 §6/§7 把 `SCAN_GNAMES` / `SCAN_OBJECTS` 标为「分钟级」，
那是按"从零全网扫描"的语义写的，**与当前实现不符**。实际两者都复用
`START_PROBE` 已解析出的指针，只采样 16 条做自校验，**是毫秒级**，标 `isFast=true` 正确。

| 命令 | 实际耗时来源 | 当前 isFast |
|---|---|---|
| `SCAN_PATTERN` | 扫最多 512MB（`kMaxScan`） | `false` ✅ |
| `SEARCH_CLASSES` | `ForEachObjectOfClass` 遍历整个 GUObjectArray | `false` ✅ |
| `SCAN_CANDIDATES` | 按 `getAllMaps` 扫内存区域 | `false` ✅ |
| `LOCATE_ENGINE_GLOBALS` | 多步编排 | `false` ✅ |
| `START_PROBE` / `START_DUMP` / `DUMP_SDK` | **自己投 `gWorkerThread`**，命令立即返回 | `false` |
| `SCAN_GNAMES` / `SCAN_OBJECTS` | 复用已解析指针 + 采样 16 条 → 毫秒级 | `true` ✅ |
| `CALL_REMOTE_FUNCTION` / `BEGIN_ATTACH_SESSION` | ptrace attach，百毫秒级 | `true` ✅ |

**结论修正**：慢命令基本都已标了 `false`，分级标注本身是对的，
问题只在于**标志不被读取**（`isFast` 死参数）→ 慢命令仍会同步执行、冻住界面。
此前列的「6 条分级存疑」里，除 `SCAN_PATTERN` 外均属误判。

#### 修复路径（二选一，**不是改个标志位就完事**）

1. **在 `PollOnce()` 里实现路由**：读 `fastFlags_[cmd]`，为 `false` 时投递 `gWorkerThread`，
   主线程改为轮询完成状态并持续发心跳（当前 `CommandServer` 的 120s 硬超时是同步阻塞的，需一并改）。
2. **handler 自己投 worker**：每个重活 handler 内部投递 + 短等。改动小，但每个命令各写一遍。

> ⚠️ 无论选哪条，都要先确认 PC 侧是否已按协议文档假定重活走"jobId 轮询"路径——
> 那需要先补协议 §8 的长任务模型。

### 5.4 仍未实现（协议已定，代码未做）

- 长轮询（`waitMs` hold 连接）
- 日志推送帧 `{"type":"log",...}`（当前只有 `GET_LOGS` 兜底）
- 心跳帧的 `activeJobId` 字段
- `brief` 响应分级 / `cursor` 分页 / 流式分块
- token 在 UI 面板展示（当前只打 logcat，见 `:3523` 注释）

---

## 6. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | 单 worker 线程阻塞一切 | 扫描期间快命令排队 | 命令分级（快命令在 `PollOnce` 直执行） |
| 2 | `ExecuteDump` 依赖 `gProbeResult` 全局 | 未探针直接 dump 报错 | 工具描述明示先 `START_PROBE` |
| 3 | `Sfps2.hpp` 文件名与类名 `ShuishaProfile` 不一致 | 误判"未注册/未 include" | 以 `UE_Games:52-61` 实际 8 个为准（均已接入）；`Lineage2.hpp` 才是真未接入 |
| 4 | `delete_directory` 清空输出 | `DUMP_SDK` 清上次产物 | 工具描述明示 |
| 5 | 日志环形缓冲 1500 行 | `GET_LOGS` 超限丢最旧 | `sinceIndex` 增量拉取 |
| 6 | `isFast` 标错的重活 | 同步执行冻结渲染循环 → 心跳停 → PC 侧误判假死 | 分钟级命令（`SCAN_GNAMES`/`SCAN_OBJECTS`）必须 `isFast=false` |
| 7 | token 只打 logcat，未上 UI | 用户拿不到 token | 需补 UI 面板展示 |

---

## 7. 命令 ↔ 源码映射（**已全部落地，非外迁清单**）

> 旧版把这张表写成"待外迁清单"。实际实现路线是 **handler 内联注册**（`SetupMcpCommands` 里直接调用匿名 namespace 的函数），未新建 `SDKQuery` 层、未做外迁。下表按现状记录映射关系。

| 源码函数 | 对应 MCP 命令 | 行号 |
|---|---|---|
| `FindAutoProcessCandidates` | `LIST_PROCESSES` | `:256` |
| `RefreshCandidates` | `LIST_PROCESSES` 刷新 / `SCAN_CANDIDATES` | `:435` |
| `InvalidateProbeReuse` | `SELECT_PROCESS` 切进程 | `:372` |
| `ExecuteProbe` | `START_PROBE` | `:734` |
| `ExecuteDump` | `START_DUMP` | `:872` |
| `ExecuteDumpUnrealLib` | `DUMP_UNREAL_LIBRARY` | `:984` |
| `CollectProbeOffsets` | `GET_PROBE_RESULTS.offsets` | `:505` |
| `CollectStructGroups` | `GET_PROBE_RESULTS.structs` | `:539` |
| `SaveDumpBuffers` | 落盘 | `:478` |
| `DumpUiState` 全套 | `GET_PROBE_STATUS` / `GET_DUMP_STATUS` | `:124-149` |
| `PushUiLog` / `LoggerSink` | `GET_LOGS` | `:317` / `:329` |

**注意**：这些函数依赖 `gProbeResult` / `gDumpUiState` / `gWorkerThread` 等**文件级全局**。因命令统一在主线程 `PollOnce()` 执行（§5.3），与 UI 天然串行，故**无需改造为可注入状态对象**——这是"内联注册"路线相比"外迁"路线省下的代价。

---

## 8. 本次未精读范围

| 范围 | 内容 |
|---|---|
| `:2728-3512` | `RenderAutoUEDumpPanel` 及全部 ImGui UI 渲染（含 `Layout_tick_UI` 内部的 UI 逻辑） |

MCP 不依赖 UI 渲染。§1–§4 已覆盖关键编排，§5 覆盖命令注册与线程模型。
