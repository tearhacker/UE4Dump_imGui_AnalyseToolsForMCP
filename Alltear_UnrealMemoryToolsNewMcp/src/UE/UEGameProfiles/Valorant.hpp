#pragma once

#include "../UEGameProfile.hpp"
using namespace UEMemory;

class ValorantProfile : public IGameProfile
{
public:
    ValorantProfile() = default;

    bool ArchSupprted() const override
    {
        auto e_machine = GetUnrealELF().header().e_machine;
        // only arm64
        return e_machine == EM_AARCH64;
    }

    std::string GetAppName() const override
    {
        return "Valorant(CN)";
    }

    std::vector<std::string> GetAppIDs() const override
    {
        return {"com.tencent.tmgp.codev"};
    }

    bool isUsingCasePreservingName() const override
    {
        return false;
    }

    bool IsUsingFNamePool() const override
    {
        return true;
    }

    bool isUsingOutlineNumberName() const override
    {
        return false;
    }
    uintptr_t GetGUObjectArrayPtr() const override
    {
        std::vector<std::pair<std::string, int>> idaPatterns = {
            {"0B ? 40 B9 7F 01 0A 6B AD ? 00 54 08 ? 40 F9", -8},
            {"2B ? 40 B9 7F 01 0A 6B CD ? 00 54 29 ? 40 F9", -8},
        };

        PATTERN_MAP_TYPE map_type = isEmulator() ? PATTERN_MAP_TYPE::ANY_R : PATTERN_MAP_TYPE::ANY_X;

        for (const auto &it : idaPatterns)
        {
            std::string ida_pattern = it.first;
            const int step = it.second;

            uintptr_t adrl = Arm64::Decode_ADRP_LDR(findIdaPattern(map_type, ida_pattern, step));

            if (adrl != 0) return vm_rpm_ptr<uintptr_t>((void *)adrl);
        }

        return 0;
    }
    uintptr_t GetFrameCount() const override
    {
        PATTERN_MAP_TYPE map_type = isEmulator() ? PATTERN_MAP_TYPE::ANY_R : PATTERN_MAP_TYPE::ANY_X;
        std::string ida_pattern = "? ? ? ? ? ? ? F9 ? ? ? F9 C0 03 5F D6 ? ? ? A9 FD 03 00 91";
        const int step = 0;
        auto FrameOff = Arm64::Decode_ADRP_LDR(findIdaPattern(map_type, ida_pattern, step));
        if (FrameOff !=0)
            return vm_rpm_ptr<uintptr_t>((void*)FrameOff);
        return  0;
    }
    uintptr_t GetMatrix() const override
    {
        std::vector<std::pair<std::string, int>> idaPatterns = {
            {"08 3D 40 F9 00 01 3F D6 E8 03 13 AA ? ? ? F9", 0x18},
            {"FD ? ? A9 28 ? ? F9 F3 ? ? F8 C0 03 5F D6", -0x1c},
            {"00 01 3F D6 E8 03 13 AA 60 ? 00 F9 ? ? ? A9", 14},
        };

        PATTERN_MAP_TYPE map_type = isEmulator() ? PATTERN_MAP_TYPE::ANY_R : PATTERN_MAP_TYPE::ANY_X;

        for (const auto &it : idaPatterns)
        {
            std::string ida_pattern = it.first;
            const int step = it.second;

            uintptr_t adrl = Arm64::Decode_ADRP_LDR(findIdaPattern(map_type, ida_pattern, step));
            if (adrl != 0) return adrl;
        }
        return 0;
    }
    uintptr_t GetPhysx() const override
    {
        std::vector<std::pair<std::string, int>> idaPatterns = {
            {"E1 ? ? ? 40 00 40 BD F4 03 02 AA",0x28},
            {"48 ? ? ? F3 03 04 AA F5 03 03 2A", -0xc},
        };

        PATTERN_MAP_TYPE map_type = isEmulator() ? PATTERN_MAP_TYPE::ANY_R : PATTERN_MAP_TYPE::ANY_X;

        for (const auto &it : idaPatterns)
        {
            std::string ida_pattern = it.first;
            const int step = it.second;

            uintptr_t adrl = Arm64::Decode_ADRP_LDR(findIdaPattern(map_type, ida_pattern, step),8);
            //printf("%lx\n", vm_rpm_ptr<uintptr_t>((void*)adrl));
            if (adrl != 0) return vm_rpm_ptr<uintptr_t>((void*)adrl);
        }
        return 0;
    }
    void print_func() const
    {
        auto base = GetUEVars()->GetBaseAddress();
        //printf("0x%lx\n", base);
        std::string GetAccessoriesAllRecoilFactorModifier = "? ? ? 34 ? ? ? F9 ? ? ? 91 ? ? ? D3 ? ? ? B8 ? ? ? 35 ? ? ? F1 ? ? ? 91 ? ? ? 54 ? ? ? B9 ? ? ? 34 ? ? ? F8 ? ? ? B4 ? ? ? 96 ? ? ? 52 E0 03 13 AA";
        PATTERN_MAP_TYPE map_type = isEmulator() ? PATTERN_MAP_TYPE::ANY_R : PATTERN_MAP_TYPE::ANY_X;
        printf("GetAccessoriesAllRecoilFactorModifierOffset:0x%lX\n", findIdaPattern(map_type, GetAccessoriesAllRecoilFactorModifier, 0) - base);

        std::string WallHackAddress = "? ? ? 35 ? ? ? 39 ? ? ? 34 F4 03 00 2A";
        PATTERN_MAP_TYPE map_type1 = isEmulator() ? PATTERN_MAP_TYPE::ANY_R : PATTERN_MAP_TYPE::ANY_X;
        printf("WallHackAddressOffset:0x%lX\n", findIdaPattern(map_type1, WallHackAddress, 0) - base);
        std::string CalShootTargetLocation = "? ? ? 2D ? ? ? FD ? ? ? BD ? ? ? 0D";
        PATTERN_MAP_TYPE map_type2 = isEmulator() ? PATTERN_MAP_TYPE::ANY_R : PATTERN_MAP_TYPE::ANY_X;
        printf("CalShootTargetLocationOffset:0x%lX\n", findIdaPattern(map_type2, CalShootTargetLocation, 0) - base);
    }
    uintptr_t GetNamesPtr() const override
    {
        print_func();
        PATTERN_MAP_TYPE map_type = isEmulator() ? PATTERN_MAP_TYPE::ANY_R : PATTERN_MAP_TYPE::ANY_X;
        auto base = GetUEVars()->GetBaseAddress();
        std::string ida_pattern = "F3 03 01 2A F4 03 00 AA E8 ? 00 36";
        const int step = 0x18;

        /*GName解密*/
        int indices[8] = {};
        auto byte_array = Arm64::Decode_ADRP_ADD(findIdaPattern(map_type, ida_pattern, step));
        printf("byte_array:0x%lx\n", byte_array - base);
        vm_rpm_ptr((void *)(byte_array + 0x80), indices, sizeof(int) * 8);
        uint64_t GName =
            (uint64_t)vm_rpm_ptr<uint8_t>((void *)(byte_array + indices[0])) |
            (uint64_t)vm_rpm_ptr<uint8_t>((void *)(byte_array + indices[1])) << 8 |
            (uint64_t)vm_rpm_ptr<uint8_t>((void *)(byte_array + indices[2])) << 16 |
            (uint64_t)vm_rpm_ptr<uint8_t>((void *)(byte_array + indices[3])) << 24 |
            (uint64_t)vm_rpm_ptr<uint8_t>((void *)(byte_array + indices[4])) << 32 |
            (uint64_t)vm_rpm_ptr<uint8_t>((void *)(byte_array + indices[5])) << 40 |
            (uint64_t)vm_rpm_ptr<uint8_t>((void *)(byte_array + indices[6])) << 48 |
            (uint64_t)vm_rpm_ptr<uint8_t>((void *)(byte_array + indices[7])) << 56;

        return vm_rpm_ptr<uintptr_t>((void *)GName);
    }

    uint8_t *GetNameEntry(int32_t id) const override
    {
        if (id < 0)
            return nullptr;

        uintptr_t namesPtr = _UEVars.GetNamesPtr();
        if (namesPtr == 0)
            return nullptr;

        if (!IsUsingFNamePool())
        {
            static uintptr_t gNames = 0;
            if (gNames == 0)
            {
                gNames = vm_rpm_ptr<uintptr_t>((void *)namesPtr);
            }

            const int32_t ElementsPerChunk = 16384;
            const int32_t ChunkIndex = id / ElementsPerChunk;
            const int32_t WithinChunkIndex = id % ElementsPerChunk;

            // FNameEntry**
            uint8_t *FNameEntryArray = vm_rpm_ptr<uint8_t *>((void *)(gNames + ChunkIndex * sizeof(uintptr_t)));
            if (!FNameEntryArray)
                return nullptr;

            // FNameEntry*
            return vm_rpm_ptr<uint8_t *>(FNameEntryArray + WithinChunkIndex * sizeof(uintptr_t));
        }

        uintptr_t blockBit = GetOffsets()->FNamePool.BlocksBit;
        uintptr_t blocks = GetOffsets()->FNamePool.BlocksOff;
        uintptr_t chunckMask = (1 << blockBit) - 1;
        uintptr_t stride = GetOffsets()->FNamePool.Stride;

        // 计算块索引和块内偏移
        uintptr_t blockIndex = (id >> blockBit);
        uintptr_t block_offset = blockIndex * sizeof(void *);
        uintptr_t chunck_offset = (id & chunckMask) * stride;

        uint8_t *chunck = vm_rpm_ptr<uint8_t *>((void *)(namesPtr + blocks + block_offset));
        if (!chunck)
            return nullptr;
        if (blockIndex == 0)
        {
            // 读取解密密钥 (FNamePool + 65600)
            uint64_t shift = vm_rpm_ptr<uint64_t>((void *)(namesPtr + 65600));

            // 将块指针转换为整数以便进行异或操�?
            uintptr_t chunkInt = reinterpret_cast<uintptr_t>(chunck);

            // 应用异或解密
            chunkInt ^= shift;

            // 转换回指�?
            chunck = reinterpret_cast<uint8_t *>(chunkInt);
        }
        return (chunck + chunck_offset);
    }
    UE_Offsets *GetOffsets() const override
    {
        static UE_Offsets offsets = UE_DefaultOffsets::UE4_25_27(isUsingCasePreservingName());

        static bool once = false;
        if (!once)
        {
            once = true;
            offsets.FNamePool.BlocksOff = 0x40;
            offsets.UFunction.Func = 0xe0;
        }

        return &offsets;
    }
};
