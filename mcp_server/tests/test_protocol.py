"""协议层单元测试（不连真机，纯逻辑）。

覆盖 docs/mcp-protocol.md §3 帧格式 / §5 错误码 / §4 编码约定。
跑：.venv/Scripts/python.exe -m pytest tests/test_protocol.py -v
"""

from __future__ import annotations

import pytest

from src.umt_mcp import protocol as proto
from src.umt_mcp import config


# ---------------------------------------------------------------- 帧编解码
class TestFrameEncodeDecode:
    def test_hello_frame_roundtrip(self) -> None:
        obj = {
            "type": proto.MSG_HELLO,
            "protocol": config.PROTOCOL_VERSION,
            "build": "1.0.0",
            "capabilities": ["PING", "LIST_PROCESSES"],
        }
        raw = proto.encode_frame(obj)
        assert raw.endswith(b"\n")
        decoded = proto.decode_frame(raw)
        assert decoded == obj

    def test_request_frame_roundtrip(self) -> None:
        obj = {"id": 42, "cmd": "MEMORY_READ", "args": {"address": "0x7a8b9c0000", "size": 256}}
        raw = proto.encode_frame(obj)
        decoded = proto.decode_frame(raw)
        assert decoded["id"] == 42
        assert decoded["cmd"] == "MEMORY_READ"
        assert decoded["args"]["address"] == "0x7a8b9c0000"

    def test_heartbeat_frame(self) -> None:
        obj = {"type": proto.MSG_HEARTBEAT, "ts": 1785312000, "busy": True, "activeJobId": "j-123"}
        decoded = proto.decode_frame(proto.encode_frame(obj))
        assert decoded["type"] == proto.MSG_HEARTBEAT
        assert decoded["busy"] is True
        assert decoded["activeJobId"] == "j-123"

    def test_log_push_frame(self) -> None:
        obj = {"type": proto.MSG_LOG, "level": "E", "message": "[Bootstrap] GUObject 失败"}
        decoded = proto.decode_frame(proto.encode_frame(obj))
        assert decoded["level"] == "E"

    def test_chinese_message_preserved(self) -> None:
        """UTF-8 中文必须原样保留（协议 §4）。"""
        obj = {"type": "log", "message": "通用方式搜索 GUObject 失败"}
        decoded = proto.decode_frame(proto.encode_frame(obj))
        assert decoded["message"] == "通用方式搜索 GUObject 失败"

    def test_empty_frame_rejected(self) -> None:
        with pytest.raises(proto.UmtProtocolError):
            proto.decode_frame(b"")

    def test_non_object_frame_rejected(self) -> None:
        with pytest.raises(proto.UmtProtocolError, match="不是 JSON 对象"):
            proto.decode_frame(b"[1, 2, 3]\n")

    def test_max_frame_size_constant(self) -> None:
        assert config.MAX_FRAME_SIZE == 1024 * 1024  # 1MB


# ---------------------------------------------------------------- 地址格式（协议 §4.1）
class TestAddressFormat:
    def test_fmt_addr_small(self) -> None:
        assert proto.fmt_addr(0x10) == "0x10"

    def test_fmt_addr_large(self) -> None:
        assert proto.fmt_addr(0x7FFF0000) == "0x7fff0000"

    def test_parse_addr_string_passthrough(self) -> None:
        assert proto.parse_addr("0x7a8b9c0000") == "0x7a8b9c0000"

    def test_parse_addr_int_converted(self) -> None:
        assert proto.parse_addr(0x1000) == "0x1000"


# ---------------------------------------------------------------- 错误分层（协议 §5）
class TestErrorLayering:
    def test_protocol_errors_raise_protocol_error(self) -> None:
        for code in proto.PROTOCOL_ERRORS:
            with pytest.raises(proto.UmtProtocolError) as exc:
                proto.raise_for_error(code, "test msg")
            assert exc.value.code == code

    def test_execution_errors_raise_execution_error(self) -> None:
        for code in proto.EXECUTION_ERRORS:
            with pytest.raises(proto.UmtExecutionError) as exc:
                proto.raise_for_error(code, "test msg")
            assert exc.value.code == code

    def test_error_detail_passed_through(self) -> None:
        detail = {"serverProtocol": 1, "clientProtocol": 99}
        exc = proto.UmtProtocolError(proto.E_PROTOCOL_MISMATCH, "版本不匹配", detail)
        assert exc.detail == detail

    def test_non_retryable_is_subset_of_protocol_errors(self) -> None:
        assert proto.NON_RETRYABLE.issubset(proto.PROTOCOL_ERRORS)


# ---------------------------------------------------------------- 常量对齐（与设备端 Protocol.hpp）
class TestConstantsAlignment:
    def test_protocol_version(self) -> None:
        assert config.PROTOCOL_VERSION == 1

    def test_default_port(self) -> None:
        assert config.DEFAULT_PORT == 35515

    def test_heartbeat_timeout(self) -> None:
        assert config.HEARTBEAT_TIMEOUT == 10.0
        assert config.HEARTBEAT_INTERVAL == 2.0

    def test_cmd_timeout(self) -> None:
        assert config.CMD_TIMEOUT == 120.0

    def test_read_memory_max_size(self) -> None:
        assert config.READ_MEMORY_MAX_SIZE == 4096

    def test_get_logs_default_lines(self) -> None:
        assert config.GET_LOGS_DEFAULT_LINES == 50
