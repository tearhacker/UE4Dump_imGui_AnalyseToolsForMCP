# 卡拉彼丘 UE4 Dump 经验与 MCP 能力补充建议

## 1. 文档目的

本文记录 Android UE4 游戏《卡拉彼丘》（包名：`com.idreamsky.klbqm`）的 MCP Dump 实测经验，并提出完善 UMT MCP 工具链的能力需求。

核心结论：`Probe success=true` 和 `Dump success=true` 不能单独证明结果有效。必须同时验证 FNamePool、GUObjectArray、UWorld、对象名称和输出文件集合。

## 2. 本次实测环境

- 引擎：自动识别为 UE4
- 进程：`com.idreamsky.klbqm`
- 架构：ARM64，`e_machine=0xB7`
- 主模块：`libUE4.so`
- MCP：协议 1，设备端 build `1.0.0`
- 实测输出：`Logs.txt`、`Objects.txt`、`Offsets.hpp`

## 3. 失败现象

本次 Dump 表面上完成，但产物不完整：

- 没有 `SDK_Classes.hpp`、`SDK_Structs.hpp`、`SDK_Enums.hpp`、`SDK_Offset.hpp` 等文件。
- `Objects.txt` 只有几十字节，无法形成对象清单。
- FName 样本出现 `adObject`、`Object`、`ject`、`ct` 等错位字符串。
- 对象样本为空、乱码或随机后缀。
- 日志出现 `GetObjectPtr(1).GetIndex() != 1` 和 `Error: Packages are empty.`。
- `ProcessEvent`、`ProcessEventIdx` 未定位。

这些现象说明名称池候选落在静态字符串或错误结构上，对象数组候选也没有通过真实 UObject 校验。

## 4. 根因分析

### 4.1 低置信度候选被当成最终地址

`locateEngineGlobals` 当前可以返回结构扫描候选，但候选仅满足“地址可读”和局部布局合理。它没有保证：

1. FName ID 0 能解析为 `None`；
2. 连续 FName entry 的 Header、长度和编码正确；
3. GUObjectArray 中多个对象的 `ClassPrivate`、`NamePrivate` 可递归验证；
4. 对象名称中出现 `Package`、`Class`、`CoreUObject` 等核心锚点。

### 4.2 mapRevision 在长任务中变化

游戏运行期间会加载和卸载映射，导致候选 session 失效。表现为：

```text
E_MAP_STALE
E_SESSION_STALE
override 地址不在可读 map
```

当前接口把候选生成、验证和应用拆成多个调用，中间没有原子快照，因此很容易在应用时失效。

### 4.3 FNamePool 语义不完整

UE4.25-4.27 常见布局包含 `BlocksOff=0x40`、`BlocksBit=16`、`Stride=2`，但不同构建可能存在：

- 全局槽位地址和池对象地址混淆；
- 一次或多次间接寻址；
- 自定义名称池布局；
- 指针加密、异或或运行时解码；
- FName entry 编码与默认 `Header >> 6` 不一致。

仅检查页面可读性不足以确认候选正确。

### 4.4 GUObjectArray 布局未完成自动修正

本次日志中 `ObjObjects` 的偏移出现 `0x10` 和 `0x20` 的冲突，说明自动布局选择没有基于对象索引一致性完成 Fixup。`NumElements=4194304` 也只能作为弱证据。

### 4.5 Ghidra-native 尚未可用

`decompilerStatus` 返回 `ready=false`，因此无法通过 native 反编译确认：

- `FName::ToString` 或 `GetPlainNameString` 的真实实现；
- `StaticFindObject`、`ProcessEvent` 的调用约定；
- GName、GUObjectArray、GWorld 的 ADRP+ADD/LDR 引用；
- 可能存在的指针解密函数。

## 5. 推荐的正确 Dump 流程

### 5.1 固定进程与快照

```text
ping
listProcesses
selectProcess(package, pid)
attach
listModules(include_segments=true)
```

保存 `pid`、`processStartTime`、`mapRevision` 和 `libUE4.so` 段信息。

### 5.2 原始内存验证

对每个候选地址执行：

1. 读取候选本体和前后指针；
2. 解引用 Blocks[0]；
3. 解析 ID 0-8 的连续名称；
4. 检查 `None`、`ByteProperty`、`IntProperty`、`BoolProperty` 等核心名称；
5. 读取 GUObjectArray 前 8 个元素；
6. 验证索引、ClassPrivate、NamePrivate 和 OuterPrivate 的一致性。

### 5.3 原子应用覆盖

候选验证和覆盖应用应使用同一 maps 快照。不要把几分钟前的绝对地址重新注入当前进程。应用后立即执行：

```text
startProbe
getProbeStatus
getProbeResults
```

只有 `NamePrivate validated`、`ClassPrivate validated` 和对象索引一致时才允许 Dump。

### 5.4 分步导出

使用：

```text
startDump
getDumpStatus
listOutputFiles
adb pull
```

不要优先使用会重新运行自动探针的 `dumpSdk`，否则可能覆盖人工验证的候选。

## 6. MCP 工具应补充的能力

### 6.1 必须补充：原子 maps 快照会话

新增 `beginMapSnapshot` / `endMapSnapshot` 或给 `attach` 增加快照句柄：

- 固定 maps 内容、revision、进程启动时间；
- 所有 read/search/override 使用同一快照；
- maps 变化时返回明确的 `SNAPSHOT_INVALIDATED`；
- 支持在一次 RPC 中完成“扫描候选→验证→应用”。

### 6.2 必须补充：FNamePool 专用验证器

新增 `validateFNamePool`：

- 输入候选地址、间接层数和布局候选集；
- 自动读取多个 block 和连续 entry；
- 返回每个 ID 的 header、长度、编码、解码文本；
- 计算可信度分数；
- 明确区分全局槽位、池对象和 Block 地址；
- 支持 XOR、加法、指针旋转等可配置解码步骤，但默认只读。

### 6.3 必须补充：GUObjectArray 专用验证器

新增 `validateObjectArray`：

- 同时测试 flat/chunked 两种布局；
- 自动搜索 `ObjObjects`、`Objects`、`NumElements`、`NumElementsPerChunk`；
- 验证索引回读一致性；
- 检查多个 UObject 的 ClassPrivate、NamePrivate、OuterPrivate；
- 返回有效对象比例和失败原因；
- 只有达到阈值才允许标记 candidate 为 verified。

### 6.4 必须补充：GWorld 定位与稳定性测试

新增 `validateWorld`：

- 区分 GWorld 槽位地址、槽位内容和 World 对象；
- 验证对象 Class 名称是否为 `World` 或派生类；
- 检查 PersistentLevel、Levels、OwningGameInstance 等字段链；
- 连续读取多次，判断地址是否稳定；
- 返回稳定性分数和指针链快照。

### 6.5 必须补充：解密/变换探测框架

新增只读 `detectPointerTransform`：

- 对候选指针执行 XOR、加减常量、ROR/ROL、低位清除、模块基址相对化等测试；
- 用可读 map、对齐、模块归属、对象类名作为评分条件；
- 输出候选变换和证据，不直接修改目标内存；
- 允许将验证过的变换传入 FNamePool/GUObjectArray/UWorld 验证器。

### 6.6 必须补充：Ghidra-native 自动部署与状态诊断

新增 `installDecompilerSpec`、`decompilerDoctor`：

- 检查 AARCH64 spec 文件是否存在；
- 返回缺失文件、版本、权限和路径；
- 支持从 PC 推送设备端 spec；
- 在首次 `decompile` 前完成初始化；
- 提供最小函数反编译自测。

### 6.7 必须补充：稳定模块转储

`dumpUnrealLibrary` 应返回真实 job 状态和 artifact，而不是启动后从 `jobs` 中消失。需要：

- `soDumpPath`、大小、SHA-256、build-id；
- FILE/MEMORY 两种来源的实际选择；
- 进度、失败原因和可重试状态；
- 与当前 `mapRevision` 绑定。

### 6.8 必须补充：完整 SDK 输出闸门

`startDump` 应在导出前执行硬校验：

- FName ID 0-8 合法；
- 至少 N 个有效 UObject；
- 至少一个 Package 和 Class；
- `ProcessEvent` 或等价调用路径已定位；
- 失败时不生成看似成功的 `Offsets.hpp`；
- 输出 `dump_manifest.json`，记录每个文件、大小、哈希和验证状态。

### 6.9 必须补充：分页和大文件传输

新增 `pullOutputFile` 或分块下载接口：

- 自动处理几十 MB 的 SDK 文件；
- 支持断点续传和 SHA-256 校验；
- MCP 只返回元数据，不把大文件塞入单次响应；
- 统一输出本地目标路径。

### 6.10 必须补充：可复现诊断包

新增 `exportDiagnosticBundle`：

- 保存 maps 快照、候选、原始内存片段、Probe 结果、日志、错误码；
- 每个地址记录模块相对偏移；
- 脱离进程后可离线重放 FName/Object 验证；
- 支持 JSON manifest 和二进制片段哈希。

## 7. 成功判定标准

完整 Dump 必须同时满足：

1. `getProbeStatus.success=true`；
2. FNamePool 连续条目可解析，ID 0 为 `None`；
3. GUObjectArray 至少有多个有效 UObject；
4. `NamePrivate`、`ClassPrivate`、`OuterPrivate` 校验通过；
5. 至少出现 `Package`、`Class`、`CoreUObject` 等核心名称；
6. `GWorld` 指针链通过类名和稳定性检查；
7. `dumpPercent=100` 且 `success=true`；
8. 输出包含完整 SDK 文件集合；
9. `Objects.txt` 大小和内容达到阈值；
10. manifest 中所有文件哈希可复核。

## 8. 优先级建议

### P0

- 原子 maps 快照；
- FNamePool 验证器；
- GUObjectArray 验证器；
- Dump 前硬闸门；
- 完整 job/artifact 状态。

### P1

- GWorld 稳定性验证；
- 指针变换探测；
- Ghidra-native 自动部署和 doctor；
- 分块文件拉取；
- 诊断包导出。

### P2

- 离线验证重放；
- 多候选评分比较；
- UE4.25-4.27 布局模板库；
- 自动生成 SDK manifest 和差异报告。

## 9. 最终经验

Dump 工具的“任务完成”只代表导出流程结束，不代表语义正确。对《卡拉彼丘》这类持续变化 maps、符号裁剪、名称池候选不稳定的 UE4 游戏，MCP 必须把“读内存、验证语义、绑定快照、导出文件”设计成一个可审计的闭环。任何缺少核心名称、有效对象和完整文件集合的结果，都应标记为失败，而不是成功。
