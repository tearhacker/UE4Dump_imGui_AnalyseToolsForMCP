"""PC 侧 ARM64 反汇编封装（E 组 disassemble 工具）。

设备端没有反汇编器（Keystone 已被 -DkNO_KEYSTONE 关闭），
所以 disassemble 工具走 PC 侧 capstone 库。

capstone 可缺失降级：缺失时工具直接返回明确提示，不静默失败。
"""

from __future__ import annotations

import logging
from typing import Any

logger = logging.getLogger(__name__)

try:
    import capstone
    import capstone.arm64

    _CAPSTONE_AVAILABLE = True
except ImportError:
    _CAPSTONE_AVAILABLE = False
    capstone = None  # type: ignore[assignment]


def disassemble_hex(hex_bytes: str, base_addr: str = "0x0",
                    arch: str = "arm64") -> list[dict[str, Any]]:
    """把 hex 字符串反汇编成指令列表。

    Args:
        hex_bytes: 十六进制字节串，如 "e10f1aa9e3031fd5"
        base_addr: 起始地址（字符串 "0x..." 或整数），指令地址从此推算
        arch: "arm64"（默认）

    Returns:
        [{address, bytes, mnemonic, operands}, ...]

    Raises:
        RuntimeError: 当 capstone 未安装时，返回明确的缺失提示
    """
    if not _CAPSTONE_AVAILABLE:
        raise RuntimeError(
            "capstone 未安装。执行以下命令安装后再试：\n"
            "    pip install capstone\n"
            "（E 组 disassemble 工具的 PC 侧依赖，缺失时无法反汇编）"
        )

    try:
        raw = bytes.fromhex(hex_bytes)
    except ValueError as exc:
        raise ValueError(f"hex_bytes 格式错误: {exc}") from exc

    try:
        if arch.lower() == "arm64":
            md = capstone.Cs(capstone.CS_ARCH_ARM64, capstone.CS_MODE_ARM)
        else:
            raise ValueError(f"不支持的架构: {arch}")
    except Exception as exc:
        raise RuntimeError(f"capstone 初始化失败: {exc}") from exc

    # 解析 base_addr
    if isinstance(base_addr, str):
        try:
            addr = int(base_addr, 16)
        except ValueError:
            addr = 0
    else:
        addr = int(base_addr)

    instructions = []
    try:
        for insn in md.disasm(raw, addr):
            instructions.append({
                "address": f"0x{insn.address:x}",
                "bytes": insn.bytes.hex(),
                "mnemonic": insn.mnemonic,
                "operands": insn.op_str,
            })
    except Exception as exc:
        raise RuntimeError(f"capstone 反汇编失败: {exc}") from exc

    return instructions


def disassemble_addr(address: str, count: int = 16,
                     arch: str = "arm64") -> list[dict[str, Any]]:
    """读取目标进程内存并按 count 条指令反汇编。

    这需要先从设备端读内存（readMemory），再在 PC 侧反汇编。
    由工具层组装，本函数只负责纯反汇编逻辑。
    """
    from . import bridge as br
    from . import protocol as proto

    # 先读内存
    try:
        mem_data = br.call("MEMORY_READ", {
            "address": address,
            "size": count * 4,  # arm64 每条 4 字节
        })
    except (proto.UmtExecutionError, proto.UmtConnectionError) as exc:
        raise RuntimeError(
            f"读取内存失败（{address}，{count*4} 字节）: {exc}"
        ) from exc

    hex_str = mem_data.get("hex", "")
    if not hex_str:
        return []

    # 截断到实际需要的字节数
    max_bytes = count * 4
    actual_bytes = (len(hex_str) // 2)
    if actual_bytes > max_bytes:
        hex_str = hex_str[:max_bytes * 2]

    return disassemble_hex(hex_str, base_addr=address, arch=arch)
