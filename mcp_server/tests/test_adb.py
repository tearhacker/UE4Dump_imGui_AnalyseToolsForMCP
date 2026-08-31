"""ADB 自动端口转发测试。"""

from __future__ import annotations

from src.umt_mcp import adb


def _online_devices() -> tuple[int, str, str]:
    return 0, "List of devices attached\nDEVICE123\tdevice", ""


def test_setup_creates_forward_without_manual_command(monkeypatch) -> None:
    calls: list[tuple[str, ...]] = []

    def fake_cmd(*args: str) -> tuple[int, str, str]:
        calls.append(args)
        if args == ("version",):
            return 0, "Android Debug Bridge version 1.0.41", ""
        if args == ("devices",):
            return _online_devices()
        if args == ("forward", "--list"):
            return 0, "", ""
        if args == ("forward", "tcp:35515", "tcp:35515"):
            return 0, "", ""
        raise AssertionError(args)

    monkeypatch.setattr(adb, "_adb_cmd", fake_cmd)

    ok, message = adb.setup(35515)

    assert ok
    assert "自动转发已建立" in message
    assert calls == [
        ("version",),
        ("devices",),
        ("forward", "--list"),
        ("forward", "tcp:35515", "tcp:35515"),
    ]


def test_setup_reuses_exact_existing_forward(monkeypatch) -> None:
    calls: list[tuple[str, ...]] = []

    def fake_cmd(*args: str) -> tuple[int, str, str]:
        calls.append(args)
        if args == ("version",):
            return 0, "adb", ""
        if args == ("devices",):
            return _online_devices()
        if args == ("forward", "--list"):
            return 0, "DEVICE123 tcp:35515 tcp:35515", ""
        raise AssertionError("已有转发时不应重复下发 adb forward")

    monkeypatch.setattr(adb, "_adb_cmd", fake_cmd)

    ok, message = adb.setup(35515)

    assert ok
    assert "自动转发已就绪" in message
    assert calls[-1] == ("forward", "--list")


def test_force_refresh_reissues_forward(monkeypatch) -> None:
    calls: list[tuple[str, ...]] = []

    def fake_cmd(*args: str) -> tuple[int, str, str]:
        calls.append(args)
        if args == ("version",):
            return 0, "adb", ""
        if args == ("devices",):
            return _online_devices()
        if args == ("forward", "tcp:35515", "tcp:35515"):
            return 0, "", ""
        raise AssertionError(args)

    monkeypatch.setattr(adb, "_adb_cmd", fake_cmd)

    ok, message = adb.setup(35515, force=True)

    assert ok
    assert "自动转发已刷新" in message
    assert ("forward", "--list") not in calls
    assert calls[-1] == ("forward", "tcp:35515", "tcp:35515")


def test_similar_port_is_not_mistaken_for_requested_forward(monkeypatch) -> None:
    calls: list[tuple[str, ...]] = []

    def fake_cmd(*args: str) -> tuple[int, str, str]:
        calls.append(args)
        if args == ("version",):
            return 0, "adb", ""
        if args == ("devices",):
            return _online_devices()
        if args == ("forward", "--list"):
            return 0, "DEVICE123 tcp:355150 tcp:355150", ""
        if args == ("forward", "tcp:35515", "tcp:35515"):
            return 0, "", ""
        raise AssertionError(args)

    monkeypatch.setattr(adb, "_adb_cmd", fake_cmd)

    assert adb.setup(35515)[0]
    assert calls[-1] == ("forward", "tcp:35515", "tcp:35515")


def test_setup_stops_before_forward_when_device_is_offline(monkeypatch) -> None:
    calls: list[tuple[str, ...]] = []

    def fake_cmd(*args: str) -> tuple[int, str, str]:
        calls.append(args)
        if args == ("version",):
            return 0, "adb", ""
        if args == ("devices",):
            return 0, "List of devices attached", ""
        raise AssertionError("无在线设备时不应执行 forward")

    monkeypatch.setattr(adb, "_adb_cmd", fake_cmd)

    ok, message = adb.setup(35515)

    assert not ok
    assert "没有检测到在线设备" in message
    assert calls == [("version",), ("devices",)]
