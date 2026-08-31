# PC 侧 mcp_server 开发步骤

> 定位：路线图 **Phase 0（契约冻结）+ Phase 1（Walking Skeleton）** 的 PC 侧落地计划。
> 上游依据：《开发必读架构 v1.2》（工具/协议/技术栈）、《MCP工具功能规格》（43 工具，含 ATTACH/DISASSEMBLE）、`docs/api/*`（设备端能力）。
> 状态：**步骤已排定，Step 1 已完成并验收**。
>
> ## 🔴 先读这段：本步骤不是起点
>
> **设备端（imgui 项目）才是服务端，PC 侧是客户端。** 没有设备端的 `CommandServer`
> （`bind + listen`），PC 侧 `connect` 必然 `ECONNREFUSED` —— **链都建不起来**。
>
> 链路：`AI 客户端 ──stdio──> PC mcp_server（客户端）──adb forward tcp:35515──> 设备端 CommandServer（服务端）──> UMT 能力`
>
> 因此**真实起点是两端共用的通信协议**（见 §2 Step 2），本步骤里的 PC 侧工作
> 在设备端服务端落地之前，只能在 mock 设备端下推进。设备端侧的设计见 §4。

---

## 0. 目标与现状

| 项 | 状态 |
|---|---|
| 骨架 | ✅ 已有（`pyproject.toml` / `server.py` / `README.md` / `.gitignore`，`mcp` 1.29.1 已装） |
| 设备端 `src/mcp/` | ❌ 不存在（**硬前提**，见 §4） |
| 通信协议 | ✅ **`docs/mcp-protocol.md` 已定稿**（帧格式/HELLO/命令表/错误码/分级） |
| 目标 | 43 工具 + 4 资源 + 设备桥接 + 契约测试 + 反汇编封装 |

---

## 1. 目标目录结构（企业级包结构）

架构 §5.1 给的平铺结构（`mcp_server.py` / `tools.py`）有个已知隐患：源项目就是这么踩坑的
（`test_mcp_server.py:8` 的 `from mcp.mcp_server import ...` 路径对不上，跑不起来）。**改为包结构**：

```
mcp_server/
├── pyproject.toml              ✅ 已有（需补 capstone 依赖）
├── server.py                   ✅ 已有（改造为入口，只做 import + mcp.run）
├── README.md                   ✅ / .gitignore ✅
├── requirements.txt            ⬜ 锁版本：mcp / capstone / pytest
├── src/umt_mcp/                ⬜ 包（避免 import 路径坑）
│   ├── __init__.py
│   ├── config.py               端口 35515、超时、waitMs 默认值
│   ├── protocol.py             设备端 wire protocol（帧/HELLO/错误码）
│   ├── bridge.py               socket 客户端 + 重连 + RLock 串行
│   ├── adb.py                  adb forward 管理
│   ├── instructions.py         Server Instructions 全文（架构 §3）
│   ├── disassemble.py          capstone 封装（可缺失降级）
│   ├── resources.py            4 个资源（架构 §2.9）
│   └── tools/                  43 工具按 A–I 分组
│       ├── a_connection.py     A 组 2
│       ├── b_process.py        B 组 2
│       ├── c_flow.py           C 组 11
│       ├── d_primitives.py     D 组 7
│       ├── e_understand.py     E 组 2
│       ├── f_ptrace.py         F 组 5
│       ├── g_engine.py         G 组 8
│       ├── h_highlevel.py      H 组 3
│       └── i_compound.py       I 组 2
└── tests/
    ├── conftest.py             mock 设备端（**不依赖真机即可开发**）
    ├── test_protocol.py        帧编解码 / hex 编解码
    ├── test_bridge.py          重连 / 串行 / 超时
    ├── test_contract.py        outputSchema 契约校验（43 工具全覆盖）
    └── test_fault_injection.py 故障注入
```

---

## 2. 开发步骤（Step 1–9）

> 每步独立可验证。Step 1–3 不需要真机（用 mock 设备端）。

### Step 1　工程结构定稿 + 依赖锁定
- **产出**：`requirements.txt`（锁版本）、`src/umt_mcp/` 包骨架、`config.py`
- **依赖**：无
- **验收**：`python -c "import umt_mcp"` 通过；`pip install -r requirements.txt` 可复现

### Step 2　协议契约层 `protocol.py`（**Phase 0 核心**）
- **产出**：✅ 协议文档 **`docs/mcp-protocol.md` 已定稿**（NDJSON 帧、HELLO、
  请求/响应、长任务与长轮询、错误码表、43 命令表、命令分级、契约样例）
- **待做**：`src/umt_mcp/protocol.py` —— 按协议实现编解码（帧读写、HELLO 校验、错误码枚举、命令表常量）
- **验收**：`tests/contract/` 黄金样例可被 PC 侧与设备端测试共同引用；`test_protocol.py` 全过

### Step 3　设备桥接层 `bridge.py`
- **产出**：socket 客户端（连 `127.0.0.1:35515`）+ HELLO 协议校验 + **指数退避重连（1/2/4…≤30s）** + `RLock` 串行化 + `asyncio.to_thread` 包装阻塞 I/O
- **🔴 心跳判活（防卡死必需）**：后台收心跳帧（协议 §3.8，设备端每 2s 一帧）；
  **超过 10s 无心跳判定设备端假死** → 断开重连。这让 PC 侧能区分「**在算**」（心跳正常 busy=true）与「**死了**」（无心跳）
- **软超时**：单命令等待超过 `SOCKET_TIMEOUT`（60s）也要能中断等待并探活，不能无限阻塞
- **验收**：mock 设备端下，断线→重连→重新 HELLO 全通过；in-flight 调用返回 `isError: true` + "连接已断开，已自动重连，请重试"；
  **心跳停止 10s 后能自动判定假死并重连**

### Step 4　ADB 辅助 `adb.py`
- **产出**：`adb forward tcp:35515 tcp:35515` 的封装（前置检查、幂等、错误提示）
- **验收**：无设备时给出明确错误，不静默失败

### Step 5　Server 入口 + Instructions + 资源
- **产出**：`server.py` 改造（FastMCP + capabilities 声明 + instructions 全文）、`resources.py`（4 个资源，**大文件不做资源**）
- **验收**：`initialize` 返回 `tools.listChanged` / `resources` / `logging` 三项能力

### Step 6　工具层（分批，A→I）
- **产出**：43 工具的 Pydantic input/output 模型 + 描述（`<use_case>` + `<important_notes>` + 中文触发词 + 耗时档位）+ `annotations`
- **分批**：D 组原语（✅ 设备端已有，最划算）→ A/B/C → G → E/I → F（ptrace，最后）
- **🔴 H 组编排在 PC 侧实现**（架构修正，见设备端设计 §10）：
  `locateEngineGlobals` / `dumpSDK` / `analyzeClass` 的**多步编排**放在 `tools/h_highlevel.py`，
  设备端只提供原子命令。理由：设备端改一次要重编译 NDK + 推包 + 重启游戏，
  而编排逻辑在 AI 场景下会频繁调整，**迭代速度差两个数量级**
- **🔴 省 token**：列表类工具支持 `brief:true` 摘要模式（协议 §8.2），
  AI 先看全貌再用 `cursor` 分页取细节，避免"一次吃全"
- **验收**：每工具 outputSchema 与实现严格一致（契约测试）

### Step 6.5　数据通道（`adb pull`）
- **产出**：大文件（SDK 产物几十 MB）**不走 socket**，用 `adb pull` 从
  `/sdcard/UnrealMemoryTools/<pkg>/` 直拉到 PC 本地缓存
- **理由**：NDJSON 传大文件会 hex 膨胀 2 倍 + JSON 解析开销 + 撑爆响应上限
- **验收**：`listOutputFiles` / `searchClasses` / `describeClass` 走服务端过滤，
  **绝不整文件读进上下文**

### Step 7　反汇编封装 `disassemble.py`
- **产出**：capstone 封装（arm64）
- **验收**：capstone 缺失时返回明确提示，**不静默失败**

### Step 8　测试体系
- **产出**：`tests/` 全套（单元 / 契约 / 故障注入）
- **验收**：不连真机跑通；故障注入覆盖 attach 看门狗、写内存校验、断线重连、ESRCH

### Step 9　真机联调（Phase 2）
- **依赖**：**设备端 `src/mcp/CommandServer` 必须先落地**
- **验收**：`ping` 返回真实 `deviceBuild` + `protocolVersion`；定位切片端到端跑通

---

## 3. 架构 v1.2 红线（写代码时必须守）

| # | 红线 | 落地位置 |
|---|---|---|
| 1 | **单工具响应 ≤ 4K token** | `readMemory` size≤4096、`getLogs` 默认 50、`maxCandidates` 默认 50；超限分页返回 `nextCursor` |
| 2 | **常驻 ≤ 25 工具**（上限 45） | 核心 20 常驻；F/E 组靠 `tools.listChanged` 动态挂载 |
| 3 | **阻塞 I/O 必须 `asyncio.to_thread`** | 所有 bridge 调用，否则卡死事件循环（progress/cancel 全停） |
| 4 | **日志一律 stderr** | stdout 是 MCP 协议通道，禁止写 |
| 5 | **错误分层** | 协议错→JSON-RPC error；执行失败→`isError: true` |
| 6 | **PC 连 `127.0.0.1:35515`** | 绝不连设备的局域网 IP（设备端 bind 127.0.0.1） |
| 7 | **缓存命中可见** | 返回带 `cached: true` + 原始参数回显 |
| 8 | **禁止 "not found"** | 搜索类工具返回次优候选 + 中性描述 |
| 9 | **危险操作默认关闭** | `writeMemory`/`callRemoteFunction` 需 `confirmDangerous: true` |
| 10 | **每个可选参数有默认值** | 数值标 min/max，数组标 maxItems，`additionalProperties: false` |

---

## 4. 关键依赖：设备端必须先动

**PC 侧 Step 1–8 可以用 mock 设备端独立推进，Step 9 真机联调必须等设备端。**

### 角色：设备端是服务端，PC 侧是客户端

| 端 | 角色 | socket 动作 |
|---|---|---|
| 设备端 UMT | **服务端** | `bind("127.0.0.1",35515)` → `listen` → `accept` |
| PC mcp_server | **客户端** | `connect("127.0.0.1:35515)` |

**源码现状**：`src/mcp/CommandServer.cpp` 已完整实现（`bind/listen/accept`），PC 侧 `connect` 可打通。
`main()`（`:1761`）初始化完 Vulkan 后进 `while(flag)` 渲染循环，同时 `CommandServer` 在独立线程运行。

### 设备端最小闭环（架构 §5.6.3，协议见 `docs/mcp-protocol.md`）

```
src/mcp/CommandServer.{hpp,cpp}     socket 监听 + HELLO + NDJSON 收发           ~400 行
src/mcp/CommandQueue.{hpp,cpp}      mutex + condition_variable + 队列           ~250 行
src/mcp/CommandDispatcher.{hpp,cpp} 命令名 → 实现映射                            ~100 行
3 个 [MVP] 命令                      PING / LIST_PROCESSES / GET_LOGS            ~100 行
executable.cpp 接线                  起线程 + 每帧 poll                          ~10 行
```

**两个接入点**（精读 `main()` 确定）：
1. **起线程**：`main()` 中 `RefreshCandidates()` 之后起 `CommandServer` 线程
2. **每帧 poll**：`Layout_tick_UI(&flag)`（`:1811`）里 poll 命令队列

**三条红线**
1. 🔴 **bind `127.0.0.1`，绝不 `0.0.0.0`** —— root + 无认证 + 内存写接口，局域网暴露 = 任意设备可读写游戏内存
2. 🔴 **命令分级**（协议 §7）—— 快命令直执行，重活投 `gWorkerThread`。否则分钟级扫描期间
   `PING`/`GET_LOGS` 全排队，AI 误判掉线触发重试风暴
3. **HELLO 后直接调用** —— 客户端只需校验协议版本，无需额外认证帧

### 推进策略（串行，单人开发并行无收益）

```
① 协议定稿        ✅ 已完成（docs/mcp-protocol.md）
② 设备端服务端    ← 下一个硬前提，打通 PING
③ PC 侧对接 + 真机联调
④ 再铺开 43 工具
```

② 一完成 `ping` 就能拿到真实 `deviceBuild` + `protocolVersion`，这是最有价值的反馈点——
证明"AI 能驱动真机"走得通，再投入剩下 3000 行才不冒险。

---

## 5. 里程碑

| 里程碑 | 标志 |
|---|---|
| M1 工程就绪 | Step 1 完成，`import umt_mcp` 通过 |
| M2 契约冻结 | Step 2 完成，`docs/mcp-protocol.md` 定稿 |
| M3 桥接可用 | Step 3–4 完成，mock 下重连/串行通过 |
| M4 服务可跑 | Step 5 完成，MCP 客户端能看到工具与资源 |
| M5 工具齐 | Step 6–7 完成，43 工具 + 契约测试全过 |
| M6 真机通 | Step 9 完成，ping 返回真实设备信息 |
