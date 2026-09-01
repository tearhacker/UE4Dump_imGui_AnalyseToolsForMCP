"""UMT MCP Server 打包脚本 —— 生成可直接分发的 zip 成品包。

用法：
    python build_release.py                # 默认 v0.1.0
    VERSION=0.2.0 python build_release.py  # 指定版本

输出：
    dist/umt-mcp-v0.1.0.zip  （内含启动指南、客户端配置模板、完整源码）
"""

from __future__ import annotations

import os
import shutil
import zipfile
from pathlib import Path

VERSION = os.environ.get("VERSION", "0.1.0")
ROOT = Path(__file__).resolve().parent          # mcp_server/
DIST = ROOT.parent / "dist"                    # 项目根/dist/
BUILD = DIST / f"umt-mcp-v{VERSION}-build"
ZIP   = DIST / f"umt-mcp-v{VERSION}.zip"

# ── 清理 ────────────────────────────────────────────────────────────────────
if BUILD.exists():
    shutil.rmtree(BUILD)
if ZIP.exists():
    ZIP.unlink()
BUILD.mkdir(parents=True, exist_ok=True)

CORE = ["server.py", "requirements.txt", "pyproject.toml", "README.md", "README_EN.md"]
CLIENT_CFG = [
    "客户端AI工具配置MCP文件-同机直连.json",
    "客户端AI工具配置MCP文件.json",
    "客户端配置-手机端Operit.json",
]


def copy_file(src: Path, dst: Path) -> None:
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(src, dst)
    print(f"  + {dst.relative_to(BUILD)}")


# ── 复制核心文件 ─────────────────────────────────────────────────────────────
print("[1/3] 复制核心文件...")
for name in CORE:
    src = ROOT / name
    if src.exists():
        copy_file(src, BUILD / name)

# ── 复制客户端配置 ───────────────────────────────────────────────────────────
print("[2/3] 复制客户端配置文件...")
for name in CLIENT_CFG:
    src = ROOT / name
    if src.exists():
        copy_file(src, BUILD / name)

# ── 复制 Python 模块（排除 __pycache__）─────────────────────────────────────
print("      复制 src/umt_mcp/ ...")
umt_src = ROOT / "src" / "umt_mcp"
for root, dirs, files in os.walk(umt_src):
    dirs[:] = [d for d in dirs if d != "__pycache__"]
    for fname in files:
        if not fname.endswith(".py"):
            continue
        src = Path(root) / fname
        rel = src.relative_to(umt_src)
        dst = BUILD / "src" / "umt_mcp" / rel
        copy_file(src, dst)

# ── 生成启动指南 ─────────────────────────────────────────────────────────────
print("[3/3] 生成启动指南...")
startup = f"""# UMT MCP Server v{VERSION}

## 快速启动

### 前置要求
- Python 3.10+
- ADB 已安装，设备已通过 USB 调试连接
- 设备端 UMT 正在运行

### 安装依赖
```bash
pip install -r requirements.txt
```

### 启动方式

**USB 调试模式（默认）：**
```bash
python server.py
```

**局域网直连：**
```bash
python server.py --no-adb --host <设备IP>
```

**同机直连（Operit）：**
```bash
python server.py --no-adb --host 127.0.0.1
```

## 文件说明

| 文件 | 说明 |
|------|------|
| `server.py` | 主入口，FastMCP stdio 服务器 |
| `src/umt_mcp/` | 全部核心模块（bridge/tools/protocol/...） |
| `requirements.txt` | 依赖列表 |
| `客户端AI工具配置MCP文件*.json` | AI 客户端连接配置模板（LAN/直连/Operit） |
| `STARTUP.md` | 本文件 |

## 完整文档

- **README.md** — 技术规格、工具清单、排障指南
- **README_EN.md** — 英文版说明

---
泪心团队 | https://teargamestorem.top/
"""
(BUILD / "STARTUP.md").write_text(startup, encoding="utf-8")
print("  + STARTUP.md")

# ── 打 zip ───────────────────────────────────────────────────────────────────
print("\n打包中...")
with zipfile.ZipFile(ZIP, "w", zipfile.ZIP_DEFLATED) as zf:
    for root, _, files in os.walk(BUILD):
        for fname in files:
            full = Path(root) / fname
            arcname = full.relative_to(BUILD)
            zf.write(full, arcname)

size = ZIP.stat().st_size
print(f"\n✓ 完成！")
print(f"  输出: {ZIP}")
print(f"  大小: {size / 1024:.1f} KB")
