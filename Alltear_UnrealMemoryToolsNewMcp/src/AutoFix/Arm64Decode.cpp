#include "Arm64Decode.hpp"

namespace AutoFixArm64
{
    uint64_t DecodeADRP(uint64_t pc, uint32_t insn)
    {
        uint64_t immhi = (insn >> 5) & 0x7FFFF;
        uint64_t immlo = (insn >> 29) & 0x3;
        uint64_t imm = (immhi << 2) | immlo;
        if (imm & (1ULL << 20))
            imm |= ~((1ULL << 21) - 1);
        int64_t offset = static_cast<int64_t>(imm) << 12;
        uint64_t base = pc & ~0xFFFULL;
        return base + offset;
    }

    uint64_t DecodeADD(uint32_t insn)
    {
        uint32_t imm12 = (insn >> 10) & 0xFFF;
        uint32_t shift = (insn >> 22) & 0x1;
        return static_cast<uint64_t>(imm12) << (shift ? 12 : 0);
    }

    bool IsADRP(uint32_t insn) { return (insn & 0x9F000000) == 0x90000000; }

    bool IsADD(uint32_t insn) { return (insn & 0x7F800000) == 0x11000000; }

    bool IsAddImm(uint32_t insn)
    {
        return (insn & 0x7F800000) == 0x11000000;
    }

    bool IsLDR_Imm(uint32_t insn)
    {
        // LDR (immediate, unsigned offset), 32-bit/64-bit
        return ((insn & 0xFFC00000) == 0xB9400000) ||  // 32-bit LDR
               ((insn & 0xFFC00000) == 0xF9400000);    // 64-bit LDR
    }

    bool IsBL(uint32_t insn) { return (insn & 0xFC000000) == 0x94000000; }
    bool IsB(uint32_t insn) { return (insn & 0xFC000000) == 0x14000000; }
    bool IsBLR(uint32_t insn) { return (insn & 0xFFFFFC1F) == 0xD63F0000; }
    bool IsBR(uint32_t insn) { return (insn & 0xFFFFFC1F) == 0xD61F0000; }
    bool IsRET(uint32_t insn) { return (insn & 0xFFFFFC1F) == 0xD65F0000; }

    bool IsMov_Reg(uint32_t insn)
    {
        // ORR (shifted register) with Rn = XZR (0b11111)
        // sf=1, opc=01, 01010, shift=00, N=0, Rm, imm6=000000, Rn=11111, Rd
        return (insn & 0x7F2003E0) == 0x2A0003E0;
    }

    bool IsMovz(uint32_t insn)
    {
        // MOVZ
        return (insn & 0x7F800000) == 0x52800000;
    }

    bool DecodeLDR_Imm(uint32_t insn,
                       int *outRn,
                       int *outRt,
                       uint32_t *outImm12,
                       uint32_t *outScale)
    {
        if (!IsLDR_Imm(insn))
            return false;
        if (outRt) *outRt = (int)(insn & 0x1F);
        if (outRn) *outRn = (int)((insn >> 5) & 0x1F);
        uint32_t imm12 = (insn >> 10) & 0xFFF;
        if (outImm12) *outImm12 = imm12;
        // size field: 10 = 32-bit (scale 4), 11 = 64-bit (scale 8)
        uint32_t size = (insn >> 30) & 0x3;
        if (outScale) *outScale = (1u << size);
        return true;
    }

    bool DecodeMovReg(uint32_t insn, int *outRd, int *outRm)
    {
        if (!IsMov_Reg(insn))
            return false;
        if (outRd) *outRd = (int)(insn & 0x1F);
        if (outRm) *outRm = (int)((insn >> 16) & 0x1F);
        return true;
    }

    bool DecodeAddImm(uint32_t insn, int *outRd, int *outRn, uint32_t *outImm)
    {
        if (!IsAddImm(insn))
            return false;
        if (outRd) *outRd = (int)(insn & 0x1F);
        if (outRn) *outRn = (int)((insn >> 5) & 0x1F);
        uint32_t imm12 = (insn >> 10) & 0xFFF;
        uint32_t shift = (insn >> 22) & 0x1;
        if (outImm) *outImm = (imm12 << (shift ? 12 : 0));
        return true;
    }

    int GetBLR_Reg(uint32_t insn)
    {
        if (!IsBLR(insn)) return -1;
        return (int)((insn >> 5) & 0x1F);
    }

    int GetBR_Reg(uint32_t insn)
    {
        if (!IsBR(insn)) return -1;
        return (int)((insn >> 5) & 0x1F);
    }

    int64_t DecodeBranchOffset(uint32_t insn)
    {
        int64_t imm26 = insn & 0x03FFFFFF;
        if (imm26 & 0x02000000)
            imm26 |= ~0x03FFFFFFLL;
        return imm26 << 2;
    }
}
