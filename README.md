# UE4 ImGui MCP — Android UE4/UE5 游戏逆向分析平台

> **AI 驱动的 UE4/UE5 内存分析工具集**：自动定位引擎全局、内存扫描、SDK Dump、ARM64 反汇编/反编译、远程函数调用。
>
> 协议：**GPL-3.0**（允许商用，修改后必须开源）
>
> 📌 **主开发分支：`tearue4mcp`**（所有最新功能、文档、构建产物均以此分支为准）

---

## 👋 关于项目 & 联系方式

本项目由 **泪心** 团队维护，开源不易，编码熬夜伤身。不求赞赏，只愿与众同行为开源创造奉献，谢谢大家支持！

| 渠道 | 链接/信息 |
|------|-----------|
| 💬 Discord | https://discord.gg/yghYHcEdD |
| 🌐 官网 | https://teargamestorem.top/ |
| 📧 邮箱 | tearhacker@outlook.com |
| 🐧 QQ | 2254013571 |

---

## 🏆 核心价值：文档 > 源码

> **泪心团队此次开源，最值钱的部分不是代码，而是沉淀下来的工程经验。**
> 源码只是实现的载体，文档里记录的是"踩过多少坑、绕过多少弯路"的真实经验。
> 这些经验一旦缺失，重写一遍代码需要数周甚至数月试错。

### 文档解决了哪些核心难题？

| 问题 | 解决方案在 |
|------|-----------|
| **MCP 工具调用无返回结果、无法精确下一步** | `docs/mcp-protocol.md` §5-6（心跳判活 + 长轮询 + `progress/etaSeconds` 反馈） |
| **Token 燃烧飞快、频繁断连** | `docs/泪心UE4_imGui_AnalyseToolsForMCp开发必读架构.md` §4.2（单工具响应 ≤4K token、输出预算控制） + `docs/MCP逆向分析UE4工具优化改造文档.md`（token 压缩策略） |
| **智能体失忆 / 胡编乱造** | `docs/MCP逆向分析UE4引擎需补充能力.md`（自动 profile → GNames/GUObjectArray 定位 → Dump → 内存重建闭环） + `docs/api/00-索引.md`（精确参数契约，让 LLM 不猜） |
| **Operit 手机端 + PC 局域网兼容** | `docs/MCP网络配置三模式分析与改造方案.md`（三种 bind 模式，无需 adb 直连） |

### 为什么文档比源码更重要？

1. **源码是静态的**：改一行就过期，但经验是跨版本的
2. **源码不解释"为什么"**：协议设计、token 约束、安全取舍——这些都在文档里
3. **源码有歧义**：API 行为、边界条件——文档用自然语言消除歧义
4. **源码不记录历史**：为什么 `NumElements` 是 0x10 不是 0x14？为什么禁用 `isPtrWritable`？——在 `docs/先行开发修正文档手册.md` 和 `docs/UMT_改动代码文档.md`

---

## 📌 一句话定位

把手机端 UE4 游戏注入器改造成 **MCP 服务器**，让 AI 大模型自主完成"找偏移 → 读内存 → 解析 SDK → 反汇编"的完整逆向闭环，无需人肉盯界面。

```
AI (Claude / Cursor / WorkBuddy)
   │ MCP stdio · 工具调用
   ▼
mcp_server/        (Python, FastMCP, 43~47 个工具)
   │ TCP + NDJSON · 127.0.0.1:35515 · adb forward
   ▼
Alltear_UnrealMemoryToolsNewMcp/   (Android ELF, C++ NDK27)
   │ 内存读取 · UE 探针 · AutoFix · ptrace · Ghidra
   ▼
目标游戏进程（com.tencent.letsgo / com.tencent.tmgp.pubgmhd / ...）
```

---

## 🎯 核心能力

| 能力 | 工具示例 | 说明 |
|------|---------|------|
| **引擎自动探针** | `startProbe` / `locateEngineGlobals` | 自动定位 GNames / GUObjectArray / GWorld |
| **内存读写扫描** | `readMemory` / `writeMemory` / `scanPattern` | 字节码范围 + IDA 风格 pattern |
| **SDK Dump** | `startDump` / `dumpSDK` | 导出 Classes/Structs/Functions 头文件 |
| **ARM64 反汇编** | `disassemble` / `decodeAdrl` | PC 侧 capstone |
| **ARM64 反编译** | `decompile` / `decompileStatus` | 内嵌 Ghidra decompiler（可选） |
| **远程函数调用** | `callRemoteFunction` / `callRemoteFunctionBatch` | ptrace 注入调用，推荐 Batch 无状态版 |
| **引擎语义检索** | `searchClasses` / `describeClass` / `inspectObject` | 按名字/类型搜类，展开对象字段 |
| **指针链追踪** | `followPointerChain` | 支持 `[k]` 数组步进（`LocalPlayers[0]`） |
| **候选扫描** | `scanGNames` / `scanObjects` / `scanCandidates` | 带内嵌样本与评分，三轮压一轮 |

---

## 📁 项目结构

> 以下路径描述以 `tearue4mcp` 分支为基准。其他分支（如 `master`、`dev-*`）仅供参考或历史版本。

```
ue4ImGuiAutoWorkingMcpBytear/
├── LICENSE                           # GPL-3.0 许可证（修改必须开源）
├── Alltear_UnrealMemoryToolsNewMcp/  # MCP 版设备端（Android C++ ELF）
│   ├── src/                          # 源码
│   │   ├── mcp/                    # 命令服务层（CommandServer / Dispatcher / Queue）
│   │   ├── UE/                     # UE 引擎层（Profile / Offsets / Wrappers）
│   │   ├── AutoFix/                # 自动偏移修正（3185 行）
│   │   ├── executable.cpp          # 主入口 + MCP 命令注册（3589 行）
│   │   └── Dumper.cpp              # Dump 流程
│   ├── build/                        # CMake 构建产物（NDK27 + VS2026 CMake/Ninja）
│   └── outputs/arm64-v8a/          # 编译产物 UnrealMemoryTools
├── Andriod_UnrealMemoryTools/        # 原始参考版本（探针成功的基准）
├── mcp_server/                       # Python MCP 服务端
│   ├── src/umt_mcp/
│   │   ├── tools.py                # 43~47 个工具注册（camelCase）
│   │   ├── bridge.py               # 设备端 socket 桥接（独立读线程 + 重连）
│   │   ├── protocol.py             # NDJSON 协议实现
│   │   └── adb.py                  # ADB forward 管理
│   └── server.py                   # FastMCP 入口
├── docs/                             # 完整文档体系（27 份）
│   ├── api/                        # 设备端 API 精读文档（15 份）
│   ├── 先行开发修正文档手册.md       # 2026-09-01 代码差异定位
│   ├── UMT_改动代码文档.md           # MCP 版 vs 原始版改动清单
│   ├── mcp-protocol.md             # 两端 wire protocol
│   ├── MCP可用工具文档.md            # 43~47 工具规格
│   ├── 泪心UE4_imGui_AnalyseToolsForMCp开发必读架构.md  # 架构 v1.2
│   └── ...
├── ilbil2cppMCP源项目/               # il2cpp MCP 参考实现（对比学习）
└── analysis_artifacts/               # diff / patch / 日志对比产物
```

---

## 🔧 构建与运行

> 以下构建步骤均以 `tearue4mcp` 分支为准。切换分支前请提交或暂存当前改动。

### 设备端（Android C++）

```bash
cd Alltear_UnrealMemoryToolsNewMcp

# 配置 CMake（NDK27 + VS2026 CMake/Ninja）
CMAKE="D:/ProgramerDevelop/VS2026/SDK/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
"$CMAKE" -S . -B build -DNDK_PATH=D:/ProgramerDevelop/windowsNDK27

# 增量编译（增删源文件后需 rm 对应 .o 再 build）
"$CMAKE" --build build

# 产物：outputs/arm64-v8a/UnrealMemoryTools
# 验证新代码：用 NDK llvm-strings 搜特征字符串
llvm-strings.exe outputs/arm64-v8a/UnrealMemoryTools | grep "诊断"
```

**已知阻塞**：`ANativeWindowCreator::ProcessMirrorDisplay()` 在 executable.cpp:4288 被调用，但头文件已删除该成员。修复它不在 MCP 任务范围内。

### PC 侧 MCP Server

```bash
cd mcp_server

# 启动 venv（首次）
python -m venv .venv
.venv/Scripts/activate

# 安装依赖
pip install -r requirements.txt

# 启动（需要 adb forward）
python server.py
```

**网络三种模式**（已实施）：
| 模式 | 场景 | 配置 |
|------|------|------|
| 模式 1 | 局域网直连 | `/sdcard/UnrealMemoryTools/mcp_bind.conf` 写 `192.168.x.x` |
| 模式 2 | 全局暴露（危险！） | 同上写 `0.0.0.0`，慎用 |
| 模式 3 | 同机直连（Operit） | 无需 adb，`--no-adb --host 127.0.0.1` |

---

## 📚 文档阅读顺序（必读优先级）

> **警告**：未读完以下文档前，**不要开始写代码**！先通读再动手，避免重复开发或无效开发。
> 按优先级分三档：🔴 必须 → 🟡 强烈建议 → 🟢 按需参考

### 🔴 第一档：必读（开发前必通读）

| 顺序 | 文档 | 必读原因 |
|------|------|---------|
| 1 | `docs/mcp_server开发步骤.md` | 知道怎么启动、配置、排查连接问题 |
| 2 | `docs/mcp-protocol.md` | 两端协议是唯一对齐事实，任何改动先改文档 |
| 3 | `docs/MCP开发路线图.md` | 清楚当前进度与下一步方向，避免重复造轮子 |
| 4 | `docs/MCP可用工具文档.md` | 43~47 工具规格，知道每个工具输入输出是什么 |
| 5 | `docs/泪心UE4_MCP工具功能规格.md` | 功能规格总纲，所有工具的设计依据 |

### 🟡 第二档：强烈建议（接手项目前必读）

| 顺序 | 文档 | 必读原因 |
|------|------|---------|
| 6 | `docs/overview.md` | 全貌概览 + 成熟度评分 + 建议优先级 |
| 7 | `docs/泪心UE4_imGui_AnalyseToolsForMCp功能说明.md` | 做什么、为什么做 |
| 8 | `docs/泪心UE4_imGui_AnalyseToolsForMCp开发必读架构.md` | 怎么写、规范、token 约束 |
| 9 | `docs/UMT_改动代码文档.md` | MCP 版 vs 原始版改动清单 |
| 10 | `docs/api/00-索引.md` | 设备端 15 模块 API 精读索引 |

### 🟢 第三档：按需参考（遇到问题时查）

| 文档 | 何时查阅 |
|------|---------|
| `docs/先行开发修正文档手册.md` | 遇到 LetsGo/pubgmhd 探针失败 |
| `docs/MCP逆向分析UE4工具优化改造文档.md` | 需要优化响应 token 量 |
| `docs/MCP逆向分析UE4引擎需补充能力.md` | 需要扩展新工具 |
| `docs/GhidraNative集成方案.md` | 需要启用反编译能力 |
| `docs/设备端命令服务设计.md` | 需要修改 CommandServer |
| `docs/api/` 各模块文档 | 精读特定模块时逐份查阅 |

---

## ⚠️ 已知问题（截至 2026-09-01）

> 详见 `docs/先行开发修正文档手册.md`，当前未修复，待排期。

| 优先级 | 问题 | 现象 | 根因假设 |
|--------|------|------|----------|
| **P0** | LetsGo（com.tencent.letsgo）探针失败 | `ERROR_INIT_GUOBJECTARRAY`，`id=1392538734 name=''` | `AutoFix.cpp:186` 强制 `_outlineNumber=true` 改变探测路径；verifyCandidate `id > 0x200000` 过滤 outline number |
| **P0** | pubgmhd（和平精英）探针失败 | `ERROR_INIT_GUOBJECTARRAY` | `PUBGMHD.hpp` 硬编码 `IsUsingFNamePool()=false`，但设备端扫到 FNamePool |
| **P1** | `NumElements` 偏移争议 | 原始版 0x14 vs MCP 版 0x10（UE4_20/UE4_23_24） | 需真机复核 LetsGo 实际布局 |
| **P1** | LetsGo 重启后探针不稳定 | `ERROR_INIT_OBJOBJECTS`，需手动 `APPLY_PROBE_OVERRIDES` | 自动探测鲁棒性不足 |
| — | 成熟度综合评分 | **7/10** | 功能覆盖强，传输健壮性偏弱 |

**已确认一致（排除嫌疑）**：`AutoFixOffsets.cpp`（3185 行）、`Dumper.cpp`、Probe/Dump 执行顺序、两版 `GetNameEntry` 逻辑。

---

## 🔐 安全说明

- 设备端默认只监听 `127.0.0.1:35515`（回环），**不以 root 暴露给局域网**。
- 非回环 bind 必须显式写 `/sdcard/UnrealMemoryTools/mcp_bind.conf`，用完即删配置并重启游戏。
- 危险操作（写内存、ptrace 远程调用）需要显式 `confirm_dangerous=true`。
- 设备端无 Token 认证——这是单对单调试场景的设计取舍，生产环境请仅用于可信网络。

---

## 📄 许可证

**GPL-3.0**（GNU General Public License v3）

- ✅ 允许商用
- ✅ 允许修改
- ❌ 修改后分发必须开源（强 copyleft）
- 详见根目录 `LICENSE` 文件

---

## 🤝 贡献

1. Fork 本仓库
2. 基于 `dev-*` 分支开发
3. 修改设备端代码后重新编译（NDK27 + CMake/Ninja）
4. 更新 `docs/UMT_改动代码文档.md` 记录改动
5. 提交 PR，描述改动目的与验证方式

---

*项目日期：2026-08-29 ~ 2026-09-01*
*维护：泪心安卓领域基本盘技术*
*当前主分支：`tearue4mcp`*
