"""UMT PC 侧 MCP server 入口（stdio 传输）。

分层（架构 v1.2 §2.2）：
    src/umt_mcp/protocol.py  设备端 wire protocol 契约
    src/umt_mcp/bridge.py    socket 客户端 + 握手 + 心跳 + 重连 + 串行化
    src/umt_mcp/tools.py     工具名 → 设备端命令
    server.py（本文件）      FastMCP 挂载与启动

运行前置条件：
    1. 手机已 adb 连接，且 UMT 正在运行
    2. 已执行端口转发：adb forward tcp:27185 tcp:27185
    3. 已设置 token 环境变量：
           export UMT_TOKEN=<UMT logcat 里的一次性 token>
       token 是设备端每次启动随机生成的，重启 UMT 后必须换新的。

启动：
    python server.py
"""

from __future__ import annotations

import logging
import sys

from mcp.server.fastmcp import FastMCP

from src.umt_mcp import config, tools
from src.umt_mcp import instructions as ins_mod
from src.umt_mcp import resources as res_mod

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)-7s %(name)s: %(message)s",
    stream=sys.stderr,   # 🔴 stdout 是 MCP 协议通道，日志只能走 stderr
)
logger = logging.getLogger("umt-mcp")

mcp = FastMCP(
    config.SERVER_NAME,
    instructions=ins_mod.get_instructions(),
)

# 工具名对 AI 是 camelCase（协议 §6），Python 函数名是 snake_case
for _fn in tools.TOOLS:
    mcp.add_tool(_fn, name=tools.to_camel_case(_fn.__name__))


# ---------------------------------------------------------------- 资源路由（架构 §2.9）
@mcp.resource("umt://protocol")
def resource_protocol() -> str:
    """设备端协议要点速查 —— 排障时先读这个。"""
    return (
        "UMT 设备端 wire protocol 要点\n"
        "================================\n"
        f"传输: TCP 127.0.0.1:{config.DEFAULT_PORT}（设备端是服务端，仅经 adb forward 暴露）\n"
        f"帧格式: NDJSON，每行一个 JSON 对象，\\n 结尾，单帧上限 {config.MAX_FRAME_SIZE} 字节\n"
        "握手: 设备端先发 HELLO(不含 token) → PC 发 AUTH → 设备端回 auth_ok\n"
        f"心跳: 设备端每 {config.HEARTBEAT_INTERVAL}s 一帧；"
        f"PC 侧超过 {config.HEARTBEAT_TIMEOUT}s 无心跳即判定进程假死并断开重连\n"
        f"命令硬超时: {config.CMD_TIMEOUT}s（设备端侧）\n"
        "串行: 严格一问一答，PC 侧用 RLock 排队，不支持并发\n"
        "重连: 指数退避 1/2/4…≤30s；重连后所有 sessionId/candidate/job 立即作废\n"
        "\n"
        "错误分层（协议 §5）\n"
        "-----------------\n"
        "协议层(E_BAD_JSON/E_UNKNOWN_CMD/E_BAD_ARGS/E_PROTOCOL_MISMATCH/E_BAD_TOKEN)\n"
        "    → 说明「我调错了」，通常改调用方式或取新 token\n"
        "执行层(E_READ_FAILED/E_PROBE_FAILED/E_NOT_READY/E_TIMEOUT/E_PTRACE_FAILED…)\n"
        "    → 说明「设备端失败了」，需要排障或换策略\n"
        "\n"
        "高频坑\n"
        "------\n"
        "· 地址一律用 \"0x...\" 字符串，不用数字\n"
        "· MEMORY_READ size 上限 4096；大文件走 adb pull，绝不整读进上下文\n"
        "· strip 过的 ELF 查不到符号 → 回退 scan_pattern\n"
        "· Array/Set/Map 字段只回摘要，不展开元素\n"
        "· SCAN_GNAMES / SCAN_OBJECTS 是分钟级，优先用 sample_* 抽样\n"
    )


@mcp.resource("umt://capabilities")
def resource_capabilities() -> str:
    """当前设备端能力清单。"""
    return res_mod.get_capabilities_summary()


@mcp.resource("umt://config")
def resource_config() -> str:
    """PC 侧配置摘要（不含 token）。"""
    return res_mod.get_config_summary()


@mcp.resource("umt://process")
def resource_process() -> str:
    """当前目标进程信息（从 bridge 状态派生）。"""
    return res_mod.get_process_info()


def main() -> None:
    problems = tools.self_check()
    if problems:
        for item in problems:
            logger.error("自检未通过: %s", item)
        raise SystemExit(1)

    logger.info(
        "umt-mcp 就绪：%d 个工具，目标 %s:%d",
        len(tools.TOOLS), config.HOST, config.DEFAULT_PORT,
    )
    mcp.run()


if __name__ == "__main__":
    main()
