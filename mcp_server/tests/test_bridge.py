"""不连真机的传输层验证：用 Python 起一个模拟设备端。

覆盖 docs/mcp-protocol.md 里最容易写错的几处：
- HELLO 后无需认证，首个命令可直接发送
- protocol 不匹配要断开并给出对应错误码
- 心跳会插在响应之前，等响应的那一侧必须能跳过
- 心跳停止 = 进程假死，必须能被区分于"还在算"
- 错误分层：协议层 vs 执行层

跑：.venv/Scripts/python.exe -m pytest tests/ -v
"""

from __future__ import annotations

import json
import socket
import threading
import time

import pytest

from src.umt_mcp import bridge as br
from src.umt_mcp import config, protocol as proto
from src.umt_mcp import tools

class MockDevice:
    """模拟设备端 CommandServer。"""

    def __init__(self, protocol: int = 1,
                  heartbeat_interval: float = 0.2) -> None:
        self.protocol = protocol
        self.heartbeat_interval = heartbeat_interval
        self.heartbeat_enabled = True
        self.received: list[dict] = []

        self._srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._srv.bind(("127.0.0.1", 0))
        self._srv.listen(1)
        self.port = self._srv.getsockname()[1]

        self._conn: socket.socket | None = None
        self._stop = threading.Event()
        self._thread = threading.Thread(target=self._serve, daemon=True)

    # ---------------------------------------------------------- 生命周期
    def start(self) -> MockDevice:
        self._thread.start()
        return self

    def stop(self) -> None:
        self._stop.set()
        for sock in (self._conn, self._srv):
            if sock is not None:
                try:
                    sock.close()
                except OSError:
                    pass
        self._thread.join(timeout=2)

    # ---------------------------------------------------------- 服务端
    def _send(self, obj: dict) -> None:
        assert self._conn is not None
        self._conn.sendall(proto.encode_frame(obj))

    def _serve(self) -> None:
        try:
            conn, _ = self._srv.accept()
        except OSError:
            return
        self._conn = conn

        # ---- HELLO：客户端校验协议版本后可直接发送命令
        self._send({
            "type": proto.MSG_HELLO,
            "protocol": self.protocol,
            "build": "1.0.0-test",
            "capabilities": ["PING", "LIST_PROCESSES", "MEMORY_READ"],
        })

        hb = threading.Thread(target=self._heartbeat_loop, daemon=True)
        hb.start()

        buf = bytearray()
        while not self._stop.is_set():
            try:
                chunk = conn.recv(4096)
            except OSError:
                break
            if not chunk:
                break
            buf.extend(chunk)
            while b"\n" in buf:
                raw, _, buf = buf.partition(b"\n")
                if not raw.strip():
                    continue
                frame = json.loads(raw)
                self.received.append(frame)
                try:
                    if not self._handle(frame):
                        return
                except OSError:
                    return

    def _heartbeat_loop(self) -> None:
        while not self._stop.is_set():
            time.sleep(self.heartbeat_interval)
            if not self.heartbeat_enabled or self._conn is None:
                continue
            try:
                self._send({"type": proto.MSG_HEARTBEAT,
                            "ts": int(time.time()), "busy": False})
            except OSError:
                return

    def _handle(self, frame: dict) -> bool:
        """处理一帧，返回 False 表示要断开。"""
        # 普通命令
        cmd = frame.get("cmd")
        rid = frame.get("id")

        if cmd == "SLOW":
            time.sleep(1.0)                      # 期间心跳照发
            self._send({"id": rid, "ok": True, "data": {"slow": True}})
        elif cmd == "MEMORY_READ":
            self._send({"id": rid, "ok": True,
                        "data": {"address": "0x1000", "hex": "7f454c46"}})
        elif cmd == "BROKEN":
            self._send({"id": rid, "ok": False,
                        "error": {"code": proto.E_READ_FAILED,
                                  "msg": "vm_rpm_ptr failed at 0xdead"}})
        elif cmd == "NOPE":
            self._send({"id": rid, "ok": False,
                        "error": {"code": proto.E_UNKNOWN_CMD, "msg": "未知命令"}})
        else:
            self._send({"id": rid, "ok": True, "data": {"cmd": cmd}})
        return True


# ---------------------------------------------------------------- fixtures
@pytest.fixture
def device() -> MockDevice:
    dev = MockDevice().start()
    yield dev
    dev.stop()


@pytest.fixture
def bridge(device: MockDevice, monkeypatch: pytest.MonkeyPatch) -> br.UmtBridge:
    """注入一个指向 mock 设备端的 bridge，并把心跳超时调短以便测试。"""
    monkeypatch.setattr(config, "HEARTBEAT_TIMEOUT", 1.0)
    monkeypatch.setattr(config, "STALE_CHECK_INTERVAL", 0.1)
    b = br.UmtBridge(host="127.0.0.1", port=device.port)
    br.set_bridge(b)
    yield b
    b.close()
    br.set_bridge(None)


# ---------------------------------------------------------------- 握手
def test_handshake_succeeds(bridge: br.UmtBridge, device: MockDevice) -> None:
    bridge.connect()
    assert bridge.connected
    assert bridge.build == "1.0.0-test"
    assert "MEMORY_READ" in bridge.capabilities
    assert device.received == []


def test_auto_forward_runs_before_socket_connect(
    device: MockDevice, monkeypatch: pytest.MonkeyPatch
) -> None:
    calls: list[tuple[int, bool]] = []

    def fake_setup(port: int, *, force: bool = False) -> tuple[bool, str]:
        calls.append((port, force))
        return True, "ready"

    monkeypatch.setattr(br.adb, "setup", fake_setup)
    b = br.UmtBridge(host="127.0.0.1", port=device.port, auto_forward=True)
    try:
        b.connect()
        assert calls == [(device.port, False)]
        assert b.connected
    finally:
        b.close()


def test_default_bridge_reads_runtime_config_port(monkeypatch: pytest.MonkeyPatch) -> None:
    monkeypatch.setattr(config, "DEFAULT_PORT", 45678)

    b = br.UmtBridge(auto_forward=False)

    assert b._port == 45678


def test_retry_force_refreshes_auto_forward(
    device: MockDevice, monkeypatch: pytest.MonkeyPatch
) -> None:
    setup_calls: list[tuple[int, bool]] = []
    socket_attempts = 0
    real_create_connection = br.socket.create_connection

    def fake_setup(port: int, *, force: bool = False) -> tuple[bool, str]:
        setup_calls.append((port, force))
        return True, "ready"

    def flaky_create_connection(*args, **kwargs):
        nonlocal socket_attempts
        socket_attempts += 1
        if socket_attempts == 1:
            raise ConnectionRefusedError("stale adb tunnel")
        return real_create_connection(*args, **kwargs)

    monkeypatch.setattr(br.adb, "setup", fake_setup)
    monkeypatch.setattr(br.socket, "create_connection", flaky_create_connection)
    monkeypatch.setattr(config, "RECONNECT_BASE_DELAY", 0.0)

    b = br.UmtBridge(host="127.0.0.1", port=device.port, auto_forward=True)
    try:
        b.connect(max_attempts=2)
        assert setup_calls == [(device.port, False), (device.port, True)]
        assert b.connected
    finally:
        b.close()


def test_auto_forward_failure_never_requires_manual_command(monkeypatch) -> None:
    monkeypatch.setattr(
        br.adb,
        "setup",
        lambda port, *, force=False: (False, "没有检测到在线设备"),
    )
    b = br.UmtBridge(port=35515, auto_forward=True)

    with pytest.raises(proto.UmtConnectionError) as exc:
        b.connect(max_attempts=1)

    message = str(exc.value)
    assert "已自动检查并建立 adb forward" in message
    assert "无需手工输入命令" in message


def test_first_command_is_sent_without_auth(device: MockDevice) -> None:
    """HELLO 之后首个命令直接发送，不夹带 AUTH 或 token。"""
    b = br.UmtBridge(host="127.0.0.1", port=device.port)
    b.connect()
    try:
        assert b.request("PING") == {"cmd": "PING"}
        assert device.received[0]["cmd"] == "PING"
        assert "type" not in device.received[0]
        assert "token" not in device.received[0]
    finally:
        b.close()


def test_legacy_token_environment_is_ignored(device: MockDevice, monkeypatch) -> None:
    monkeypatch.setenv("UMT_TOKEN", "obsolete-wrong-token")
    b = br.UmtBridge(host="127.0.0.1", port=device.port)
    try:
        assert b.request("PING") == {"cmd": "PING"}
    finally:
        b.close()


def test_protocol_mismatch_raises() -> None:
    dev = MockDevice(protocol=99).start()
    b = br.UmtBridge(host="127.0.0.1", port=dev.port)
    try:
        with pytest.raises(proto.UmtProtocolError) as exc:
            b.connect()
        assert exc.value.code == proto.E_PROTOCOL_MISMATCH
    finally:
        b.close()
        dev.stop()


def test_missing_token_environment_does_not_block(device: MockDevice, monkeypatch) -> None:
    monkeypatch.delenv("UMT_TOKEN", raising=False)
    b = br.UmtBridge(host="127.0.0.1", port=device.port)
    try:
        assert b.request("PING") == {"cmd": "PING"}
    finally:
        b.close()


# ---------------------------------------------------------------- 请求响应
def test_request_roundtrip(bridge: br.UmtBridge) -> None:
    data = bridge.request("MEMORY_READ", {"address": "0x1000", "size": 4})
    assert data["hex"] == "7f454c46"


def test_slow_command_survives_interleaved_heartbeats(bridge: br.UmtBridge) -> None:
    """心跳会插在响应之前 —— 等响应的一侧必须能跳过它们（协议 §3.8）。"""
    start = time.monotonic()
    data = bridge.request("SLOW", timeout=10)
    assert data["slow"] is True
    assert time.monotonic() - start >= 1.0
    # 心跳确实在这段时间里来过
    assert bridge.seconds_since_heartbeat() < 1.0


def test_execution_error_raises_execution_layer(bridge: br.UmtBridge) -> None:
    with pytest.raises(proto.UmtExecutionError) as exc:
        bridge.request("BROKEN")
    assert exc.value.code == proto.E_READ_FAILED


def test_protocol_error_raises_protocol_layer(bridge: br.UmtBridge) -> None:
    with pytest.raises(proto.UmtProtocolError) as exc:
        bridge.request("NOPE")
    assert exc.value.code == proto.E_UNKNOWN_CMD


# ---------------------------------------------------------------- 心跳判活
def test_heartbeat_stopping_is_detected_as_death(
    bridge: br.UmtBridge, device: MockDevice, monkeypatch: pytest.MonkeyPatch
) -> None:
    """心跳停了 = 死了，必须区别于"还在算"（协议 §3.8）。"""
    monkeypatch.setattr(config, "HEARTBEAT_TIMEOUT", 0.3)
    bridge.connect()
    device.heartbeat_enabled = False           # 模拟设备端进程假死

    with pytest.raises(proto.UmtConnectionError) as exc:
        bridge.request("SLOW", timeout=10)
    assert "假死" in str(exc.value)
    assert not bridge.connected                # 必须已断开，下次调用会重连


def test_waiting_keeps_going_while_heartbeat_alive(bridge: br.UmtBridge) -> None:
    """心跳正常但没结果 = 在算，要继续等到超时，不能误判死亡。"""
    with pytest.raises(proto.UmtTimeoutError):
        bridge.request("SLOW", timeout=0.5)


def test_connection_refused_reports_actionable_hint() -> None:
    """连不上时必须给出 adb forward 提示，而不是一个干巴巴的 ConnectionRefused。"""
    b = br.UmtBridge(host="127.0.0.1", port=1)
    with pytest.raises(proto.UmtConnectionError) as exc:
        b.connect(max_attempts=1)
    assert "adb forward" in str(exc.value)


# ------------------------------------------------------- 三种连接模式
def test_host_default_is_not_frozen_at_import_time() -> None:
    """🔴 回归：默认参数在 import 期求值，`host: str = config.HOST` 会让 --host 失效。

    必须写成 `host: str | None = None` 再在函数体内取 config.HOST，
    否则 server.py 的 --host 改了 config.HOST 也读不到。
    """
    original = config.HOST
    try:
        config.HOST = "192.168.1.23"
        b = br.UmtBridge()                 # 不传 host，走默认
        assert b._host == "192.168.1.23"   # 冻结时这里会是 127.0.0.1
    finally:
        config.HOST = original


def test_auto_forward_only_for_loopback_when_adb_enabled(
    monkeypatch: pytest.MonkeyPatch
) -> None:
    """adb forward 只在「目标是本机回环 + 未禁用 adb」时才有意义。"""
    original_host = config.HOST
    original_disable = config.DISABLE_ADB
    try:
        config.DISABLE_ADB = False
        config.HOST = "127.0.0.1"
        assert br.UmtBridge()._auto_forward is True      # 默认 adb 隧道

        config.HOST = "192.168.1.23"
        assert br.UmtBridge()._auto_forward is False     # 局域网直连，无需隧道

        config.DISABLE_ADB = True
        config.HOST = "127.0.0.1"
        assert br.UmtBridge()._auto_forward is False     # --no-adb 同机直连
    finally:
        config.HOST = original_host
        config.DISABLE_ADB = original_disable


def test_direct_mode_hint_omits_adb_advice(monkeypatch: pytest.MonkeyPatch) -> None:
    """直连模式连不上时，不该再让用户去查 USB 调试（那是 adb 隧道的排查项）。"""
    original = config.DISABLE_ADB
    try:
        config.DISABLE_ADB = True
        b = br.UmtBridge(host="127.0.0.1", port=1, auto_forward=False)
        with pytest.raises(proto.UmtConnectionError) as exc:
            b.connect(max_attempts=1)
        msg = str(exc.value)
        assert "直连模式" in msg
        assert "USB 调试" not in msg        # adb 专属排查项不该出现
    finally:
        config.DISABLE_ADB = original


# ---------------------------------------------------------------- 工具层
def test_self_check_passes() -> None:
    """每个工具推导出的命令名必须真实存在于设备端。"""
    assert tools.self_check() == []


def test_param_extraction_and_device_table_agree() -> None:
    """PC 侧下发的参数名必须落在设备端真正读取的键集合里。

    这是真实踩过的坑：readMemoryValue 曾把 valueType 写成 type，
    设备端永远按默认类型返回，不报错、看不出任何异常。
    """
    for fn in tools.TOOLS:
        cmd_in_src, sent = tools._sent_params(fn)
        if not cmd_in_src:
            continue
        cmd = tools._cmd(fn.__name__)
        allowed = tools.DEVICE_PARAMS[cmd]
        assert sent <= allowed, (
            f"{fn.__name__} → {cmd} 下发了设备端不读的参数 {sorted(sent - allowed)}；"
            f"该命令只接受 {sorted(allowed)}"
        )


def test_sent_params_extraction_is_accurate() -> None:
    """_sent_params 本身要可靠，否则上面的守卫就是摆设。"""
    cmd, sent = tools._sent_params(tools.read_memory_value)
    assert cmd == "read_memory_value"
    assert sent == {"address", "valueType"}

    cmd, sent = tools._sent_params(tools.ping)
    assert cmd == "ping"
    assert sent == set()


def test_dangerous_params_are_pc_side_only() -> None:
    """confirmDangerous 在 writeMemory / allocScratch 上只是 PC 侧安全门。

    设备端 WRITE_MEMORY / ALLOC_SCRATCH 并不读这个键，所以不能下发，
    否则自检会报"下发了设备端不读的参数"。
    """
    assert "confirmDangerous" not in tools.DEVICE_PARAMS["WRITE_MEMORY"]
    assert "confirmDangerous" not in tools.DEVICE_PARAMS["ALLOC_SCRATCH"]
    # 但 F 组设备端自己有这道门，必须下发
    assert "confirmDangerous" in tools.DEVICE_PARAMS["CALL_REMOTE_FUNCTION"]
    assert "confirmDangerous" in tools.DEVICE_PARAMS["CALL_REMOTE_FUNCTION_BATCH"]


def test_stateful_commands_require_select_process_first() -> None:
    """ATTACH / START_PROBE 不收 pid —— 设备端从 SELECT_PROCESS 的选中项取目标。

    这两条曾错误地带了 pid / waitMs 参数，传了也不会被读。
    """
    assert tools.DEVICE_PARAMS["ATTACH"] == frozenset()
    assert tools.DEVICE_PARAMS["START_PROBE"] == frozenset()
    assert tools.DEVICE_PARAMS["START_DUMP"] == frozenset()
    assert tools.DEVICE_PARAMS["CANCEL_JOB"] == frozenset()


def test_d_group_command_names_match_device() -> None:
    """D 组读/写语序是反的，最容易写错，单独钉住。"""
    assert tools._cmd("read_memory") == "MEMORY_READ"
    assert tools._cmd("read_memory_value") == "MEMORY_READ_VALUE"
    assert tools._cmd("write_memory") == "WRITE_MEMORY"   # 协议文档写的是 MEMORY_WRITE
    assert tools._cmd("read_string") == "READ_STRING"


def test_dangerous_tools_require_confirmation(bridge: br.UmtBridge) -> None:
    """危险操作默认拒绝，必须显式 confirm_dangerous。"""
    from mcp.server.fastmcp.exceptions import ToolError

    with pytest.raises(ToolError, match="confirm_dangerous"):
        tools.write_memory(address="0x1000", hex_bytes="90", confirm_dangerous=False)
    with pytest.raises(ToolError, match="confirm_dangerous"):
        tools.alloc_scratch(size=4096, confirm_dangerous=False)


def test_tool_end_to_end(bridge: br.UmtBridge) -> None:
    out = json.loads(tools.read_memory(address="0x1000", size=4))
    assert out["hex"] == "7f454c46"
