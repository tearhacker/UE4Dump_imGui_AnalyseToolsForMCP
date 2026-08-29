# L5 批处理层 API · UEDumper（探测 / 转储编排）

> **源码**：`src/Dumper.hpp`（59 行）/ `Dumper.cpp`（560 行）
> **定位**：SDK 转储主流程。MCP 的 `startDump` / `getDumpStatus` / `dumpSDK` 直接底座。
> **已精读范围**：hpp 全文 + cpp 全文（1–560）。

---

## 0. 全局约定

1. **依赖 `IGameProfile` + `UEWrappers` 全局单例**，非线程安全，MCP 命令服务串行调用。
2. **产出走 `BufferFmt` 内存缓冲**（`outBuffersMap`），不直接落盘——落盘由 `executable.cpp` 的 `SaveDumpBuffers` 负责。MCP 侧 `dumpSDK` 需自行把 buffer 写到 `/sdcard/UnrealMemoryTools/<pkg>/`。
3. **进度回调**：`_objectsProgressCallback` / `_dumpProgressCallback`（`SimpleProgressBar`）——`getDumpStatus.percent` 的来源。
4. **错误语义**：`Init`/`Dump` 返回 bool，失败时 `_lastError` 存可读字符串（`GetLastError()`）。

---

## 1. `Init(IGameProfile *profile, bool reuseInitializedState = false)`　`:99-137`

| 分支 | 行为 | 失败返回 |
|---|---|---|
| `profile == nullptr` | — | `"ERROR_INVALID_PROFILE"` |
| `reuseInitializedState=true` | 校验 7 个字段非空 → `UEWrappers::Init(vars)` | `"ERROR_REUSE_UNINITIALIZED_PROFILE"` |
| 默认 | `InitUEVars()` → **`AutoFix::RunFixup(profile)`** | `UEVars::InitStatusToStr(initStatus)` |

**reuseInitializedState 校验的 7 字段**（`:111-113`）：`BaseAddress` / `NamesPtr` / `GUObjectsArrayPtr` / `ObjObjectsPtr` / `ObjObjects_Objects` / `GetOffsets()`（缺一即失败）。

⚠️ **`AutoFix::RunFixup` 副作用**：转储前自动修补偏移表（同 §2 陷阱 2）。`_lastError` 只在 Init 失败时设置，RunFixup 失败**不阻断**（仅日志）。

---

## 2. `Dump(std::unordered_map<std::string, BufferFmt> *outBuffersMap)`　`:139-216`

**9 个产出 buffer**，按 6 阶段生成：

| 阶段 | 产物 | 方法 |
|---|---|---|
| 1 | `Logs.txt` | `DumpExecutableInfo`（ELF 头 + 段） |
| 2 | `Logs.txt` | `DumpNamesInfo`（GNames/FNamePool + 前 5 名字） |
| 3 | `Logs.txt` | `DumpObjectsInfo`（GUObjectArray + 前 5 对象） |
| 4 | `Offsets.hpp` | `DumpOffsetsInfo`（引擎指针 + GEngine/GWorld 反查） |
| 5 | `Objects.txt` + packages | `GatherUObjects` |
| 6 | `AIOHeader.hpp`/`SDK_Enums`/`SDK_Structs`/`SDK_Classes`/`SDK_Offset` + `script.json` | `DumpAIOHeader` |

**关键失败点**：`GatherUObjects` 后若 `packages.empty()` → `_lastError = "ERROR_EMPTY_PACKAGES"`，返回 false（`:176-182`）。

### 2.1 `script.json` 生成（`:14-40` + `:196-215`）

`dumper_jf_ns::JsonFunction` → `to_json`：
- **名字 = `Parent$$Name`**（`replace_specials` 后拼接，`:34-36`）
- **Address = 绝对地址 - base_address（RVA）**（`:38`）
- **过滤**：Parent/Name 为空或 "None"/"null" 跳过；`Address==0 || <= base` 跳过（`:27-32`）

⚠️ `__author = "曦曦(DreamFekk)"`、`__notice = "禁止圈钱盗卖"`（`:204-206`）——产物带作者声明。

### 2.2 `DumpOffsetsInfo` 的 GEngine/GWorld 反查（`:289-344`）

`FindObject("Class Engine.Engine"/"Class Engine.World")` → `ForEachObject` 找非 CDO 实例 → **`FindAlignedPointerRefrence` 在 `.bss` 反向搜谁指向它**（`:313-327`）→ 得 GEngine/GWorld 全局指针。

⚠️ 依赖 `GetObjectPtr(1).GetIndex()==1` 前置校验（`:291`），不满足则**跳过整个反查**。

### 2.3 `FindProcessEventIdx`（`:58-96`）

遍历对象 0 的 vtable 前 500 槽位，找 `i∈(50,100)` 且前三条指令是 STP/STR（函数序言特征）的槽位；`processEvent` 匹配则返回 i，否则返回第一个 fallbackIdx。

---

## 3. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | `Dump` 依赖 `Init` 成功 | 未 Init 直接 Dump 崩 | `startDump` 前先 `startProbe` |
| 2 | `Init` 内 `AutoFix::RunFixup` 有副作用 | 偏移表被改 | `getProbeResults` 报告修正值 |
| 3 | 产出是内存 buffer 不是落盘 | 误以为已写盘 | MCP 侧自行 SaveDumpBuffers |
| 4 | `script.json` 名字 `Parent$$Name`、地址 RVA | 外部脚本解析错 | 文档明示命名/地址规则 |
| 5 | `GatherUObjects` 校验 `GetObjectPtr(1).GetIndex()` | 定位错时静默产出空 | 看 `Objects.txt` 是否空 |
| 6 | 进度回调贯穿，但无取消点 | 转储中无法 cancel | `cancelJob` 需在设备端加取消点 |
| 7 | 大游戏转储分钟级 | AI 重试风暴 | `waitMs` + 长轮询 |

---

## 4. 本次未精读范围

无。hpp + cpp 已全文精读。
