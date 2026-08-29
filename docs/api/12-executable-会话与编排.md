# L0 会话层 API · executable（进程枚举 / 状态机 / 编排 / 主循环）

> **源码**：`src/executable.cpp`（1823 行）
> **定位**：整个工具的会话层与编排层。MCP 的 `listProcesses` / `selectProcess` / `startProbe` / `startDump` / `getProbeStatus` / `getDumpStatus` / `getLogs` 的**直接外迁源**。
> **已精读范围**：1–1000（匿名 namespace 全文）+ 1750–1823（main 全文）。中间 1000–1750 是 ImGui UI 渲染（MCP 不需要，未逐行精读）。

---

## 0. 全局约定

1. **全局配置**（`:53-54`）：`kOutputDirectory = "/sdcard/UnrealMemoryTools"`、`kMaxLogLines = 1500`。
2. **8 个内置 profile**（`UE_Games` `:38-47`）：ArenaBreakout / DeltaForce / Farlight / Shuisha / Valorant / NRC / PUBGMHD / PUBG，**全部已 include 且已注册**。
   ⚠️ **文件名 `Sfps2.hpp` 里装的类叫 `ShuishaProfile`**（`Sfps2.hpp:6`）——源码存在**文件名与类名不一致**，审查时别误判成"未注册"。
   真正未接入的是 **`Lineage2.hpp`**（`Lineage2Profile`，既未 include 也未被 `UE_Games` 注册）。见 §6 陷阱 3。
3. **全部业务在匿名 namespace**，MCP 化需**外迁**。
4. **单 worker 线程模型**（`gWorkerThread` `:140`）：重活投递 worker，快速状态在 UI 线程读 `gDumpUiState`（mutex 保护）。

---

## 1. 核心数据结构（全部匿名 namespace）

| 结构 | 字段 | 行号 |
|---|---|---|
| `AutoProcessCandidate` | `pid` / `package` / `profileName` / `dedicated` | `:64-70` |
| `ProbeOffsetEntry` | `name` / `value` / `relative` / `found` | `:72-78` |
| `StructFieldEntry` | `name` / `type` / `offset` / `found` / `description` | `:80-87` |
| `StructGroup` | `name` / `fields[]` | `:89-93` |
| `ProbeResult` | `valid`/`success`/`pid`/`package`/`profileName`/`dedicated`/`baseAddress`/`offsets[]`/`structGroups[]`/`profile`/`autoProfileOwner` | `:95-108` |
| `DumpUiState` | `mutex` + `logLines[]` + `phase` + `activePackage` + `resultPath` + `lastError` + 三套 running/finished/success（probe/dump/soDump）+ `objectsPercent`/`dumpPercent` + `probeOffsets`/`probeStructGroups` | `:110-135` |

**全局单例**（`:137-141`）：`gDumpUiState` / `gCandidates` / `gSelectedIndex` / `gWorkerThread` / `gProbeResult`。

---

## 2. 进程枚举（→ `listProcesses`）

### `FindAutoProcessCandidates()`　`:166-216`

两条来源：
1. **profile 匹配**（`:170-178`）：遍历 `UE_Games`，`GetPIDsOf(appId)` → `dedicated=true`
2. **/proc 扫描**（`:180-200`）：遍历数字目录，`HasUnrealLib(pid)`（maps 含 `libUE4.so`/`libUnreal.so`）→ `dedicated=false`，profileName = "自动识别 (UE4/UE5 通用)"

**排序**（`:209-214`）：`dedicated` 优先，再按包名字典序。**去重**靠 `unordered_map<package, candidate>`。

### `HasUnrealLib(pid_t pid)`　`:154-164`

`getAllMaps(pid)` 找 maps 含 `libUE4.so` / `libUnreal.so`。

### `RefreshCandidates()`　`:345-386`

重新枚举 + 保持原选中包名（`gSelectedIndex` 复位后按 package 找回）；**进程列表变化时调 `InvalidateProbeReuse`**（`:383-384`）。

---

## 3. 日志与状态机（→ `getLogs` / `getProbeStatus` / `getDumpStatus`）

### 日志环形缓冲（`PushUiLog` `:227-237`）

`Logger::SetSink(LoggerSink)`（main `:1767`）→ 所有 `LOGI/LOGE` 进 `gDumpUiState.logLines`，**上限 1500 行，超出删最旧**（`:231-236`）。格式 `<level>: <msg>`。

### 状态机函数

| 函数 | 作用 | 行号 |
|---|---|---|
| `SetDumpPhase(phase)` | 设当前阶段 | `:244-248` |
| `SetProgress(objects, dump)` | 设进度（-1 不覆盖） | `:250-257` |
| `BeginProbeState(candidate)` | 探针开始（清 log/offset/struct） | `:259-280` |
| `FinishProbeState(success, offsets, groups, err)` | 探针结束 | `:302-313` |
| `InvalidateProbeReuse(reason)` | 跨进程作废旧探针 | `:282-300` |
| `BeginDumpState(package)` / `FinishDumpState(...)` | 转储状态 | `:315-343` |

**`getProbeStatus.phase/percent` 的数据源就是 `DumpUiState.phase` / `objectsPercent`**；`getProbeResults.offsets/structs` 就是 `probeOffsets` / `probeStructGroups`。

---

## 4. 编排函数（→ `startProbe` / `startDump` / `dumpUnrealLibrary`）

### `ExecuteProbe(const AutoProcessCandidate)`　`:644-752`

```
BeginProbeState
→ kMgr.initialize(pid, EK_MEM_OP_SYSCALL) 失败回退 EK_MEM_OP_IO（:655-656）
→ 匹配专用 profile（:669-682）
→ probeDumper.Init(matchedProfile)，失败回退 AutoFixProfile（:705-719）
→ CollectProbeOffsets + CollectStructGroups（:731-732）
→ FinishProbeState(true, ...)（:751）
```

⚠️ **profile 三级回退**：专用 profile → AutoFixProfile 兜底 → 都失败 `ERROR_PROBE_INIT_FAILED`。

### `ExecuteDump(const AutoProcessCandidate)`　`:754-857`

```
BeginDumpState
→ 校验 gProbeResult.valid/success/profile（:758-763，缺 → ERROR_NO_PROBE_RESULT）
→ 校验 package+pid 一致（:764-771，否则 ERROR_PROBE_MISMATCH）
→ delete_directory + mkdir_recursive（:774-775）
→ uEDumper.Init(profile, reuse=true)（:812）
→ Dump → SaveDumpBuffers → FinishDumpState(true, dumpGameDir, ...)
```

⚠️ **`delete_directory` 先清空输出目录**（`:774`）——`dumpSDK` 会清掉上次产物。

### `ExecuteDumpUnrealLib(const AutoProcessCandidate)`　`:859-943`

独立 soDump 状态机（`soDumpRunning/Finished/Success/Path`），`kMgr.dumpMemELF(ue_elf, path)`（`:931`）转储 `libUE4.so`/`libUnreal.so`。

---

## 5. 线程与主循环（→ MCP 命令服务接入点）

### 单 worker 线程（`:945-989`）

`StartProbeSelected` / `StartDumpAfterProbe` / `StartDumpUnrealLib` 都是 **`join()` 旧线程再 `std::thread(...)` 起新线程**（`:952-954` 等）。

🔴 **这是架构 v1.2 issue #8「单 worker 阻塞一切」的根源**：分钟级扫描/转储期间，`getLogs`/`getProbeStatus` 若也走 worker 会排队。**MCP 命令分级**：快速命令（读日志/状态）在命令线程直接读 `gDumpUiState`，重活才投 worker。

### `main()`　`:1761-1823`

```
setbuf 全关 → Logger::SetSink(LoggerSink) → RefreshCandidates
→ VULKAN 图形后端 + ANativeWindow + 渲染初始化
→ while(flag) { drawBegin; NewFrame; Layout_tick_UI(&flag); EndFrame }  ← 阻塞主循环
→ join gWorkerThread → Shutdown
```

🔴 **main 无 socket 命令服务**（源码 `AF_INET`/`bind`/`listen` 零命中）。MCP 化的两处接入点：
1. **起线程**：main 里 `RefreshCandidates()` 后起 `CommandServer` 线程（bind 127.0.0.1:27185）
2. **每帧 poll**：`Layout_tick_UI(&flag)`（`:1811`）里 poll 命令队列（快速命令直执行，重活投 worker）

---

## 6. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | 单 worker 线程阻塞一切 | 扫描期间 ping/getLogs 排队 | 命令分级（快命令直执行） |
| 2 | `ExecuteDump` 依赖 `gProbeResult` 全局 | 未探针直接 dump 报错 | 工具描述明示先 startProbe |
| 3 | `Sfps2.hpp` 文件名与类名 `ShuishaProfile` 不一致 | 误判"未注册/未 include" | 以 `UE_Games:38-47` 实际 8 个为准（均已接入）；`Lineage2.hpp` 才是真未接入 |
| 4 | `delete_directory` 清空输出 | dumpSDK 清上次产物 | 工具描述明示 |
| 5 | 日志环形缓冲 1500 行 | getLogs 超限丢最旧 | `sinceIndex` 增量拉取 |
| 6 | 全部业务锁匿名 namespace | MCP 不能直接调 | 外迁清单（§7） |
| 7 | main 无命令服务 | 无法远程驱动 | 起线程 + 每帧 poll |

---

## 7. MCP 外迁清单（《设备端API清单》L0/L5 的落点）

| 源码函数 | 外迁到 MCP 工具 | 行号 |
|---|---|---|
| `FindAutoProcessCandidates` | `listProcesses` | `:166-216` |
| `RefreshCandidates` | `listProcesses` 刷新 | `:345` |
| `InvalidateProbeReuse` | `selectProcess` 切进程 | `:282-300` |
| `ExecuteProbe` | `startProbe` | `:644-752` |
| `ExecuteDump` | `startDump` | `:754-857` |
| `ExecuteDumpUnrealLib` | `dumpUnrealLibrary` | `:859-943` |
| `CollectProbeOffsets` | `getProbeResults.offsets` | `:415-447` |
| `CollectStructGroups` | `getProbeResults.structs` | `:449-641` |
| `SaveDumpBuffers` | 落盘 | `:388-413` |
| `DumpUiState` 全套 | `getProbeStatus`/`getDumpStatus` | `:110-135` |
| `PushUiLog`/`LoggerSink` | `getLogs` | `:227-242` |

**注意**：这些函数都依赖 `gProbeResult` / `gDumpUiState` / `gWorkerThread` 等**文件级全局**，外迁时需把全局换成可注入的状态对象（架构文档 §5.6.3 的"换掉 gProbeResult 全局"）。

---

## 8. 本次未精读范围

| 范围 | 内容 |
|---|---|
| `:1000-1750` | `RenderAutoUEDumpPanel` 及全部 ImGui UI 渲染（含 `Layout_tick_UI` 内部的 UI 逻辑） |

MCP 不依赖 UI 渲染，此段已由《设备端API清单》L0/L5 的行号索引覆盖关键编排，UI 细节无需精读。
