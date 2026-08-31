"""MCP 资源层（4 个资源）。

架构 §2.9 规定：大文件（几十 MB 的 SDK 产物）**不走 socket**，
只走 adb pull 到 PC 本地缓存，再由服务端过滤后按需返回片段。

本模块定义 4 个资源路由：
- umt://protocol      - 协议要点速查（已在 server.py 内联）
- umt://processes     - 当前目标进程信息（从 bridge 状态派生）
- umt://capabilities  - 当前设备端能力清单
- umt://config        - PC 侧配置摘要（不含 token）
"""

from __future__ import annotations

import logging

from src.umt_mcp import config
from src.umt_mcp.bridge import get_bridge

logger = logging.getLogger(__name__)


def get_process_info() -> str:
    """返回当前目标进程摘要（从 bridge 状态派生）。

    bridge 本身不保存进程信息（那是设备端状态），
    此资源目前为空占位，后续 bridge 扩展后可填充。
    """
    bridge = get_bridge()
    if not bridge.connected:
        return "连接未建立。请先运行 ping 确认设备端在线。"
    return (
        f"目标进程：未显式选中（设备端维护当前选中项）\n"
        f"设备端 build: {bridge.build}\n"
        f"能力集: {', '.join(bridge.capabilities)}\n"
        f"心跳状态: {'正常' if bridge.seconds_since_heartbeat() < config.HEARTBEAT_TIMEOUT else '异常（可能假死）'}"
    )


def get_capabilities_summary() -> str:
    """列出当前设备端支持的所有命令。"""
    bridge = get_bridge()
    if not bridge.connected:
        return "连接未建立，无法获取能力清单。"
    caps = bridge.capabilities
    return (
        f"设备端 build: {bridge.build}\n"
        f"协议版本: {config.PROTOCOL_VERSION}\n"
        f"支持 {len(caps)} 条命令:\n"
        + "\n".join(f"  - {c}" for c in sorted(caps))
    )


def get_config_summary() -> str:
    """返回 PC 侧配置摘要（不含 token 等敏感信息）。"""
    return (
        f"UMT MCP Server 配置\n"
        f"{'=' * 40}\n"
        f"服务器名: {config.SERVER_NAME}\n"
        f"版本: {config.SERVER_VERSION}\n"
        f"设备端地址: {config.HOST}:{config.DEFAULT_PORT}\n"
        f"adb forward: tcp:{config.DEFAULT_PORT} → tcp:{config.DEFAULT_PORT}\n"
        f"心跳间隔: {config.HEARTBEAT_INTERVAL}s\n"
        f"心跳超时: {config.HEARTBEAT_TIMEOUT}s\n"
        f"命令硬超时: {config.CMD_TIMEOUT}s\n"
        f"重连退避: {config.RECONNECT_BASE_DELAY}s ~ {config.RECONNECT_MAX_DELAY}s\n"
        f"单工具响应上限: {config.MAX_RESPONSE_TOKENS} token\n"
        f"readMemory size 上限: {config.READ_MEMORY_MAX_SIZE}\n"
        f"getLogs 默认行数: {config.GET_LOGS_DEFAULT_LINES}\n"
        f"工具总数: {len(__import__('src.umt_mcp.tools', fromlist=['TOOLS']).TOOLS)}\n"
    )
