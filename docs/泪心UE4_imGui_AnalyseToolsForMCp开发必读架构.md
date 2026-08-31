# 泪心 UE4 ImGui 分析工具 MCP 化 · 开发必读架构

> **写代码前必读。** 本文档是 `泪心UE4_imGui_AnalyseToolsForMCp功能说明.md`（下称《功能说明》）的
> 架构落地篇，负责回答「**具体怎么写**」，并把 `awesome-mcp-best-practices` 的规范逐条落到本项目。
>
> 两份文档的关系：《功能说明》讲**做什么、为什么**；本文档讲**怎么写、按什么规矩写**。
> **命名与工具描述以本文档为准**（原因见 §1.1）。

**版本：v1.1（协议与安全性审查后修订）**

---

## 0.1 v1.1 修订记录（**本次审查发现并修复的问题**）

按严重程度排序。前三条是**会导致线上事故**的级别。

| # | 问题 | 严重度 | 修复 |
|---|---|---|---|
| **1** | `beginAttachSession` / `endAttachSession` 是有状态工具，AI 漏调 `end` 会**让游戏永久冻结** | 🔴 灾难 | §2.6 改为主推无状态的 `callRemoteFunctionBatch`；会话加服务端看门狗 + 四重兜底 |
| **2** | `writeMemory` / `callRemoteFunction` 无目标地址校验，AI 传错地址直接**搞崩游戏** | 🔴 灾难 | §2.8 写前强制用 `KittyPtrValidator` 校验映射权限；远程调用需显式 `confirmDangerous` |
| **3** | 长任务一律异步轮询，快速任务也要 2 次往返，且**没有取消手段** | 🔴 严重 | §4.2 改为「短等优先」混合策略；新增 `cancelJob` |
| 4 | 未声明 `capabilities`（resources / logging / tools.listChanged） | 🟠 功能缺失 | §2.1 |
| 5 | 错误分层模糊：协议错误与执行失败混为一谈 | 🟠 规范 | §2.1 明确二分 |
| 6 | 缺少 `outputSchema`，`structuredContent` 无契约 | 🟠 规范 | §2.7 全量补 |
| 7 | 未使用工具 `annotations`，客户端无法自动放行只读工具 | 🟠 体验 | §1.6 强制规范 |
| 8 | 缺少 `getLogs`，AI 看不到设备端失败原因 | 🟠 功能缺失 | §1.1 C 组新增 |
| 9 | PC 侧与设备侧无版本协商，二进制换版后静默不兼容 | 🟠 稳定性 | §2.1 握手 |
| 10 | 完全没有 `resources`，大文件只能靠分页工具硬读 | 🟡 设计缺口 | §2.9 补资源（并说明哪些**不该**做资源） |
| 11 | 工具数膨胀到 41，描述变长后上下文成本未核算 | 🟡 成本 | §1.7 设上限 45 + 上下文预算核算 |
| 12 | 部分可选参数无默认值说明 | 🟡 规范 | §1.8 每个可选参数必须有默认值 |

---

## 0.1b v1.2 修订记录（架构级审查：性能 / token / 延迟 / 幻觉 / 安全）

> 审查触发：用户要求对"架构设计、技术栈、性能、高 token、延迟、AI 乱答"做最严格复查。
> 前三条是**事故级**，第 4 条是**安全漏洞**。

| # | 问题 | 严重度 | 修正 |
|---|---|---|---|
| **1** | **单次响应无 token 上限**：`readMemory size≤65536` = hex 13 万字符 ≈ **33K token 一次响应**，可直接打爆上下文；`getLogs maxLines=200`≈6K；`scanCandidates maxCandidates=200`≈10K | 🔴 灾难 | **硬约束：单工具响应 ≤ 4K token**。`readMemory` size 上限砍到 4096；`getLogs` 默认 50；`maxCandidates` 默认 50；大响应一律分页 |
| **2** | **慢工具无防护**：全内存扫描在 45.8 万对象游戏上分钟级，AI 只见调用挂起 → **重试风暴**（重扫 = 又几分钟，还翻倍占 worker） | 🔴 灾难 | ① 工具描述必须写**预期耗时档位**（快<1s / 中<30s / 慢<5min）；② 轮询响应带 `progress + etaSeconds + suggestedWaitMs`，AI 按建议等待；③ **长轮询**：`waitMs` 内未完成时设备端可 hold 连接至完成或上限，把 N 次空轮询压成 1 次 |
| **3** | **AI 判定无证据内嵌**：scan 返回地址列表 → AI 挑一个 → 再调 sample → 三轮往返，且 AI 可能跳过采样直接"猜"最像的 | 🔴 幻觉高危 | `scanGNames`/`scanObjects` 返回**每个候选内嵌前 3 个名字样本** + `engineNameHits` 评分。扫描-采样-判定**三轮压一轮**，且证据强制在场，AI 想跳过都跳不过 |
| **4** | **socket 未规定 bind 地址**：root 权限 + 无认证 + 内存写接口，若 bind 0.0.0.0 = **局域网任意设备可读写目标游戏内存** | 🔴 安全漏洞 | **强制 bind 127.0.0.1**，仅经 adb forward 暴露；PC 侧连 `127.0.0.1:35515`，HELLO 后无需认证可直接调用 |
| **5** | **断线重连协议缺失**：USB 拔插 / adb 掉线 / UMT 被杀是**最高频故障**，架构只处理了设备进程崩溃（ESRCH） | 🔴 严重 | PC 侧 bridge：指数退避重连（1s/2s/4s/…上限 30s）；重连成功必**重新 HELLO**；**所有 sessionId 立即失效**；in-flight 调用返回 `isError: true` + `"连接已断开，已自动重连，请重试"` |
| 6 | **outputSchema 的 token 成本漏算**：§1.7 只算 description（10K），每个 outputSchema 200–500 token × 42 = **另加 8–20K**，真实常驻 25–35K | 🟠 成本 | ① 描述瘦身：`<important_notes>` ≤5 条、总 ≤150 token；② **分组按需挂载**：核心 20 个常驻，F 组（ptrace）/E 组靠 `tools.listChanged` 动态加载；③ 上限 45 不变但**常驻 ≤25** |
| 7 | **PC 并发与串行协议矛盾**：MCP 允许并行 tool calls，但设备协议是"发一收一"，架构只说 RLock 串行化——并行调用会排队且无请求匹配 | 🟠 延迟 | 明写：**协议为严格请求-响应串行**，PC 侧 RLock 必须；并行 tool calls 串行执行（正确但延迟叠加）。若将来要并行，协议加 `reqId` 匹配——v1 不做 |
| 8 | **单 worker 阻塞一切**：gWorkerThread 单线程，分钟级扫描期间 getLogs/ping/getProbeStatus 全部排队 | 🟠 延迟 | 命令分级：**快速命令**（读日志/状态/小内存读）在命令线程直接执行；**重活**（扫描/转储）才进 worker。二者以 `DumpUiState` mutex 保护的数据为界 |
| 9 | **缓存命中不可见**：AI 换参数重评可能拿到缓存结果而不自知，误以为"重扫过了" | 🟡 幻觉 | 缓存命中响应必须带 `cached: true` + 原始参数回显 |
| 10 | **AI 结论不持久**：判定结果只存在对话里，新会话全重来（token 浪费 + 结果漂移） | 🟡 成本 | 新增**分析报告落盘**：验证通过的定位结论 + 证据写 `/sdcard/UnrealMemoryTools/<pkg>/mcp_analysis.json`，新会话先读它（复用 `readOutputFile`） |
| 11 | `followPointerChain` 不支持数组步进 | 🟡 功能 | offsets 支持 `"[k]"` 后缀（如 `"0x38[0]"` = 解引用后取数组第 0 元素），真实链路 `LocalPlayers[0]` 需要 |
| 12 | 设备端 JSON 库未指定 | 🟡 规范 | **定 nlohmann/json**（`deps/nlohmann/json.hpp` 已在仓库，Dumper 已用）；编译期开销可接受，禁止再造轮子 |
| 13 | 轮询浪费 token：每轮 ~100-200 token，30 次轮询 = 6K 白烧 | 🟡 成本 | 由 #2 的长轮询解决；轮询响应必须带 `suggestedWaitMs` |

**v1.2 保留 v1.1 全部决定**；本节只增补。工具数 42（§1.1 表格计数勘误：A2+B2+C11+D7+E2+F5+G8+H3+I2 = **42**，v1.1 标题"41"为笔误）。

---

## 0.2 三句话总结

1. **命名 camelCase**，描述用 `<use_case>` + `<important_notes>`，参数全部带默认值与校验。
2. **高层用例工具做正门**（`locateEngineGlobals` / `dumpSDK`），复合操作降轮次，原语兜底。
3. **危险的必须防住**：写内存校验映射权限、attach 会话必须有看门狗、每个长任务可取消。

---

# 第一部分　工具设计规范

## 1.1 工具清单（v1.2 定稿，43 个）

> 命名 camelCase（依据最佳实践 1.1，GPT-4o tokenization 更省：`memoryRead` 2 token vs `memory_read` 3 token）

| 组 | 工具 | 说明 |
|---|---|---|
| **A 连接** (2) | `ping` `getCapabilities` | 后者返回活动会话状态，用于**恢复泄漏的 attach** |
| **B 进程** (2) | `listProcesses` `selectProcess` | |
| **C 流程** (11) | `startProbe` `getProbeStatus` `getProbeResults` `startDump` `getDumpStatus` `dumpUnrealLibrary` `listOutputFiles` `readOutputFile` **`getLogs`** **`cancelJob`** `applyProbeOverrides` | 合并了原 `getProbeOffsets`/`getProbeStructs`；新增日志与取消 |
| **D 原语** (7) | `readMemory` `writeMemory` `readMemoryValue` `scanPattern` `listModules` `resolveSymbol` `readString` | 合并了原 `listElfSymbols` 进 `resolveSymbol(filter?)` |
| **E 理解** (2) | `disassemble` `decodeAdrl` | PC 侧 capstone |
| **F ptrace** (5) | `beginAttachSession` `endAttachSession` `callRemoteFunction` `callRemoteFunctionBatch` `allocScratch` | **主推 `callRemoteFunctionBatch`（无状态）** |
| **G 引擎语义** (8) | `detectUEVersion` `scanGNames` `sampleGNames` `scanObjects` `sampleObjects` `searchClasses` `describeClass` `inspectObject` | |
| **H 高层用例** (3) | `locateEngineGlobals` `dumpSDK` `analyzeClass` | **正门** |
| **I 复合操作** (2) | `followPointerChain` `scanCandidates` | 专门降轮次 |

**原生命令名保持大写下划线不变**（`MEMORY_READ` / `CALL_FUNCTION`）——
那是设备端协议，与 MCP 工具名是两层，不要混。

## 1.2 描述里写明别名（最佳实践 1.2）

工具名太具体会导致 AI 想不起来调用。用户可能说"找函数""定位地址""搜特征码"，
对应的是同一个 `scanPattern`。**每个工具描述里必须带中文同义触发词。**

```
scanPattern — 按字节特征码（IDA 风格，支持 ? 通配）在目标进程内存中搜索。
也可用于：找函数地址、定位特征码、搜索指令序列、匹配机器码、搜 pattern。
```

## 1.3 工具描述模板（最佳实践 1.3）

```jsonc
{
  "name": "scanGNames",
  "title": "扫描 UE 全局名字表候选",
  "description":
    "<use_case>在目标进程中扫描 GNames / FNamePool 的候选地址。</use_case>\n" +
    "<important_notes>\n" +
    "1. 先调 detectUEVersion 确认版本，可显著缩小搜索范围。\n" +
    "2. anchorNames 至少给 3 个引擎内建名，单锚点误判率高（这是 AutoFix 失败主因）。\n" +
    "3. 本工具只返回候选摘要，必须用 sampleGNames 取样本后才能判定。\n" +
    "4. 结果按 session 缓存，换判据重评无需重扫。\n" +
    "</important_notes>",
  "inputSchema": {
    "type": "object",
    "properties": {
      "minPtr":  { "type": "string", "default": "0x4000000000" },
      "maxPtr":  { "type": "string", "default": "0x8000000000" },
      "anchorNames": { "type": "array", "items": { "type": "string" },
                       "default": ["None","ByteProperty","IntProperty"] },
      "maxCandidates": { "type": "integer", "minimum": 1, "maximum": 2000, "default": 200 }
    },
    "additionalProperties": false
  },
  "outputSchema": {
    "type": "object",
    "properties": {
      "sessionId":   { "type": "string" },
      "candidateCount": { "type": "integer" },
      "candidates": {
        "type": "array",
        "items": {
          "type": "object",
          "properties": {
            "id": { "type": "integer" },
            "address": { "type": "string" },
            "anchorHits": { "type": "integer" }
          },
          "required": ["id", "address", "anchorHits"]
        }
      }
    },
    "required": ["sessionId", "candidateCount", "candidates"]
  },
  "annotations": {
    "readOnlyHint": true,
    "destructiveHint": false,
    "idempotentHint": true,
    "openWorldHint": false
  }
}
```

**五条硬性要求：**

1. 必须有 `<use_case>` —— 一句话说清"什么时候该调它"
2. 必须有 `<important_notes>` —— 前置条件、易错点、**下一步该调什么**
3. 必须有 `outputSchema`（MCP 2025-06-18+ 支持，用于结构化结果校验）
4. 必须有 `annotations`（见 §1.6）
5. 中文项目：`title` 中文，`description` 里技术术语保留英文原文
   （GNames / FNamePool / ADRP）—— **别翻译术语，AI 认的是原文**

## 1.4 禁止 "not found" 响应（最佳实践 1.4）

> 原话：LLMs can be overly influenced by negative statements like "not found"
> causing them to ignore the useful information that follows.

| 场景 | ❌ 禁止 | ✅ 正确 |
|---|---|---|
| `scanGNames` 零命中 | `"未找到 GNames"` | 返回放宽条件后的次优候选 + `"严格条件下 0 命中；以下 5 个是放宽指针范围后的候选，命中锚点数较低，建议用 sampleGNames 逐个验证"` |
| `searchClasses` 无精确匹配 | `"类 X 不存在"` | 返回模糊匹配的类名列表 |
| `resolveSymbol` 符号不存在 | `"符号未找到"` | 返回名字相近的符号 + 提示"可能已 strip，可改用 scanPattern" |
| `sampleGNames` 解出乱码 | 替 AI 判定"无效" | **原样返回字符串**，让 AI 自己看 |

**与"返回原始观测值"一致**：工具给数据 + 中性描述，**判定权永远在 AI**。

## 1.5 冲突裁决：原语优先 vs 不要 1:1 映射（最佳实践 3.2）

```
┌─ H 组 高层用例（正门）──────┐  AI 首选，一次调用完成完整用例
│  locateEngineGlobals        │
│  dumpSDK / analyzeClass     │
└─────────────────────────────┘
            ↓ 失败或需细粒度控制
┌─ I 组 复合操作 ─────────────┐  一次调用顶 N 次原语，专门降轮次
│  followPointerChain         │
│  scanCandidates             │
└─────────────────────────────┘
            ↓ 仍不够
┌─ D/E/F/G 组 原语 ───────────┐  兜底，让 AI 能拼出未预置的策略
└─────────────────────────────┘
```

**三条落地规则：**

1. **Instructions 里写明"优先用高层工具"**，让 AI 默认走正门
2. **复合操作是必做项**，直接对冲 3.2 的"轮次太多"担忧
3. **原语不删**——我们是"未知游戏"，必然有高层覆盖不到的情况。
   这是 3.2 没考虑的场景差异（它假设 API 已知）

## 1.6 工具注解（annotations）—— **v1.1 新增，强制**

客户端靠 `readOnlyHint` 决定是否自动放行。缺了它，用户每次读内存都要点确认。

| 注解 | 适用 |
|---|---|
| `readOnlyHint: true` | 所有纯读工具（A/B/C 查询/D 读/G 扫描与取样/E/I） |
| `destructiveHint: true` | `writeMemory`、`callRemoteFunction*` |
| `idempotentHint` | 扫描/取样类为 `true`；`startProbe`/`startDump` 为 `false` |
| `openWorldHint: false` | **全部工具**。我们不访问外部网络 |

## 1.7 工具总数与上下文预算（**v1.2 重算**）

> v1.1 只算了 description（约 10K），漏算了 **outputSchema（每项 200–500 token × 43 = 另加 8–20K）**。
> 真实常驻上下文 **25–35K token**（issue #6）。

**43 个工具**（v1.1 标题"41"为笔误，A2+B2+C11+D7+E2+F5+G8+H3+I2 = 43，含 ATTACH + DISASSEMBLE）。

- **硬上限：45 个工具**（不变）
- **常驻 ≤ 25 个**：核心 20 个常驻；F 组（ptrace）/ E 组（`decodeAdrl` 等 PC 侧重活）靠
  `tools.listChanged` **动态加载**（设备端不支持 ptrace / 用户未启用时摘除）——不计入常驻预算（issue #6）
- 描述瘦身：`<important_notes>` ≤ 5 条、总 ≤ 150 token；能用 `use_case` 说清的不写进 `description`
- **🔴 单工具响应体积硬约束 ≤ 4K token**（issue #1）：`readMemory` size 上限 **4096**、
  `getLogs` 默认 `maxLines=50`、`scanCandidates` 默认 `maxCandidates=50`；超阈值一律**分页**
  （返回 `nextCursor`，AI 翻页而非一次拉全）。任何工具违反此约束视为 bug
- 每次新增工具，在 PR 里写明"增加了多少常驻 token + 是否常驻"

## 1.8 参数规范（**v1.1 补齐**）

- **每个可选参数必须有 `default`**（最佳实践：optional params have defaults）
- 数值参数一律标 `minimum` / `maximum`
- 十六进制串用 `pattern: "^[0-9A-Fa-f]+$"` 并在实现里查偶数长度
- 数组参数一律标 `maxItems`
- 全部 `additionalProperties: false`

---

# 第二部分　协议与架构

## 2.1 协议细节（**v1.1 修正 4 处**）

| 项 | 决定 |
|---|---|
| 传输 | stdio，换行分隔 JSON |
| 协议版本 | 支持 `2024-11-05` / `2025-03-26` / `2025-06-18` / `2025-11-25` |
| 端口 | **35515**（避开源项目 27184） |

### ❗修正 1：`initialize` 必须完整声明 capabilities

```jsonc
{
  "capabilities": {
    "tools":     { "listChanged": true },   // ptrace 不可用时动态摘除工具
    "resources": { "subscribe": false, "listChanged": true },
    "logging":   {}                          // 设备端日志透传
  },
  "serverInfo": { "name": "unreal-memory-tools", "version": "1.0.0",
                  "title": "UE4/UE5 运行时分析器" },
  "instructions": "……（见 §3）"
}
```

`tools.listChanged: true` 很重要：`callRemoteFunction` 等 ptrace 工具在
SELinux 拒绝或设备不支持时应**动态摘除**并通知客户端，而不是让 AI 调了才报错。

### ❗修正 2：错误必须分层

| 类型 | 返回方式 | 例子 |
|---|---|---|
| **协议层错误** | JSON-RPC `error` | 未知工具名、参数 schema 校验失败、JSON 解析失败 |
| **执行层失败** | `isError: true` 的 tool result | 读内存失败、扫描无结果、ptrace attach 被拒 |

v1.0 把两者混为一谈，导致客户端无法区分"我调错了"和"设备端失败了"。

### ❗修正 3：设备端握手与版本协商

PC 侧与设备端是两个二进制，**换版后静默不兼容是真实风险**。

- 连接建立后先发 `HELLO` → 设备端回 `{ "protocol": 1, "build": "...", "capabilities": [...] }`
- PC 侧校验 `protocol`；不匹配则**明确报错**而不是让后续命令一路失败
- `getCapabilities` 同时暴露设备端 build，便于排障

### ❗修正 4：设备端日志透传

用 MCP `notifications/message`（已在 capabilities 声明 `logging`），
同时保留 `getLogs` 工具做兜底——**AI 看不到设备端失败原因就无法自我纠错**。

### ❗修正 5：设备端 socket 必须 bind 127.0.0.1（**v1.2 安全，issue #4**）

> UMT 以 root 运行、提供内存读写接口、**无认证**。bind 0.0.0.0 = 局域网任意设备可读写目标游戏内存。

- **设备端 `CommandServer` 强制 `bind("127.0.0.1", 35515)`**，绝不监听 `0.0.0.0` / `INADDR_ANY`
- 仅经 `adb forward tcp:35515 tcp:35515` 暴露；PC 侧 `ToolDispatcher` 连 `127.0.0.1:35515`
- `HELLO` 握手后直接调用：客户端只校验 `protocol`，不发送 Token 或 AUTH 帧
- 端口号从 `35515` 提为可配置（避免与已运行实例冲突），但默认仍仅本地

### ❗修正 6：断线重连协议（**v1.2，issue #5**）

> USB 拔插 / adb 掉线 / UMT 被杀是**最高频故障**，v1.1 只处理了设备进程崩溃（ESRCH）。

- PC 侧 `ToolDispatcher` 维护连接，断线后**指数退避重连**（1s / 2s / 4s / … 上限 30s）
- 重连成功**必重新 `HELLO`**并校验 `protocol`
- 重连后**所有 `sessionId` 立即失效**（扫描候选集、类索引、job 全部作废），AI 须重建
- 重连期间 in-flight 调用返回 `isError: true` + `"连接已断开，已自动重连，请重试"`
- 设备端被杀重启后，旧会话数据不恢复——这是预期行为，文档写明

### ❗修正 7：协议为严格请求-响应串行（**v1.2，issue #7**）

- **设备协议是"发一收一"**，不是全双工。MCP 允许并行 tool calls，但 PC 侧 `ToolDispatcher`
  必须用 `RLock` **串行化所有设备调用**（同 v1.1），并行 tool calls 会**排队顺序执行**（延迟叠加，但正确）
- 若将来要真并行，协议加 `reqId` 字段做请求-响应匹配——**v1 不做**

## 2.2 抽象 Server 能力（最佳实践 3.1）

```
ToolDispatcher      只负责「名字 → 实现」，不碰传输
    ↓
McpServer          只负责 JSON-RPC 分发，不碰业务逻辑
    ↓
Transport (stdio)  可替换
```

**禁止**在工具实现里操作 stdout——stdout 是协议通道，日志一律走 stderr。

## 2.3 编码约定（改代码必守）

- 字符串参数一律 UTF-8 **hex** 传输（防引号/空格/中文/分隔符冲突）
- 地址 `int(x,0)` 解析后统一 `0x...` 下发
- 类型化值 `struct` 小端编码

## 2.4 缓存

| 缓存对象 | 键 | 失效条件 |
|---|---|---|
| 扫描候选集 | `sessionId` | 显式清理 / 切换进程 / 超时（建议 10 分钟） |
| 模块列表 | pid | 切换进程 |
| 反汇编结果 | (addr, size) | 内容不变，可长期缓存 |
| 类索引 | pid + dump 版本 | 重新 dump |

**`scanGNames` 换判据重评时不得重扫。**

> 🔴 **缓存命中必须可见（v1.2，issue #9）**：任何返回缓存结果的响应必须带
> `cached: true` + **原始参数回显**，否则 AI 会误以为"又扫了一遍"而做出错误判断。
> 换判据/换参数重评时，若命中缓存，响应显式标注 `cached: true, params: {...}`，
> 并提示"如需强制重扫传 `forceRefresh: true`"。

## 2.5 反汇编位置

**放 PC 侧**。设备端无反汇编器（Keystone 头文件在，但 CMake `-DkNO_KEYSTONE` 已关闭）。
MCP Server 里 `pip install capstone`，设备零体积代价。

## 2.6 ❗无状态化改造：attach 会话（**v1.1 最重要修复**）

### 问题

`beginAttachSession` / `endAttachSession` 是有状态工具。
AI 漏调 `endAttachSession` → **游戏永久冻结**。 MCP 工具应尽可能无状态。

### 修复：主推无状态批处理

**`callRemoteFunctionBatch(address, argSets[], returnKind)` —— 原子操作**

```
内部：attach → N 次调用 → detach
外部：一次工具调用，无状态，不可能泄漏
```

这覆盖 95% 的场景（批量取名字、批量验证候选）。
原本 N 次调用 → 1 次，同时拿到最优冻结效率。

### 会话模式仅用于高级交错场景

需要「调用 → 读内存 → 再调用」交错时才用 `beginAttachSession`。
**四重兜底，缺一不可：**

1. **`maxHoldMs` 参数**（默认 3000，上限 30000）—— 服务端到点强制 detach
2. **空闲看门狗** —— 会话内超过 N 毫秒无操作，自动 detach
3. **异常兜底** —— 任何工具异常、JSON 解析错误，先 detach 再返回错误
4. **断连兜底** —— stdio 流关闭 / 进程退出，析构函数里强制 detach

另需处理：目标进程崩溃时 `ptrace` 返回 `ESRCH`，此时清理状态并**正常返回**
（不要抛异常，否则又会漏掉 detach）。

`getCapabilities` 必须回报 `activeAttachSession: true/false`，
让 AI 在会话泄漏时能**自我发现并恢复**。

## 2.7 ❗结构化输出 outputSchema（**v1.1 新增**）

MCP 2025-06-18 起支持 `outputSchema`。**每个有返回数据的工具都必须声明**，
否则 `structuredContent` 是无契约的自由格式，AI 解析容易出错。

- `outputSchema` 与 `structuredContent` 必须严格一致（写单元测试校验）
- 数组字段一律带 `maxItems`，避免 AI 一次吃进上万条
- 数值地址统一用**字符串**（`0x...`），不要用 number —— 避免精度问题与进制歧义

## 2.8 ❗安全：地址校验与危险操作确认（**v1.1 新增**）

### 写内存必须校验目标映射

`writeMemory` 收到地址后，**先用 `KittyPtrValidator` 校验**：

- 地址落在已映射区间内
- 该区间权限包含 `PROT_WRITE`
- 越界或不可写 → **拒绝执行并返回中性说明**，不要尝试写入

项目里已有 `KittyPtrValidator`（`KittyMemoryEx/KittyPtrValidator.hpp`），直接用。

### 远程调用需显式确认

`callRemoteFunction` 可以在目标进程里执行**任意地址的任意函数**，
这是本项目能力最强也最危险的工具。

- 参数 `confirmDangerous: boolean`，**默认 false**
- 为 false 时返回中性提示而非执行
- Instructions 里明写：调用前必须已验证过该函数地址

### 只读为默认

所有写操作默认关闭，需显式开启（沿用《功能说明》§3.6），
且写操作走 `MemoryBackup` 支持撤销。

## 2.9 ❗资源（Resources）—— **v1.1 新增，并说明哪些不该做**

| 资源 URI | 内容 | 适合做资源？ |
|---|---|---|
| `umt://processes` | 进程列表（小） | ✅ 适合 |
| `umt://probe/{pkg}/results` | 探测结果（几十 KB） | ✅ 适合 |
| `umt://dump/{pkg}/summary` | 产物清单 | ✅ 适合 |
| `umt://logs` | 设备端日志 | ✅ 适合 |
| `umt://sdk/{pkg}/SDK_Classes.hpp` | **几十 MB** | ❌ **禁止** |

**大文件绝不做资源**——客户端可能整个读进上下文，直接爆炸。
大文件一律走 `searchClasses` / `describeClass` **服务端过滤**，
只回 AI 真正要的那一个类。

资源变更时发 `notifications/resources/list_changed`。

---

# 第三部分　Server Instructions 全文草案

> 最佳实践 2.1。**这是全套工具成败的关键**——AI 不知道工作流就会乱调原语。

```text
你接入的是 UnrealMemoryTools 运行时分析服务：一个运行在 Android 设备上、
通过读取目标进程内存来分析 Unreal Engine（UE4/UE5）游戏的外部分析器。
它不注入、不 hook，能力来自 /proc/<pid>/mem 与 ptrace。

## 资源模型
- 进程：设备上所有加载了 libUE4.so / libUnreal.so 的进程都是可分析目标
- 输出根目录：/sdcard/UnrealMemoryTools/<package>/
- 关键引擎全局：GNames（或 FNamePool）、GUObjectArray、GWorld

## 首选工作流（务必按此顺序）
1. listProcesses 选定目标，selectProcess 确认
2. **locateEngineGlobals** —— 一键定位 GNames / GUObjectArray / GWorld。
   这是正门，绝大多数情况一次就够。
3. 只有它失败时才展开细粒度流程，**顺序不可颠倒**：
   detectUEVersion
   → **resolveSymbol（符号优先，O(1) 最快）**
       按版本试这四个引擎原生符号：
         GUObjectArray                             （全版本，对象数组）
         GNameBlocksDebug                          （UE4.23+，名字表，
                                                     需解引用后回减 FNamePool.BlocksOff）
         _ZN5FName16GetIsInitializedEv             （UE4.00–4.20，需 DecodeADRL）
         GFNameTableForDebuggerVisualizers_MT      （UE4.22，直接即地址）
   → scanPattern（符号失败才走，按版本挑 18/14/14/6 条库）
   → scanGNames → sampleGNames → scanObjects → sampleObjects
   → applyProbeOverrides
4. dumpSDK 一键产出 SDK；或 startProbe → 检查 → startDump 分步执行

**为什么符号必须排在 pattern 前面**：符号表查找是 O(1)，pattern 要扫整个可执行段；
且符号名是引擎源码标识符，比字节序列稳定得多。上游 28 个 profile 里
19 个都把符号作为第一策略——这是实证，不是猜测。

## 判定候选的黄金法则
- 名字表对不对，看前 16 个字符串：真实的 FName 表开头必然是
  None / ByteProperty / IntProperty / BoolProperty / FloatProperty /
  ObjectProperty / NameProperty / StructProperty / ArrayProperty / Class / Object。
  这些是引擎内建名，所有 UE 游戏都一样。
- 对象数组对不对，看前几个对象全名：必然包含 /Script/CoreUObject。
- **永远用 sampleGNames / sampleObjects 取样本后自己判断，不要凭地址猜测。**
- 🔴 **证据必须内嵌在场（v1.2，issue #3）**：`scanGNames` / `scanObjects` 的返回
  已**内嵌每个候选的前 3 个名字样本 + `engineNameHits` 评分**，扫描-采样-判定三轮压成一轮。
  **不要跳过内嵌样本另起一轮 sample**——那既浪费 token 又容易在采样前就猜错。
  判定结论必须引用具体样本字符串，禁止只报"最可能的地址"。

## 硬性约束
1. 长任务先同步等待；超过 waitMs 才返回 jobId，用 getProbeStatus 轮询。
2. **优先用 callRemoteFunctionBatch（无状态、自动 attach/detach）**。
   只有需要「调用→读内存→再调用」交错时，才用 beginAttachSession，
   且必须配对 endAttachSession——漏调会让游戏永久冻结。
3. 默认只读。writeMemory 会校验目标映射可写性；仍然必须先向用户确认。
4. callRemoteFunction 需传 confirmDangerous: true 才执行，
   且调用前必须已通过 scanPattern 或 disassemble 验证过函数地址。
5. 读 SDK 产物必须分页或走 searchClasses/describeClass 服务端过滤，
   禁止整文件读取。
6. 工具返回的都是原始观测值，判断权在你。
   工具说"命中数较低"不等于"不是"。
7. 长任务可随时 cancelJob 取消。
8. 🔴 **分析结论落盘（v1.2，issue #10）**：验证通过的定位结论 + 证据写入
   `/sdcard/UnrealMemoryTools/<pkg>/mcp_analysis.json`。新会话/重连后**先 `readOutputFile`
   读它**，避免重做整轮扫描和判定（省 token、防结果漂移）。未落盘的结论视为临时、不可信。

## 易错点
- 不要假设偏移：偏移来自 dump 的真实数据，不是推理出来的。
- 同一 UE 版本的不同游戏，引擎函数序言高度相似
  （那是引擎代码，不是游戏代码）。
- FName 可能是 UTF-16，读字符串不确定时两种都试。
- 见到"未识别"字段不要硬猜，报告出来让用户决定。
- 出错了先看 getLogs，设备端日志通常直接指出原因。
```

---

# 第四部分　执行模型

## 4.1 并发

设备侧：命令线程只投递，重活仍走现有 `gWorkerThread` 或渲染循环，
**一条线程模型都不改**（复用 `executable.cpp:110-135` 的 `DumpUiState` + mutex 模式）。

PC 侧：`ToolDispatcher` 用 `RLock` 串行化（沿用源项目），
但**长任务不占住锁**——立即返回 jobId 后在后台线程继续。

> 协议层细节见 §2.1 修正 7：**设备协议严格请求-响应串行**，并行 tool calls 由 `RLock`
> 排队顺序执行；不要假设设备端能并发处理多条命令。

## 4.2 ❗短等优先混合策略（**v1.1 修复**）

v1.0 规定"长任务一律立即返回 jobId"，导致快速任务也要 2 次往返。

**新策略：**

```
工具带 waitMs 参数（默认 5000，上限 60000）
  ├─ 任务在 waitMs 内完成 → 直接返回最终结果（1 次往返）
  └─ 超时 → 返回 { jobId, status: "running", progress }，客户端轮询
```

这一条直接省掉大量无用往返。

> 🔴 **长轮询（v1.2，issue #2）**：设备端在 `waitMs` 窗口内**可 hold 连接不放**，
> 把"N 次空轮询"压成"1 次带结果的返回"。即：超时不是立刻返 jobId，而是
> 在连接上等到 `waitMs` 上限或任务完成，再返回。仅在任务接近完成时收益最大。
>
> 🔴 **每个工具描述必须标注预期耗时档位**（issue #2）：`快 <1s` / `中 <30s` / `慢 <5min`。
> 慢工具 AI 才能正确选大 `waitMs`、不触发重试风暴。
>
> 🔴 **轮询响应必带 `progress + etaSeconds + suggestedWaitMs`**（issue #2/#13）：
> AI 按 `suggestedWaitMs` 等待，避免盲目短轮询白烧 token（30 次 × 200 ≈ 6K）。
> 轮询返回 `{ jobId, status, progress: 0.42, etaSeconds: 18, suggestedWaitMs: 15000 }`。

## 4.3 取消

所有长任务必须可取消：`cancelJob(jobId)`。
设备侧对应中断 `gWorkerThread` 的协作式取消点
（`DumpUiState` 里加 `cancelRequested` 标志，worker 循环里检查）。

## 4.4 进度

优先用 `waitMs` + 轮询返回 `progress`。
对支持 `notifications/progress` 的客户端可作为增强（不作为依赖）。

---

# 第五部分　技术栈、代码结构与开发规范

## 5.0 技术栈选定（**v1.1 新增**）

### ❗先纠正一个前提

源项目 `mcp_server.py` **不是 FastMCP**，它是**纯标准库手写**的
（import 仅 `argparse/json/math/os/socket/struct/subprocess/sys/threading/dataclasses/typing`，
无任何 MCP SDK，JSON-RPC 2.0 手工实现）。

**它那些限制是手写实现的代价，不是 MCP 本身的限制**：
无 resources、无 logging、无 progress、无取消、单线程串行会阻塞。
**不要把它当范本照抄。**

### 候选对比（按本项目权重打分）

| 方案 | capstone | 我要的功能 | 依赖量 | 传输抽象 | Windows | 结论 |
|---|---|---|---|---|---|---|
| **Python + 官方 SDK FastMCP** | ✅ 原生绑定 | ✅ 全有 | 中 | ✅ | ✅ | **选它** |
| TypeScript + 官方 SDK | ❌ 需 WASM/原生插件，生态弱 | ✅ 全有 | 大 | ✅ | ✅ | 反汇编卡死 |
| Python 纯手写（照抄源项目） | ✅ | ❌ 全要做 | 极小 | ❌ 自己写 | ✅ | 重蹈覆辙 |
| C++ / 其他 | 麻烦 | 麻烦 | — | — | — | 排除 |

**决定项：capstone。** 我们的 `disassemble` 是核心工具（加密 FName 解密要靠它读汇编），
Python 有成熟原生绑定，Node 生态弱（capstonejs 长期失修，需 WASM 或原生插件）。
**这一条直接排除 TypeScript。**

**第二决定项：SDK 白送的四个能力**——resources、logging、progress、cancellation。
这正是 v1.1 审查里我们要补的东西。手写等于把 SDK 的活重做一遍，还容易做错。

### 选定

```
设备端：C++20（已定，无选择余地）
PC 侧 ：Python 3.11+ / 官方 MCP SDK 的 FastMCP / capstone
```

- **用官方 `mcp` 包里的 FastMCP**（`mcp.server.fastmcp`），
  不用第三方 `fastmcp` 包——后者是社区版，与官方协议节奏不同步
- **锁定版本**：MCP 协议一年迭代了 4 个版本，SDK churn 很快，`requirements.txt` 必须 pin
- **传输抽象**：FastMCP 同一套工具定义可跑 stdio / HTTP，直接满足最佳实践 3.1（IoC）

> 🔴 **设备端 JSON 库定 nlohmann/json（v1.2，issue #12）**：`deps/nlohmann/json.hpp`
> 已在仓库（Dumper 已用），编译期 header-only 开销可接受。**禁止再造轮子**手写 JSON 解析——
> 手写解析是 v1.0 安全/兼容问题的根因之一。所有命令服务收发 JSON 统一走它。

### ⚠️ 两个必须注意的坑

**1. FastMCP 是 asyncio 的，我们的设备 I/O 是阻塞的**

探测可能几分钟。绝不能在 async 函数里直接做同步 socket 调用——会卡死整个事件循环。

```python
# ✅ 正确
result = await asyncio.to_thread(self._client.call, command)

# ❌ 错误：阻塞事件循环，progress / cancel / 其他工具全部停摆
result = self._client.call(command)
```

**2. Pydantic 生成的 schema 必须与文档一致**

43 个工具手写 `inputSchema`/`outputSchema` 极易与实现漂移。
用类型注解 + Pydantic 自动生成，再写单测**反过来校验文档里的 JSON Schema**。

### 骨架

```python
# mcp_server/server.py
import asyncio
from mcp.server.fastmcp import FastMCP, Context
from pydantic import Field

mcp = FastMCP(
    "unreal-memory-tools",
    instructions=INSTRUCTIONS,          # §3 全文
    capabilities={"tools": {"listChanged": True},
                  "resources": {"listChanged": True},
                  "logging": {}},
)

@mcp.tool(
    name="scanGNames",
    title="扫描 UE 全局名字表候选",
    annotations={"readOnlyHint": True, "openWorldHint": False},
)
async def scan_gnames(
    anchor_names: list[str] = Field(
        default=["None", "ByteProperty", "IntProperty"],
        description="引擎内建名锚点，建议至少 3 个"),
    max_candidates: int = Field(default=200, ge=1, le=2000),
    ctx: Context = None,
) -> dict:
    """<use_case>扫描 GNames / FNamePool 的候选地址。</use_case>
    <important_notes>只返回候选摘要，必须用 sampleGNames 取样本后判定。</important_notes>"""
    # 阻塞 I/O 必须丢到线程，否则卡死事件循环
    return await asyncio.to_thread(bridge.scan_gnames, anchor_names, max_candidates)


@mcp.resource("umt://processes")
async def processes() -> str:
    """当前可分析的进程列表。"""
    return await asyncio.to_thread(bridge.list_processes_markdown)


@mcp.tool(name="startProbe")
async def start_probe(wait_ms: int = Field(default=5000, ge=0, le=60000)) -> dict:
    """<use_case>启动探测流程。</use_case>
    <important_notes>waitMs 内完成则直接返回结果，超时返回 jobId 供轮询。</important_notes>"""
    ...
```

---

## 5.6 ❗设备端代码清单（**v1.1 新增，澄清"不搬 profile ≠ 不写代码"**）

> 不移植 AndUEDumper 的 24 个 profile，**不等于** `src/` 不用改。
> 设备侧新增约 **3000 行 C++**，另有一批"已有但从未接线"的能力需要接上。

### 5.6.1 已有且已接线（零改动，直接用）

| 能力 | 位置 |
|---|---|
| 读/写/扫描/ELF/转储 | `KittyMemoryMgr` |
| pattern 与字节搜索 | `KittyScanner` |
| 指针可读性校验 | `KittyPtrValidator` |
| 远程读值 / ADRP 解码 | `UEMemory`（含 `DecodeADRL` / `Decode_ADRP_ADD` / `Decode_ADRP_LDR`） |
| UE 层 / AutoFix / Dumper | `src/UE`、`src/AutoFix`、`src/Dumper.cpp` |

### 5.6.2 已有但**从未接线**（要写胶水，这是最划算的部分）

| 能力 | 位置 | 现状 | 需要补 |
|---|---|---|---|
| **`findSymbol`（符号定位）** | `KittyScanner.hpp:211` | **`src/` 里零调用** | 符号优先策略（优先级①）的全部逻辑 |
| **`KittyTraceMgr`（ptrace 远程调用）** | `KittyTrace.cpp:140` | **`src/` 里零引用** | 实例化 + 批量封装 + scratch + 会话看门狗 |

> `src/` 中出现的 `GUObjectArray` 字样仅 2 处
> （`executable.cpp:437` / `:491`），都是**输出标签**，不是符号查找。

### 5.6.3 完全不存在（全新代码，约 3000 行）

| 模块 | 内容 | 预估行数 |
|---|---|---|
| 命令服务 | socket 监听 + `HELLO` 握手 + 协议收发 | 400 |

> 🔴 **命令分级（v1.2，issue #8）**：`CommandServer` 收到命令后先分级——
> **快速命令**（ping / getLogs / getProbeStatus / readMemory≤4096 / listProcesses）在
> **命令线程直接执行**，不进 `gWorkerThread`；**重活**（scanGNames / scanObjects / dumpSDK /
> callRemoteFunction）才投递 worker。否则分钟级扫描期间 ping/getLogs 全排队，AI 误判掉线。
> 两类命令以 `DumpUiState` mutex 保护的数据为界，避免竞态。
>
> 🔴 **安全（v1.2，issue #4）**：命令服务 `bind("127.0.0.1")`，详见 §2.1 修正 5。
> 绝不监听 `0.0.0.0`；HELLO 后无需认证可直接调用。
| 命令队列与分发 | mutex + condition_variable + 分发表 | 250 |
| 符号定位 | 按版本试 4 个引擎原生符号 + 解引用回减 | 150 |
| **多锚点改造** | 改 `GetNamesPtr()`，用上 §2.3 那份名单 | 80 |
| 参数化扫描 | `scanGNames` / `scanObjects`（指针范围、锚点、区域可调） | 400 |
| 采样 | 取名字/对象样本（AI 判定命脉） | 200 |
| ptrace 胶水 | `callRemoteFunctionBatch` + `allocScratch` + 会话看门狗 | 300 |
| 类索引与查询 | 建索引 + `searchClasses` / `describeClass` | 300 |
| Sample/Job 管理 | 长任务 jobId、进度、取消 | 150 |
| 复合操作 | `followPointerChain` 等 | 150 |
| 高层用例 | `locateEngineGlobals` / `dumpSDK` | 350 |
| **接线** | `executable.cpp` 起线程 + 每帧 poll（±10 行） | 15 |

**PC 侧另需约 900 行 Python**（server + 工具定义 + instructions + 反汇编封装）。

### 5.6.4 建议实施顺序（每项都可独立验证）

```
① 多锚点改造（80 行）        ← 最小改动，最先验证价值
② 符号定位接线（150 行）      ← 最快路径，性价比最高
③ extract 出的 pattern 库接入  ← 数据已备好（ue_pattern_library.json）
④ 命令服务 + 队列（650 行）    ← 打通链路，此后才能远程驱动
⑤ 参数化扫描 + 采样（600 行）  ← AI 闭环的核心
⑥ ptrace 胶水（300 行）        ← 加密场景与函数级验证
⑦ 类索引 + 高层用例（650 行）  ← 体验层
```

---

## 5.1 目录结构

```
Andriod_UnrealMemoryTools/
├── src/mcp/                          ← 新增
│   ├── CommandServer.cpp/hpp          监听 + 握手(HELLO)
│   ├── CommandQueue.cpp/hpp           mutex + condition_variable
│   ├── CommandDispatcher.cpp/hpp      命令 → 实现
│   ├── SessionRegistry.cpp/hpp        attach 会话看门狗（四重兜底）
│   ├── JobRegistry.cpp/hpp            长任务 jobId / 取消
│   ├── Commands/
│   │   ├── Primitives.cpp             D 组
│   │   ├── PtraceCalls.cpp            F 组（含 Batch 原子实现）
│   │   ├── EngineSemantics.cpp        G 组
│   │   ├── HighLevel.cpp              H 组
│   │   └── Compound.cpp               I 组
│   └── JsonHelpers.cpp/hpp
├── src/executable.cpp                ← 仅增约 10 行：起线程、每帧 poll
└── mcp_server/                       ← PC 侧（仓库顶层新建）
    ├── mcp_server.py                  stdio server + 能力声明 + 日志透传
    ├── tools.py                       工具定义（名称/描述/inputSchema/outputSchema/annotations）
    ├── resources.py                   §2.9 的资源
    ├── instructions.py                §3 全文
    ├── disassemble.py                 capstone 封装（可缺失降级）
    ├── requirements.txt               仅 capstone，锁版本
    └── test_mcp_server.py
```

**`executable.cpp` 侵入最小化**：只在 `main()` 起线程、
在 `Layout_tick_UI()` poll 一次队列。

## 5.2 提交规范

沿用项目既有 **Conventional Commits**。破坏性变更加 `!`。

红线（沿用既有约定）：提交前检查 `*.pem`、`.workbuddy/` 未误提交；
`dev` 分支仅本地提交，用户明确说"推送"才推。

## 5.3 测试策略

| 层次 | 内容 |
|---|---|
| 单元 | 协议收发、hex 编解码、参数校验、struct pack/unpack |
| 契约 | 每个工具返回严格符合 `outputSchema`（**自动化校验**） |
| 故障注入 | 漏调 `endAttachSession` → 看门狗必须 detach；写不可写地址 → 必须拒绝 |
| 集成 | 用用户自己的 UE4 游戏跑完整流程（有 ground truth） |

**注意源项目的坑**：`test_mcp_server.py` 第 8 行
`from mcp.mcp_server import ...` 与目录结构不匹配，跑不起来。
我们从一开始就把包结构定对。

## 5.4 依赖

| 依赖 | 用途 | 策略 |
|---|---|---|
| `mcp`（官方 SDK） | 协议 / 工具 / resources / logging / progress | **锁版本**，跟着协议走 |
| `capstone` | PC 侧反汇编 | **锁版本**，可缺失降级 |
| `pydantic`（随 mcp 带入） | schema 生成与校验 | 随 SDK |

**不再声称"零依赖"**——v1.0 那个说法在引入 capstone 时就已经不成立了。
依赖策略是：**少而锁定，全部 PC 侧，设备端仍然零新增依赖**。

## 5.5 有理由的偏离（沿用 v1.0 裁决）

**"避免描述性错误"不采纳。** 该建议前提是"多租户 / 不可信客户端 / 防泄露内部路径"。
本项目是**本地单用户调试工具，客户端就是用户自己**，
详细错误（含地址、错误码）是**核心功能而非风险**——AI 要靠它决定下一步换什么参数。
已记录；若将来暴露到不可信环境需重新评估。

**Docker 打包不适用**：需 adb + 真机 + root，容器化无收益。

---

# 第六部分　落地检查清单

### 工具定义
- [ ] 全部 camelCase，与 §1.1 一致（43 个，不超 45）
- [ ] 每个工具有中文 `title`
- [ ] 描述含 `<use_case>` + `<important_notes>` + 中文同义词 + 下一步指引
- [ ] **每个工具有 `outputSchema`** 且与 `structuredContent` 严格一致
- [ ] **每个工具有 `annotations`**（§1.6）
- [ ] 每个可选参数有 `default`，数值有 min/max，数组有 maxItems
- [ ] **没有任何搜索类工具返回 "not found"**（§1.4）

### 协议与安全
- [ ] `initialize` 声明 tools.listChanged / resources / logging
- [ ] **协议错误走 JSON-RPC error，执行失败走 isError:true**（§2.1 修正 2）
- [ ] 设备端 `HELLO` 握手 + 协议版本校验
- [ ] **`callRemoteFunctionBatch` 是无状态原子操作**（§2.6）
- [ ] attach 会话四重兜底：maxHoldMs / 空闲看门狗 / 异常兜底 / 断连兜底
- [ ] `getCapabilities` 回报 `activeAttachSession`
- [ ] **`writeMemory` 用 KittyPtrValidator 校验映射可写**
- [ ] **`callRemoteFunction` 需 `confirmDangerous`**
- [ ] `cancelJob` 已实现，worker 有协作式取消点

### 执行与体验
- [ ] 长任务支持 `waitMs` 短等，超时才返 jobId
- [ ] 工具实现内无 stdout 操作（日志走 stderr）
- [ ] 扫描候选按 session 缓存
- [ ] `getLogs` + MCP logging 双通道
- [ ] 大文件不走 resources，只走服务端过滤

### 文档
- [ ] Instructions 已落地（§3 或更好版本）
- [ ] 《功能说明》工具名已同步
- [ ] 新增工具同步两份文档 + PR 里写明增加的常驻 token

---

## 附录 A　与《功能说明》的差异

| 项 | 《功能说明》 | 本文档 | 原因 |
|---|---|---|---|
| 工具命名 | snake_case | **camelCase** | 最佳实践 1.1 |
| 工具描述 | 一句话 | **`<use_case>` + `<important_notes>`** | 最佳实践 1.3 |
| 工具总数 | 约 35 | **41**（上限 45） | +高层 3 +复合 2 +getLogs/cancelJob，合并 3 |
| 搜索无结果 | 未规定 | **禁止 "not found"** | 最佳实践 1.4 |
| Server Instructions | 未涉及 | **§3 全文** | 最佳实践 2.1 |
| 依赖 | 称"零依赖" | **capstone 唯一可选依赖** | 反汇编需求 |
| 长任务 | 一律异步 | **短等优先，超时才异步** | 省往返（v1.1） |
| attach | 会话式 | **批处理无状态优先** | 防泄漏（v1.1） |
| 写内存 | 未规定校验 | **强制映射权限校验** | 防崩溃（v1.1） |

## 附录 B　关键代码位置索引

见《功能说明》第 10 章。本文档新增引用：

| 内容 | 位置 |
|---|---|
| `DumpUiState` + mutex（复用其线程模型） | `src/executable.cpp:110-135` |
| worker 线程（复用） | `src/executable.cpp:945-989` |
| 主循环 / 渲染（poll 队列插入点） | `src/executable.cpp:1761` / `:1805` |
| `KittyTraceMgr::callFunction`（F 组基础） | `KittyMemoryEx/KittyTrace.cpp:140` |
| **`KittyPtrValidator`（写内存校验用）** | `KittyMemoryEx/KittyPtrValidator.hpp` |
| CMake 源文件清单（新增文件要登记） | `CMakeLists.txt:48-54` |
| Keystone 关闭开关 | `CMakeLists.txt:16-17`（`-DkNO_KEYSTONE`） |
