"""设备端 socket 桥接层：连接 / 握手 / 分帧 / 心跳 / 重连 / 串行化。

这是 PC 侧唯一碰 socket 的地方，工具层只调 request()。

三个关键设计：

1. **独立读线程**（`_reader_loop`）
   设备端心跳是"每 2 秒一帧，无论是否有请求在处理"（协议 §3.8）。
   命令执行到一半时心跳照样会来，所以不能"发一帧、读一帧"地同步泵 ——
   读线程把心跳/日志/响应分开投递，等响应的线程只看响应队列。

2. **心跳判活**（协议 §3.8）
   等响应时不傻等：每 0.5s 检查一次心跳新鲜度。
   - 心跳正常但没结果 → "在算"，继续等到超时
   - 超过 10s 没心跳 → 判定进程假死，断开重连
   没有这个区分，AI 只能盲等，最终触发重试风暴。

3. **严格串行**（协议 §9）
   设备协议是"发一收一"，单对单场景下串行是最优解而非折中。
   所有 request() 走同一把 RLock，MCP 的并行 tool call 在这里排队。
"""

from __future__ import annotations

import itertools
import json
import logging
import os
import queue
import socket
import threading
import time
from collections import deque
from typing import Any

from . import config, protocol as proto

logger = logging.getLogger(__name__)

# token 只从环境变量取 —— 它是设备端一次性生成的密钥，不进代码、不进仓库
TOKEN_ENV_VAR = "UMT_TOKEN"


class UmtBridge:
    """设备端命令通道。线程安全，可跨 MCP tool call 复用。"""

    def __init__(
        self,
        host: str = config.HOST,
        port: int = config.DEFAULT_PORT,
        token: str | None = None,
    ) -> None:
        self._host = host
        self._port = port
        self._token = token if token is not None else os.environ.get(TOKEN_ENV_VAR, "")

        self._sock: socket.socket | None = None
        self._reader: threading.Thread | None = None
        self._stop = threading.Event()

        self._lock = threading.RLock()          # 协议 §9：串行化所有设备调用
        self._ids = itertools.count(1)          # 请求 id 单调递增
        self._responses: queue.Queue[dict] = queue.Queue()

        self._handshake: dict[str, Any] = {}
        self._handshake_event = threading.Event()

        self._last_heartbeat = 0.0
        self._busy = False
        self._active_job_id: str | None = None
        self._logs: deque[dict] = deque(maxlen=500)

        self._build = ""
        self._capabilities: list[str] = []

    # ------------------------------------------------------------ 属性
    @property
    def connected(self) -> bool:
        return self._sock is not None and not self._stop.is_set()

    @property
    def build(self) -> str:
        return self._build

    @property
    def capabilities(self) -> list[str]:
        return list(self._capabilities)

    @property
    def busy(self) -> bool:
        return self._busy

    @property
    def active_job_id(self) -> str | None:
        return self._active_job_id

    def seconds_since_heartbeat(self) -> float:
        if self._last_heartbeat <= 0:
            return 0.0
        return time.monotonic() - self._last_heartbeat

    def drain_logs(self) -> list[dict]:
        """取走设备端推送的日志帧（协议 §3.7），取完即清空。"""
        with self._lock:
            logs = list(self._logs)
            self._logs.clear()
        return logs

    # ------------------------------------------------------------ 连接
    def connect(self, max_attempts: int = 1) -> None:
        """建立连接并完成 HELLO + AUTH 握手。失败按指数退避重试。

        max_attempts=1 表示只试一次（默认，让工具快速失败而不是挂住）。
        """
        last_exc: Exception | None = None
        attempt = 0
        while True:
            attempt += 1
            try:
                self._connect_once()
                return
            except (OSError, proto.UmtError) as exc:
                last_exc = exc
                self._teardown()

                limit = config.RECONNECT_MAX_ATTEMPTS
                if max_attempts > 1:
                    limit = max_attempts
                if limit and attempt >= limit:
                    break

                delay = min(
                    config.RECONNECT_BASE_DELAY * (2 ** (attempt - 1)),
                    config.RECONNECT_MAX_DELAY,
                )
                logger.warning(
                    "设备端连接失败（第 %d 次）：%s —— %.1fs 后重试", attempt, exc, delay
                )
                time.sleep(delay)

        raise proto.UmtConnectionError(
            f"无法连接设备端 {self._host}:{self._port}（已试 {attempt} 次）。\n"
            f"请确认：1) 手机已 adb 连接  2) 已执行 "
            f"`adb forward tcp:{self._port} tcp:{self._port}`  "
            f"3) UMT 已启动并在运行\n"
            f"底层错误：{last_exc}"
        )

    def _connect_once(self) -> None:
        self._stop.clear()
        self._handshake.clear()
        self._handshake_event.clear()
        self._responses = queue.Queue()

        logger.debug("connecting to %s:%d", self._host, self._port)
        sock = socket.create_connection((self._host, self._port), timeout=config.SOCKET_TIMEOUT)
        sock.settimeout(None)  # 读线程阻塞读，超时由我们自己在等待侧控制
        self._sock = sock
        self._last_heartbeat = time.monotonic()

        self._reader = threading.Thread(target=self._reader_loop, name="umt-reader", daemon=True)
        self._reader.start()

        # ---- HELLO（设备端主动发，🔴 不含 token）
        hello = self._await_handshake(timeout=5.0)
        if hello.get("type") != proto.MSG_HELLO:
            raise proto.UmtProtocolError(
                proto.E_PROTOCOL_MISMATCH, f"期望 HELLO，收到 {hello.get('type')!r}"
            )

        remote_proto = hello.get("protocol")
        if remote_proto != config.PROTOCOL_VERSION:
            self._teardown()
            raise proto.UmtProtocolError(
                proto.E_PROTOCOL_MISMATCH,
                f"协议版本不匹配：设备端 {remote_proto}，PC 侧 {config.PROTOCOL_VERSION}。"
                f"两端必须同时升级，本错误不重试。",
            )

        self._build = str(hello.get("build", ""))
        self._capabilities = list(hello.get("capabilities", []))
        logger.info("设备端已连接：build=%s，%d 条命令", self._build, len(self._capabilities))

        # ---- AUTH（PC → 设备端，首个命令之前必发）
        if not self._token:
            self._teardown()
            raise proto.UmtProtocolError(
                proto.E_BAD_TOKEN,
                f"未设置 token。请从 UMT 的 logcat 里取一次性 token，"
                f"设置环境变量 {TOKEN_ENV_VAR} 后重启本服务。",
            )

        self._handshake_event.clear()
        self._send_unlocked({"type": proto.MSG_AUTH, "protocol": config.PROTOCOL_VERSION,
                             "token": self._token})

        result = self._await_handshake(timeout=5.0)
        if result.get("type") != proto.MSG_AUTH_OK:
            err = result.get("error") or {}
            self._teardown()
            raise proto.UmtProtocolError(
                err.get("code", proto.E_BAD_TOKEN),
                err.get("msg", "token 校验失败") + " —— 请从 UMT 界面/日志取新 token",
            )

    def _await_handshake(self, timeout: float) -> dict:
        if not self._handshake_event.wait(timeout):
            self._teardown()
            raise proto.UmtConnectionError(f"等待设备端握手帧超时（{timeout}s）")
        return dict(self._handshake)

    # ------------------------------------------------------------ 读线程
    def _reader_loop(self) -> None:
        buf = bytearray()
        sock = self._sock
        if sock is None:
            return
        try:
            while not self._stop.is_set():
                try:
                    chunk = sock.recv(config.RECV_CHUNK_SIZE)
                except OSError:
                    break
                if not chunk:                      # 对端关闭
                    break

                buf.extend(chunk)
                while True:
                    idx = buf.find(proto.FRAME_DELIMITER)
                    if idx < 0:
                        break
                    raw = bytes(buf[:idx])
                    del buf[: idx + 1]

                    if len(raw) > config.MAX_FRAME_SIZE:
                        logger.error("帧超过 %d 字节，丢弃", config.MAX_FRAME_SIZE)
                        continue
                    if not raw.strip():
                        continue

                    try:
                        self._dispatch(proto.decode_frame(raw))
                    except proto.UmtProtocolError as exc:
                        logger.warning("丢弃非法帧：%s", exc)
        finally:
            self._stop.set()
            # 唤醒所有等待者，避免调用方挂死
            self._responses.put({"__fatal__": True})
            self._handshake_event.set()

    def _dispatch(self, frame: dict) -> None:
        ftype = frame.get("type")

        if ftype in (proto.MSG_HELLO, proto.MSG_AUTH_OK, proto.MSG_AUTH_FAIL):
            self._handshake = frame
            self._handshake_event.set()
            return

        if ftype == proto.MSG_HEARTBEAT:
            self._last_heartbeat = time.monotonic()
            self._busy = bool(frame.get("busy", False))
            self._active_job_id = frame.get("activeJobId")
            return

        if ftype == proto.MSG_LOG:
            self._logs.append(frame)
            return

        # 其余一律视为响应（按 id 配对）
        self._responses.put(frame)

    # ------------------------------------------------------------ 收发
    def _send_unlocked(self, obj: dict) -> None:
        if self._sock is None:
            raise proto.UmtConnectionError("socket 未连接")
        self._sock.sendall(proto.encode_frame(obj))

    def request(
        self,
        cmd: str,
        args: dict[str, Any] | None = None,
        timeout: float = config.CMD_TIMEOUT,
    ) -> dict[str, Any]:
        """发一条命令并等它的响应。返回 data 字段（成功）或抛出分层异常。

        协议 §9：严格串行，整段收发持锁。
        """
        with self._lock:
            if not self.connected:
                self.connect(max_attempts=3)

            req_id = next(self._ids)
            try:
                self._send_unlocked({"id": req_id, "cmd": cmd, "args": args or {}})
            except OSError as exc:
                self._teardown()
                raise proto.UmtConnectionError(f"发送 {cmd} 失败：{exc}") from exc

            return self._await_response(req_id, cmd, timeout)

    def _await_response(self, req_id: int, cmd: str, timeout: float) -> dict[str, Any]:
        deadline = time.monotonic() + timeout

        while True:
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                raise proto.UmtTimeoutError(
                    f"{cmd} 等待响应超时（{timeout:.0f}s）。"
                    f"重活在跑时需轮询 GET_*_STATUS，不要用长超时硬等。"
                )

            try:
                frame = self._responses.get(timeout=min(config.STALE_CHECK_INTERVAL, remaining))
            except queue.Empty:
                # 没收到响应 —— 借这个时间片检查设备端是否还活着
                self._check_alive(cmd)
                continue

            if frame.get("__fatal__"):
                self._teardown()
                raise proto.UmtConnectionError(f"{cmd} 执行中连接断开（设备端进程可能已退出）")

            # 协议 §3.2：id 用于校验响应配对，乱序即协议错误
            if frame.get("id") != req_id:
                raise proto.UmtProtocolError(
                    proto.E_BAD_JSON,
                    f"响应 id 不匹配：期望 {req_id}，收到 {frame.get('id')!r}（命令 {cmd}）",
                )

            if frame.get("ok"):
                data = frame.get("data")
                return data if isinstance(data, dict) else {}

            err = frame.get("error") or {}
            proto.raise_for_error(
                err.get("code", proto.E_INTERNAL),
                err.get("msg", "设备端未给出错误信息"),
                err.get("detail"),
            )

    def _check_alive(self, cmd: str) -> None:
        """心跳判活：区分「在算」与「死了」（协议 §3.8）。"""
        if self._last_heartbeat <= 0:
            return
        silent_for = time.monotonic() - self._last_heartbeat
        if silent_for > config.HEARTBEAT_TIMEOUT:
            self._teardown()
            raise proto.UmtConnectionError(
                f"{cmd}：设备端已 {silent_for:.1f}s 无心跳（阈值 {config.HEARTBEAT_TIMEOUT}s），"
                f"判定进程假死。连接已断开，下次调用会重连；"
                f"若当时有重活在跑，该 job 已作废（E_CANCELLED 语义）。"
            )

    # ------------------------------------------------------------ 生命周期
    def _teardown(self) -> None:
        self._stop.set()
        sock, self._sock = self._sock, None
        if sock is not None:
            try:
                sock.shutdown(socket.SHUT_RDWR)
            except OSError:
                pass
            finally:
                sock.close()
        self._capabilities = []
        self._last_heartbeat = 0.0
        self._busy = False
        self._active_job_id = None

    def close(self) -> None:
        with self._lock:
            self._teardown()

    def __enter__(self) -> UmtBridge:
        return self

    def __exit__(self, *exc: object) -> None:
        self.close()


# ---------------------------------------------------------------- 单例
# MCP 允许并行 tool call，但设备端是单对单串行 —— 全局共享一个 bridge 即可，
# 也保证重连后所有工具立刻看到同一个连接状态。
_bridge: UmtBridge | None = None
_bridge_lock = threading.Lock()


def get_bridge() -> UmtBridge:
    global _bridge
    with _bridge_lock:
        if _bridge is None:
            _bridge = UmtBridge()
        return _bridge


def set_bridge(bridge: UmtBridge | None) -> None:
    """测试注入用；也用于强制重连（置 None 后下次调用会新建）。"""
    global _bridge
    with _bridge_lock:
        _bridge = bridge


def call(cmd: str, args: dict[str, Any] | None = None,
         timeout: float = config.CMD_TIMEOUT) -> dict[str, Any]:
    """工具层唯一入口：发一条设备端命令。"""
    return get_bridge().request(cmd, args, timeout)


def to_json(value: Any) -> str:
    """把设备端返回的数据转成给 AI 看的文本。"""
    return json.dumps(value, ensure_ascii=False, indent=2)
