# UMT MCP v0.1.0

> **Unreal Engine 内存分析 · 47 功能全开 · 完整源码 + 27 篇文档双开源**
> *不求赞赏，开源不易，编码熬夜伤身，愿此项目为逆向与引擎分析同行者点亮一盏灯。*

---

## 🌟 一句话介绍

泪心团队开源的 **UE4 / UE5 引擎内存调试 MCP 工具链**，47 项功能覆盖探查、扫描、反汇编、Dump 全链路，四层架构设计清晰，所有源码与文档完整公开——不只是工具，更是踩坑换来的经验体系。

---

## 🔥 为什么值得用

### 完整开源，零门槛
- **C++ 设备端**：`Alltear_UnrealMemoryToolsNewMcp/` 全部源码开放
- **Python 服务端**：`mcp_server/` 完整源码 + 一键打包脚本
- **技术文档**：27 篇文档全公开，架构/协议/API 一应俱全
- **GPL-3.0**：允许商用，修改必须开源回馈社区

### 47 项功能，UE4+UE5 全覆盖
```
内存操作    MEMORY_READ / WRITE_MEMORY / READ_STRING
引擎探测    START_PROBE / APPLY_PROBE_OVERRIDES
扫描工具    SCAN_GNAMES / SCAN_OBJECTS / SEARCH_MEMORY
反汇编      DISASSEMBLE / SYMBOL_QUERY
调试功能    PROCESS_LIST / GET_LOGS / CALL_FUNCTION
... 其余 37 项
```

### 三层架构，AI 可编排
```
L1  KittyMemory 内存原语
L2  UEGameProfile 探针定位（GObjectArray / GNames）
L3  AutoFix 运行时偏移修正
L4  MCP 47 项标准化工具 → AI 智能体调用
```

### 三种连接模式
| 模式 | 场景 | 启动命令 |
|------|------|---------|
| USB 调试 | PC ↔ 手机 | `python server.py` |
| 局域网直连 | 同 WiFi | `python server.py --no-adb --host <设备IP>` |
| Operit 同机 | 手机端直跑 | `python server.py --no-adb --host 127.0.0.1` |

---

## 📚 文档比源码更值钱

逆向工程的真正壁垒是**踩过坑之后的判断方法**，不是代码本身。本项目 27 篇文档是无数次真机调试的活知识沉淀：

| 痛点 | 对应文档 |
|------|---------|
| 调用工具无法得到精确结果 | `mcp_server开发步骤.md` |
| Token 燃烧飞快 | `工具功能规格.md`（4K token 硬约束） |
| AI 失忆乱编 | `mcp-protocol.md`（协议契约固化） |
| 手机端直连 | `MCP网络配置三模式分析.md` |

---

## ⚠️ 已知问题

| 游戏 | 状态 | 根因 |
|------|------|------|
| 元梦之星 (LetsGo) | 🔴 probe 失败 | FNAME_OUTLINE_NUMBER，ID > 0x200000 被过滤 |
| 和平精英 HD | 🔴 probe 失败 | PUBGMHD.hpp 硬编码 IsUsingFNamePool=false |
| NumElements 偏移 | 🟡 待验证 | UE4.2x 0x10 vs 0x14 真机复核 |

修复方向见 `docs/先行开发修正文档手册.md`。

---

## 🔐 安全说明

- 设备端默认监听 `127.0.0.1:35515`，非回环需显式配置
- 客户端无认证，请在可信网络下使用
- 危险操作需 `confirmDangerous=true`

---

## 📄 License

**GPL-3.0** — 允许商用，修改须开源 © 2026 TearGame

---

## 🙏 致谢

- **[DreamFekk](https://github.com/DreamFekk)** — 核心研发与架构设计
- **[ssyclr](https://github.com/ssyclr)** — 核心研发与工具链实现

---

**泪心团队** | 🌐 https://teargamestorem.top/ | 💬 https://discord.gg/yghYHcEdD | 📧 tearhacker@outlook.com | QQ: 2254013571
