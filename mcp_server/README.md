# UMT MCP Server (PC-side)

Blank scaffold using the **official Python MCP SDK** (`mcp` package).

Transports / I/O (adb forward, device socket, command dispatch) are **not** wired yet — this is just the boot-up shell. Add real tools under `server.py`.

## Layout

```
mcp_server/
  pyproject.toml    # project metadata + dependency on mcp>=1.0
  server.py         # FastMCP("umt-mcp") with a single ping tool
  .gitignore        # ignore .venv, __pycache__, build artefacts
```

## Install (managed Python)

```powershell
# one-shot: create venv and install
& "C:\Users\52334\.workbuddy\binaries\python\versions\3.13.12\python.exe" `
    -m venv .venv
.\.venv\Scripts\python.exe -m pip install -e .
```

## Run (stdio)

```powershell
.\.venv\Scripts\python.exe -m server
# or
.\.venv\Scripts\python.exe server.py
```

It will wait on stdio for an MCP client (Claude Desktop, MCP Inspector, etc.) to speak first.

## Next steps

1. Define the device-side wire protocol (HELLO handshake, JSON over TCP on port 27185).
2. Add ADB forwarding helper (`adb forward tcp:27185 tcp:27185`).
3. Translate `docs/泪心UE4_MCP工具功能规格.md` 42 tools into `@mcp.tool()` functions.
