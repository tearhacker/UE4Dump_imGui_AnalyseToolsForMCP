#pragma once

#include <cstdint>

namespace AutoFixArm64
{
    uint64_t DecodeADRP(uint64_t pc, uint32_t insn);
    uint64_t DecodeADD(uint32_t insn);

    bool IsADRP(uint32_t insn);
    bool IsADD(uint32_t insn);
    bool IsLDR_Imm(uint32_t insn);
    bool IsBL(uint32_t insn);
    bool IsB(uint32_t insn);
    bool IsBLR(uint32_t insn);
    bool IsBR(uint32_t insn);
    bool IsRET(uint32_t insn);
    bool IsMov_Reg(uint32_t insn);
    bool IsAddImm(uint32_t insn);
    bool IsMovz(uint32_t insn);

    bool DecodeLDR_Imm(uint32_t insn,
                       int *outRn,
                       int *outRt,
                       uint32_t *outImm12,
                       uint32_t *outScale);

    bool DecodeMovReg(uint32_t insn, int *outRd, int *outRm);
    bool DecodeAddImm(uint32_t insn, int *outRd, int *outRn, uint32_t *outImm);

    int GetBLR_Reg(uint32_t insn);
    int GetBR_Reg(uint32_t insn);

    int64_t DecodeBranchOffset(uint32_t insn);
}
