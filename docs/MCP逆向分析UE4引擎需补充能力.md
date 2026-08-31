# MCP 逆向分析 UE4 引擎需补充能力

> 本文定义 UMT MCP 在 UE4/UE5 自动探针失败后，继续完成内存逆向、候选验证、
> `GUObjectArray` / `GWorld` 定位、探针覆盖与 SDK Dump 所必须补齐的能力。
> 内容以当前设备端 `Alltear_UnrealMemoryToolsNewMcp`、PC 侧 `mcp_server` 和真机调用结果为依据。

- 文档版本：1.0（2026-08-31）
- 适用协议：UMT wire protocol v1
- 当前基线：43 个 MCP 工具、43 条设备端命令
- 目标：建立“探针失败后仍可自主诊断并恢复”的完整逆向闭环

---

## 1. 结论

当前 MCP 已具备连接、进程选择、内存读取、基础 pattern 扫描和 SDK Dump 等能力，
但这些能力以“探针已经成功”为中心组织。探针一旦在 `FNamePool`、`GUObjectArray`
或 `GWorld` 任一步失败，多个本应承担诊断职责的工具也会被 `E_NOT_READY` 门禁阻断。

因此，当前系统的问题不是工具数量不足，而是缺少以下闭环：

```text
attach
  -> 获取完整真实内存映射
  -> 在明确区间内搜索字符串 / pattern / 指针 / ARM64 引用
  -> 生成 FNamePool / GUObjectArray / GWorld 候选
  -> 对每个候选做独立结构验证并返回证据
  -> applyProbeOverrides
  -> startProbe 复跑
  -> dumpSdk
```

补充能力必须遵循一个核心边界：

- **诊断层只依赖 attach**，不得依赖 Probe 成功。
- **语义层可以依赖已验证候选**，但不能只接受全局 Probe 状态。
- **编排层允许返回部分结果**，不能把“未全部定位”折叠成无数据。

---

## 2. 真机失败链路暴露的问题

一次 UE4.27 真机探测得到以下结果：

```text
MCP PING              -> connected=true, protocol=1
LIST_PROCESSES        -> 成功
SELECT_PROCESS        -> 成功
ATTACH                -> attached=true, isMemValid=true
START_PROBE           -> started=true
GetNamesPtr           -> 找到 FNamePool
GetGUObjectArrayPtr   -> 失败
GET_PROBE_STATUS      -> finished=true, success=false
错误                  -> ERROR_INIT_GUOBJECTARRAY
```

在失败后的诊断阶段又观察到：

1. `readMemory` 可以正常读取任意已知地址。
2. `scanPattern` 可以回扫并命中已知 ELF 头字节，说明基础扫描器可用。
3. UE4.25-4.27 现有 pattern 库未命中该目标。
4. `resolveSymbol` 因 Probe 失败返回 `E_NOT_READY`。
5. `dumpUnrealLibrary` 因 Probe 失败返回 `E_NOT_READY`。
6. `locateEngineGlobals` 在没有 Probe 结果时只返回手动扫描提示，没有执行真实回退定位。
7. `listModules` 丢失同一路径的后续映射和匿名 `.bss`，不能给扫描器提供可靠区间。

这说明 `readMemory` 本身不是主要瓶颈。真正缺少的是从“未知地址”发现候选、验证候选，
并把候选重新注入探针的设备端搜索能力。

---

## 3. 当前能力缺口

### 3.1 循环前置条件

| 工具 | 当前门禁 | 为什么形成死循环 | 目标门禁 |
|---|---|---|---|
| `resolveSymbol` | Probe 必须成功 | 符号解析本应帮助 Probe 找全局变量 | 仅需 attach + Unreal ELF 有效 |
| `dumpUnrealLibrary` | Probe 必须成功 | Probe 失败时最需要拿 SO 离线诊断 | 仅需 selectProcess + attach |
| `scanObjects` | `RequireObjects()` | `RequireObjects()` 又依赖 GUObjectArray 已初始化 | 支持 pre-probe 候选模式 |
| `sampleGNames` | Probe 必须成功 | 无法验证 `GetNamesPtr` 的中间候选 | 接受候选地址和布局参数 |
| `sampleObjects` | Probe 必须成功 | 无法验证 GUObjectArray 候选 | 接受候选地址和布局参数 |
| `locateEngineGlobals` | 主要读取 ProbeResult | Probe 失败时没有有效输入 | attach 后可独立执行 |

### 3.2 内存映射信息丢失

当前 `LIST_MODULES`：

- 跳过 `pathname.empty()` 的匿名区域；
- 使用 pathname 去重；
- 同一路径只返回第一次出现的映射；
- 没有返回文件偏移、inode、设备号和 map 类型；
- 无法表达一个 ELF 的 `r--` / `r-x` / `rw-` / `.bss` 多段布局；
- 无法给出映射快照版本，长扫描期间无法判断 maps 是否已经变化。

这会直接影响：

- 代码 pattern 扫描；
- `.data/.bss` 全局变量扫描；
- 指针引用搜索；
- ADRP 页目标过滤；
- 候选地址归属和可信度判断。

### 3.3 扫描范围无法完整表达

设备端 `SCAN_PATTERN` 已读取 `module` 或 `start + end`，但 PC 工具只暴露：

```python
scan_pattern(pattern, module=None, max_results=None)
```

缺失能力包括：

- PC 侧无法传 `start/end`；
- module 模式只选第一个匹配 map，而不是该模块的全部段；
- 无法按权限选择 `r-x`、`r--`、`rw-`；
- 无法一次指定多个 map；
- 无分页、无扫描统计、无取消检查点；
- 不能返回因读失败而跳过的区间；
- 不支持字符串、整数和指针值搜索。

### 3.4 缺少交叉引用能力

定位 UE 全局变量不能只依赖固定 pattern。至少需要识别：

- 内存中的原始 64 位指针引用；
- ARM64 `ADRP + ADD`；
- ARM64 `ADRP + LDR`；
- ARM64 literal load；
- 跳板函数中的间接引用；
- 指向候选槽位和指向槽位内容的两种引用。

当前 `decodeAdrl(address)` 只能解码已知地址，缺少“扫描哪些指令引用目标”的反向发现过程。

### 3.5 候选验证与候选生成耦合

当前 `GetGUObjectArrayPtr()` 在扫描过程中直接要求候选首对象解析成固定锚点。
如果名字布局、对象起始下标、对象数组布局或扫描距离任一假设不符，候选会被直接丢弃，
AI 看不到次优结果，也无法调整参数重新评分。

正确模型应拆成：

```text
扫描 -> 返回候选 -> 结构验证 -> 名字验证 -> 评分 -> AI 选择或自动选择
```

而不是：

```text
扫描 + 单一硬编码断言 -> 成功地址或 0
```

### 3.6 `GWorld` 没有独立定位通道

当前 Dump 流程主要在对象系统可用后枚举 `UWorld` 对象，再寻找 UE 模块内的指针引用。
这条路径合理，但完全依赖 `GUObjectArray` 成功。缺少：

- 根据 `UWorld` 对象候选搜索全局槽位；
- 基于 ARM64 引用定位 `GWorld`；
- 独立验证 `gWorldPtrAddr` 与 `worldObject`；
- 在对象系统不完整时返回低置信度候选；
- 区分全局槽位地址和槽位解引用后的对象地址。

---

## 4. 设计原则

### 4.1 Attach-only 诊断层

以下操作只应要求 `selectProcess + attach`：

- 完整 maps 枚举；
- 内存读取；
- ELF 段与符号解析；
- pattern / string / value / pointer 搜索；
- ARM64 引用搜索与反汇编；
- SO 转储；
- FNamePool / GUObjectArray / GWorld 候选扫描；
- 候选结构验证。

### 4.2 候选地址不等于事实

每个定位工具都必须返回：

```json
{
  "address": "0x...",
  "kind": "GUObjectArray",
  "score": 82,
  "confidence": "HIGH",
  "evidence": [],
  "failedChecks": [],
  "layout": {},
  "source": "STRUCTURAL_SCAN"
}
```

禁止只返回裸地址，也禁止设备端因一项软校验失败而静默丢弃候选。

### 4.3 地址语义必须明确

所有全局变量统一区分：

| 字段 | 含义 |
|---|---|
| `slotAddress` | UE 模块内保存全局指针或全局结构的地址 |
| `valueAddress` | 从槽位读取到的实际对象/数组地址 |
| `moduleOffset` | `slotAddress - moduleBase` |
| `indirection` | 需要解引用的层数 |

例如 `GWorld` 必须同时返回：

```json
{
  "gWorldPtrAddr": "0x...",
  "worldObject": "0x...",
  "moduleOffset": "0x...",
  "indirection": 1
}
```

### 4.4 保持工具预算

当前配置要求常驻工具不超过 45 个。本文不建议把每种搜索拆成一个新工具。

实施策略：

- 扩展已有 `listModules`、`scanPattern`、`scanGNames`、`scanObjects`、
  `sampleGNames`、`sampleObjects`、`resolveSymbol`、`dumpUnrealLibrary`、
  `locateEngineGlobals`；
- 只新增统一的 `searchMemory` 和 `findReferences`；
- 复杂验证收敛到现有扫描/编排工具的 `validate` 模式，不额外增加常驻工具。

这样常驻工具总数从 43 增至 45，满足现有预算。

---

## 5. P0：解除诊断死锁

### 5.1 扩展 `listModules`

保留工具名，升级为完整映射枚举。

**建议输入：**

```json
{
  "nameFilter": "libUE4.so",
  "includeSegments": true,
  "includeAnonymous": true,
  "permissions": ["r--", "r-x", "rw-"],
  "start": "0x...",
  "end": "0x...",
  "cursor": null,
  "limit": 200
}
```

**建议输出：**

```json
{
  "mapRevision": "pid:starttime:hash",
  "modules": [
    {
      "moduleId": "m1",
      "name": "libUE4.so",
      "path": "/data/.../libUE4.so",
      "baseAddress": "0x...",
      "endAddress": "0x...",
      "segments": [
        {
          "mapId": "m1:s0",
          "start": "0x...",
          "end": "0x...",
          "permissions": "r-x",
          "fileOffset": "0x...",
          "kind": "ELF_LOAD"
        },
        {
          "mapId": "m1:s3",
          "start": "0x...",
          "end": "0x...",
          "permissions": "rw-",
          "fileOffset": "0x...",
          "kind": "BSS_OR_DATA"
        }
      ]
    }
  ],
  "anonymousMaps": [],
  "nextCursor": null
}
```

**验收要求：**

- 同一路径的所有 map 必须保留；
- 匿名可读映射可按需返回；
- `baseAddress/endAddress` 是模块汇总，`segments` 是真实扫描边界；
- 每次重 attach 或 maps 变化时 `mapRevision` 改变。

### 5.2 扩展 `scanPattern`

**建议输入：**

```json
{
  "pattern": "AA BB ? ? CC",
  "module": "libUE4.so",
  "allModuleSegments": true,
  "segmentPermissions": ["r-x"],
  "start": "0x...",
  "end": "0x...",
  "mapIds": ["m1:s1", "m1:s2"],
  "maxResults": 200,
  "cursor": null
}
```

**规则：**

1. `mapIds`、`module`、`start/end` 三种范围来源互斥。
2. module 模式默认扫描该模块全部可读段，而不是第一段。
3. 返回每个命中的所属 map、模块偏移和邻近字节。
4. 返回 `scannedBytes`、`skippedBytes`、`readErrors`、`truncated`。
5. PC `tools.py` 必须真实暴露设备端已经支持的 `start/end`。

### 5.3 解除 `resolveSymbol` 的 Probe 门禁

**新前置条件：**

```text
selected process + attached + Unreal ELF valid
```

**建议输入：**

```json
{
  "symbol": "GUObjectArray",
  "module": "libUE4.so",
  "match": "EXACT_THEN_FUZZY",
  "includeDebug": true,
  "maxResults": 20
}
```

**建议输出：**

```json
{
  "matches": [
    {
      "name": "GUObjectArray",
      "address": "0x...",
      "moduleOffset": "0x...",
      "symbolType": "OBJECT",
      "table": "DYNSYM"
    }
  ]
}
```

符号未找到返回 `E_NOT_FOUND`，不能因 Probe 失败返回 `E_NOT_READY`。

### 5.4 解除 `dumpUnrealLibrary` 的 Probe 门禁

**新前置条件：**

```text
selected process + attached + Unreal module found
```

工具应支持：

- `source: "FILE" | "MEMORY" | "AUTO"`；
- 文件可读时直接复制原始 SO；
- 文件不可读或需要运行时修复时走内存 ELF Dump；
- 返回 SHA-256、文件大小、build-id、设备路径；
- 作为长任务返回 jobId，不占用单次命令到超时。

---

## 6. P0：新增两个通用逆向入口

### 6.1 `searchMemory`

对应设备命令：`SEARCH_MEMORY`。

这是统一搜索入口，避免分别新增字符串、整数、指针工具。

**建议输入：**

```json
{
  "queryType": "ASCII | UTF8 | UTF16LE | HEX | U32 | U64 | POINTER",
  "query": "GUObjectArray",
  "module": "libUE4.so",
  "mapIds": [],
  "start": "0x...",
  "end": "0x...",
  "alignment": 1,
  "permissions": ["r--", "rw-"],
  "maxResults": 200,
  "contextBefore": 16,
  "contextAfter": 32
}
```

**建议输出：**

```json
{
  "sessionId": "scan-...",
  "queryType": "ASCII",
  "hits": [
    {
      "address": "0x...",
      "mapId": "m1:s0",
      "moduleOffset": "0x...",
      "contextHex": "..."
    }
  ],
  "scannedBytes": 123456,
  "truncated": false,
  "nextCursor": null
}
```

**实现要求：**

- `POINTER` 按目标架构小端编码；
- 只扫描选定的可读 maps；
- 支持对上一次 session 重新过滤，无需重扫；
- 每 1-4 MiB 检查取消标志；
- 单响应仍受 4K token 限制，结果分页。

### 6.2 `findReferences`

对应设备命令：`FIND_REFERENCES`。

**建议输入：**

```json
{
  "target": "0x...",
  "module": "libUE4.so",
  "kinds": ["POINTER", "ADRP_ADD", "ADRP_LDR", "LITERAL_LOAD"],
  "segmentPermissions": ["r-x", "rw-"],
  "maxResults": 200,
  "includeDisassembly": true
}
```

**建议输出：**

```json
{
  "target": "0x...",
  "references": [
    {
      "instructionAddress": "0x...",
      "kind": "ADRP_LDR",
      "resolvedTarget": "0x...",
      "moduleOffset": "0x...",
      "instructions": []
    }
  ]
}
```

**ARM64 最低支持：**

- 同一基本块内 `ADRP + ADD`；
- 同一基本块内 `ADRP + LDR`；
- 寄存器被一次 `MOV` 转移后的简单链；
- literal load；
- 对目标本身和 `target & ~0xFFF` 页地址两级过滤。

---

## 7. P1：FNamePool 候选模式

### 7.1 扩展 `scanGNames`

该工具必须能在 Probe 之前运行。

**建议输入：**

```json
{
  "region": "ELF_SEGMENTS | BSS | ALL_READABLE",
  "mapIds": [],
  "minPtr": "0x...",
  "maxPtr": "0x...",
  "anchorNames": ["None", "ByteProperty", "IntProperty", "Object"],
  "anchorOffsets": [4, 6, 8, 10, 12, 16],
  "layouts": [
    {
      "kind": "FNamePool",
      "stride": 2,
      "blocksBit": 16,
      "blocksOff": 64,
      "headerOff": 0,
      "lengthShift": 6
    }
  ],
  "maxCandidates": 200
}
```

**输出候选必须包含：**

- `candidateId`；
- `poolAddress` 与 `slotAddress`；
- 布局参数；
- 命中的锚点和值；
- 精确/宽松命中；
- 失败读取数；
- 评分与降分理由。

### 7.2 扩展 `sampleGNames`

同时支持两种模式：

```json
{"source":"PROBE_RESULT","startIndex":0,"count":32}
```

```json
{
  "source":"CANDIDATE",
  "sessionId":"scan-...",
  "candidateId":3,
  "startIndex":0,
  "count":32
}
```

候选模式不得调用 `RequireObjects()` 或检查 `gProbeResult.success`。

### 7.3 FNamePool 验证规则

建议评分：

| 检查 | 分值 |
|---|---:|
| 地址位于可读 map | +5 |
| Blocks 指针可读 | +10 |
| `None` 命中 | +15 |
| 三个以上内建名命中 | +25 |
| 连续 16 项中至少 12 项可解析 | +20 |
| 字符串长度和字符集合理 | +10 |
| 多个 block 可解析 | +10 |
| 大量粘连、越界或乱码 | -10 至 -40 |

设备端只负责给出证据和评分，不应清洗或猜改原始名字。

---

## 8. P1：GUObjectArray 候选模式

### 8.1 扩展 `scanObjects`

该工具必须支持不依赖 FNamePool 的结构预筛选，也支持使用 names 候选增强评分。

**建议输入：**

```json
{
  "namesSessionId": "scan-...",
  "namesCandidateId": 3,
  "region": "BSS | MODULE_RW | ALL_READABLE",
  "mapIds": [],
  "direction": "UP | DOWN | BOTH | REGION",
  "origin": "0x...",
  "maxDistanceBytes": 67108864,
  "layouts": [
    {
      "objObjectsOff": 16,
      "objectsOff": 0,
      "numElementsOff": 20,
      "chunked": true,
      "numElementsPerChunk": 65536,
      "itemObjectOff": 0,
      "itemSize": 24,
      "namePrivateOffsets": [24, 28, 32, 36, 40]
    }
  ],
  "maxCandidates": 200
}
```

**关键改动：**

1. `maxDistanceBytes` 以字节表达，不能把循环次数和 8 字节步长混淆。
2. 支持直接扫描 `rw-/.bss` maps，不强制围绕 FNamePool。
3. 首对象为空或不是 `/Script/CoreUObject` 时保留候选。
4. 同时测试 flat 与 chunked 两类数组。
5. 结构预筛选不能调用名字解析；名字验证是第二阶段。
6. 候选结果缓存为 session，允许换布局重新评分。

### 8.2 扩展 `sampleObjects`

候选模式建议输出：

```json
{
  "candidateId": 5,
  "arrayAddress": "0x...",
  "numElements": 183245,
  "samples": [
    {
      "index": 0,
      "objectAddress": "0x...",
      "nameId": 1,
      "name": "Object",
      "classAddress": "0x...",
      "outerAddress": "0x...",
      "valid": true
    }
  ],
  "readErrors": []
}
```

### 8.3 GUObjectArray 验证规则

| 检查 | 分值 |
|---|---:|
| FUObjectArray 地址位于 `rw-/.bss` | +10 |
| Objects/Chunks 指针可读 | +15 |
| NumElements 在 1,024-5,000,000 | +10 |
| 前 32 项存在多个可读 UObject | +15 |
| UObject.ClassPrivate 指向可读对象 | +10 |
| NameId 可由已验证 FNamePool 解析 | +20 |
| 命中 `Object` / `Package` / `Class` / `/Script/CoreUObject` | +20 |
| 数量、chunk 或 item stride 自相矛盾 | -20 至 -50 |

评分达到阈值后，返回建议的完整布局覆盖，而不只是数组地址。

---

## 9. P1：GWorld 定位与验证

### 9.1 `locateEngineGlobals` 必须真实执行回退

保留现有工具名，改造成 attach-only 编排器。内部顺序：

1. `detectUEVersion`；
2. attach-only `resolveSymbol`；
3. 按版本扫描代码 pattern；
4. `findReferences` 解出全局槽位；
5. `scanGNames + sampleGNames`；
6. `scanObjects + sampleObjects`；
7. 枚举或筛选 `UWorld` 对象候选；
8. 反向搜索 UE 模块中引用该对象的槽位；
9. 分别验证 `FNamePool`、`GUObjectArray`、`GWorld`；
10. 返回部分结果、证据和可直接用于 `applyProbeOverrides` 的建议。

任何一步失败都不得清空前面已经找到的结果。

### 9.2 UWorld 候选来源

按优先级：

1. `GWorld` 动态/调试符号；
2. 已知版本 pattern + ADRP 解码；
3. 从已验证 GUObjectArray 枚举 `Class Engine.World` 实例；
4. 对世界对象地址执行 `findReferences(POINTER)`；
5. 对候选槽位的代码引用执行 `findReferences(ADRP_*)`；
6. 无对象系统时返回 pattern/引用产生的低置信度候选。

### 9.3 UWorld 最低验证

候选 `slotAddress` 需要满足：

- 位于 UE 模块 `rw-/.bss` 或明确的重定位目标；
- 解引用得到可读地址；
- 连续采样时对象地址稳定或符合关卡切换行为；
- 对象的 ClassPrivate 可读；
- 对象系统可用时类名为 `World` 或派生类；
- `PersistentLevel`、`OwningGameInstance` 等至少一个版本布局字段可读；
- 返回所有尝试过的字段偏移和验证结果。

禁止仅凭“指针非零”判定 `GWorld` 成功。

---

## 10. Probe Override 闭环

### 10.1 扩展 `applyProbeOverrides`

建议输入：

```json
{
  "overrides": {
    "namesPtr": {
      "address": "0x...",
      "semantics": "POOL_BASE",
      "layout": {}
    },
    "guObjectArrayPtr": {
      "address": "0x...",
      "semantics": "FUOBJECTARRAY_BASE",
      "layout": {}
    },
    "gWorldPtr": {
      "address": "0x...",
      "semantics": "GLOBAL_SLOT",
      "indirection": 1
    }
  },
  "validateBeforeApply": true,
  "pid": 12345,
  "processStartTime": 987654,
  "mapRevision": "12345:abcd",
  "sourceSessionIds": ["scan-..."],
  "ttlSeconds": 300
}
```

### 10.2 应用规则

- 覆盖必须绑定当前 pid、进程 start time 和 maps revision；三者任一变化都返回 `E_SESSION_STALE` 或 `E_MAP_STALE`。
- 切换进程、进程重启或 maps revision 改变后自动失效；
- `validateBeforeApply=true` 时复用候选验证器；
- Probe 日志必须标明每个字段来自 AUTO、SYMBOL、PATTERN 或 OVERRIDE；
- Probe 成功后在 `getProbeResults` 中保留覆盖证据和最终采用布局；
- 覆盖只影响下一次 Probe，不应永久污染 profile 静态默认值。

---

## 11. 错误与部分结果模型

建议新增或细化以下错误：

| 错误码 | 含义 | 是否可继续 |
|---|---|---|
| `E_MAP_STALE` | 扫描期间 maps revision 变化 | 刷新 maps 后重试 |
| `E_SCAN_LIMIT` | 达到扫描字节/时间/候选上限 | 可用已返回部分结果 |
| `E_CANDIDATE_INVALID` | 指定候选未通过硬校验 | 换候选或布局 |
| `E_AMBIGUOUS` | 多个候选分数接近 | 读取更多样本 |
| `E_SESSION_STALE` | 候选 session 已因重连/换进程失效 | 重新扫描 |
| `E_UNSUPPORTED_LAYOUT` | 当前布局集合均不匹配 | 增加布局参数 |

扫描类工具应优先返回：

```json
{
  "complete": false,
  "reason": "SCAN_LIMIT",
  "candidates": [],
  "nextCursor": "..."
}
```

只有完全无法执行时才返回 tool error。这样 AI 可以基于部分证据继续，而不是从头重试。

---

## 12. 长任务、分页与缓存

### 12.1 长任务

以下操作必须进入 JobRegistry：

- 全模块 pattern 扫描；
- `ALL_READABLE` 内存搜索；
- 指针引用搜索；
- GUObjectArray 结构扫描；
- `locateEngineGlobals` 完整编排；
- SO 内存转储。

状态响应必须包含：

```json
{
  "jobId": "j-...",
  "status": "running",
  "phase": "SCAN_MODULE_RW",
  "progress": 0.42,
  "scannedBytes": 12345678,
  "totalBytes": 30000000,
  "candidateCount": 4,
  "suggestedWaitMs": 5000
}
```

### 12.2 候选 session

候选缓存键至少包含：

```text
connection generation + pid + process start time + maps revision + scan arguments hash
```

候选 session 用于：

- 分页读取结果；
- 不重扫情况下更换评分规则；
- sample 工具按 candidateId 读取；
- applyProbeOverrides 追溯证据。

### 12.3 响应体积

- 每页默认 20 个候选；
- 每个候选默认最多 5 条 evidence；
- 原始内存上下文默认不超过 64 字节；
- 完整扫描报告写设备文件，工具只返回路径与摘要；
- 保持单工具响应不超过 4K token。

---

## 13. 推荐工作流

### 13.1 正常探针

```text
ping
  -> listProcesses
  -> selectProcess
  -> attach
  -> startProbe
  -> getProbeStatus
  -> startDump / dumpSdk
```

### 13.2 `ERROR_INIT_GUOBJECTARRAY`

```text
getLogs
  -> listModules(includeSegments=true, includeAnonymous=true)
  -> scanGNames(region=BSS|ALL_READABLE)
  -> sampleGNames(candidate)
  -> scanObjects(namesCandidateId=..., region=MODULE_RW|BSS)
  -> sampleObjects(candidate)
  -> applyProbeOverrides(namesPtr + guObjectArrayPtr + layouts)
  -> startProbe
  -> getProbeResults
```

### 13.3 定位 `GWorld`

```text
resolveSymbol(GWorld)                    # 不依赖 Probe
  -> 未命中: scanPattern(UE version patterns, all r-x segments)
  -> decodeAdrl / findReferences
  -> 有 GUObjectArray: 枚举 UWorld 对象
  -> findReferences(worldObject, POINTER + ADRP_*)
  -> validate slotAddress + worldObject
  -> applyProbeOverrides(gWorldPtr)
```

### 13.4 Probe 失败后的 SO 诊断

```text
dumpUnrealLibrary(source=AUTO)           # 不依赖 Probe
  -> 返回路径 / hash / build-id
  -> PC 侧离线分析新 pattern
  -> scanPattern(start/end or mapIds)
  -> findReferences
  -> validate candidate
```

---

## 14. 实现落点

### 14.1 设备端建议拆分

当前大量命令内联在 `src/executable.cpp`，新增逆向逻辑不应继续堆入同一文件。

建议新增：

```text
src/mcp/analysis/
  MemoryMapIndex.{hpp,cpp}         # maps 快照、模块分组、revision
  MemorySearch.{hpp,cpp}           # pattern/string/value/pointer 统一搜索
  Arm64ReferenceScanner.{hpp,cpp}  # ADRP/ADD/LDR/literal 引用
  ElfInspector.{hpp,cpp}           # attach-only ELF、符号、段信息
  UENameCandidateScanner.{hpp,cpp} # FNamePool/GNames 候选与评分
  UEObjectCandidateScanner.{hpp,cpp}
  UEWorldCandidateScanner.{hpp,cpp}
  CandidateSessionStore.{hpp,cpp}  # session/cursor/失效规则

src/mcp/commands/
  MemoryAnalysisCommands.{hpp,cpp}
  UEAnalysisCommands.{hpp,cpp}
```

这些模块只能依赖 `kMgr`、ELF/maps 读取和明确传入的布局参数，不能读取 ImGui 状态。

### 14.2 PC 侧

需要同步修改：

| 文件 | 改动 |
|---|---|
| `mcp_server/src/umt_mcp/tools.py` | 暴露完整 schema，增加两个工具，更新 self_check |
| `mcp_server/src/umt_mcp/config.py` | 候选分页、扫描预算、job 默认值 |
| `mcp_server/src/umt_mcp/instructions.py` | 加入失败恢复工作流和地址语义 |
| `mcp_server/src/umt_mcp/resources.py` | 暴露 map revision、候选 session、活跃 job |
| `mcp_server/tests/test_bridge.py` | 覆盖部分结果、分页、session 失效 |
| `mcp_server/tests/test_protocol.py` | 新错误码和 schema 契约 |

`bridge.py` 的 NDJSON、HELLO、心跳和串行模型无需为本次能力补充重写。

---

## 15. 测试矩阵

### 15.1 纯单元测试

使用合成内存 fixture 覆盖：

| Fixture | 必须验证 |
|---|---|
| 多段 ELF maps | 同路径所有段保留，匿名 `.bss` 可见 |
| 跨 chunk pattern | 分块边界命中不丢失 |
| ARM64 ADRP+ADD | 正负页偏移、立即数、目标地址 |
| ARM64 ADRP+LDR | 32/64 位 load、缩放立即数 |
| FNamePool | stride 2/4、BlocksBit 16/18、BlocksOff 0x40/0xD0 |
| GUObjectArray | flat/chunked、不同 ObjObjects/Objects/NumElements 偏移 |
| UWorld | 正确槽位、假指针、类不匹配、关卡切换 |
| maps 变化 | session 返回 `E_MAP_STALE/E_SESSION_STALE` |

### 15.2 PC/设备契约测试

- 45 个工具与设备命令一一对应；
- Python 参数能够完整下发 `start/end/mapIds`；
- `None` 参数不下发；
- 地址始终使用 `0x...` 字符串；
- 扫描部分结果不会被桥接层转成协议错误；
- 重连后候选 session 失效；
- 大结果分页且单页不超过响应预算。

### 15.3 真机验收

至少覆盖：

1. 带动态符号的 UE4 样本；
2. 完全 strip 的 UE4.27 样本；
3. FNamePool 位于匿名或扩展 `.bss` 的样本；
4. GUObjectArray 不在 FNamePool 固定邻域的样本；
5. chunked 与 flat 对象数组各一个；
6. UE5 `libUnreal.so` 样本；
7. Probe 初次失败、通过 override 二次成功的完整流程。

---

## 16. 分阶段交付

### Phase 0：解除阻断

- 修复 `listModules` 映射丢失；
- PC 侧暴露 `scanPattern(start/end)`；
- module 扫描覆盖全部段；
- 解除 `resolveSymbol` Probe 门禁；
- 解除 `dumpUnrealLibrary` Probe 门禁。

**完成标准：** Probe 失败后仍能获取完整 maps、解析符号、扫描指定区间并转储 SO。

### Phase 1：候选发现

- 新增 `searchMemory`；
- 新增 `findReferences`；
- `scanGNames/sampleGNames` 支持候选模式；
- `scanObjects/sampleObjects` 支持候选模式；
- 输出评分、证据和失败检查。

**完成标准：** 不依赖 ProbeResult 也能生成并验证 FNamePool 与 GUObjectArray 候选。

### Phase 2：全局变量闭环

- 实现独立 UWorld 候选与验证；
- 重写 `locateEngineGlobals` 的真实回退编排；
- 扩展 `applyProbeOverrides` 的布局和证据绑定；
- Probe 二次运行采用已验证覆盖。

**完成标准：** 初次 `ERROR_INIT_GUOBJECTARRAY` 后，MCP 能自主定位候选、应用覆盖并让 Probe 成功。

### Phase 3：工程化

- JobRegistry、分页、候选 session、maps revision；
- 扫描取消点和资源预算；
- 扫描报告落盘；
- 扩充真机回归样本。

**完成标准：** 长扫描可取消、可恢复、可诊断，不触发重试风暴或上下文爆炸。

---

## 17. 最终验收清单

- [ ] `resolveSymbol` 在 Probe 失败后仍可调用。
- [ ] `dumpUnrealLibrary` 在 Probe 失败后仍可调用。
- [ ] `listModules` 返回 libUE4/libUnreal 的全部真实映射段。
- [ ] 匿名 `.bss` 和相邻可读区域可以被明确选择。
- [ ] `scanPattern` 的 `start/end/mapIds` 从 MCP 到设备端完整透传。
- [ ] `searchMemory` 支持字符串、数值、hex 和指针搜索。
- [ ] `findReferences` 支持 pointer、ADRP+ADD、ADRP+LDR 和 literal load。
- [ ] `scanGNames/sampleGNames` 不依赖 Probe 成功。
- [ ] `scanObjects/sampleObjects` 不依赖 Probe 成功。
- [ ] GUObjectArray 候选返回布局、评分、证据和失败检查。
- [ ] `locateEngineGlobals` 能返回部分结果，而不是只给提示。
- [ ] `GWorld` 同时返回槽位地址和对象地址。
- [ ] `applyProbeOverrides` 绑定 pid、maps revision 和候选证据。
- [ ] 初次 Probe 失败后可以通过覆盖让第二次 Probe 成功。
- [ ] 第二次 Probe 成功后可以正常执行 `dumpSdk`。
- [ ] 常驻 MCP 工具总数不超过 45。
- [ ] 所有扫描结果分页，单页响应不超过 4K token。

完成以上项目后，UMT MCP 才具备真正的 UE 引擎逆向分析闭环：不仅能在已知 profile
和成功 Probe 上工作，也能在未知游戏、定制布局和旧 pattern 失效时自主产生证据、调整假设、
恢复探针并完成 Dump。
