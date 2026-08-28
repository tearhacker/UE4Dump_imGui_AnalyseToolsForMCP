#include "VirtualFunctionResolver.hpp"

#include <array>
#include <cstring>
#include <unordered_map>

#include "Arm64Decode.hpp"
#include "../UE/UEMemory.hpp"
#include "../UE/UEWrappers.hpp"

using namespace UEMemory;
using namespace AutoFixArm64;

namespace AutoFixVTable
{
    namespace
    {
        enum class RegKind : uint8_t
        {
            Unknown,
            ThisPtr,
            VTable,
            VTableSlotAddress,
            VirtualFunction
        };

        struct RegState
        {
            RegKind Kind = RegKind::Unknown;
            uintptr_t Offset = 0;
        };

        using RegisterState = std::array<RegState, 32>;

        bool IsReasonableVTableOffset(uintptr_t offset)
        {
            return offset != 0 && (offset % sizeof(uintptr_t)) == 0 && offset < 0x40000;
        }

        void ClearCallerSaved(RegisterState &regs)
        {
            for (int i = 0; i <= 18; ++i)
                regs[i] = {};
        }

        bool ReadInsn(uintptr_t pc, uint32_t *out)
        {
            return vm_rpm_ptr((const void *)pc, out, sizeof(uint32_t));
        }

        uintptr_t GetVftOffset()
        {
            // VTable pointer is at offset 0 in C++ object layout.
            return 0;
        }
    }  // namespace

    uintptr_t FindVTableCallOffset(uintptr_t functionAddress)
    {
        static std::unordered_map<uintptr_t, uintptr_t> cache;
        static uint32_t cachedGeneration = 0;
        const uint32_t generation = UEWrappers::GetInitGeneration();
        if (cachedGeneration != generation)
        {
            cache.clear();
            cachedGeneration = generation;
        }
        auto it = cache.find(functionAddress);
        if (it != cache.end())
            return it->second;

        if (!functionAddress)
        {
            cache.emplace(functionAddress, 0);
            return 0;
        }

        const uintptr_t vftOff = GetVftOffset();
        (void)vftOff;

        RegisterState regs{};
        regs[0] = {RegKind::ThisPtr, 0};

        uintptr_t resolved = 0;
        for (size_t i = 0; i < 192; ++i)
        {
            const uintptr_t pc = functionAddress + i * sizeof(uint32_t);
            uint32_t insn = 0;
            if (!ReadInsn(pc, &insn) || insn == 0)
                break;

            // BLR / BR Xn -> check if it points to a tracked virtual function
            if (IsBLR(insn) || IsBR(insn))
            {
                int reg = IsBLR(insn) ? GetBLR_Reg(insn) : GetBR_Reg(insn);
                if (reg >= 0 && reg < 32 &&
                    regs[reg].Kind == RegKind::VirtualFunction &&
                    IsReasonableVTableOffset(regs[reg].Offset))
                {
                    resolved = regs[reg].Offset;
                    break;
                }
                if (IsBLR(insn))
                    ClearCallerSaved(regs);
                continue;
            }

            if (IsBL(insn))
            {
                ClearCallerSaved(regs);
                continue;
            }

            if (IsRET(insn))
                break;

            // MOV Xd, Xm
            int rd = -1, rm = -1;
            if (DecodeMovReg(insn, &rd, &rm))
            {
                if (rd >= 0 && rd < 32 && rm >= 0 && rm < 32 && rm != 31 && rd != 31)
                    regs[rd] = regs[rm];
                continue;
            }

            // ADD Rd, Rn, #imm
            int ad_rd = -1, ad_rn = -1; uint32_t ad_imm = 0;
            if (DecodeAddImm(insn, &ad_rd, &ad_rn, &ad_imm))
            {
                if (ad_rd >= 0 && ad_rd < 32 && ad_rn >= 0 && ad_rn < 32)
                {
                    const RegState base = regs[ad_rn];
                    if (base.Kind == RegKind::VTable || base.Kind == RegKind::VTableSlotAddress)
                        regs[ad_rd] = {RegKind::VTableSlotAddress, base.Offset + ad_imm};
                    else if (base.Kind == RegKind::ThisPtr && ad_imm == 0)
                        regs[ad_rd] = base;
                    else
                        regs[ad_rd] = {};
                }
                continue;
            }

            // LDR Rt, [Rn, #imm12]
            int ld_rn = -1, ld_rt = -1; uint32_t ld_imm = 0, ld_scale = 8;
            if (DecodeLDR_Imm(insn, &ld_rn, &ld_rt, &ld_imm, &ld_scale))
            {
                if (ld_rt >= 0 && ld_rt < 32 && ld_rn >= 0 && ld_rn < 32)
                {
                    const uintptr_t off = (uintptr_t)ld_imm * ld_scale;
                    const RegState base = regs[ld_rn];
                    if (base.Kind == RegKind::ThisPtr && off == vftOff)
                    {
                        regs[ld_rt] = {RegKind::VTable, 0};
                    }
                    else if (base.Kind == RegKind::VTable)
                    {
                        regs[ld_rt] = {RegKind::VirtualFunction, off};
                    }
                    else if (base.Kind == RegKind::VTableSlotAddress)
                    {
                        regs[ld_rt] = {RegKind::VirtualFunction, base.Offset + off};
                    }
                    else
                    {
                        regs[ld_rt] = {};
                    }
                }
                continue;
            }
        }

        cache.emplace(functionAddress, resolved);
        return resolved;
    }

    uintptr_t FindDirectBranchCallTarget(uintptr_t functionAddress)
    {
        static std::unordered_map<uintptr_t, uintptr_t> cache;
        static uint32_t cachedGeneration = 0;
        const uint32_t generation = UEWrappers::GetInitGeneration();
        if (cachedGeneration != generation)
        {
            cache.clear();
            cachedGeneration = generation;
        }
        auto it = cache.find(functionAddress);
        if (it != cache.end())
            return it->second;

        if (!functionAddress)
        {
            cache.emplace(functionAddress, 0);
            return 0;
        }

        uintptr_t bestTarget = 0;
        for (size_t i = 0; i < 192; ++i)
        {
            const uintptr_t pc = functionAddress + i * sizeof(uint32_t);
            uint32_t insn = 0;
            if (!ReadInsn(pc, &insn) || insn == 0)
                break;

            if (IsBL(insn) || IsB(insn))
            {
                int64_t off = DecodeBranchOffset(insn);
                uintptr_t target = (uintptr_t)((int64_t)pc + off);
                bool internalBranch =
                    target >= functionAddress && target < functionAddress + 0x400;
                if (!internalBranch && target != functionAddress &&
                    target >= 0x10000 && (target % sizeof(uint32_t)) == 0)
                {
                    bestTarget = target;
                    if (IsBL(insn))
                        break;  // BL is more reliable
                }
            }

            if (IsRET(insn))
                break;
        }

        cache.emplace(functionAddress, bestTarget);
        return bestTarget;
    }

    uintptr_t ResolveVTableFunction(uintptr_t objectAddress, uintptr_t vtableOffset)
    {
        if (!objectAddress || !IsReasonableVTableOffset(vtableOffset))
            return 0;

        const uintptr_t vftOff = GetVftOffset();
        const uintptr_t vtableAddress = vm_rpm_ptr<uintptr_t>((const void *)(objectAddress + vftOff));
        if (!UEMemory::kPtrValidator.isPtrReadable(vtableAddress + vtableOffset))
            return 0;

        return vm_rpm_ptr<uintptr_t>((const void *)(vtableAddress + vtableOffset));
    }
}  // namespace AutoFixVTable
