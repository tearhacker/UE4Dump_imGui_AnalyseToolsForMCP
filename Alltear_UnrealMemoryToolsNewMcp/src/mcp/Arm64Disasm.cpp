#include "Arm64Disasm.hpp"

#include <cctype>
#include <cstdio>
#include <cstdint>
#include <string>

namespace UmtMcp
{
namespace
{
// 通用寄存器名（sf=1 → x0..x30/sp，sf=0 → w0..w30/wsp）
const char *GPReg(bool sf, int n)
{
    static char buf[4];
    if (n == 31)
        std::snprintf(buf, sizeof(buf), "%s", sf ? "sp" : "wsp");
    else
        std::snprintf(buf, sizeof(buf), "%s%d", sf ? "x" : "w", n);
    return buf;
}

// 有符号扩展（bits 位宽）
int64_t SignExtend(uint64_t v, int bits)
{
    if (bits <= 0 || bits >= 64)
        return static_cast<int64_t>(v);
    const uint64_t m = static_cast<uint64_t>(1) << (bits - 1);
    if (v & m)
        return static_cast<int64_t>(v - (static_cast<uint64_t>(1) << bits));
    return static_cast<int64_t>(v);
}

std::string HexU64(uint64_t v)
{
    char buf[20];
    std::snprintf(buf, sizeof(buf), "0x%llx", static_cast<unsigned long long>(v));
    return std::string(buf);
}
}  // namespace

std::string DisassembleArm64(uint64_t pc, uint32_t insn)
{
    const int sf = (insn >> 31) & 1;

    // NOP
    if (insn == 0xD503201F)
        return "nop";
    // HINT / NOP 变体（imm != 0）
    if ((insn & 0xFFF00000) == 0xD5030000)
        return "hint";
    // RET / BR / BLR（系统分支，固定编码）
    if ((insn & 0xFFFFFC1F) == 0xD65F03C0)
    {
        const int rd = insn & 0x1F;
        return rd == 30 ? std::string("ret") : ("ret " + std::string(GPReg(1, rd)));
    }
    if ((insn & 0xFFFFFC1F) == 0xD61F0000)
        return "br " + std::string(GPReg(1, insn & 0x1F));
    if ((insn & 0xFFFFFC1F) == 0xD63F0000)
        return "blr " + std::string(GPReg(1, insn & 0x1F));
    // BRK
    if ((insn & 0xFFE00000) == 0xD4200000)
        return "brk";

    const uint32_t top9 = (insn >> 23) & 0x1FF;

    // ADRP：page = PC & ~0xFFF，imm21 左移 12
    if ((insn & 0x9F000000) == 0x90000000)
    {
        const int rd = insn & 0x1F;
        uint64_t imm = ((static_cast<uint64_t>(insn >> 29) & 0x3) |
                        ((static_cast<uint64_t>(insn >> 5) & 0x7FFFF) << 2));
        imm = static_cast<uint64_t>(SignExtend(imm, 21)) << 12;
        const uint64_t target = (pc & ~static_cast<uint64_t>(0xFFF)) + imm;
        return "adrp " + std::string(GPReg(sf, rd)) + ", " + HexU64(target);
    }
    // ADR：PC + imm21
    if ((insn & 0x9F000000) == 0x10000000)
    {
        const int rd = insn & 0x1F;
        const uint64_t imm = static_cast<uint64_t>(SignExtend(
            ((static_cast<uint64_t>(insn >> 29) & 0x3) |
             ((static_cast<uint64_t>(insn >> 5) & 0x7FFFF) << 2)),
            21));
        return "adr " + std::string(GPReg(sf, rd)) + ", " + HexU64(pc + imm);
    }
    // MOVN（opc=00）
    if (top9 == 0x124 || top9 == 0x250)
    {
        const int hw = (insn >> 21) & 0x3, rd = insn & 0x1F;
        const uint32_t imm16 = (insn >> 5) & 0xFFFF;
        return "movn " + std::string(GPReg(sf, rd)) + ", #0x" + HexU64(imm16) +
               ", lsl #" + std::to_string(hw * 16);
    }
    // MOVZ（opc=01）
    if (top9 == 0x1A4 || top9 == 0x0A4)
    {
        const int hw = (insn >> 21) & 0x3, rd = insn & 0x1F;
        const uint32_t imm16 = (insn >> 5) & 0xFFFF;
        const uint64_t val = static_cast<uint64_t>(imm16) << (hw * 16);
        return "movz " + std::string(GPReg(sf, rd)) + ", #0x" + HexU64(val);
    }
    // MOVK（opc=10）
    if (top9 == 0x1E4 || top9 == 0x0E4)
    {
        const int hw = (insn >> 21) & 0x3, rd = insn & 0x1F;
        const uint32_t imm16 = (insn >> 5) & 0xFFFF;
        return "movk " + std::string(GPReg(sf, rd)) + ", #0x" + HexU64(imm16) +
               ", lsl #" + std::to_string(hw * 16);
    }
    // ADD (immediate)
    if (top9 == 0x122 || top9 == 0x022)
    {
        const int rn = (insn >> 5) & 0x1F, rd = insn & 0x1F;
        const uint32_t imm12 = (insn >> 10) & 0xFFF;
        const int sh = (insn >> 22) & 1;
        const uint64_t amount = static_cast<uint64_t>(imm12) << (sh * 12);
        return "add " + std::string(GPReg(sf, rd)) + ", " + GPReg(sf, rn) + ", #" + HexU64(amount);
    }
    // SUB (immediate)
    if (top9 == 0x1A2 || top9 == 0x0A2)
    {
        const int rn = (insn >> 5) & 0x1F, rd = insn & 0x1F;
        const uint32_t imm12 = (insn >> 10) & 0xFFF;
        const int sh = (insn >> 22) & 1;
        const uint64_t amount = static_cast<uint64_t>(imm12) << (sh * 12);
        return "sub " + std::string(GPReg(sf, rd)) + ", " + GPReg(sf, rn) + ", #" + HexU64(amount);
    }
    // LDR (literal, PC-relative)
    if ((insn & 0x3B000000) == 0x18000000)
    {
        const int rt = insn & 0x1F;
        const uint32_t imm19 = (insn >> 5) & 0x7FFFF;
        const int64_t off = SignExtend(imm19, 19) << 2;
        return "ldr " + std::string(GPReg(sf, rt)) + ", " + HexU64(pc + static_cast<uint64_t>(off));
    }
    // B (imm26)
    if ((insn & 0xFC000000) == 0x14000000)
    {
        const int64_t off = SignExtend(insn & 0x3FFFFFF, 26) << 2;
        return "b " + HexU64(pc + static_cast<uint64_t>(off));
    }
    // BL (imm26)
    if ((insn & 0xFC000000) == 0x94000000)
    {
        const int64_t off = SignExtend(insn & 0x3FFFFFF, 26) << 2;
        return "bl " + HexU64(pc + static_cast<uint64_t>(off));
    }
    // B.cond
    if ((insn & 0x7F000000) == 0x54000000)
    {
        static const char *kCond[16] = {"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
                                       "hi", "ls", "ge", "lt", "gt", "le", "al", "nv"};
        const int cond = insn & 0xF;
        const uint32_t imm19 = (insn >> 5) & 0x7FFFF;
        const int64_t off = SignExtend(imm19, 19) << 2;
        return std::string("b.") + kCond[cond & 0xF] + " " + HexU64(pc + static_cast<uint64_t>(off));
    }
    // CBZ / CBNZ
    if ((insn & 0x7F000000) == 0x34000000 || (insn & 0x7F000000) == 0x35000000)
    {
        const bool n = ((insn & 0x7F000000) == 0x35000000);
        const int rt = insn & 0x1F;
        const uint32_t imm19 = (insn >> 5) & 0x7FFFF;
        const int64_t off = SignExtend(imm19, 19) << 2;
        return std::string(n ? "cbnz " : "cbz ") + GPReg(sf, rt) + ", " +
               HexU64(pc + static_cast<uint64_t>(off));
    }
    // TBZ / TBNZ
    if ((insn & 0x7F000000) == 0x36000000 || (insn & 0x7F000000) == 0x37000000)
    {
        const bool n = ((insn & 0x7F000000) == 0x37000000);
        const int rt = insn & 0x1F;
        const int bit = ((insn >> 19) & 0x1F) | (((insn >> 31) & 1) << 5);
        const uint32_t imm14 = (insn >> 5) & 0x3FFF;
        const int64_t off = SignExtend(imm14, 14) << 2;
        return std::string(n ? "tbnz " : "tbz ") + GPReg(sf, rt) + ", #" +
               std::to_string(bit) + ", " + HexU64(pc + static_cast<uint64_t>(off));
    }
    // 未识别（SIMD / 浮点 / 加密扩展 / 其他）
    return "???";
}

}  // namespace UmtMcp
