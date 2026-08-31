"""UMT MCP Server 配置常量（PC 侧）。

所有数值均来自《开发必读架构 v1.2》的硬性约束，改动需同步文档。
"""

from __future__ import annotations

# ---------------------------------------------------------------- 连接
# 设备端 CommandServer 默认 bind 127.0.0.1，经 adb forward 暴露给 PC。
HOST = "127.0.0.1"
DEFAULT_PORT = 35515

# 直连模式：True 时跳过 adb forward，直接连 HOST:DEFAULT_PORT。
# 两种场景需要它：
#   1) MCP server 与设备端同机（手机端 Operit 内运行本 server）—— 同机回环无需隧道
#   2) 设备端已 bind 到局域网 IP（改过 mcp_bind.conf）—— 本就能直连
# 命令行用 --no-adb 开启。
DISABLE_ADB = False

# adb forward 只在连接目标是本机回环时才有意义（隧道两端都是 127.0.0.1）
LOOPBACK_HOSTS = frozenset({"127.0.0.1", "localhost", "::1"})

# ---------------------------------------------------------------- 重连（v1.2 issue #5）
# USB 拔插 / adb 掉线 / UMT 被杀是最高频故障
RECONNECT_BASE_DELAY = 1.0      # 秒
RECONNECT_MAX_DELAY = 30.0      # 秒，指数退避上限
RECONNECT_MAX_ATTEMPTS = 0      # 0 = 无限重试

# ---------------------------------------------------------------- 超时（v1.2 §4.2）
DEFAULT_WAIT_MS = 5_000         # 短等优先：waitMs 内完成则直接返回结果
MAX_WAIT_MS = 60_000

SOCKET_TIMEOUT = 60.0           # 单次命令 socket 读超时（秒）

# ---------------------------------------------------------------- 帧与心跳（协议 §8.1）
# 与设备端 src/mcp/Protocol.hpp 的数值保持一致，改任一端必须同步另一端
MAX_FRAME_SIZE = 1024 * 1024    # 单帧上限 1MB（设备端 kMaxFrameSize）
RECV_CHUNK_SIZE = 8192          # 单次 recv 字节数（设备端 kRecvChunkSize）
HEARTBEAT_INTERVAL = 2.0        # 设备端心跳间隔（秒）
HEARTBEAT_TIMEOUT = 10.0        # PC 侧判活阈值：超过即视为进程假死（约 5 个周期）
CMD_TIMEOUT = 120.0             # 设备端单命令硬超时，PC 侧等待上限取此值
STALE_CHECK_INTERVAL = 0.5      # 等待响应时检查心跳是否过期的时间片

# ---------------------------------------------------------------- 响应体积（v1.2 issue #1）
# 硬约束：单工具响应 <= 4K token，超出一律分页返回 nextCursor
MAX_RESPONSE_TOKENS = 4_096

READ_MEMORY_MAX_SIZE = 4_096    # readMemory size 上限（原 65536 会打出 33K token）
GET_LOGS_DEFAULT_LINES = 50     # getLogs 默认行数（原 200 ≈ 6K token）
SCAN_DEFAULT_CANDIDATES = 50    # scanCandidates 默认候选数
SCAN_PAGE_RESULTS = 20          # 扫描单页上限，保证上下文/证据不撑爆 4K token
SCAN_DEFAULT_BUDGET = 64 * 1024 * 1024
CANDIDATE_SESSION_LIMIT = 32

# ---------------------------------------------------------------- 工具预算（v1.2 issue #6）
MAX_TOOLS = 47                  # 硬上限
RESIDENT_TOOLS = 25             # 常驻上限，其余靠 tools.listChanged 动态挂载

# ---------------------------------------------------------------- 协议
PROTOCOL_VERSION = 1            # 设备端 HELLO 返回的 protocol，不匹配则明确报错
SERVER_NAME = "unreal-memory-tools"
SERVER_VERSION = "0.1.0"

# ---------------------------------------------------------------- 安全
# 危险操作默认关闭，需显式 confirmDangerous=true
WRITE_MEMORY_REQUIRES_CONFIRM = True
CALL_FUNCTION_REQUIRES_CONFIRM = True

# ---------------------------------------------------------------- 输出目录（设备端）
DEVICE_OUTPUT_ROOT = "/sdcard/UnrealMemoryTools"
ANALYSIS_JSON_NAME = "mcp_analysis.json"   # v1.2 issue #10 结论落盘
