# UMT MCP v0.1.0 — 泪心团队开源发布

> **UE 引擎内存调试 · 47 功能全开 · 完整源码 + 文档双开源**
> *不求赞赏，开源不易，编码熬夜伤身，愿此项目为逆向与引擎分析同行者点亮一盏灯。*

---

## 🌟 项目简介

**UMT (Unreal Memory Tools) MCP** 是泪心团队历时投入打造的 UE 引擎内存分析工具链，面向 UE4 / UE5 全系列游戏。通过 MCP（Model Context Protocol）标准接口，将设备端 C++ 探测能力与 AI 智能体编排无缝衔接，实现 **探查 → 定位 → 反汇编 → Dump → 分析** 全链路自动化。

这不是又一个小工具，而是一套**经过实战验证、文档先行、架构清晰**的完整工程体系——从底层内存原语到上层 AI 可调用工具，四层架构分层解耦，每一个设计决策背后都有真实踩坑换来的经验沉淀。

---

## 🚀 核心亮点

### 完整开源 · 零门槛

| 项目 | 说明 |
|------|------|
| **C++ 设备端源码** | `Alltear_UnrealMemoryToolsNewMcp/` 全部 `.cpp/.hpp/.h` 完整开源 |
| **Python MCP 服务端** | `mcp_server/` 完整源码 + 打包分发脚本 |
| **技术文档** | 27 篇文档全公开，架构设计、协议规范、API 手册一应俱全 |
| **License** | GPL-3.0，允许商用，修改必须开源回馈社区 |

### 47 项功能，覆盖 UE 逆向全场景

```
▸ 内存操作    MEMORY_READ / MEMORY_READ_VALUE / WRITE_MEMORY / READ_STRING
▸ 引擎探测    START_PROBE / GET_PROBE_STATUS / APPLY_PROBE_OVERRIDES
▸ 扫描工具    SCAN_GNAMES / SCAN_OBJECTS / SEARCH_MEMORY / SCAN_CANDIDATES
▸ 反汇编      DISASSEMBLE / SYMBOL_QUERY / FUNCTION_ANALYSIS
▸ 调试功能    PROCESS_LIST / GET_LOGS / CALL_FUNCTION
▸ 配置管理    GET_CONFIG / LIST_TOOLS / HEALTH_CHECK
▸ ... 其余 30+ 项功能
```

### 三种连接模式，全平台覆盖

| 模式 | 场景 | 配置 |
|------|------|------|
| **USB 调试** | PC 连手机 | 默认，adb forward 自动建立隧道 |
| **局域网直连** | 同 WiFi 内网 | `--no-adb --host <设备IP>` |
| **Operit 同机** | 手机端直接跑 | `--no-adb --host 127.0.0.1` |

### 四层架构，设计严谨

```
L1  Memory Primitives   — KittyMemory 内存读写原语
L2  Engine Location     — UEGameProfile 探针定位（EUObjectArray/GNames）
L3  AutoFix             — 运行时偏移自动修正（适配各游戏魔改）
L4  MCP Tools           — 47 项标准化工具暴露给 AI 智能体
```

---

## 📚 为何这个项目的文档比源码更值钱

逆向工程的真正壁垒从来不是代码本身，而是**踩过坑之后总结出的判断方法和排障路径**。本项目的 27 篇文档就是这套经验的完整沉淀：

- **MCP 调用工具无法得到精确结果** → 有 `mcp_server开发步骤.md` 讲解如何引导 AI 分步探查
- **Token 燃烧飞快** → 有 `工具功能规格.md` 定义 4K token 硬约束与分页策略
- **AI 失忆乱编** → 有 `mcp-protocol.md` 固化协议契约，强制 AI 遵循标准流程
- **Operit 手机端直连** → 有完整的三模式配置方案和实测验证

这些是无数次真机调试后写下来的活知识，不是空泛的理论。

---

## ⚠️ 已知问题

以下问题已定位根因并有修复方向，欢迎各位一起完善：

| 游戏 | 状态 | 根因 |
|------|------|------|
| **元梦之星 (LetsGo)** | 🔴 probe 失败 | FNAME_OUTLINE_NUMBER 模式，ID > 0x200000 被 verifyCandidate 过滤 |
| **和平精英 HD (pubgmhd)** | 🔴 probe 失败 | PUBGMHD.hpp 硬编码 IsUsingFNamePool=false，实际设备为 FNamePool 模式 |
| **NumElements 偏移** | 🟡 待验证 | UE4.2x 的 NumElements 在 MCP 版为 0x10，原始版为 0x14 |

详细分析与修复方向见 `docs/先行开发修正文档手册.md`。

---

## 🔐 安全说明

- 设备端 `CommandServer` 默认只监听 `127.0.0.1:35515`（安全红线）
- 非回环 bind 地址需通过 `/sdcard/UnrealMemoryTools/mcp_bind.conf` 显式配置
- 客户端与设备端**无认证**，请确保仅在可信网络下使用
- 危险操作（WRITE_MEMORY / CALL_FUNCTION）默认需要 `confirmDangerous=true`

---

## 📄 许可证

本项目采用 **GPL-3.0** 许可证：
- ✅ 允许商业使用
- ✅ 允许修改和二次开发
- ⚠️ 修改后的分发必须开源源代码
- ℹ️ 版权 © 2026 TearGame（泪心团队）

---

## 🤝 贡献指南

欢迎 fork、提 PR、提 Issue！贡献前请务必阅读文档优先级列表（详见 [README.md](README.md)），避免重复造轮子。

---

## 🙏 致谢

感谢以下作者对本项目的技术支持与贡献：

- **[DreamFekk](https://github.com/DreamFekk)** — 项目核心研发与架构设计
- **[ssyclr](https://github.com/ssyclr)** — 项目核心研发与工具链实现

---

**泪心团队 · 不求赞赏，开源不易，愿此项目为每一位引擎分析者添砖加瓦。**

🌐 官网：https://teargamestorem.top/
💬 Discord：https://discord.gg/yghYHcEdD
📧 邮箱：tearhacker@outlook.com
🐧 QQ：2254013571
