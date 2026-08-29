# 泪心 UE4 ImGui 分析工具 MCP 化 功能说明

> ⚠️ **阅读顺序**：本文件讲「做什么、为什么」。
> **具体怎么写（工具命名、描述模板、目录结构、开发规范）以
> [`泪心UE4_imGui_AnalyseToolsForMCp开发必读架构.md`](./泪心UE4_imGui_AnalyseToolsForMCp开发必读架构.md) 为准。**
>
> **命名已修订**：本文件第 4 章用的 snake_case（`memory_read` 等）已废弃，
> 正式名统一为 camelCase（`readMemory` 等），映射表见架构文档 §1.1。

> 项目：`Andriod_UnrealMemoryTools`（下称 UMT）
> 目标：为 UMT 增加进程内命令服务 + PC 侧 MCP 桥接，使 AI 大模型能够自主完成
> UE4/UE5 游戏的偏移定位、结构分析与 SDK 消费。
> 版本：v1.0（方案稿）　日期：2026-08-29

---

## 0. 这份文档要解决什么

UMT 目前是一个**外部 Dumper**：独立 ELF 进程，纯 `/proc/<pid>/mem` 读内存，
不注入、不 hook，靠 Vulkan + ImGui 悬浮窗交互。它有两个硬伤：

1. **AutoFix 在部分单机游戏上定位失败**（找不到 GNames / GWorld），且失败后无从下手
2. **Dump 结果只能人肉看**：界面里点来点去，几千个类名无法快速理解

本文档给出把 UMT 改造成 **AI 可驱动的远程分析平台** 的完整方案，
含架构、工具规格、实施路线与验收标准。

---

## 1. 架构总览

### 1.1 整体链路

```
AI 客户端
  │ stdio · 换行分隔 JSON-RPC 2.0
  ▼
mcp_server.py（PC 侧，零第三方依赖 / 仅 capstone 可选）
  │ adb forward tcp:27185
  ▼
命令服务线程（UMT 进程内，新增）
  │ 命令队列（mutex + condition_variable）
  ▼
主线程 / 现有 gWorkerThread 消费执行
  │
  ▼
KittyMemoryMgr · UE 层 · AutoFix · KittyScanner · KittyTraceMgr
```

### 1.2 为什么可行

关键：UMT **自身就是独立进程**，天然处在"服务端"位置，
不需要注入、不需要 hook、不需要 Magisk 模块。

| 已有资产 | 位置 | 用途 |
|---|---|---|
| `DumpUiState` + `std::mutex` | `executable.cpp:110-135` | 线程安全共享状态，UI 每帧加锁 poll |
| `gWorkerThread` + `ExecuteProbe/ExecuteDump/ExecuteDumpUnrealLib` | `executable.cpp:945-989` | 单 worker 跑重活的现成模型 |
| `PushUiLog` / `LoggerSink` | `executable.cpp:227-242` | 日志已结构化收在内存 |
| `UEDumper::Init()` / `Dump()` | `Dumper.hpp:31-33` | 干净的 API 边界 |
| `KittyMemoryMgr` | `KittyMemoryMgr.hpp` | 读写内存 / ELF 扫描 / 符号 / 转储 |
| `KittyScanner` | `KittyScanner.hpp` | IDA 风 pattern 扫描 |
| **`KittyTraceMgr::callFunction`** | `KittyTrace.cpp:140` | **ptrace 远程函数调用（从未被使用）** |

**结论：引擎已造好，缺的是远程控制台。**

---

## 2. 现状诊断：AutoFix 为什么失败

### 2.1 致命依赖链

```
IGameProfile::GetGUObjectArrayPtr()   [UEGameProfile.cpp:893]
    └─ 第 900 行：const uintptr_t namesPtr = GetNamesPtr();
       └─ 拿不到就直接 return 0
```

**GNames 一挂，GUObjectArray 连尝试都不做，GWorld 一并全崩。**

### 2.2 `GetNamesPtr()` 的四个硬编码卡点

| 卡点 | 位置 | 后果 |
|---|---|---|
| 指针值范围 `0x4FFFFFFFFF ~ 0x7FFFFFFFFF` | `:995-996` | 分配器布局不同 → 一个候选都进不来 |
| FNamePool 锚点偏移 `val + 0x8` | `:1030` | 条目头长度变了 → 永远读不到 |
| GNames 锚点偏移 `p1 + 0x24` | `:1046` | 同上 |
| 锚点字符串**只有 `"ByteProperty"`** | `:999-1001` | 该 FName 不在预期位置 → 判定失败 |

### 2.3 白捡的改进点

`UEGameProfile.cpp:80-84` 已有完整多锚点名单：

```cpp
static const char *kKnownNames[] = {
    "None", "ByteProperty", "IntProperty", "BoolProperty",
    "FloatProperty", "ObjectProperty", "NameProperty",
    "StructProperty", "ArrayProperty", "Object", "Class", "Function"
};
```

**但这份名单只在 `BruteForceDeltaForceNames`（三角洲专用）里使用，通用路径没用上。**
把它接到通用路径，是纯 C++ 小改动，可能立刻救活一批游戏。

### 2.4 上游素材：AndUEDumper 的 pattern 库

UMT 的上游 `AndUEDumper` 保留了 **29 个游戏 profile**，UMT 只继承了 6 个，丢了 23 个。

**关键认知：这些 pattern 不是"游戏特征码"，是"引擎函数序言"。**

`UEGameProfiles/Torchlight.hpp` 注释明写：
`FNameEntry const* FName::GetEntry(FNameEntryId id);`、`DebugDump`、
`GetPlainName ToString AppendString GetStringLength` —— 全是 UE 引擎函数。
其字节序列由「UE 版本 + 编译器」决定，**跨游戏可迁移**。

**⚠️ 上表的 211/39/42/28 是早期 grep 粗估（含注释与重复）。**
用 `extract_patterns.py` 精确抽取并去重后，真实数字见 §2.5 下方。
**以脚本产出为准。**

UMT 侧机制齐全（`findIdaPattern` @ `UEGameProfile.cpp:834`、
`Arm64::DecodeADRL` @ `UEMemory.hpp`、`PATTERN_MAP_TYPE`、`isEmulator()`），
**只是没数据喂**。

### 2.5 精确抽取结果（`extract_patterns.py` 产出）

脚本解析 29 个 profile 中的 **28 个**（`PUBG` 结构特殊未解析），按版本去重后：

| UE 版本 | 游戏数 | 原始 pattern | **去重后** |
|---|---|---|---|
| **UE4.25–27** | 17 | 130 | **18** |
| UE5.00–02 | 3 | 34 | **14** |
| UE5.03 | 2 | 20 | **14** |
| UE4.23–24 | 3 | 18 | **6** |

**17 个游戏只产出 18 条独立 pattern** —— 这个高重复率**反过来证实了**
"pattern 是引擎函数序言"的判断：同一 UE 版本的游戏，用的就是同一批 pattern。

### 2.6 更快的路：引擎原生符号（**新发现，优先级最高**）

抽取后发现的意外收获：**28 个 profile 里 19 个的第一策略不是 pattern，而是符号定位。**

```cpp
// ArkUltimate::GetNamesPtr —— 先查符号，查不到才走 pattern
uintptr_t blocks_p = GetUnrealELF().findSymbol("GNameBlocksDebug");
if (blocks_p != 0) {
    blocks_p = vm_rpm_ptr<uintptr_t>((void *)blocks_p);
    if (blocks_p != 0) return (blocks_p - GetOffsets()->FNamePool.BlocksOff);
}
// ↓ 符号失败，才用 pattern
std::vector<std::pair<std::string, int>> idaPatterns = { ... };
```

**四个引擎原生符号（跨版本稳定，上游实证）：**

| 符号 | 用途 | 覆盖范围 |
|---|---|---|
| `GUObjectArray` | 对象数组 | **全版本** |
| `GNameBlocksDebug` | 名字表（需解引用后回减 `FNamePool.BlocksOff`） | UE4.23 → UE5.03 |
| `_ZN5FName16GetIsInitializedEv` | 名字表（老版本，需 DecodeADRL） | UE4.00 – 4.20 |
| `GFNameTableForDebuggerVisualizers_MT` | 名字表（直接即地址） | UE4.22 |

**为什么它必须排第一：**
- 符号表查找是 **O(1)**；pattern 要扫整个可执行段，是 O(n)
- 符号名是**引擎源码里的标识符**，比字节序列稳定——pattern 随编译器版本变，符号名不变
- 上游 19/28 的用法已经给出了优先级答案

### 2.7 定位策略的最终优先级（**修订，v1.0 漏了第一条**）

```
① 符号定位    O(1)，最快最稳 —— §2.6 的四个符号按版本挑
② pattern     秒级，按版本挑（18/14/14/6 条去重库）
③ 结构扫描    分钟级，不依赖符号与 pattern，兜底
④ ptrace 调用 加密时让游戏自己解
```

v1.0 直接把 pattern 当第一路径，**漏掉了最快的 ①**——这是本轮最重要的修正。

---

## 3. 核心设计原则

### 3.1 原语 vs 领域工具：决定 AI 自主性上限

> 只给 `find_gnames()` → AI 只能做你写死的事
> 给足 `read_ptr / read_bytes / scan / call_function` → AI 能拼出你没预置的策略

**四层结构：**

```
AI（顶层循环：观察 → 假设 → 调用 → 修正）
  ↓
领域工具（names_scan / class_search / probe）—— 快路径，覆盖常见情况
  ↓
复合操作（follow_chain / scan_candidates）—— 省调用轮次
  ↓
原语（read_ptr / read_bytes / scan / call_function / resolve_symbol）
```

### 3.2 扫描在设备，判断在 AI

AI **绝不**直接读大内存。漏斗式收敛：

```
全部可读内存 ~800MB
  → 指针对齐 + 落点可读筛选（设备端，廉价判断）~50 万
  → 结构验证：能解析出 FNameEntry（设备端）~200
  → AI 看每个候选的前 16 个字符串样本（PC 端）→ 1 个
```

### 3.3 验证纪律（最容易做坏的一条）

**工具必须返回原始观测值**（十六进制字节、解析出的字符串），
绝不能返回"看起来是正确的"这类总结。
否则 AI 会在幻觉里自我确认，闭环立刻失效。

### 3.4 上下文预算铁律

`SDK_Classes.hpp` 可达几十 MB，塞不进上下文：

| 该做 | 不该做 |
|---|---|
| `class_search` → 返回 20 个「类名 + 大小 + 字段数」 | 返回 20 个类的完整定义 |
| `class_describe` → 单个类的完整字段与偏移 | 一次返回整个文件 |
| `object_search` → 先回「命中 N 条」再分页 | 直接展开全部命中 |
| 过滤下沉到 C++ 侧执行 | 全量传到 Python 再过滤 |

### 3.5 反汇编放 PC 侧

设备端**没有反汇编器**（`Deps/Keystone` 仅头文件，且 CMake `-DkNO_KEYSTONE` 已关闭）。
在 MCP Server 侧用 `capstone`（pip 安装）实现 `disassemble`，
**设备零体积代价，AI 却获得了读汇编的能力**。这是本方案的点睛之笔。

### 3.6 默认只读 + 护栏

自主模式下让 AI 自由写内存是危险的。
**默认只读**，写操作需显式开关，且用 `MemoryBackup` 自动备份支持撤销。

---

## 4. 工具规格

> 协议：JSON-RPC 2.0 over stdio（复用源项目 `mcp_server.py` 骨架）
> 原生命令：`OK <len>\n<body>` / `ERR <msg>`，body 为 JSON
> 编码约定：字符串参数 UTF-8 hex 传输；地址 `int(x,0)` 归一化后以 `0x...` 下发；
> 类型化值 `struct` 小端编码

### 4.1 A 组 · 连接与能力（2）

| 工具 | 输入 | 输出 |
|---|---|---|
| `ping` | — | `{connected, host, port}` |
| `capabilities` | — | 后端可用性：ptrace / 扫描 / 版本检测 / 输出目录 |

### 4.2 B 组 · 进程与目标（2）

| 工具 | 输入 | 输出 | 约束 |
|---|---|---|---|
| `list_processes` | `dedicated_only?` | 进程列表：pid / 包名 / profile 名 / 模式 | 最多 256 条 |
| `select_process` | `pid` 或 `package` | 确认与当前 UE ELF 基址 | — |

### 4.3 C 组 · 探针与输出（复用现有流程，8）

| 工具 | 说明 |
|---|---|
| `probe_start` / `probe_status` | 异步：`probe_start` 立即回 `job_id` |
| `probe_offsets` | 返回 `ProbeOffsetEntry[]`（name / value / relative / found） |
| `probe_structs` | 返回 `StructGroup[]`（含红色未识别字段） |
| `dump_start` / `dump_status` | 异步，同上 |
| `dump_library` | 转储 `libUE4.so` / `libUnreal.so` |
| `list_output_files` / `read_output_file` | 读 SDK 产物，**必须分页**（`offset` / `limit`） |

### 4.4 D 组 · 定位原语（7）

| 工具 | 输入 | 输出 |
|---|---|---|
| `memory_read` | `address, size(1-65536)` | hex 字符串 |
| `memory_write` | `address, hex_bytes` | `{written, verified, previous_hex}`（需显式开关） |
| `memory_read_value` | `address, value_type` | 解码后的值（bool/i8-i64/u8-u64/f32/f64/ptr32/ptr64） |
| `scan_pattern` | `pattern, region, max_results` | 命中地址列表 |
| `list_modules` | `name_filter?` | 模块路径 / 起止 / 基址 / 权限 |
| `elf_symbols` | `filter?` | 符号表（strip 后可能为空） |
| `resolve_symbol` | `name, image?` | 符号地址 |

### 4.5 E 组 · 理解层（PC 侧，2）

| 工具 | 输入 | 输出 |
|---|---|---|
| **`disassemble`** | `hex_bytes, base_addr, arch` | 汇编文本（capstone，PC 侧） |
| `decode_adrl` | `address` | ADRP+ADD 解出的绝对地址（UMT 已有） |

### 4.6 F 组 · ptrace 调用（6）

| 工具 | 输入 | 输出 | 说明 |
|---|---|---|---|
| `attach_begin` | `timeout_ms` | `{session_id}` | 开始冻结会话 |
| `attach_end` | `session_id` | `{ok, elapsed_ms}` | 结束并恢复 |
| `call_function` | `session_id, address, args[], return_kind` | **原始返回值**（x0） | 核心 |
| `call_function_batch` | `session_id, address, arg_sets[]` | 结果数组 | 省轮次 |
| `scratch_alloc` | `size` | 目标进程内可写地址 | ptrace 调 mmap |
| `read_string` | `address, max_len, wide` | 字符串 | **FName 可能是 UTF-16** |

### 4.7 G 组 · 引擎语义（领域层，8）

| 工具 | 参数化内容（对应 2.2 的硬编码卡点） |
|---|---|
| `version_detect` | 返回版本字符串 + 选中的偏移表（区分"版本错"与"锚点错"） |
| `names_scan` | `min_ptr` / `max_ptr` / `anchor_names[]` / `anchor_offset` / `region` / `max_candidates` |
| `names_sample` | `candidate_id, count` → 前 N 个 FName 字符串 ← **AI 判断的命脉** |
| `objects_scan` | `names_candidate_id?` → **解耦 GNames 依赖**，可独立扫描 |
| `objects_sample` | `candidate_id, count` → 前 N 个对象全名 |
| `class_search` | `pattern, limit` → 类名 + 大小 + 字段数（**不含完整定义**） |
| `class_describe` | `class_name` → 完整字段 / 属性 / 偏移 |
| `probe_apply` | 验证过的偏移组合写回 `UEVars`，继续走正常 dump 流程 |

**合计：约 35 个工具**（原语 15、复合与领域 14、流程与其他 6）

---

## 5. 关键场景流程

### 5.1 常规自主定位（非加密，预期覆盖 97%）

```
① probe_status           → lastError = "GetNamesPtr 返回 0"
② version_detect         → "++UE4+Release-4.27"，偏移表 = UE4_25_27（版本没问题）
③ scan_pattern           → 用 211 条 UE4.25-27 pattern 找 GetEntry（秒级）
④ names_scan（多锚点）    → 47 个候选，每个带前 16 个字符串
⑤ AI 看样本              → #12 是 None/ByteProperty/IntProperty/... → 命中
⑥ objects_scan{12}       → 3 个候选
⑦ objects_sample         → /Script/CoreUObject, Package, Class... → 确认
⑧ probe_apply            → probe 成功
```

### 5.2 加密 FName 解密

```
① scan_pattern 找 GetEntry / ToString
② disassemble 该地址前 64 字节     ← AI 读汇编判断签名（几参、是否 sret）
③ scratch_alloc(256)               备 FString 输出缓冲
④ attach_begin  ┐
⑤ call_function × 16               ├ 一次冻结，30–80ms
⑥ attach_end    ┘
⑦ read_string × 16                 → 明文名字
⑧ AI 判断"像不像 UE 名字表"         → 收敛或换 pattern
```

**AI 全程不需要知道加密算法——让游戏自己解。**

### 5.3 批量问题的解法：分块 + 块间放行

全量 dump 需几十万次调用，一次冻结跑完会触发看门狗。解法：

```
测出安全窗口（如 1s）→ 每块调用数 = 窗口 ÷ 单次耗时（约 200）
→ 每块结束 detach，让游戏正常跑半秒 → 循环
```

- **AI 分析场景：只需几百个名字 → 几块 → 几秒。加密几乎无影响。**
- 全量 dump（50 万名字）→ 数十分钟到一小时，慢但游戏不死。
  这是 3% 的兜底路径，可接受慢（甚至跑一晚上）。

---

## 6. 冻结风险评估

| 风险 | 阈值 | 是否杀进程 |
|---|---|---|
| ANR（主线程不响应输入） | 5s | **不自动杀**，只弹系统对话框 |
| 游戏自带看门狗（UE 有 hang detector） | 各家不同 | **会 abort** ← 真正的闪退来源 |
| 服务器踢线 | 各家不同 | **单机游戏不存在此风险** |
| 系统回收后台进程 | 仅后台/息屏 | 与 attach 无关 |

**量化**：attach 开销是"每次 attach 一次"，不是"每次调用一次"。
`attach_begin` → N 次调用 → `attach_end`，单次调用 1–5ms，
开锁阶段 16 次 ≈ **30–80ms**，距 ANR 的 5s 差两个数量级。

**五阶段中只有 ptrace 验证有体感**，其余（版本检测 / pattern 快路径 /
结构扫描 / 全量 dump）游戏全程正常运行，只是要等。

**比闪退更隐蔽的风险是死锁**：若只挂单个线程让游戏继续跑，
调用 `FName::ToString` 可能撞上游戏持有的锁 → 永久卡住。
`KittyTrace` 现有实现是 `ptrace(PTRACE_ATTACH, remotePID())` = 挂全进程，**正合用**。

---

## 7. 实施路线

| 阶段 | 内容 | 产出 | 预估 |
|---|---|---|---|
| **0** | **冻结安全窗口测量** | 本机真实安全时长曲线 | **10 分钟** |
| 1 | pattern 库抽取（按版本去重、标来源） | 可直接用的版本→pattern 表 | 半天 |
| 2 | 多锚点接进 `GetNamesPtr()` 通用路径 | C++ 小改动 | 半天 |
| 3 | C++ 命令服务 + 队列 + 工具分发 | 新增约 500–800 行 | 2–3 天 |
| 4 | PC 侧 `mcp_server.py`（含 `disassemble`） | 新增约 700–900 行 | 2 天 |
| 5 | ptrace 工具层（F 组 6 个） | 复用 `KittyTrace`，胶水 100–200 行 | 1–2 天 |
| 6 | **10 款游戏实测** | 命中率 / 耗时 / 失败分布报告 | 2–3 天 |
| 7 | 注入（条件触发） | 独立子系统 | **暂缓** |

**阶段 0 先做**，它决定后面所有冻结相关参数。

### 阶段 7 的触发条件（满足任一再上）

1. 加密 FName **真的出现**（实测撞上，非理论 3%），且需全量 dump
2. 开始需要回答"谁在改这个值"这类**行为分析**问题
3. 80ms 实测下来确实无法接受

---

## 8. 风险与边界

### 8.1 无解或高难度的情况

| 情况 | 解法 | 备注 |
|---|---|---|
| FNameEntry 加密 / 自定义名字池 | ptrace 调 `GetEntry` / `ToString`，让游戏自己解 | 29 个 profile 仅 DeltaForce 一例，约 3% |
| 版本字符串被 strip | 逐个尝试所有版本偏移表；或在进程内调引擎版本 API | 慢但可行 |
| libUE4.so 改名 / 静态链接 | 枚举 `/proc/pid/maps` 所有可执行模块逐个检查 UE 特征 | **与 hook 无关，纯外部可做** |
| pattern 随游戏更新失效 | 保留结构扫描兜底 | 这正是 AutoFix 诞生的原因 |

### 8.2 已知缺陷与兜底

`KittyTrace` 的返回陷阱用 `SIGSEGV/SIGILL` 判断函数返回，
**无法区分"陷阱触发"与"被调用函数自己崩了"**。
兜底：**校验返回值**——检查指针可读、字符串是否像 UE 名字。
崩了会返回垃圾，一验就露馅，且这恰好是 AI 擅长的。

### 8.3 NULL待补充


---

## 9. 验收标准

| 指标 | 目标 |
|---|---|
| 自主定位命中率 | 10 款第三方单机游戏 ≥ 7 款成功定位 GNames + GWorld |
| 单次定位耗时 | 常规（pattern 命中）≤ 30s；兜底（结构扫描）≤ 5min |
| 单次冻结时长 | ≤ 实测安全窗口的 50% |
| AI 判定准确率 | `names_sample` 后 AI 选中的候选，正确率 ≥ 90% |
| 崩溃率 | 10 款游戏测试期间，因工具导致的闪退 = 0 |
| 上下文占用 | 单次完整定位流程 ≤ 50K tokens |

---

## 10. 附录：关键代码位置索引

| 内容 | 位置 |
|---|---|
| 主循环 / 渲染 | `src/executable.cpp:1761` / `:1805` |
| 共享状态 `DumpUiState` | `src/executable.cpp:110-135` |
| worker 线程 | `src/executable.cpp:945-989` |
| `GetNamesPtr()` | `src/UE/UEGameProfile.cpp:971` |
| 指针范围硬编码 | `src/UE/UEGameProfile.cpp:995-996` |
| 单锚点 `ByteProperty` | `src/UE/UEGameProfile.cpp:999-1001` |
| FNamePool 锚点偏移 | `src/UE/UEGameProfile.cpp:1030` |
| GNames 锚点偏移 | `src/UE/UEGameProfile.cpp:1046` |
| **多锚点名单（未用于通用路径）** | `src/UE/UEGameProfile.cpp:80-84` |
| `GetGUObjectArrayPtr()` | `src/UE/UEGameProfile.cpp:893`（依赖在 `:900`） |
| `findIdaPattern` | `src/UE/UEGameProfile.cpp:834` |
| 版本检测 | `src/UE/UEGameProfiles/AutoFix.cpp:140` |
| **`KittyTraceMgr::callFunction`** | `KittyMemoryEx/KittyTrace.cpp:140` |
| `KittyScanner` 接口 | `KittyMemoryEx/KittyScanner.hpp:30-112` |
| `KittyMemoryMgr` 接口 | `KittyMemoryEx/KittyMemoryMgr.hpp:57-133` |
| 反汇编能力 | **无**（Keystone 头文件在，但 `-DkNO_KEYSTONE` 已关闭） |
