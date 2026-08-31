# MCP 逆向分析 UE4 工具优化改造设计文档

> 本文以 docs/MCP逆向分析UE4引擎需补充能力.md 和当前 Alltear_UnrealMemoryToolsNewMcp 实现为基线，目标是把 UE4/UE5 逆向分析闭环改造成“证据足够、响应可控、结果可恢复、重复调用最少”的 MCP 工具链。
>
> 文档版本：1.0  
> 日期：2026-08-31  
> 适用对象：Android UE4.25-4.27、UE4.27+、UE5 的 UMT MCP 分析服务  
> 关键约束：单次 MCP 响应默认不超过 1,200 token，硬上限不超过 3,800 token。

---

## 1. 改造目标

### 1.1 目标不是简单截断

token 优化不能靠静默截断。每次压缩都必须保留：

- 当前状态；
- 结论所需的最小证据；
- 是否完整；
- 下一页或下一步句柄；
- 被省略的字段和恢复方法。

任何截断都要显式返回 truncated、nextCursor 或 omittedFields。

### 1.2 量化目标

| 指标 | 当前常见形态 | 改造目标 |
|---|---:|---:|
| 普通工具默认响应 | 1,000-4,000 token | 200-800 token |
| 单次响应硬上限 | 约 4,000 token | 3,800 token，留协议封装余量 |
| getLogs 诊断一轮 | 50-140 行 | 关键增量 10-30 行 |
| 长任务轮询 | 每 1-2 秒一次 | 1 次长轮询覆盖一个等待窗口 |
| 候选分析往返 | 扫描 → sample → 再验证 | 扫描内嵌最小证据，通常 1-2 次 |
| 原始内存读取 | hex 全量回传 | 摘要优先，原始数据按需分页 |
| 进程重连后分析 | 重新扫描和判定 | 恢复 report，再做一次轻量复验 |
| Probe → Dump | 依赖全局状态 | probeId 绑定，避免重复 Probe |

### 1.3 token 计算口径

设备端和 PC 侧都使用同一套估算，估算值不是模型真实分词结果，而是预算保护值：

~~~
estimatedTokens = ceil(utf8Bytes / 4)
~~~

对中文、代码和十六进制存在偏差，因此预算必须留 5% 以上余量。响应中返回：

~~~
estimatedTokens
budgetTokens
truncated
omittedFields
~~~

服务端在发送前完成预算裁剪，设备端仍返回完整结构化结果到本地 report 文件。

---

## 2. 当前 token 浪费的主要来源

### 2.1 工具描述重复

每个工具都重复描述：

- attach 前置条件；
- 地址格式；
- maps revision；
- 错误码；
- 大文件限制；
- 轮询方式。

这些内容在模型上下文中会长期常驻。应把稳定规则压成一份 instructions 摘要，工具描述只保留参数和一条关键约束。

### 2.2 日志重复拉取

当前日志是环形缓冲，若不传 sinceIndex，AI 会重复读取已经解释过的日志。诊断一次 Probe 往往只需要最后 10-30 行和错误附近窗口，不需要整个 1500 行缓冲。

### 2.3 候选信息冗余

候选对象同时携带 address、slotAddress、valueAddress、evidence、failedChecks、layout 和长文本 reason。对于排序阶段，只需要：

~~~
candidateId
address
score
confidence
topEvidence[3]
failedCheckCodes[3]
layoutId
~~~

完整证据写入设备端报告，只有选中候选后再按 candidateId 取细节。

### 2.4 轮询没有等待语义

startProbe 和 startDump 只返回 started，状态工具没有统一 jobId 和 waitMs。客户端只能自己 sleep，再重复发送状态请求，产生大量无效请求和响应 token。

### 2.5 原始数据没有摘要层

readMemory 返回 hex；大文件 readOutputFile 直接转为 adb pull。两种方式都缺少：

- hash；
- 可读摘要；
- 指定字段投影；
- 增量游标；
- 数据是否发生变化的判断。

### 2.6 失败后没有可恢复报告

候选、映射、Probe 结果和验证样本主要存在内存或对话里。服务重启后需要重新扫描，既浪费 token，也可能因为 maps 或运行时状态变化得到不同结果。

---

## 3. 总体架构

改造后将响应链路拆成四层：

~~~
AI 需要的结论
      │  brief / fields / budgetTokens
      ▼
PC 侧编排层
      │  sessionId / jobId / reportId
      ▼
设备端分析层
      │  完整证据写 report 文件
      ▼
Android 目标进程
~~~

### 3.1 控制通道和数据通道分离

控制通道只返回：

- 状态；
- 摘要；
- 句柄；
- 关键证据；
- 下一步。

数据通道负责：

- 完整 Logs；
- 完整 Objects；
- SDK 大文件；
- 原始内存块；
- 动态库镜像。

控制通道响应不得携带几十 MB 文件。数据通道返回 size、sha256、path 和 chunk 读取方式。

### 3.2 工具数量不增加为目标

常驻工具保持不超过 45 个。优先扩展已有工具：

- listModules；
- scanPattern；
- searchMemory；
- findReferences；
- scanGNames；
- sampleGNames；
- scanObjects；
- sampleObjects；
- locateEngineGlobals；
- applyProbeOverrides；
- startProbe；
- startDump；
- getLogs；
- readOutputFile。

复杂操作通过 mode、fields、budgetTokens 和 sessionId 扩展，不为每个字段再增加一个工具。

---

## 4. 统一响应预算协议

### 4.1 公共请求参数

以下参数可由所有扫描、状态和文件工具按需支持：

~~~json
{
  "brief": true,
  "fields": ["status", "progress", "topEvidence"],
  "budgetTokens": 800,
  "cursor": null,
  "limit": 20,
  "waitMs": 5000,
  "sessionId": "optional",
  "jobId": "optional"
}
~~~

含义：

| 参数 | 行为 |
|---|---|
| brief | 只返回决策所需摘要 |
| fields | 服务端字段投影，未列出的字段不序列化 |
| budgetTokens | 本次响应预算，范围 128-3,800 |
| cursor | 读取已缓存结果的下一页，不重复扫描 |
| limit | 数组项目上限，默认 20 |
| waitMs | 长轮询等待窗口，最长 60,000 ms |
| sessionId | 复用候选或分析会话 |
| jobId | 指定状态或取消目标 |

### 4.2 统一响应包络

~~~json
{
  "schemaVersion": "1.1",
  "requestId": "r-123",
  "status": "ok",
  "brief": true,
  "estimatedTokens": 412,
  "budgetTokens": 800,
  "truncated": false,
  "omittedFields": [],
  "nextCursor": null,
  "data": {}
}
~~~

错误也使用相同包络：

~~~json
{
  "schemaVersion": "1.1",
  "requestId": "r-124",
  "status": "error",
  "error": {
    "code": "E_MAP_STALE",
    "retryable": true,
    "action": "REFRESH_SNAPSHOT"
  },
  "estimatedTokens": 180,
  "budgetTokens": 800
}
~~~

### 4.3 字段投影规则

服务端先按 fields 删除字段，再按预算执行第二层裁剪：

1. 删除未请求字段；
2. evidence 保留最高分前三项；
3. samples 保留首项、核心锚点和末项；
4. 长字符串保留前后窗口并给出 hash；
5. 数组返回 nextCursor；
6. 最后才允许截断 description。

禁止先生成完整 JSON 再让模型忽略，因为被忽略的字段仍然消耗响应和上下文。

---

## 5. 低 token 的证据模型

### 5.1 reason code 代替长文本

设备端内部保留完整说明，MCP 默认只返回稳定 code：

| code | 含义 |
|---|---|
| PTR_READABLE | 地址和指针可读 |
| BLOCK0_READABLE | FNamePool Blocks[0] 可读 |
| FNAME_NONE | ID 0 解码为 None |
| FNAME_ANCHOR | 命中 UE 内建名称 |
| FNAME_PRINTABLE | 名称字符合法 |
| OBJ_COUNT_RANGE | 对象数量在合理范围 |
| OBJ_CLASS_READABLE | ClassPrivate 可读 |
| MAP_STABLE | 扫描期间 maps 未变化 |
| NAME_MISALIGNED | 名称疑似错位 |
| STATIC_ASCII_LIKE | 候选疑似静态字符串表 |

### 5.2 候选摘要格式

~~~json
{
  "candidateId": 3,
  "address": "0x7970d275c0",
  "kind": "FNamePool",
  "score": 93,
  "confidence": "HIGH",
  "topEvidence": [
    "BLOCK0_READABLE",
    "FNAME_NONE",
    "FNAME_ANCHOR"
  ],
  "failedCheckCodes": [],
  "layoutId": "ue4_pool_16_40_2_6",
  "detailReportId": "report-..."
}
~~~

完整 Header、样本和失败原因写入 report，不在默认响应中重复。

### 5.3 证据去重

同一字符串、同一 map、同一布局只出现一次。对象候选中的名称引用 FName 候选的 sampleId，不重复携带文本。

---

## 6. 长任务和轮询改造

### 6.1 所有重任务统一 JobRegistry

以下操作必须统一返回 jobId：

- startProbe；
- startDump；
- dumpSdk；
- scanPattern；
- searchMemory；
- findReferences；
- scanGNames；
- scanObjects；
- locateEngineGlobals；
- dumpUnrealLibrary。

startProbe 和 startDump 当前是裸线程入口，必须接入 JobRegistry。

### 6.2 长轮询

getProbeStatus 和 getDumpStatus 支持 waitMs：

~~~json
{
  "jobId": "probe-123",
  "waitMs": 10000,
  "fields": ["status", "phase", "progress", "error"]
}
~~~

未完成时设备端保持连接，直到：

- 状态变化；
- 任务完成；
- waitMs 到期；
- 连接断开。

响应示例：

~~~json
{
  "jobId": "probe-123",
  "status": "running",
  "phase": "VALIDATE_NAMES",
  "progress": 0.64,
  "suggestedWaitMs": 3000,
  "estimatedTokens": 90
}
~~~

这样原本 10 次状态请求可以压成 1-2 次。

### 6.3 取消只影响指定任务

cancelJob 必须接受 jobId：

~~~json
{
  "jobId": "probe-123"
}
~~~

不存在或已完成时返回明确状态，不修改其他任务。

### 6.4 状态字段分级

默认状态只返回：

~~~json
{
  "jobId": "dump-123",
  "status": "running",
  "phase": "DUMP_OBJECTS",
  "progress": 0.42,
  "suggestedWaitMs": 5000
}
~~~

只有 fields 明确请求时才返回 jobs 列表、错误堆栈和详细计数。

---

## 7. 内存和扫描工具改造

### 7.1 readMemory 摘要优先

新增 encoding 和 summary：

~~~json
{
  "address": "0x79774c0000",
  "size": 128,
  "encoding": "base64",
  "summary": {
    "u16": [286, 16, 450, 18],
    "asciiPreview": "None.ByteProperty.IntProperty"
  },
  "budgetTokens": 300
}
~~~

规则：

- 默认返回 summary 和 sha256；
- encoding=base64 比 hex 少约三分之一文本；
- 只有明确 fields=raw 或 encoding=hex 才回原始内容；
- size 大于预算时自动分块并返回 nextCursor；
- 原始内存仍写入 report，便于复核。

### 7.2 scanPattern/searchMemory

默认只返回前五个命中和统计：

~~~json
{
  "sessionId": "search-123",
  "complete": true,
  "returned": 5,
  "totalHits": 17,
  "topHits": [
    {
      "address": "0x...",
      "moduleOffset": "0x...",
      "mapId": "map:...",
      "contextHash": "sha256:..."
    }
  ],
  "scannedBytes": 12345678,
  "readErrors": 0,
  "nextCursor": "5"
}
~~~

完整命中通过 cursor 读取，不重复扫描。contextHex 默认关闭，按需请求前后各 16 字节。

### 7.3 listModules

模块列表默认字段：

~~~json
{
  "mapRevision": "14720:...",
  "modules": [
    {
      "moduleId": "m1",
      "name": "libUE4.so",
      "baseAddress": "0x...",
      "endAddress": "0x...",
      "segmentCount": 4
    }
  ],
  "anonymousCount": 18
}
~~~

只有 fields 包含 segments 时才发送所有段。segments 按 cursor 分页，避免把全部 maps 固定塞进上下文。

### 7.4 followPointerChain

结果只返回每一步的地址、读取值和状态，不回传重复 map 描述：

~~~json
{
  "steps": [
    {"i": 0, "address": "0x...", "value": "0x...", "ok": true},
    {"i": 1, "address": "0x...", "value": "0x...", "ok": true}
  ],
  "terminal": "0x...",
  "valid": true
}
~~~

增加数组步进和失败即停，减少 AI 重新解释整条链。

---

## 8. FNamePool、GUObjectArray 和 GWorld 专项优化

### 8.1 一个编排调用内完成候选初筛

扩展 locateEngineGlobals 的 mode=closed_loop：

~~~json
{
  "mode": "closed_loop",
  "region": "ELF_SEGMENTS",
  "budgetTokens": 1200,
  "maxCandidates": 5,
  "includeSamples": true,
  "includeRawEvidence": false
}
~~~

设备端内部执行：

~~~
读取 maps
→ 符号优先
→ FNamePool 结构扫描
→ Blocks[0] 和 None 验证
→ GUObjectArray 结构扫描
→ 对象样本和类名验证
→ GWorld 槽位/对象双重验证
→ 返回 Top-K 候选和 applyProbeOverrides 建议
~~~

默认只回 Top-K，不把所有低分候选送给模型。

### 8.2 FNamePool 最小样本

每个候选默认只回：

- block0 地址；
- ID 0；
- 两个 UE 内建 anchor；
- 一个连续性统计；
- 一个失败 code 集合。

### 8.3 GUObjectArray 最小样本

只返回前 3 个可读对象、一个 Package、一个 Class 和统计值。不要默认返回 32-200 个对象的完整字段。

### 8.4 地址语义强类型化

applyProbeOverrides 的输入必须明确区分：

~~~json
{
  "namesPoolBase": {"address": "0x...", "semantics": "POOL_BASE"},
  "guObjectArrayBase": {"address": "0x...", "semantics": "FUOBJECTARRAY_BASE"},
  "gWorldSlot": {"address": "0x...", "semantics": "GLOBAL_SLOT", "indirection": 1}
}
~~~

服务端先做语义验证，再写入 pending plan。标量兼容形式保留，但响应必须回显规范化结果。

---

## 9. Probe、Dump 原子闭环

### 9.1 Probe plan

applyProbeOverrides 不再只返回 applied 数量，而是生成不可变 plan：

~~~json
{
  "planId": "plan-123",
  "pid": 14720,
  "processStartTime": "2420",
  "mapRevision": "14720:...",
  "overridesHash": "sha256:...",
  "expiresAt": "...",
  "validated": true
}
~~~

### 9.2 startProbe

~~~json
{
  "planId": "plan-123",
  "waitMs": 10000,
  "budgetTokens": 600
}
~~~

返回：

~~~json
{
  "jobId": "probe-123",
  "probeId": "probe-result-123",
  "planId": "plan-123",
  "status": "succeeded",
  "fieldSources": {
    "names": "OVERRIDE",
    "objects": "OVERRIDE",
    "world": "OVERRIDE"
  }
}
~~~

### 9.3 startDump

~~~json
{
  "probeId": "probe-result-123",
  "preservePrevious": true,
  "waitMs": 10000,
  "budgetTokens": 600
}
~~~

设备端校验 probeId 对应的 pid、start time、mapRevision 和 profile hash。校验通过后复用已初始化的 Profile/Offsets，不再重新 AUTO Probe。

### 9.4 输出目录事务

Dump 写入：

~~~
/sdcard/UnrealMemoryTools/<package>/.runs/<runId>/staging/
~~~

成功后：

~~~
staging → committed
manifest.json
~~~

只有 committed 目录更新 current 指针。失败任务不能删除上一次成功结果。

---

## 10. 大文件和报告优化

### 10.1 readOutputFile 分页

实现原规划中的 offset/limit：

~~~json
{
  "package": "com.huitgames.shikigami.summons",
  "filename": "Objects.txt",
  "offset": 0,
  "limit": 4096,
  "mode": "lines",
  "pattern": "Package|Class",
  "budgetTokens": 900
}
~~~

返回：

~~~json
{
  "filename": "Objects.txt",
  "totalBytes": 3786398,
  "offset": 0,
  "returnedBytes": 4096,
  "content": "...",
  "hasMore": true,
  "nextOffset": 4096,
  "sha256": "sha256:..."
}
~~~

mode=lines 和 pattern 由设备端过滤，避免把整文件送到 PC 或模型。

### 10.2 report 文件

每次分析生成：

~~~
mcp_analysis.json
mcp_analysis.summary.json
mcp_analysis.raw/
~~~

summary 文件只包含：

- 目标进程；
- maps revision；
- 模块基址；
- Top-K 候选；
- 选中候选；
- Probe/Dump 状态；
- 证据 code；
- reportId 和 hash。

新会话默认读取 summary，不读取 raw。

### 10.3 adb 数据通道

保留 adb pull，但 MCP 应返回可执行元数据：

~~~json
{
  "path": "/sdcard/UnrealMemoryTools/...",
  "sizeBytes": 3786398,
  "sha256": "sha256:...",
  "transport": "ADB_PULL",
  "pullCommand": "adb pull ...",
  "verified": false
}
~~~

PC 侧拉取后自动计算 hash，并把 verified=true 写回本地 runtime state。

---

## 11. 面向 token 的 PC 侧改造

### 11.1 tools.py

每个高频工具增加可选参数：

~~~python
brief: bool | None = None
fields: list[str] | None = None
budget_tokens: int | None = None
wait_ms: int | None = None
cursor: str | None = None
limit: int | None = None
~~~

None 参数继续不下发。参数名由 Python snake_case 映射到设备端 camelCase。

### 11.2 instructions.py

常驻 instructions 只保留以下规则：

1. 先 ping、selectProcess、attach；
2. 先取 mapRevision 和 moduleBase；
3. 候选必须带证据；
4. Probe plan 绑定 pid、start time 和 maps；
5. 状态优先长轮询；
6. 大文件读 summary 或服务端过滤；
7. 出现 stale 只刷新 snapshot，不重做全部分析；
8. 只有 report 缺失时才重新扫描。

完整错误表、字段说明和示例移到按需资源，不常驻模型上下文。

### 11.3 bridge.py

PC 侧统一处理：

- budgetTokens 默认值；
- 超限响应二次摘要；
- requestId/jobId 关联；
- waitMs 超时；
- nextCursor 自动保存；
- reportId 本地缓存；
- 重连后 summary 恢复。

不能把原始超大响应先放进模型上下文再裁剪，裁剪必须发生在 bridge 或设备端。

---

## 12. 优化后的实际工作流

### 12.1 正常 Profile 命中

~~~
ping(brief)
→ listProcesses(fields=package,pid,profile)
→ selectProcess
→ attach(fields=pid,startTime,mapRevision)
→ startProbe(waitMs=10000, budgetTokens=600)
→ startDump(probeId, waitMs=10000, budgetTokens=600)
→ listOutputFiles(fields=name,size,sha256)
~~~

目标：6-7 次调用，不读取完整日志。

### 12.2 Probe 失败恢复

~~~
getLogs(sinceIndex=last, maxLines=20, brief=true)
→ locateEngineGlobals(mode=closed_loop, budgetTokens=1200)
→ applyProbeOverrides(plan)
→ startProbe(planId, waitMs=10000)
→ getProbeResults(fields=offsets,structGroups,fieldSources)
→ startDump(probeId, waitMs=10000)
→ listOutputFiles(summary)
~~~

目标：一次候选编排加一次 Probe，不再先做全量日志、全量扫描和人工重复 sample。

### 12.3 maps stale 恢复

~~~
E_MAP_STALE
→ refresh snapshot
→ 仅重验受影响 candidateId
→ 生成新 plan
→ startProbe
~~~

不允许从 ping 重新开始，也不允许直接沿用旧绝对地址。

### 12.4 新会话恢复

~~~
ping
→ listProcesses
→ attach
→ read mcp_analysis.summary.json
→ 校验 pid/startTime/mapRevision
→ 复用 report 或只做轻量样本复验
~~~

---

## 13. 预估 token 节省

以下为预算估算，不代表特定模型的精确分词数：

| 场景 | 旧方式 | 新方式 | 预估变化 |
|---|---:|---:|---:|
| 4096 bytes 内存 hex | 约 2,048 token | base64 + 摘要约 900-1,400 | 减少约 30%-55% |
| 140 行日志 | 约 4,000-6,000 | 增量 20 行约 500-900 | 减少约 75%-90% |
| 50 个候选完整证据 | 约 6,000-10,000 | Top-5 摘要约 800-1,500 | 减少约 75%-90% |
| 10 次状态轮询 | 约 1,000-2,000 | 1 次长轮询约 100-250 | 减少约 75%-90% |
| 大文件读取 | 失败后切 adb，重复描述 | summary + 服务端过滤 | 减少无效往返 |
| 重连后重新扫描 | 数千至数万 | summary 复用 + 轻量复验 | 大幅减少 |

总 token 预算模型：

~~~
T_total =
  T_tool_descriptions
+ T_request_envelopes
+ T_status
+ T_evidence
+ T_raw_data
~~~

改造优先削减：

~~~
T_tool_descriptions
T_status
T_raw_data
~~~

不能削减：

~~~
选中候选的最小证据
最终 Probe/Dump 状态
hash、revision 和恢复句柄
~~~

---

## 14. 兼容性和迁移策略

### 14.1 保持旧调用可用

旧客户端不传 brief、fields、budgetTokens 时：

- 使用安全默认值；
- 返回旧字段；
- 增加 estimatedTokens 和 truncated；
- 不改变地址字符串格式。

### 14.2 分阶段启用

| 阶段 | 改造 | 兼容策略 |
|---|---|---|
| Phase 0 | 公共响应包络、预算估算、日志增量 | 旧字段保留 |
| Phase 1 | fields、brief、cursor、limit | 默认开启摘要 |
| Phase 2 | jobId、waitMs、cancelJob(jobId) | 兼容无 jobId 的旧状态调用 |
| Phase 3 | probeId、planId、reportId | dumpSdk 兼容旧一键模式 |
| Phase 4 | 分页文件读取、staging/committed | 保留 adb pull |

### 14.3 失败兼容原则

新字段缺失时，PC 侧按旧响应处理；旧设备不理解新字段时，服务端回退到 v1 请求格式，但必须记录 capability downgrade，避免模型误以为启用了长轮询或分页。

---

## 15. 实现落点

### 15.1 设备端

建议新增或拆分：

~~~
src/mcp/analysis/ResponseBudget.{hpp,cpp}
src/mcp/analysis/ReportStore.{hpp,cpp}
src/mcp/analysis/AnalysisSnapshot.{hpp,cpp}
src/mcp/analysis/CandidateEvidence.{hpp,cpp}
src/mcp/analysis/OutputManifest.{hpp,cpp}
src/mcp/JobRegistry.{hpp,cpp}
~~~

重点修改：

- executable.cpp：START_PROBE、START_DUMP、DUMP_SDK 统一 JobRegistry；
- executable.cpp：applyProbeOverrides 返回 planId；
- executable.cpp：Dump 使用 probeId，禁止隐式重跑；
- MemoryAnalysis.cpp：字段投影、摘要、cursor 和预算；
- UECandidateAnalysis.cpp：reason code、Top-K、证据引用；
- 输出保存：staging/committed 和 manifest；
- READ_OUTPUT_FILE：offset/limit/pattern/sha256。

### 15.2 PC 侧

重点修改：

- mcp_server/src/umt_mcp/tools.py：暴露新参数；
- mcp_server/src/umt_mcp/bridge.py：预算裁剪、长轮询、句柄缓存；
- mcp_server/src/umt_mcp/instructions.py：删除重复说明，加入最短恢复路径；
- mcp_server/src/umt_mcp/resources.py：提供 summary、active job 和 report；
- mcp_server/tests：预算、分页、jobId、stale 恢复和旧协议兼容测试。

---

## 16. 测试和验收

### 16.1 响应预算测试

- 默认响应 estimatedTokens ≤ 1,200；
- 硬上限 estimatedTokens ≤ 3,800；
- budgetTokens 降低时只减少可选字段；
- truncated=true 时一定有 nextCursor 或 omittedFields；
- 不允许把半截 JSON 发给客户端。

### 16.2 闭环测试

- applyProbeOverrides → startProbe → startDump 不重新 AUTO Probe；
- probeId 不匹配时返回 E_PROBE_MISMATCH；
- maps stale 只重验候选，不重扫全部内存；
- cancelJob(jobId) 不影响其他任务；
- 一个 Job 只产生一个终态。

### 16.3 token 回归测试

固定合成 fixture，记录：

~~~
baseline_tokens
optimized_tokens
reduction_ratio
response_bytes
field_count
poll_count
scan_count
~~~

验收阈值：

- 普通诊断路径 token 至少减少 50%；
- 失败恢复路径 token 至少减少 60%；
- 长任务轮询次数减少 70%；
- 相同 maps 和参数的重复扫描次数为 0；
- 关键信息召回率 100%：状态、地址、revision、证据 code、恢复句柄不得被裁掉。

### 16.4 真机验收

至少覆盖：

1. UE4.27 FNamePool 正常；
2. 错误静态 ASCII 候选；
3. E_MAP_STALE；
4. ERROR_EMPTY_PACKAGES；
5. 大型 Objects.txt；
6. 服务重启后 report 恢复；
7. Probe override 后 Dump 不重复 Probe；
8. UE5 libUnreal.so。

---

## 17. 最终改造优先级

### P0：立即减少无效 token

- 公共响应预算；
- brief 和 fields；
- getLogs sinceIndex；
- status waitMs 长轮询；
- 候选 Top-K 和 reason code；
- startProbe/startDump 接入统一 jobId。

### P1：消除重复分析

- planId/probeId；
- maps snapshot 和 stale 局部重验；
- report summary 持久化；
- cursor/session 复用；
- readOutputFile 服务端过滤。

### P2：降低原始数据成本

- readMemory 摘要和 base64；
- 原始内存 report；
- 大文件 manifest、hash 和分块；
- Dump staging/committed；
- capability downgrade 和旧协议兼容。

---

## 18. 一句话规范

~~~
先返回结论，再返回证据；
先返回摘要，再按句柄取详情；
先复用 session，再决定是否重扫；
先绑定 probeId，再执行 Dump；
先控制 token，再扩大数据量。
~~~

完成这些改造后，MCP 的优化结果不是“少返回一些内容”，而是把完整证据留在设备端，把模型真正需要的决策信息压缩到稳定预算内，同时保证失败可恢复、结果可追溯、Probe 不重复、Dump 不回归。

