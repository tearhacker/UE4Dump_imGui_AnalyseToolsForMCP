"""UMT PC-side MCP server (blank scaffold).

Skeleton using the official Python MCP SDK (`mcp`).
Add real tools (ADB forwarding, device socket, UMT commands) here.

Run via stdio:
    python server.py
"""
from mcp.server.fastmcp import FastMCP

mcp = FastMCP("umt-mcp")


@mcp.tool()
def ping() -> str:
    """Sanity check: returns 'pong'. Remove once real tools are wired."""
    return "pong"


def main() -> None:
    mcp.run()


if __name__ == "__main__":
    main()
