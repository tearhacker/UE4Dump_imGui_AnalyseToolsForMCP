# MCP 网络配置三模式分析与最小改造方案

> ✅ **已实施**（2026-09-01）。改动 A、改动 B 均已落到代码，Python 侧 64 个测试全绿，
> 设备端 `CommandServer.cpp` 增量编译通过。改动 C（多连接 / 设备端 MCP over HTTP）按计划未做。
>
> 分析对象
> - `mcp_server/客户端AI工具配置MCP文件.json`
> - `mcp_server/src/umt_mcp/bridge.py`
> - `Alltear_UnrealMemoryToolsNewMcp/src/mcp/`（`CommandServer.cpp` / `Protocol.hpp`）
>
> 结论日期：2026-09-01

---

## 0. 结论速览

| 问题 | 结论 |
|---|---|
| 手机端 Operit 是否需要 ADB？ | **不需要。** Operit 与 UMT 同机、同 network namespace，`127.0.0.1:35515` 直连可达。ADB 只是「PC → 手机」的跨机隧道，同机场景是给自己转自己。 |
| Operit 现在能直接用吗？ | **不能。** 网络层是通的，卡在两处：① `_auto_forward` 硬编码为 `True`，adb 一失败就抛异常；② `CommandServer` 说的是私有 NDJSON 协议，不是 MCP。 |
| 三种模式是同一件事吗？ | **不是。** 模式 1/2 是**设备端 bind 地址**问题（要改 C++）；模式 3 是**协议/传输**问题（主要改 Python）。把它们当同一层来改会走弯路。 |
| 最小改动量 | 解锁模式 3：**约 12 行 Python**。解锁模式 1/2：再加 **约 25 行 C++**。 |

---

## 1. 关键澄清：三个模式不在同一层

用户表述的「三种监听模式」实际上横跨两个不同层次，混在一起会得出错误结论：

```
模式 1  (192.168.x.x:port) ─┐
                            ├─► 【设备端 bind 地址层】CommandServer.cpp:214 的 kBindAddress
模式 2  (0.0.0.0:port)     ─┘     → 改 C++，涉及安全红线

模式 3  (Operit 本机)       ──►  【协议/传输层】Operit 是 MCP 客户端，
                                 CommandServer 说私有 NDJSON，两者不通
                                 → 主要改 Python（或给设备端加 MCP over HTTP）
```

**模式 3 的 bind 地址根本不用动。** Operit 和 UMT 在同一台手机上，`127.0.0.1` 已经是正确答案。
这也是为什么「给 Operit 也配 adb」是南辕北辙 —— 问题从来不在网络。

---

## 2. Operit 到底要不要 ADB：**不要**

### 2.1 网络层面：已经通了

- Operit 是 Android 应用，UMT 注入在游戏进程里，**两者在同一台设备**。
- Android 所有普通应用共享**同一个内核 network namespace**（不像容器那样每 app 一个 netns），回环地址 `127.0.0.1` 是全系统共享的。
- 所以 `Operit → connect(127.0.0.1, 35515)` 是纯本机回环，TCP 握手直接完成。
- `adb forward` 的本质是把 **PC 的** `127.0.0.1:35515` 映射到 **手机的** `127.0.0.1:35515`。同机场景下这是在给自己转自己，纯多余。
- 反而有害：Operit 的 Ubuntu 24 环境（proot/chroot）里既没有可用的 adb server，也没有 USB host 权限，`adb.setup()` 必然失败。

### 2.2 真正的阻塞点

**阻塞点 A（一行代码级）——`_auto_forward` 无法关闭**

`bridge.py:51-55`：

```python
self._auto_forward = (
    host == config.HOST and port is None
    if auto_forward is None
    else auto_forward
)
```

`get_bridge()`（`bridge.py:379`）永远调用 `UmtBridge()`，不传参 → `host == config.HOST` 为 `True`、`port is None` 为 `True` → `_auto_forward` **恒为 True**。
CLI 里也没有任何开关能关掉它。于是 `_connect_once`（`bridge.py:150-156`）必然走 `adb.setup()`，adb 在手机上不可用 → 直接 `raise UmtConnectionError`。

即：**现有代码在手机上跑，第一次工具调用必挂。** 这是模式 3 唯一的硬阻塞，也是最小改动的落点。

**阻塞点 B（架构级）——协议不通**

| | 协议 | 传输 |
|---|---|---|
| `CommandServer`（设备端） | 私有 NDJSON：`{"id":N,"cmd":"...","args":{}}` | 裸 TCP |
| MCP 标准 | JSON-RPC 2.0：`initialize` / `tools/list` / `tools/call` | stdio / HTTP+SSE / Streamable HTTP |
| `server.py`（PC 侧） | FastMCP，**stdio** | stdio |

MCP 层只存在于 PC 侧 `server.py`，且是 stdio 传输。Operit 无法直接消费 NDJSON。

---

## 3. 三种模式对比

| | 模式 1：局域网 IP | 模式 2：0.0.0.0 | 模式 3：Operit 同机 |
|---|---|---|---|
| bind 地址 | `192.168.x.x` | `0.0.0.0` | `127.0.0.1`（**现状即是**） |
| 典型场景 | PC 与手机同 WiFi，不想插 USB | 不确定走哪张网卡时的兜底 | AI 客户端就跑在手机上，脱机独立工作 |
| 是否需要 ADB | 否（应改用 `--no-adb`） | 否 | **否** |
| 跨机 | 是 | 是 | 否 |
| 暴露面 | 同 WiFi 子网 | 所有网卡（含蜂窝网） | 仅本机进程 |
| 风险 | **高**：root + 无认证 + 内存读写 | **最高** | 低（同其他 app 平级，无额外暴露） |
| 现状 | ❌ 未实现 | ❌ 未实现 | ⚠️ 网络通，协议不通 |
| 改动量 | C++ ~25 行 + Python ~10 行 | 同模式 1（bind 值不同） | **Python ~12 行**（方案 A） |

### 关于 0.0.0.0「等效 localhost」

这个说法方向对但不完整：`0.0.0.0` 表示"监听所有网卡"，**它包含** `127.0.0.1`，所以回环确实能连上；但它同时把服务暴露给 WiFi 网卡、**蜂窝网卡（rmnet）**以及热点虚拟网卡。在 root + 无认证 + 提供内存读写的上下文里，这不是"localhost 的等价写法"，而是**把当前的安全红线整条拆掉**。

建议定位：
- `127.0.0.1` —— 默认，唯一无风险选项
- `192.168.x.x` —— 受控场景（家庭/实验室可信 WiFi）下按需开启
- `0.0.0.0` —— **不推荐**，仅在调试且清楚后果时临时使用

---

## 4. 现状差距清单（精确到行）

### PC 侧 Python

| # | 位置 | 问题 | 影响 |
|---|---|---|---|
| G1 | `bridge.py:44` `host: str = config.HOST` | **Python 默认参数在导入期求值**。运行时改 `config.HOST` 完全无效，host 被永久冻结为 `127.0.0.1` | 即使加了 `--host` 也不生效（**潜伏 bug**） |
| G2 | `bridge.py:51-55` | `_auto_forward` 恒为 `True`，无 CLI 开关 | 手机上跑必挂（模式 3 硬阻塞） |
| G3 | `server.py:103-108` | 只有 `--adb` / `--port`，**无 `--host`、无 `--no-adb`** | 模式 1/2/3 都无从配置 |
| G4 | `config.py:15-16` | `ADB_FORWARD_SPEC` / `ADB_FORWARD_ARGS` 是**死代码**，全仓库无引用；`adb.py:18` 又重复定义了 `DEFAULT_PORT = 35515` | 误导读代码的人，删掉更清爽 |
| G5 | `bridge.py:143-146` | 报错文案硬编码"PC 侧已自动检查并建立 adb forward"，直连模式下是误导 | 排障时被带偏 |
| G6 | `server.py:112` `config.DEFAULT_PORT = args.port` | 靠改写模块常量传参，脆弱 | 可维护性问题 |
| G7 | `客户端AI工具配置MCP文件.json` | 硬编码 `C:/Program Files/Python310/python.exe`、`C:/Program Files/platform-tools/adb.exe`（本机实测两个路径都存在，但换机即失效）；未提供 Operit / 直连变体 | 换机要手改；Operit 场景无样例 |

### 设备端 C++

| # | 位置 | 问题 | 影响 |
|---|---|---|---|
| G8 | `Protocol.hpp:16` `kBindAddress = "127.0.0.1"` | 编译期常量，**无运行期配置入口** | 模式 1/2 完全无法实现 |
| G9 | `CommandServer.cpp:214` | 直接用 `kBindAddress`，无注入点 | 同上 |
| G10 | `CommandServer.cpp:229` `listen(serverFd, 1)` + 单线程 `accept→serve→close` | **同时只能服务一个客户端**。Operit 与 PC 同时连时，后到者卡在 backlog 里拿不到 HELLO，一直挂到超时 | 模式 3 与 PC 并存会 hang |
| G11 | `CommandServer` 说 NDJSON，非 MCP | 无 MCP 端点 | Operit 无法直接消费 |
| G12 | `docs/`（35 篇）与 `README.md` | **全文零处提及 Operit** | 团队无从知晓该场景 |

---

## 5. 最小改动方案

### 改动 A：解锁模式 3（Operit 同机）—— 约 12 行，零 C++ 改动

思路：**在 Operit 的 Ubuntu 24 环境里跑现有的 Python `server.py`，加一个 `--no-adb` 开关跳过 adb。**
这样 tools.py 的编排逻辑、4K token 预算、分页缓存全部复用，不需要在 C++ 里重写一遍 MCP。

#### A-1. `mcp_server/src/umt_mcp/config.py`

```python
HOST = "127.0.0.1"
DEFAULT_PORT = 35515

# 直连模式：True 时跳过 adb forward，直接连 host:port。
# 用于 MCP server 与设备端同机的场景（如手机端 Operit 内运行本 server）。
DISABLE_ADB = False

# adb forward 仅对回环地址有意义
LOOPBACK_HOSTS = frozenset({"127.0.0.1", "localhost", "::1"})
```

（顺手可删 `config.py:15-16` 的 `ADB_FORWARD_SPEC` / `ADB_FORWARD_ARGS` 死代码。）

#### A-2. `mcp_server/src/umt_mcp/bridge.py`

**①　修掉默认参数冻结 bug（G1）+ 尊重 `DISABLE_ADB`（G2）**

```python
    def __init__(
        self,
        host: str | None = None,
        port: int | None = None,
        *,
        auto_forward: bool | None = None,
    ) -> None:
        self._host = config.HOST if host is None else host
        self._port = config.DEFAULT_PORT if port is None else port
        self._auto_forward = (
            (not config.DISABLE_ADB
             and self._host in config.LOOPBACK_HOSTS
             and port is None)
            if auto_forward is None
            else auto_forward
        )
```

> 注意：`host: str | None = None` 这一改动是**必须**的，不是风格问题。
> 原写法 `host: str = config.HOST` 会在 import 时把默认值固化，后面 `config.HOST = args.host` 改不动它。

**②　报错文案随模式切换（G5）**

```python
        if config.DISABLE_ADB:
            hint = (
                f"直连模式（--no-adb）：请确认设备端 UMT 已启动，"
                f"且 {self._host}:{self._port} 可达。"
            )
        else:
            hint = (
                "PC 侧已自动检查并建立 adb forward，无需手工输入命令。\n"
                "请确认：1) 手机已连接并授权 USB 调试  2) UMT 已启动并在运行"
            )
        raise proto.UmtConnectionError(
            f"无法连接设备端 {self._host}:{self._port}（已试 {attempt} 次）。\n"
            f"{hint}\n"
            f"底层错误：{last_exc}"
        )
```

#### A-3. `mcp_server/server.py`

```python
    parser.add_argument("--host", default=config.HOST,
                        help=f"设备端地址（默认 {config.HOST}）")
    parser.add_argument("--no-adb", action="store_true",
                        help="直连模式：跳过 adb forward，直接连 --host:--port。"
                             "用于与设备端同机（如手机端 Operit）或局域网直连。")
    ...
    args = parser.parse_args()

    config.HOST = args.host
    config.DISABLE_ADB = args.no_adb          # 🔴 必须在 mcp.run() 之前

    if args.adb:
        adb.set_adb_bin(args.adb)
    config.DEFAULT_PORT = args.port

    if not args.no_adb:
        ok, msg = adb.setup(args.port)
        if not ok:
            logger.warning("ADB 自动连接预热暂未完成，首次工具调用会继续自动重试: %s", msg)
        else:
            logger.info("ADB 无感连接已就绪: %s", msg.replace("\n", "；"))
    else:
        logger.info("直连模式：目标 %s:%d", config.HOST, config.DEFAULT_PORT)
```

#### A-4. Operit 侧 MCP 配置样例

在 Operit 的 Ubuntu 24 环境里 `pip install mcp` 后：

```json
{
  "mcpServers": {
    "ue4dump-mcp": {
      "command": "python3",
      "args": [
        "/data/local/umt_mcp/server.py",
        "--no-adb",
        "--host", "127.0.0.1",
        "--port", "35515"
      ]
    }
  }
}
```

> **前置验证**：proot/chroot 默认共享宿主 network namespace，故 chroot 内 `127.0.0.1:35515` 即宿主的端口。
> 若 Operit 的环境用了独立 netns（少数 proot `-n` 配置），则会连不通 —— 先用 `python3 -c "import socket;socket.create_connection(('127.0.0.1',35515),3);print('ok')"` 探一下即可确认。

---

### 改动 B：解锁模式 1 / 2（设备端 bind 可配置）—— 约 25 行 C++

**设计原则：默认值绝不放宽。** `kBindAddress` 默认仍是 `127.0.0.1`，非回环地址必须显式写配置文件才生效，避免"改一行常量就把安全红线拆了"。

#### B-1. `src/mcp/Protocol.hpp`

```cpp
constexpr const char *kBindAddress = "127.0.0.1";   // 默认值，安全红线不放松
constexpr const char *kBindConfigPath = "/sdcard/UnrealMemoryTools/mcp_bind.conf";
```

#### B-2. `src/mcp/CommandServer.hpp`

```cpp
    static void SetBindAddress(const std::string &addr);
    ...
    static std::string bindAddress_;
```

#### B-3. `src/mcp/CommandServer.cpp`

```cpp
std::string CommandServer::bindAddress_ = kBindAddress;

void CommandServer::SetBindAddress(const std::string &addr)
{
    bindAddress_ = addr.empty() ? kBindAddress : addr;
}
```

在 `Start()` 内、`thread_ = std::thread(ServerLoop);` 之前读配置：

```cpp
    // 可选：运行期覆盖监听地址。文件不存在/为空 → 保持 127.0.0.1。
    // 写入 0.0.0.0 或局域网 IP 即放开到非回环，须自行承担暴露风险。
    if (FILE *f = ::fopen(kBindConfigPath, "r"))
    {
        char buf[64] = {0};
        if (::fgets(buf, sizeof(buf), f))
        {
            std::string line(buf);
            const auto b = line.find_first_not_of(" \t\r\n");
            const auto e = line.find_last_not_of(" \t\r\n");
            if (b != std::string::npos && e != std::string::npos)
            {
                bindAddress_ = line.substr(b, e - b + 1);
                if (bindAddress_ != kBindAddress)
                    LOGW("[MCP] 警告：监听非回环地址 %s，服务将暴露到网络",
                         bindAddress_.c_str());
                else
                    LOGI("[MCP] 配置文件指定监听 %s", bindAddress_.c_str());
            }
        }
        ::fclose(f);
    }
```

`ServerLoop()` 里把 `kBindAddress` 换成 `bindAddress_`：

```cpp
    // 🔴 安全红线：默认只监听回环。0.0.0.0 / 局域网 IP 需显式配置文件才生效。
    if (::inet_pton(AF_INET, bindAddress_.c_str(), &addr.sin_addr) != 1)
    {
        LOGE("[MCP] 地址解析失败: %s", bindAddress_.c_str());
        ::close(serverFd);
        return;
    }
```

（`CommandServer.cpp:223` 的 bind 失败日志、`:237` 的启动日志同理换成 `bindAddress_.c_str()`。）

#### B-4. 使用方式

```bash
# 手机上写入配置（文件内容即 bind 地址，一行）
adb shell "echo 192.168.1.23 > /sdcard/UnrealMemoryTools/mcp_bind.conf"
# 重启游戏使 UMT 重新加载

# PC 侧用直连模式，不再走 adb forward
python server.py --no-adb --host 192.168.1.23 --port 35515
```

---

### 改动 C：可选 / 后续（不建议本轮做）

| # | 问题 | 建议 |
|---|---|---|
| G10 | 单连接，Operit 与 PC 并存会 hang | **短期**：文档写死"同时只允许一个客户端"，并在 `umt://protocol` 资源里注明。**长期**：拆出 accept 线程，新连接到来时回 `E_BUSY` 错误帧并 close（改动较大，本轮不做） |
| G11 | 设备端无 MCP 端点 | 方案 A（本方案，零 C++）已够用。若嫌 chroot 依赖重，再考虑设备端实现 MCP over Streamable HTTP（约 200+ 行 C++，需处理并发、会话、SSE，且违反项目"设备端只给事实"的分层原则） |
| G7 | mcp.json 硬编码路径 | 补一份直连/Operit 变体配置，并用相对路径或环境变量 |
| G12 | 文档零处提 Operit | 在 `README.md` 增补"三种连接模式"章节 |

---

## 6. 落地顺序建议

1. **先做改动 A**（约 12 行 Python，无需重编译 NDK、无需重启游戏）—— 立刻解锁 Operit 场景，风险最低。
2. **验证 A**：`python server.py --no-adb --host 127.0.0.1 --port 35515` 能连上并跑通 `PING`。
3. **再做改动 B**（需要重新编译 NDK + `adb push` + 重启游戏，成本高）—— 只有确实需要免 USB 线的局域网调试时才做。
4. **模式 2（`0.0.0.0`）不建议常态化启用**，仅作临时调试。

---

## 7. 验收清单

改动 A：

- [ ] `python server.py --no-adb --host 127.0.0.1` 启动日志出现"直连模式"，无 adb 相关报错
- [ ] 手机 chroot 内 `python3 -c "import socket;socket.create_connection(('127.0.0.1',35515),3)"` 成功
- [ ] 首次工具调用（`PING`）返回 `ok:true`，无 `UmtConnectionError`
- [ ] 连不上时的报错文案是"直连模式"分支，不再提 adb forward
- [ ] 原有 adb 模式（不带 `--no-adb`）行为完全不变

改动 B：

- [ ] 无配置文件时，行为与现在完全一致（bind `127.0.0.1`）
- [ ] 写入 `192.168.1.23` 后重启，`adb shell netstat` 可见 `192.168.1.23:35515` LISTEN
- [ ] PC 侧 `--no-adb --host 192.168.1.23` 能连上并收到 HELLO
- [ ] 日志出现"监听非回环地址"的安全警告
