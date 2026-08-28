#pragma once

#include "../UEGameProfile.hpp"
using namespace UEMemory;

class PUBGProfile : public IGameProfile
{
public:
    PUBGProfile() = default;

    bool ArchSupprted() const override
    {
        auto e_machine = GetUnrealELF().header().e_machine;
        return e_machine == EM_AARCH64;
    }

    std::string GetAppName() const override
    {
        return "PUBG";
    }

    std::vector<std::string> GetAppIDs() const override
    {
        return {
            "com.tencent.ig",
            "com.rekoo.pubgm",
            "com.pubg.imobile",
            "com.pubg.krmobile",
            "com.vng.pubgmobile",
        };
    }

    bool isUsingCasePreservingName() const override
    {
        return false;
    }

    bool IsUsingFNamePool() const override
    {
        return false;
    }

    bool isUsingOutlineNumberName() const override
    {
        return false;
    }

    uintptr_t GetGUObjectArrayPtr() const override
    {
        std::string ida_pattern = "12 40 B9 ? 3E 40 B9 ? ? ? 6B ? ? ? 54 ? ? ? ? ? ? ? 91";
        const int step = 0xf;

        PATTERN_MAP_TYPE map_type = isEmulator() ? PATTERN_MAP_TYPE::ANY_R : PATTERN_MAP_TYPE::ANY_X;

        return Arm64::DecodeADRL(findIdaPattern(map_type, ida_pattern, step));
    }

    uintptr_t GetNamesPtr() const override
    {
        std::string ida_pattern =  "81 80 52 ? ? ? ? ? 81 80 52 ? 03 1F 2A";
        const int step = 0x1f;

        PATTERN_MAP_TYPE map_type = isEmulator() ? PATTERN_MAP_TYPE::ANY_R : PATTERN_MAP_TYPE::ANY_X;

        uintptr_t param_1 = Arm64::DecodeADRL(findIdaPattern(map_type, ida_pattern, step));
        if (param_1 == 0)
            return 0;

        int64_t var_2;
        int64_t var_5[16];

        var_2 = (vm_rpm_ptr<int32_t>((void *)(param_1)) - 100) / 3u;
        var_5[(uint32_t)(var_2 - 1)] = vm_rpm_ptr<int64_t>((void *)(param_1 + 8));

        while (var_2 - 2 >= 0)
        {
            var_5[(uint32_t)(var_2 - 2)] = vm_rpm_ptr<int64_t>((void *)(var_5[var_2 - 1]));
            --var_2;
        }

        return var_5[0];
    }

    UE_Offsets *GetOffsets() const override
    {
        static UE_Offsets offsets = UE_DefaultOffsets::UE4_18_19(isUsingCasePreservingName());

        static bool once = false;
        if (!once)
        {
            once = true;
            offsets.FNameEntry.Index = sizeof(void *);
            offsets.FNameEntry.Name = sizeof(void *) + sizeof(int32_t);
        }

        return &offsets;
    }
};