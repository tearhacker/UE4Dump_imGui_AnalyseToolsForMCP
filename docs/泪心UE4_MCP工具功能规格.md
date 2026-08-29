# UMT MCP 工具功能规格

> 本文档**只描述 MCP 服务对外提供什么功能**。
> 设计论证、实施路线、验收标准、代码位置索引见《功能说明》与《开发必读架构》。
>
> 服务名：`unreal-memory-tools`　协议：MCP over stdio　设备端端口：27185
> **42 个工具**，按 A–I 九个模块划分。工具名 camelCase，原生命令名大写下划线。

**实现基础标记（本文档特有，用于判断实现成本）：**

| 标记 | 含义 |
|---|---|
| ✅ 已有 | 设备端能力已具备，只需封装暴露 |
| 🔧 外迁 | 已实现但锁在匿名 namespace，需导出（约 200–300 行总量） |
| ⚠️ 新建 | 设备端没有，需从零写 |
| 🖥️ PC 侧 | 不需要设备端参与，在 MCP Server 进程内完成 |

---

## 1. 模块总览

| 组 | 模块 | 数量 | 作用 |
|---|---|---|---|
| A | 连接与能力 | 2 | 探活、查询服务端与设备端能力 |
| B | 进程 | 2 | 列出可选目标、选定目标进程 |
| C | 流程与产出 | 11 | 驱动探测/转储、读取产物、日志、取消 |
| D | 内存原语 | 7 | 读写内存、扫描、模块与符号 |
| E | 理解层 | 2 | 反汇编、ADRP 解码 |
| F | 远程调用 | 5 | 在目标进程内调用函数（ptrace） |
| G | 引擎语义 | 8 | UE 版本、名字表、对象数组、类检索 |
| H | 高层用例 | 3 | 正门：一键完成完整用例 |
| I | 复合操作 | 2 | 一次调用顶 N 次原语，降轮次 |

---

## 2. A 组 · 连接与能力

### `ping`
**用途**：确认 MCP 服务、adb 通道、设备端命令服务三者是否连通。

**输入**：无

**输出**：`{connected, transport, deviceBuild, protocolVersion}`

**实现基础**：⚠️ 新建（依赖 L4 命令服务）

---

### `getCapabilities`
**用途**：查询当前可用的能力集合，以及**是否存在泄漏的 attach 会话**。

**输入**：无

**输出**：
```
{ ptraceAvailable, scanAvailable, versionDetectAvailable,
  outputDir, deviceBuild, protocolVersion,
  activeAttachSession: boolean,        // AI 据此发现并恢复泄漏的会话
  activeSessionId?: string }
```

**实现基础**：⚠️ 新建

**约束**：
- `ptraceAvailable` 为 false 时，F 组工具应已通过 `tools/list_changed` 动态摘除
- AI 发现 `activeAttachSession: true` 时应提示用户或主动 `endAttachSession`

---

## 3. B 组 · 进程

### `listProcesses`
**用途**：列出设备上所有可分析的 UE 进程。

**输入**：`dedicatedOnly?: boolean = false`（仅返回已适配 profile 的进程）

**输出**：`ProcessEntry[]`，每项 `{pid, package, profileName, dedicated}`

**实现基础**：🔧 外迁（`FindAutoProcessCandidates()` @ `executable.cpp:166-216`，无参，逻辑完整；两条来源：profile 的 `GetAppIDs()` 匹配 + 扫 `/proc` 判 maps 含 `libUE4.so`/`libUnreal.so`）

**约束**：最多 256 条，按 `dedicated` 优先、包名字典序排序

**下一步**：`selectProcess`

---

### `selectProcess`
**用途**：选定目标进程，初始化内存访问上下文，返回 UE ELF 基址。

**输入**：`pid?: integer` 或 `package?: string`（二选一）

**输出**：`{pid, package, ueElfPath, ueElfBase, isEmulator, arch}`

**实现基础**：⚠️ 新建（设备端 `KittyMemoryMgr` 初始化流程已有，需封装为可复用入口）

**约束**：切换进程会失效所有 session 缓存

**下一步**：`locateEngineGlobals`（H 组正门）

---

## 4. C 组 · 流程与产出

### `startProbe`
**用途**：启动偏移探测流程（UE 全局定位 + 结构解析）。

**输入**：`waitMs?: integer = 5000`（0–60000）

**输出**：`waitMs` 内完成则直接返回 `ProbeResult`；超时返回 `{jobId, status:"running", progress}`

**实现基础**：🔧 外迁（`ExecuteProbe` @ `executable.cpp:644`，需移出匿名 namespace 并把全局 `gProbeResult` 换成状态对象）

**下一步**：`getProbeStatus`（轮询）或 `getProbeResults`

---

### `getProbeStatus`
**用途**：查询探测任务进度与阶段。

**输入**：`jobId: string`

**输出**：`{status, phase, percent, lastError?}`

**实现基础**：🔧 外迁（`DumpUiState` 已有 `phase`/`objectsPercent`/`dumpPercent`）

---

### `getProbeResults`
**用途**：取回探测结果（合并原 `getProbeOffsets` + `getProbeStructs`）。

**输入**：`jobId?: string`（省略则取最近一次）

**输出**：
```
{ offsets: [{name, value, relative, found}],
  structs: [{group, name, fields:[{name, type, offset, recognized}]}],
  unrecognizedFields: [...] }     // 红色字段，AI 兜底推理的输入
```

**实现基础**：🔧 外迁

---

### `startDump`
**用途**：启动 SDK 转储。

**输入**：`waitMs?: integer = 5000`

**输出**：同 `startProbe` 的短等/异步双形态

**实现基础**：🔧 外迁（`ExecuteDump` @ `executable.cpp:754`）

**约束**：依赖 `startProbe` 成功；`ExecuteDump` 会校验 `valid/success/profile`

---

### `getDumpStatus`
**用途**：查询转储进度。**输入** `jobId`，**输出** `{status, percent, currentPackage}`。
**实现基础**：🔧 外迁

---

### `dumpUnrealLibrary`
**用途**：转储目标进程的 `libUE4.so` / `libUnreal.so` 本体到输出目录。

**输入**：`waitMs?: integer = 5000`

**输出**：`{path, sizeBytes}`

**实现基础**：🔧 外迁（`ExecuteDumpUnrealLib` @ `executable.cpp:859`）

---

### `listOutputFiles`
**用途**：列出某包名的产物文件清单。

**输入**：`package: string`

**输出**：`{path, sizeBytes, modifiedAt}[]`

**实现基础**：⚠️ 新建（目录列举，简单）

**约束**：输出根目录 `/sdcard/UnrealMemoryTools/<package>/`

---

### `readOutputFile`
**用途**：读取产物文件内容，**必须分页**。

**输入**：`path: string`, `offset?: integer = 0`, `limit?: integer = 4096`

**输出**：`{content, totalBytes, hasMore}`

**实现基础**：⚠️ 新建

**约束**：
- `SDK_Classes.hpp` 可达几十 MB，**禁止整文件读取**
- 需要按类检索时改用 `searchClasses` / `describeClass`（服务端过滤）

---

### `getLogs`
**用途**：取回设备端日志（AI 自我纠错的关键入口）。

**输入**：`sinceIndex?: integer = 0`, `maxLines?: integer = 200`（≤1500）

**输出**：`{lines: [{index, level, timestamp, message}], totalLines}`

**实现基础**：🔧 外迁（设备端已有 1500 行环形缓冲 `gDumpUiState.logLines`，mutex 保护）

**约束**：出错时先调它——设备端日志通常直接指出失败原因

---

### `cancelJob`
**用途**：取消进行中的长任务。

**输入**：`jobId: string`

**输出**：`{cancelled, statusAtCancel}`

**实现基础**：⚠️ 新建（设备端**完全无取消点**，需加 `std::atomic<bool>` 并在两个扫描热循环插检查点：GUObject 扫描循环、Names 分块循环）

---

### `applyProbeOverrides`
**用途**：把 AI 验证过的偏移组合写回运行时，继续走正常 dump 流程。

**输入**：`overrides: {namesPtr?, guObjectArrayPtr?, gWorldPtr?, [key:string]: string}`

**输出**：`{applied[], rejected[{key, reason}]}`

**实现基础**：⚠️ 新建

**约束**：只影响当前会话内存中的 `UEVars`，不落盘

---

## 5. D 组 · 内存原语

### `readMemory`
**用途**：读取目标进程原始内存。

**输入**：`address: string`（`0x...`）, `size: integer`（1–65536）

**输出**：`{address, size, hex}`

**实现基础**：✅ 已有（`KittyMemoryMgr` 读路径）

---

### `writeMemory`
**用途**：写入目标进程内存。

**输入**：`address: string`, `hexBytes: string`, `confirmDangerous: boolean = false`

**输出**：`{written, verified, previousHex}`

**实现基础**：✅ 已有（`KittyMemoryMgr` 写路径 + `MemoryBackup`）

**约束**：
- 写前**必须**用 `KittyPtrValidator` 校验：地址已映射且含 `PROT_WRITE`，否则拒绝
- `confirmDangerous` 默认 false，为 false 时返回中性提示而非执行
- 自动走 `MemoryBackup` 支持撤销

---

### `readMemoryValue`
**用途**：按类型读取并解码单个值。

**输入**：`address: string`, `valueType: enum`（bool / i8–i64 / u8–u64 / f32 / f64 / ptr32 / ptr64）

**输出**：`{address, valueType, value, rawHex}`

**实现基础**：✅ 已有（`vm_rpm_ptr<T>` 模板）

---

### `scanPattern`
**用途**：按字节特征码搜索内存。

**输入**：`pattern: string`（IDA 风格，支持 `?` 通配）, `region?: enum = "ANY_X"`（ANY_R / ANY_X / ANY_W / BSS）, `maxResults?: integer = 50`, `skipResult?: integer = 0`

**输出**：`{hits: string[], totalFound, truncated}`

**实现基础**：✅ 已有（`KittyScanner::findIdaPatternAll/First` + `findIdaPattern` 的段筛选逻辑）

**别名触发词**：找函数地址、定位特征码、搜索指令序列、匹配机器码、搜 pattern

---

### `listModules`
**用途**：列出目标进程的内存映射/模块。

**输入**：`nameFilter?: string`

**输出**：`{path, startAddress, endAddress, base, permissions, isPrivate, isExecutable}[]`

**实现基础**：✅ 已有（`KittyMemoryEx::getAllMaps`，`UEGameProfile.cpp:884` 有使用先例）

---

### `resolveSymbol`
**用途**：按名字查符号地址，或按过滤词列符号。

**输入**：`name?: string`, `filter?: string`（二者至少其一）, `image?: string`

**输出**：命中时 `{name, address, size}`；用 filter 时 `{symbols: [...], total}`

**实现基础**：🔧 需接线（`ElfScanner::findSymbol` @ `KittyScanner.hpp:211` **存在但 `src/` 零调用**，需接进定位流程，约 80–150 行）

**约束**：
- **这是定位优先级最高的路径**（符号查找 O(1)，pattern 扫描 O(n)）
- 四个引擎原生符号：`GUObjectArray`（全版本）、`GNameBlocksDebug`（UE4.23+）、`_ZN5FName16GetIsInitializedEv`（UE4.00–4.20）、`GFNameTableForDebuggerVisualizers_MT`（UE4.22）
- 符号可能已 strip → 不得返回"未找到"，应返回相近符号并提示改用 `scanPattern`

---

### `readString`
**用途**：读取目标进程字符串。

**输入**：`address: string`, `maxLen?: integer = 256`, `wide?: boolean = false`

**输出**：`{value, length, encoding, truncated}`

**实现基础**：✅ 已有（`vm_rpm_str`）

**约束**：**FName 可能是 UTF-16**，不确定时 `wide` 两种都试

---

## 6. E 组 · 理解层

### `disassemble`
**用途**：反汇编机器码，让 AI 能读汇编判断函数签名。

**输入**：`hexBytes: string`, `baseAddr: string`, `arch?: enum = "arm64"`

**输出**：`{instructions: [{address, bytes, mnemonic, operands}]}`

**实现基础**：🖥️ PC 侧（capstone，pip 安装；设备端无反汇编器，Keystone 已被 `-DkNO_KEYSTONE` 关闭）

**约束**：capstone 可缺失降级——缺失时返回明确提示，不静默失败

---

### `decodeAdrl`
**用途**：解码 ADRP+ADD / ADRP+LDR 指令对，得到绝对地址。

**输入**：`address: string`

**输出**：`{instructionAddress, absoluteTarget, decoded}`

**实现基础**：✅ 已有（`Arm64::DecodeADRL` / `Decode_ADRP_ADD` / `Decode_ADRP_LDR` @ `UEMemory.hpp`）

---

## 7. F 组 · 远程调用（ptrace）

> **本组整体风险最高**：启用后由"无注入外部读取"变为"进程内执行"，
> 与现有 `/proc/pid/mem` 设计冲突，也更容易被反作弊识别。单机游戏场景可接受。

### `callRemoteFunctionBatch`　← **主推**
**用途**：在目标进程内批量调用同一函数（原子：内部 attach → N 次 → detach）。

**输入**：
```
address: string,
argSets: string[][],          // 每组一次调用，元素为 "0x..." 或 "i:123"
returnKind: enum = "ptr",     // ptr / i32 / i64 / f32 / void
maxHoldMs?: integer = 3000,   // ≤30000
confirmDangerous: boolean = false
```
**输出**：`{results: [{ok, value, error?}], elapsedMs, callCount}`

**实现基础**：🔧 需接线（`KittyTraceMgr::callFunctionFrom` @ `KittyTrace.cpp:140` **存在但 `src/` 零引用**，约 100–200 行）

**约束**：
- **无状态**——一次调用内部完成 attach/detach，不可能泄漏
- 返回陷阱用 SIGSEGV/SIGILL 判断，**无法区分"陷阱触发"与"被调函数自己崩了"** → 必须校验返回值
- 目标进程崩溃时 ptrace 返回 ESRCH，应清理状态并**正常返回**，不抛异常

---

### `beginAttachSession` / `endAttachSession`
**用途**：需要「调用 → 读内存 → 再调用」交错时，维持一个冻结会话。

**输入（begin）**：`maxHoldMs?: integer = 3000`（≤30000）
**输出（begin）**：`{sessionId}`
**输入（end）**：`sessionId: string`
**输出（end）**：`{ok, elapsedMs}`

**实现基础**：🔧 需接线

**约束 —— 四重兜底，缺一不可**：
1. `maxHoldMs` 到点服务端强制 detach
2. 空闲看门狗：会话内超时无操作自动 detach
3. 异常兜底：任何工具异常先 detach 再返回错误
4. 断连兜底：stdio 关闭 / 进程退出，析构强制 detach

> **漏调 `endAttachSession` 会让游戏永久冻结。** 能用 `callRemoteFunctionBatch` 就绝不开会话。

---

### `callRemoteFunction`
**用途**：在会话内调用单个函数。需先 `beginAttachSession`。

**输入**：`sessionId`, `address`, `args: string[]`, `returnKind`, `confirmDangerous: boolean = false`

**输出**：`{ok, value, rawHex}`

**实现基础**：🔧 需接线

**约束**：`confirmDangerous` 默认 false；调用前必须已用 `scanPattern` 或 `disassemble` 验证过该函数地址

---

### `allocScratch`
**用途**：在目标进程内分配可写缓冲区（供 `FName::ToString(FString&)` 等需要输出参数的函数使用）。

**输入**：`size: integer`（1–65536）

**输出**：`{address, size}`

**实现基础**：🔧 需接线（ptrace 调目标进程 mmap）

---

## 8. G 组 · 引擎语义

### `detectUEVersion`
**用途**：识别 UE 版本并选定偏移表。

**输入**：无

**输出**：`{versionString, detectedFrom: enum, offsetsTable, confidence}`

**实现基础**：✅ 已有（`DetectVersion()` @ `AutoFix/AutoFix.cpp:140`）

**约束**：`detectedFrom` 区分 `SO_NAME`（libUnreal.so→UE5_03）、`RODATA_STRING`（扫描 `++UE4+Release-X.YY`）、`FALLBACK`（找不到则默认 UE4_25_27）——**区分"版本判错"与"锚点判错"**

---

### `scanGNames`
**用途**：扫描 GNames / FNamePool 的候选地址。**参数化是这个工具的核心价值。**

**输入**：
```
minPtr?: string = "0x4FFFFFFFFF",
maxPtr?: string = "0x7FFFFFFFFF",
anchorNames?: string[] = ["None","ByteProperty","IntProperty",...],
anchorOffsets?: string[] = ["0x8","0x6","0xA","0x4","0xC","0x10"],
region?: enum = "ELF_SEGMENTS",        // ELF_SEGMENTS / ALL_READABLE / BSS
maxCandidates?: integer = 200          // 1–2000
```

**输出**：`{sessionId, candidateCount, candidates: [{id, address, anchorHits, anchorOffset, exact}]}`

**实现基础**：⚠️ 需参数化改造（当前 `GetNamesPtr()` 无参、范围/偏移/区域**全硬编码**，且扫描区域**仅限 ELF 可读段**、不能扫全内存；约 60–120 行）

**约束**：
- `anchorNames` 至少给 3 个引擎内建名——**单锚点误判率高，这是 AutoFix 失败主因**
- 只返回候选摘要，**必须**用 `sampleGNames` 取样本后才能判定
- 结果按 `sessionId` 缓存，换判据重评**不得重扫**

---

### `sampleGNames`
**用途**：取指定候选的前 N 个 FName 字符串。**AI 判定的命脉。**

**输入**：`sessionId: string`, `candidateId: integer`, `count?: integer = 16`（1–256）

**输出**：`{names: [{index, value, wide}], readErrors}`

**实现基础**：⚠️ 需批量封装（现有只有逐 id 的 `GetNameByID`，采样点写死 `for(i<5)`；约 40–80 行）

**约束**：
- **返回原始字符串，绝不替 AI 判定"是否有效"**——解出乱码也原样返回
- `readErrors` 单独列出，让 AI 知道哪些没读出来

---

### `scanObjects`
**用途**：扫描 GUObjectArray 候选，**接受 names 候选 id 但不强制依赖**。

**输入**：
```
namesCandidateId?: integer,      // 省略则独立扫描，解耦 GNames 依赖
minPtr?: string, maxPtr?: string,
anchorFullName?: string = "/Script/CoreUObject",
direction?: enum = "BOTH",       // UP / DOWN / BOTH
maxDistanceBytes?: integer = 0x1800000,
maxCandidates?: integer = 200
```

**输出**：`{sessionId, candidateCount, candidates: [{id, address, firstObjAddress}]}`

**实现基础**：⚠️ 需参数化 + **修 bug**（当前循环 `namesScanBase + 8*i` **只往高地址单向扫**，若 GUObjectArray 在 FNamePool 低地址方向则永远扫不到；约 60–120 行）

**约束**：不得返回"未找到"，应返回放宽条件后的次优候选

---

### `sampleObjects`
**用途**：取指定对象候选的前 N 个对象全名。

**输入**：`sessionId`, `candidateId`, `count?: integer = 16`

**输出**：`{objects: [{index, fullName, classIndex, nameIndex}], readErrors}`

**实现基础**：⚠️ 需批量封装（现有 `GetObjectPtr(i)` / `GetName()` 可用，采样点写死 5 条）

---

### `searchClasses`
**用途**：按名字模式检索类，**只返回摘要不含完整定义**。

**输入**：`pattern: string`, `limit?: integer = 20`（1–100）

**输出**：`{total, classes: [{fullName, size, fieldCount, packageName}]}`

**实现基础**：⚠️ 新建无头检索层（现有 `SDKExplorer.cpp` 逻辑在**内层匿名 namespace**、250 处 `ImGui::` 调用、状态全在文件级全局，**不可直接复用**；需另起 `SDKQuery` 层复用 `UEWrappers`，约 200–400 行）

**约束**：绝不可返回完整类定义（会撑爆上下文）

---

### `describeClass`
**用途**：返回单个类的完整字段与偏移。

**输入**：`className: string`, `includeInherited?: boolean = true`

**输出**：`{fullName, size, parentClass, fields: [{name, type, offset, size, flags}]}`

**实现基础**：⚠️ 新建（同上，可复用 `SDKExplorer.cpp` 里的纯逻辑函数：`CollectFields`、`ReadFieldValue`、`BuildPropertyDecl`）

---

### `inspectObject`
**用途**：读取一个运行时对象的实例数据。

**输入**：`address: string` 或 `objectIndex: integer`

**输出**：`{address, className, fullName, fields: [{name, type, offset, value, rawHex}]}`

**实现基础**：⚠️ 新建

---

## 9. H 组 · 高层用例（正门）

> AI 应**默认先走 H 组**。只有 H 组失败时才展开 G/D 组细粒度流程。

### `locateEngineGlobals`
**用途**：一键定位 GNames / GUObjectArray / GWorld。

**输入**：`waitMs?: integer = 30000`

**输出**：
```
{ success,
  namesPtr?, guObjectArrayPtr?, gWorldPtr?,
  method: "SYMBOL" | "PATTERN" | "STRUCTURAL_SCAN" | "PTRACE_CALL",
  confidence, evidence: {sampleNames?: string[], sampleObjects?: string[]} }
```

**实现基础**：⚠️ 新建编排层（依赖 G 组全部 + D 组 `resolveSymbol`/`scanPattern`）

**内部顺序（不可颠倒）**：
1. `detectUEVersion`
2. **`resolveSymbol`**（符号优先，O(1)，四个引擎原生符号按版本挑）
3. `scanPattern`（按版本挑 18/14/14/6 条去重库）
4. `scanGNames` → `sampleGNames` → `scanObjects` → `sampleObjects`
5. `applyProbeOverrides`

**约束**：返回的 `evidence` 必须包含原始样本，让 AI 能自己复核

---

### `dumpSDK`
**用途**：一键完成「探测 → 转储 → 产出 SDK」全流程。

**输入**：`waitMs?: integer = 10000`, `outputDir?: string`

**输出**：`{jobId? 或 result, outputFiles: [{path, sizeBytes}]}`

**实现基础**：⚠️ 新建编排层（依赖 C 组）

---

### `analyzeClass`
**用途**：分析一个类或结构体，给出字段解读与可疑未识别字段。

**输入**：`className: string`, `includeRuntimeSample?: boolean = false`

**输出**：`{classInfo, unrecognizedFields: [{offset, size, hints}], suggestions?}`

**实现基础**：⚠️ 新建编排层（依赖 G 组 `describeClass`）

---

## 10. I 组 · 复合操作（降轮次专用）

### `followPointerChain`
**用途**：一次调用走完整条多级指针链，返回每一步的解引用结果。

**输入**：
```
baseAddress: string,
offsets: string[],              // ["0x10","0x28",...]，支持负数
derefEachStep?: boolean = true
```
**输出**：`{steps: [{address, valueAtAddress, readable, module?}], finalValue, brokenAtStep?}`

**实现基础**：⚠️ 新建（约 80 行）

**约束**：每步都记录落在哪个模块——这是 AI 判断"这个值合理吗"的依据

---

### `scanCandidates`
**用途**：按结构条件批量扫描候选（把 N 次 `readMemory` 压成 1 次）。

**输入**：
```
region?: enum, alignment?: integer = 8,
valueRange?: {min: string, max: string},
derefCheck?: {offset: string, expectReadable: boolean},
maxCandidates?: integer = 200
```
**输出**：`{sessionId, scannedBytes, candidateCount, candidates: [{address, value, derefValue?}]}`

**实现基础**：⚠️ 新建（约 70 行，复用 `KittyPtrValidator` 做落点校验）

---

## 附录 · 通用约定

### 编码
- 地址统一用**字符串** `"0x..."`（不用 number，避免精度丢失与进制歧义）
- 字符串参数 UTF-8 **hex** 传输（防引号/空格/中文/分隔符冲突）
- 类型化值 `struct` 小端编码

### 错误分层
| 类型 | 返回方式 |
|---|---|
| 协议层（未知工具名、schema 校验失败、JSON 解析失败） | JSON-RPC `error` |
| 执行层（读内存失败、扫描无结果、ptrace 被拒） | `isError: true` 的 tool result |

### 禁止 "not found"
搜索类工具**不得**返回"未找到"——AI 会被负面表述带偏，忽略后续有用信息。
正确做法：返回放宽条件后的次优候选 + 中性描述（例："严格条件下 0 命中；以下 5 个是放宽指针范围后的候选，命中锚点数较低，建议用 sampleGNames 逐个验证"）。

### 判定权永远在 AI
工具返回**原始观测值**（十六进制、解析出的字符串），不返回"看起来正确"这类总结。

### 长任务：短等优先
所有长任务带 `waitMs`（默认 5000，上限 60000）：
- `waitMs` 内完成 → 直接返回最终结果（1 次往返）
- 超时 → 返回 `{jobId, status, progress}`，客户端轮询

### 缓存
| 对象 | 键 | 失效 |
|---|---|---|
| 扫描候选集 | `sessionId` | 显式清理 / 切换进程 / 超时（10 分钟） |
| 模块列表 | pid | 切换进程 |
| 反汇编结果 | (addr, size) | 内容不变，长期缓存 |
| 类索引 | pid + dump 版本 | 重新 dump |

---

## 实现成本汇总

| 组 | 工具数 | 主要成本项 |
|---|---|---|
| A | 2 | 新建：探活 + 能力探测（含泄漏会话检测） |
| B | 2 | 外迁 `FindAutoProcessCandidates`（约 20–30 行）+ 新建进程选择 |
| C | 11 | 外迁 3 个 Execute + 日志（约 200–300 行）；**新建取消机制（80–150 行）** |
| D | 7 | 6 个封装即可；`resolveSymbol` 需接线（80–150 行） |
| E | 2 | PC 侧 capstone；`decodeAdrl` 已有 |
| F | 5 | 全部需接线 `KittyTraceMgr`（100–200 行）+ 四重兜底 |
| G | 8 | 参数化改造（60–120 行）+ 批量采样（40–80 行）+ **新建无头检索层（200–400 行）** |
| H | 3 | 新建编排层（约 350 行） |
| I | 2 | 新建（约 150 行） |

**设备端总计约 1200–2000 行**（不含 PC 侧 MCP Server 约 900 行 Python）。
**硬前置**：A/B/C/D 组依赖的 socket 命令服务（约 500–850 行）必须先落地。
