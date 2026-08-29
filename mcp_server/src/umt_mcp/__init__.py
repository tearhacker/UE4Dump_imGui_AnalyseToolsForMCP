"""UMT MCP Server —— PC 侧（官方 Python MCP SDK）。

分层（架构 v1.2 §2.2）：
    tools/     只负责「名字 → 实现」，不碰传输
    bridge     socket 客户端 + 重连 + 串行化
    protocol   设备端 wire protocol 契约
    server     JSON-RPC 分发入口
"""

__version__ = "0.1.0"
