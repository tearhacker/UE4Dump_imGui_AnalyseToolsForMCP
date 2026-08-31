"""ADB 端口转发管理。

协议 §1 规定设备端 bind 127.0.0.1:27185，仅经 adb forward 暴露给 PC。
本模块封装 adb forward 的前置检查和幂等执行。

错误处理原则：报错时给出明确的操作指引，不静默失败。
"""

from __future__ import annotations

import logging
import subprocess
import sys
from typing import Sequence

logger = logging.getLogger(__name__)

DEFAULT_PORT = 27185

# 可执行文件路径，默认走 PATH；可用 set_adb_bin() 覆盖（如 mcp.json 传入 --adb）
_adb_bin = "adb"


def set_adb_bin(path: str) -> None:
    """指定 adb 可执行文件的绝对路径（替代 PATH 查找）。"""
    global _adb_bin
    _adb_bin = path
    logger.info("adb 已指定为: %s", path)


def _adb_cmd(*args: str) -> tuple[int, str, str]:
    """执行 adb 命令，返回 (returncode, stdout, stderr)。"""
    try:
        result = subprocess.run(
            [_adb_bin] + list(args),
            capture_output=True,
            text=True,
            timeout=10,
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


def ensure_forward(port: int = DEFAULT_PORT) -> tuple[bool, str]:
    """确保 adb forward tcp:{port} tcp:{port} 已设置（幂等）。

    返回 (ok, message)。
    """
    # 先检查是否已存在
    rc, stdout, _ = _adb_cmd("forward", "--list")
    if rc == 0:
        spec = f"tcp:{port}"
        for line in stdout.splitlines():
            if spec in line:
                logger.debug("forward 已存在: %s", line.strip())
                return True, f"adb forward 已设置: {line.strip()}"

    # 执行 forward
    rc, stdout, stderr = _adb_cmd("forward", f"tcp:{port}", f"tcp:{port}")
    if rc != 0:
        return False, (
            f"adb forward 失败: {stderr or stdout}\n"
            f"请手动执行：\n"
            f"  adb forward tcp:{port} tcp:{port}"
        )

    logger.info("adb forward 已设置: tcp:%d → tcp:%d", port, port)
    return True, f"adb forward 已设置: tcp:{port} → tcp:{port}"


def setup(port: int = DEFAULT_PORT) -> tuple[bool, str]:
    """一次性检查 adb 可用性 + 设置端口转发。

    返回 (ok, full_message)。
    """
    ok, msg = check_adb()
    if not ok:
        return False, msg

    ok2, msg2 = ensure_forward(port)
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
