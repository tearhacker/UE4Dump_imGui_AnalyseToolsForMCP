# ghidra-native 反编译器集成方案

> 状态：方案设计（未实施）
> 日期：2026-08-31
> 范围：`Alltear_UnrealMemoryToolsNewMcp`（设备端 ImGui 读写工具 + MCP 命令服务）
> 依赖源码：`ilbil2cppMCP源项目/ghidra-native`（radareorg/ghidra-native @ 0.6.4，Ghidra commit `a14dad67`）

---

## 1. 背景与目标

### 1.1 为什么集成

项目现有 MCP 能力止于"反汇编"（`DISASSEMBLE` 命令 + 自研 `Arm64Disasm` 轻量解码器）。日常逆向中三类高频需求无法被反汇编满足：

1. **读函数逻辑**：SDK 里每个函数只有 RVA，知道在哪、不知道干啥，只能对着 ARM64 汇编猜；
2. **AI 验证候选**：`AutoFix` / `UECandidateAnalysis` / `NativeFunctionResolver` 找到候选偏移/函数后，Python 侧 AI 只能读汇编判断对错——LLM 读 C 伪代码的能力远强于读汇编；
3. **理解 native 逻辑**：伤害计算、血量写入、AC 检查等目标游戏逻辑，需要 C 级可读性才能写出正确的内存操作。

集成 Ghidra 反编译器后，以上三类场景全部升级为"给 RVA → 出 C 伪代码"，且反编译结果通过 MCP 管道直接喂给 Python AI，形成 **AI 辅助逆向** 闭环。

### 1.2 核心诉求

- 作为**依附模块**嵌入 `UnrealMemoryTools` 源码，独立目录、独立 CMake target，删除目录即功能消失，现有模块零改动；
- 在设备端进程内运行，直接读**目标进程内存**（而非磁盘 dump 文件）——当游戏代码被运行时 patch/hook、或磁盘 dump 与实际内存不一致时，只有设备端反编译看到的是真代码；
- 通过现有 `CommandDispatcher` 管道暴露 `DECOMPILE` MCP 命令，Python 端加对应 tool，AI 对话闭环；
- 不引入 Java、不引入重型第三方依赖（capstone/unicorn 等已有取舍先例）。

### 1.3 非目标（v1 不做）

- 不做全量函数自动发现（只反编译调用方指定地址的函数）；
- 不做交叉引用 / 符号恢复 / 类型传播的全库分析（Ghidra 引擎有能力，但 v1 只按单函数窗口跑）；
- 不做 armeabi-v7a（目标均为 arm64-v8a）；
- 不做 ImGui 可视化面板（留作 Phase 4 可选项）。

---

## 2. 现状盘点（集成前已具备的资产）

| # | 资产 | 位置 | 价值 |
|---|---|---|---|
| 1 | ghidra-native 完整源码 | `ilbil2cppMCP源项目/ghidra-native/` | 反编译器核心 `src/decompiler/`（~200 文件，纯 C++11）+ AARCH64 处理器数据 |
| 2 | **预编译 arm64 反编译器 .so（3.3MB）** | `ilbil2cppMCP源项目/decompiler/arm64-v8a.so`（NDK r29 构建，stripped） | 已导出干净的 C API（见 2.2），可 dlopen 直接复用，零编译成本 |
| 3 | AARCH64 spec 全套（含**已编译的 .sla**） | `ilbil2cppMCP源项目/decompiler/spec/`（ldefs/pspec/cspec/sla 四件） | `.sla` 是 Sleigh 编译器产物——仓库里 `src/Processors/AARCH64/data/` 只有 `.slaspec` 源，**这份 .sla 省掉了 Sleigh 编译器环节** |
| 4 | 参考实现：Zygisk 版 decompiler MCP | `ilbil2cppMCP源项目/`（module.prop v2.2.1，作者：洋葱落日） | 其 `mcp/mcp_server.py` 已实现 `decompiler_status` / `decompile_function` 两个 tool，验证了"设备端加载 ghidra 反编译器 + MCP 暴露"路线可行 |
| 5 | 测试目标库 | `.codex_artifacts/live_ue4_analysis/libUE4.so`（195MB，aarch64，stripped） | Phase 0 质量验证的现成素材 |
| 6 | MCP 命令管道 | `src/mcp/CommandServer` + `CommandDispatcher` + `Protocol.hpp`（端口 35515，NDJSON 帧） | 新命令照 `DISASSEMBLE` 模式注册即可（`executable.cpp:1734`） |
| 7 | 长任务模式 | `START_*` / `GET_*_STATUS` + `jobId` + 主线程/worker 线程约定（`CommandDispatcher.hpp` 头注） | 反编译大函数属重活，直接套用现有 jobId 模式 |
| 8 | 内存读取层 | `KittyMemoryEx`（`kMgr.readMem`） | 自定义 LoadImage 的唯一数据来源 |
| 9 | Python MCP 端 | `mcp_server/src/umt_mcp/`（tools.py 的 `_dev(...)` 模式） | 加一个 `decompile` tool 包装 adb 调用 |

### 2.1 ghidra-native 仓库结构（只用其中一部分）

```
ghidra-native/
├── src/decompiler/          # ← 反编译器核心（本方案唯一要编译的源码目录）
│   ├── Makefile             #    源文件分组清单（CORE/DECCORE/SLEIGH/...）
│   ├── grammar.y pcodeparse.y slghparse.y slghscan.l xml.y ruleparse.y   # 需 bison/flex 生成
│   ├── loadimage.hh/.cc     # LoadImage 抽象基类（自定义加载器的接口）
│   ├── sleigh_arch.hh/.cc   # SleighArchitecture（spec 加载/语言注册）
│   ├── ifacedecomp.cc       # 控制台命令实现 = 官方 API 调用链的参考实现
│   └── consolemain.cc ...   # 控制台入口（不编译进库）
├── src/Processors/AARCH64/data/
│   └── languages/           # AARCH64.slaspec / .pspec / .cspec / .ldefs（无 .sla）
├── patches/                 # 15 个启用补丁（Windows 下需要 0023-Undef-LoadImage）
└── Makefile                 # 维护脚本（sync 上游 / 打补丁），与编译无关
```

### 2.2 预编译 .so 的导出符号（已用 llvm-readelf 验证）

```
ghidra_decompiler_api_version
ghidra_decompiler_initialize
ghidra_decompiler_decompile
ghidra_decompiler_free
inflate / inflateEnd / inflateInit_   # 静态链入 zlib 的残留导出
```

**重要限制**：该 .so 的 C wrapper 源码不在本仓库（全仓 grep `ghidra_decompiler_` 零命中），
四个函数的**参数契约（地址格式、size 语义、返回缓冲、错误约定）未知**，复用前必须先
逆向/实验确认 ABI（见 8.1 风险 R4）。

---

## 3. 总体架构

### 3.1 模块分层

```
┌────────────────────────────────────────────────────────────────────┐
│ Python MCP (mcp_server/src/umt_mcp)                                │
│   tools.py: decompile(address, size?)  ── adb/bridge ──┐           │
└────────────────────────────────────────────────────────┼───────────┘
                                                         │ NDJSON (35515)
┌────────────────────────────────────────────────────────▼───────────┐
│ 设备端 UnrealMemoryTools (arm64)                                    │
│                                                                    │
│  CommandDispatcher::Register("DECOMPILE", ...)   ← 新增 handler    │
│        │  (重活 → gWorkerThread + jobId，沿用现有长任务模式)        │
│  ┌─────▼──────────────────────────────────────────────┐            │
│  │ src/mcp/DecompileHandler.cpp                       │  ← 新增    │
│  │   参数校验 / 引擎懒初始化 / 结果格式化            │            │
│  └─────┬──────────────────────────────────────────────┘            │
│  ┌─────▼──────────────────────────────────────────────┐            │
│  │ third_party/ghidra_decomp/   ★ 依附模块（本方案主体）│           │
│  │  ├── glue/GhidraEngine.{hpp,cpp}                   │  ← 新增    │
│  │  │     引擎单例：init(spec 目录) / decompile(rva) / │            │
│  │  │     线程互斥 / 异常兜底                          │            │
│  │  ├── glue/MemLoadImage.{hpp,cpp}                   │  ← 新增    │
│  │  │     自定义 LoadImage：loadFill() → kMgr.readMem │            │
│  │  └── (B 路线) libghidra_decomp.a                   │  ← 新增    │
│  │        ghidra-native src/decompiler 核心 ~90 个 .cc │            │
│  └─────┬──────────────────────────────────────────────┘            │
│        │ 字节来源                                                    │
│  ┌─────▼──────────────┐    ┌──────────────────────────┐            │
│  │ KittyMemoryEx(kMgr) │    │ spec/AARCH64/            │            │
│  │ 目标进程内存读取    │    │ ldefs+pspec+cspec+sla    │            │
│  └────────────────────┘    └──────────────────────────┘            │
└────────────────────────────────────────────────────────────────────┘
```

### 3.2 目录落位

```
Alltear_UnrealMemoryToolsNewMcp/
├── third_party/ghidra_decomp/          # ★ 依附模块根目录（删除即功能消失）
│   ├── CMakeLists.txt                  # libghidra_decomp 静态库 target（B 路线）
│   ├── ghidra/                         # 从 ghidra-native 拷贝的 src/decompiler
│   │   │                               #   + bison/flex 生成物（.cc 提交进库）
│   │   │                               #   + 补丁 0023 的 Windows 兼容修改
│   │   └── ...
│   ├── spec/AARCH64/                   # ldefs / pspec / cspec / sla（4 文件，~3MB）
│   ├── glue/
│   │   ├── GhidraEngine.hpp/.cpp       # 引擎封装单例
│   │   └── MemLoadImage.hpp/.cpp       # 内存 LoadImage
│   └── LICENSE                         # Apache-2.0 保留声明
├── src/mcp/
│   ├── DecompileHandler.hpp/.cpp       # DECOMPILE 命令 handler
│   └── Protocol.hpp                    # 新增错误码 Err::kDecompileFailed
└── CMakeLists.txt                      # 加 target_link_libraries + option(UMT_GHIDRA "..." ON)
```

### 3.3 依附性设计原则

1. **零侵入**：现有源码只在 `executable.cpp` 命令注册段 + `Protocol.hpp` 错误码 + 根 CMakeLists 三处加行；
2. **可裁剪**：`-DUMT_GHIDRA=OFF` 时不编译、不链接 ghidra_decomp 目录，`DECOMPILE` 命令不注册（`GET_CAPABILITIES` 自动反映，因 capabilities 走 `RegisteredCommands()` 动态枚举）；
3. **单线程访问**：Ghidra 引擎对象非线程安全，`GhidraEngine` 内部持互斥锁，所有调用串行化（反编译本身在 worker 线程执行，锁粒度=单次反编译）；
4. **异常兜底**：引擎抛 `RecovError`/`LowlevelError` 在 glue 层捕获转 `HandlerError`，绝不向 CommandServer 主线程逃逸。

---

## 4. 实施路线：A 复用 / B 自编译（决策）

### 4.1 两条路线对比

| 维度 | 路线 A：dlopen 现成 .so | 路线 B：源码自编译 libdecomp.a |
|---|---|---|
| 编译成本 | **零**（so 已构建） | 需要 bison/flex（PC 端一次性生成）+ NDK CMake target |
| ABI 契约 | **未知**（wrapper 源码不在仓库，需逆向确认） | 自己写，完全可控 |
| 版本/补丁 | 锁死在该二进制，无法修 bug | 可随时 `make sync` 升级上游、打自定义补丁 |
| 与 r2ghidra 一致性 | 未知 | 官方 `libdecomp.a` 源文件分组，r2ghidra 同款 |
| 维护性 | 黑盒 | 白盒，问题可定位到源码行 |
| 二进制体积 | 运行时 dlopen，可延迟加载（.so 不占主二进制） | 静态链接，主二进制 +3~8MB |

### 4.2 决策：分阶段混合

- **Phase 0（质量验证）**：用**路线 A**（现成 .so）。零编译、最快拿到"C 代码长什么样"的答案，
  回答整个项目最大的不确定性——**Ghidra 对 UE4 ARM64 代码的反编译质量是否够用**。
  若质量不可接受，方案终止，不投入 B 路线。
- **Phase 1 起（正式集成）**：走**路线 B**（源码自编译）。A 路线的 .so 留作"质量基准对照物"
  与应急回退（`UMT_GHIDRA_BACKEND=dlopen` 运行时开关，可选做）。

理由：A 路线的 ABI 黑盒在"试水"阶段无所谓，但作为正式功能不可接受（参数语义未知 = 线上
行为不可预期）；B 路线是唯一能长期维护的形态。Phase 0 用 A 不是为了 A 本身，
是**用最低成本消灭最大风险**。

### 4.3 备选验证环境（Phase 0 的 .so 怎么跑起来）

按成本从低到高：

1. **已部署的 Zygisk 模块**（若你机器上正跑着 `ilbil2cppMCP源项目` 的模块）：直接调它
   `mcp_server.py` 的 `decompile_function` tool，零开发；
2. **小驱动程序**：写 ~50 行 C（dlopen + dlsym + 调 4 个导出函数），NDK 交叉编译成
   arm64 可执行，在 Android 模拟器/真机 shell 里跑，对 `libUE4.so` 的已知函数地址反编译；
   需先确认 ABI（`strings` 看 .so 内错误串 + 试错调用）；
3. **Docker**（本机 Docker Desktop 已装，daemon 未启动）：`docker run ubuntu` + g++/bison/flex，
   直接编 ghidra-native 的 `decomp_opt` 命令行工具（Linux x64 构建，Makefile 原生支持），
   对 libUE4.so 文件直接反编译——**这是不需要路线 A 的纯 B 路线验证**，反而更干净。

> 建议：Phase 0 首选方案 3（Docker 编官方 decomp 命令行工具验证质量），
> 路线 A 的 .so 验证降级为备选——因为方案 3 顺带把 B 路线的构建链也验了，
> 且不存在 ABI 黑盒问题。
