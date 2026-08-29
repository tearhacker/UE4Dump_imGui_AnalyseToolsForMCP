# UMT MCP 工具开发路线图（企业级）

> 本文档是 MCP 工具**从 0 到上线**的工程路线图，遵循企业级软件交付规范。
> 规划依据（已通读）：《开发必读架构 v1.2》《MCP工具功能规格》《设备端API接口清单》《api/00-索引》。
>
> 状态：**待评审**。评审通过后按 Phase 顺序执行，每阶段有明确退出条件（DoD），不满足不进入下一阶段。

---

## 0. 现状盘点（规划输入）

| 项 | 现状 |
|---|---|
| PC 侧 `mcp_server/` | ✅ 空白骨架已起（FastMCP，`mcp` 1.29.1 pin v1，venv 就绪，`ping` 可跑） |
| 设备端 `src/mcp/` | ❌ **不存在**，socket 命令服务 0 实现（`AF_INET`/`bind`/`listen` 源码零命中） |
| 设备端能力盘点 | ✅ 已有 60%+（L0/L1/L2/L3/L4/L5 大量现成）；🔧 需外迁（匿名 namespace，~200-300 行）；🔧 需接线（`findSymbol`/`KittyTraceMgr` 存在但零调用）；⚠️ 需新建（命令服务 ~700 行、参数化扫描、批量采样、SDKQuery ~400 行） |
| 42 工具规格 | ✅ 已定稿（A–I 九组，input/output/实现基础/约束齐全） |
| 协议/架构 | ✅ 已定稿（v1.2：stdio + 设备端 127.0.0.1:27185、HELLO+token、命令分级、短等优先、outputSchema、annotations） |

**核心结论**：规格与架构已足够成熟，工程风险集中在**两端对接**——设备端命令服务是从未存在的全新代码，PC 侧是与阻塞 I/O 交互的 asyncio 服务。必须用契约 + 骨架 + 切片的方式把风险前移。

---

## 1. 企业级流程的五条原则

1. **契约先行（Contract First）**：设备端是 C++、PC 侧是 Python，是两个独立二进制。先用一份 wire protocol 契约把它们解耦，两边并行开发、互不等待。
2. **Walking Skeleton**：先跑通一条最薄的端到端链路（ping 打通真机），把架构 v1.2 里 13 项风险（bind、token、断线、阻塞 I/O）在投入大批量代码前暴露。
3. **垂直切片优先于水平铺开**：先完成一条完整业务用例（定位引擎全局），再横向补齐 42 工具。避免"所有层都写一半、没有一个能跑"。
4. **测试左移（Shift-Left）**：契约测试、故障注入从 Phase 1 就建，不是上线前才补。
5. **每步可独立验证、可回滚**：每个 Phase 结束有 DoD 验收，产物单独可测。

---

## 2. 五阶段路线图

### Phase 0 — 契约冻结
**目标**：设备端 wire protocol 与 PC 侧 schema 定稿，成为唯一的对齐事实。

| 交付物 | 说明 |
|---|---|
| `docs/mcp-protocol.md` | 设备端 socket 协议：帧格式（换行分隔 JSON）、HELLO 握手、请求/响应格式、错误码表、命令分级（快/重活）、版本协商、断线语义 |
| `mcp_server/schemas.py` | 42 工具的 Pydantic input/output 模型（先声明，逻辑留空） |
| `tests/contract/*.json` | 契约样例：每个命令的请求/响应黄金样例 |

**DoD**：协议文档评审通过；schema 与《工具功能规格》逐字段一致；契约样例可被两端测试引用。

---

### Phase 1 — Walking Skeleton（端到端最小闭环）
**目标**：PC 侧 `ping` 能打通真机设备端，返回 build + protocol。

| 交付物 | 说明 |
|---|---|
| `src/mcp/CommandServer.{hpp,cpp}` | socket `bind(127.0.0.1, 27185)` + HELLO + token 校验 + 换行 JSON 收发 |
| `src/mcp/CommandDispatcher.{hpp,cpp}` | 命令分级：快命令直执行 / 重活投队列 |
| 设备端 3 个 demo 命令 | `PING` / `LIST_PROCESSES`（外迁 `FindAutoProcessCandidates`）/ `GET_LOGS`（接环形缓冲） |
| PC 侧 `bridge.py` + 接线 | 阻塞 socket 走 `asyncio.to_thread`；`ping`/`getCapabilities` 对接真设备 |
| `adb_forward` 辅助 | `adb forward tcp:27185 tcp:27185` |

**DoD**：PC `ping` 返回真实 `{deviceBuild, protocolVersion}`；断线重连（指数退避）可演示；单测覆盖协议收发与 hex 编解码。

---

### Phase 2 — MVP 垂直切片（一条完整用例）
**目标**：`listProcesses → selectProcess → detectUEVersion → resolveSymbol → scanGNames → sampleGNames` 端到端跑通，AI 能定位并**自证** GNames 候选。

**DoD**：在真机（元梦之星 ground truth）上走通定位流程；`sampleGNames` 返回前 16 个 FName 含引擎内建名；证据内嵌（每候选带前 3 样本 + 评分）；单响应 ≤4K token。

---

### Phase 3 — 全量工具分批实现
按「依赖 + 性价比」拆 4 批（详见 §3）。每批 DoD：所有工具含 outputSchema + annotations + 中文触发词 + 耗时档位标注；契约测试通过。

### Phase 4 — 质量加固（Quality Gates）
| 门禁 | 验收 |
|---|---|
| 契约测试 | 每个工具返回严格符合 outputSchema（自动化） |
| 故障注入 | attach 看门狗触发、写不可写地址必拒绝、ESRCH 正常返回、断线重连 |
| token 预算 | 单响应 ≤4K、常驻 ≤25 工具（F/E 组 listChanged 动态挂载） |
| 安全 | bind 127.0.0.1、confirmDangerous、token 校验、`getCapabilities` 报泄漏会话 |

### Phase 5 — 集成验证与发布
**DoD**：真机完整流程回归通过；三份文档同步；PR 写明每工具增加的常驻 token；提交推送。

---

## 3. Phase 3 工具实现批次

| 批 | 范围 | 关键动作 | 依赖 |
|---|---|---|---|
| 3.1 原语层 | D 组 7 工具 + L0/L1/L3 封装 | 直接包装现有 `KittyMemoryMgr`/`UEMemory`/`UEWrappers`（✅ 已有） | Phase 1 |
| 3.2 外迁 | C 组 11 工具 + L2 参数化 | 移出匿名 namespace；`GetNamesPtr`/`GetGUObjectArrayPtr` 参数化 + 修单向扫描 bug；`findSymbol` 接线（符号优先） | Phase 2 |
| 3.3 接线 | F 组 5 工具 | `KittyTraceMgr` 接线 + `callRemoteFunctionBatch` 原子化 + attach 四重兜底 + 取消点 | Phase 1 |
| 3.4 新建 | G 组 SDKQuery + H 组编排 + I 组复合 | 另起无头 SDKQuery（复用 `UEWrappers`）；`locateEngineGlobals`/`dumpSDK`/`analyzeClass` | Phase 2/3.1 |

---

## 4. 依赖关系

```
Phase 0 契约
   └─► Phase 1 Walking Skeleton（设备端命令服务 + PC bridge）
           ├─► Phase 2 MVP 垂直切片（定位用例）
           │       ├─► Phase 3.1 原语 / 3.2 外迁
           │       └─► Phase 3.4 高层编排
           └─► Phase 3.3 ptrace 接线
Phase 3（全批完成）─► Phase 4 质量加固 ─► Phase 5 发布
```

---

## 5. 风险与对策

| 风险 | 等级 | 对策（架构 v1.2 已定） |
|---|---|---|
| 命令服务 bind 0.0.0.0 = 局域网可写游戏内存 | 🔴 | 强制 bind 127.0.0.1 + HELLO token，Phase 1 就落实 |
| FastMCP asyncio 被阻塞 I/O 卡死 | 🔴 | 所有设备调用走 `asyncio.to_thread`，Phase 1 骨架就埋 |
| 断线重连（USB/adb/杀进程） | 🔴 | 指数退避 + 重连必重新 HELLO + sessionId 失效，Phase 1 实现 |
| 慢工具触发 AI 重试风暴 | 🟠 | 耗时档位 + 长轮询 + suggestedWaitMs，Phase 3 落地 |
| 单响应打爆上下文 | 🔴 | 单响应 ≤4K 硬约束，Phase 4 门禁 |
| 设备端 3000 行 C++ 编译/真机部署成本 | 🟠 | Walking Skeleton 先最小化验证，再批量 |

---

## 6. 里程碑（按依赖推进，不设工时）

M1 契约冻结 → M2 端到端 ping 打通 → M3 定位用例闭环 → M4 42 工具齐 → M5 质量门禁过 → M6 真机发布

**下一步建议**：从 **Phase 0（契约冻结）** 开始——它是 Phase 1 两端并行的前置，也是当前唯一没有成文的缺口。
