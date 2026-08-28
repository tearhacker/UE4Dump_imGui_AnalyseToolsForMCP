#pragma once

#include "../UEGameProfile.hpp"
using namespace UEMemory;

class Lineage2Profile : public IGameProfile
{
public:
    Lineage2Profile() = default;

    bool ArchSupprted() const override
    {
        auto e_machine = GetUnrealELF().header().e_machine;
        return e_machine == EM_AARCH64 || e_machine == EM_ARM;
    }

    std::string GetAppName() const override
    {
        return "Lineage 2 Revolution";
    }

    std::vector<std::string> GetAppIDs() const override
    {
        return {"com.netmarble.lin2ws"};
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
        return IGameProfile::GetGUObjectArrayPtr();
    }
    uintptr_t GetNamesPtr() const override
    {
        return IGameProfile::GetNamesPtr();
    }

    UE_Offsets *GetOffsets() const override
    {
        static UE_Offsets offsets = UE_DefaultOffsets::UE4_00_17(isUsingCasePreservingName());

        static bool once = false;
        if (!once)
        {
            once = true;
            offsets.FUObjectItem.Size = (sizeof(void *) + (sizeof(int32_t) * 2));
        }

        return &offsets;
    }
};
