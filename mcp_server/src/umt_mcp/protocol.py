"""设备端 wire protocol 的 PC 侧契约。

唯一对齐事实是 docs/mcp-protocol.md —— 两端任一改动必须先改文档再改代码。

要点（协议 §1/§2/§3）：
- TCP + NDJSON（每行一个 JSON 对象，以 \\n 结尾）
- 设备端是服务端，bind 127.0.0.1:35515，仅经 adb forward 暴露
- 连接后设备端先发 HELLO，PC 校验协议版本后即可直接发送命令
- 严格串行，一问一答；心跳帧会插在响应之前，必须能跳过

错误分层（协议 §5）：
- 协议层错误 → JSON-RPC error（说明"我调错了"）
- 执行层失败 → isError 的 tool result（说明"设备端失败了"）
两者绝不可混。
"""

from __future__ import annotations

import json
from typing import Any

from . import config

# ---------------------------------------------------------------- 帧
FRAME_DELIMITER = b"\n"
RECV_CHUNK = 8192
MAX_FRAME_SIZE = config.MAX_FRAME_SIZE  # 1MB，与设备端 Protocol.hpp 一致

# ---------------------------------------------------------------- 握手
MSG_HELLO = "hello"
MSG_HEARTBEAT = "heartbeat"
MSG_LOG = "log"

# ---------------------------------------------------------------- 错误码
# 协议 §5 全表。字符串常量与设备端 Protocol.hpp 的 Err:: 一一对应。
E_PROTOCOL_MISMATCH = "E_PROTOCOL_MISMATCH"
E_BAD_JSON = "E_BAD_JSON"
E_UNKNOWN_CMD = "E_UNKNOWN_CMD"
E_BAD_ARGS = "E_BAD_ARGS"
E_TIMEOUT = "E_TIMEOUT"
E_INTERNAL = "E_INTERNAL"
E_NOT_ATTACHED = "E_NOT_ATTACHED"
E_READ_FAILED = "E_READ_FAILED"
E_WRITE_FAILED = "E_WRITE_FAILED"
E_PROBE_FAILED = "E_PROBE_FAILED"
E_NOT_READY = "E_NOT_READY"
E_NOT_FOUND = "E_NOT_FOUND"
E_DECODE_FAILED = "E_DECODE_FAILED"
E_CANCELLED = "E_CANCELLED"
E_PTRACE_FAILED = "E_PTRACE_FAILED"
E_MAP_STALE = "E_MAP_STALE"
E_SESSION_STALE = "E_SESSION_STALE"

# 协议层错误：调用方的错，转 JSON-RPC error
PROTOCOL_ERRORS = frozenset(
    {
        E_PROTOCOL_MISMATCH,
        E_BAD_JSON,
        E_UNKNOWN_CMD,
        E_BAD_ARGS,
    }
)

# 执行层失败：设备端的错，转 isError tool result
EXECUTION_ERRORS = frozenset(
    {
        E_TIMEOUT,
        E_INTERNAL,
        E_NOT_ATTACHED,
        E_READ_FAILED,
        E_WRITE_FAILED,
        E_PROBE_FAILED,
        E_NOT_READY,
        E_NOT_FOUND,
        E_DECODE_FAILED,
        E_CANCELLED,
        E_PTRACE_FAILED,
        E_MAP_STALE,
        E_SESSION_STALE,
    }
)

# 协议版本、未知命令和参数错误不会因重试而改变。
NON_RETRYABLE = frozenset({E_PROTOCOL_MISMATCH, E_UNKNOWN_CMD, E_BAD_ARGS})


# ---------------------------------------------------------------- 异常
class UmtError(Exception):
    """所有 UMT PC 侧异常的基类。"""


class UmtConnectionError(UmtError):
    """连不上、断线、或设备端假死（心跳超时）。

    触发重连；重连后 sessionId/candidate/class index/job 全部作废。
    """


class UmtTimeoutError(UmtError):
    """命令超过 PC 侧读超时仍未返回。

    不同于 E_TIMEOUT（设备端侧超时）—— 这条是 PC 侧等不到任何字节。
    """


class UmtProtocolError(UmtError):
    """协议层错误："我调错了"。应转 JSON-RPC error。"""

    def __init__(self, code: str, msg: str = "", detail: Any = None):
        self.code = code
        self.msg = msg
        self.detail = detail
        super().__init__(f"[{code}] {msg}")


class UmtExecutionError(UmtError):
    """执行层失败："设备端失败了"。应转 isError tool result。"""

    def __init__(self, code: str, msg: str = "", detail: Any = None):
        self.code = code
        self.msg = msg
        self.detail = detail
        super().__init__(f"[{code}] {msg}")


def raise_for_error(code: str, msg: str = "", detail: Any = None) -> None:
    """按错误分层抛出对应异常（协议 §5）。"""
    if code in PROTOCOL_ERRORS:
        raise UmtProtocolError(code, msg, detail)
    raise UmtExecutionError(code, msg, detail)


# ---------------------------------------------------------------- 编解码
def encode_frame(obj: dict[str, Any]) -> bytes:
    """序列化一帧 NDJSON。"""
    return (json.dumps(obj, ensure_ascii=False, separators=(",", ":")) + "\n").encode("utf-8")


def decode_frame(raw: bytes | str) -> dict[str, Any]:
    """反序列化一帧 NDJSON。解析失败按协议层错误抛出。"""
    try:
        obj = json.loads(raw)
    except (ValueError, TypeError) as exc:
        raise UmtProtocolError(E_BAD_JSON, f"帧不是合法 JSON: {exc}") from exc
    if not isinstance(obj, dict):
        raise UmtProtocolError(E_BAD_JSON, f"帧不是 JSON 对象: {type(obj).__name__}")
    return obj


# ---------------------------------------------------------------- 地址
def fmt_addr(value: int) -> str:
    """地址一律用字符串 "0x..." 传输（协议 §4.1），避免精度丢失与进制歧义。"""
    return f"0x{value:x}"


def parse_addr(value: int | str) -> str:
    """把 int 或 "0x..." 字符串统一成设备端期望的字符串形式。"""
    if isinstance(value, str):
        return value
    return fmt_addr(value)
