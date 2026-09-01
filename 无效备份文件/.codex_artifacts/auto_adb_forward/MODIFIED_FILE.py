"""ADB 端口转发管理。

协议 §1 规定设备端 bind 127.0.0.1:35515，仅经 adb forward 暴露给 PC。
本模块封装 adb forward 的自动检查、幂等建立和失败刷新。

错误处理原则：自动恢复失败时返回可诊断原因，不要求用户手工补命令。
"""

from __future__ import annotations

import logging
import subprocess
import sys
import threading

logger = logging.getLogger(__name__)

DEFAULT_PORT = 35515

# 可执行文件路径，默认走 PATH；可用 set_adb_bin() 覆盖（如 mcp.json 传入 --adb）
_adb_bin = "adb"
_forward_lock = threading.RLock()


def set_adb_bin(path: str) -> None:
    """指定 adb 可执行文件的绝对路径（替代 PATH 查找）。"""
    global _adb_bin
    _adb_bin = path
    logger.info("adb 已指定为: %s", path)


def _adb_cmd(*args: str) -> tuple[int, str, str]:
    """执行 adb 命令，返回 (returncode, stdout, stderr)。"""
    try:
        creationflags = subprocess.CREATE_NO_WINDOW if sys.platform == "win32" else 0
        result = subprocess.run(
            [_adb_bin] + list(args),
            capture_output=True,
            text=True,
            timeout=10,
            creationflags=creationflags,
        )
        return result.returncode, result.stdout.strip(), result.stderr.strip()
    except FileNotFoundError:
        return 1, "", f"adb 命令未找到（{_adb_bin}），请确认 Android SDK Platform Tools 已安装、路径正确并在 PATH 中"
    except subprocess.TimeoutExpired:
        return 1, "", "adb 命令超时（10s）"


def check_adb() -> tuple[bool, str]:
    """检查 adb 是否可用以及是否有设备连接。

    返回 (ok, message)。
    """
    rc, stdout, stderr = _adb_cmd("version")
    if rc != 0:
        return False, f"adb 不可用: {stderr or stdout}"

    rc, stdout, stderr = _adb_cmd("devices")
    if rc != 0:
        return False, f"adb devices 失败: {stderr}"

    # 解析输出，找在线设备
    lines = stdout.splitlines()
    devices = []
    for line in lines[1:]:  # 跳过 "List of devices attached"
        parts = line.split()
        if len(parts) >= 2 and parts[1] == "device":
            devices.append(parts[0])

    if not devices:
        return False, (
            "没有检测到在线设备。\n"
            "请确认：\n"
            "  1. 手机已通过 USB 连接\n"
            "  2. 手机端已授权 USB 调试\n"
            "  3. 运行 `adb devices` 确认设备状态为 device（不是 unauthorized）"
        )

    return True, f"检测到 {len(devices)} 个在线设备: {', '.join(devices)}"


def _is_matching_forward(line: str, port: int) -> bool:
    """匹配 adb forward --list 的完整 local/remote 端口列。"""
    parts = line.split()
    spec = f"tcp:{port}"
    return len(parts) >= 3 and parts[-2:] == [spec, spec]


def ensure_forward(port: int = DEFAULT_PORT, *, force: bool = False) -> tuple[bool, str]:
    """确保 adb forward tcp:{port} tcp:{port} 已设置（幂等）。

    force=True 会重新下发转发命令，用于 socket 连接失败后的隧道自愈。
    返回 (ok, message)。
    """
    with _forward_lock:
        if not force:
            rc, stdout, _ = _adb_cmd("forward", "--list")
            if rc == 0:
                for line in stdout.splitlines():
                    if _is_matching_forward(line, port):
                        logger.debug("自动转发已存在: %s", line.strip())
                        return True, f"ADB 自动转发已就绪: {line.strip()}"

        spec = f"tcp:{port}"
        rc, stdout, stderr = _adb_cmd("forward", spec, spec)
        if rc != 0:
            return False, f"ADB 自动端口转发失败: {stderr or stdout}"

        action = "已刷新" if force else "已建立"
        logger.info("ADB 自动转发%s: %s → %s", action, spec, spec)
        return True, f"ADB 自动转发{action}: {spec} → {spec}"


def setup(port: int = DEFAULT_PORT, *, force: bool = False) -> tuple[bool, str]:
    """检查 ADB 和在线设备，并自动建立端口转发。

    返回 (ok, full_message)。
    """
    with _forward_lock:
        ok, msg = check_adb()
        if not ok:
            return False, msg

        ok2, msg2 = ensure_forward(port, force=force)
        if not ok2:
            return False, f"{msg}\n{msg2}"

        return True, f"{msg}\n{msg2}"


def cli_main() -> None:
    """命令行入口：python -m src.umt_mcp.adb"""
    ok, msg = setup()
    print(msg)
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    cli_main()
