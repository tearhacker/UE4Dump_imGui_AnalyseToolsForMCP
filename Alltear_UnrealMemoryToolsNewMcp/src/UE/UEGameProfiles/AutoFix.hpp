#pragma once

#include "../UEGameProfile.hpp"
using namespace UEMemory;

// AutoFixProfile: 当用户没有用 -p 指定包名,而进程里又匹配不上任何专用 profile 时,
// 走这个通用的 auto fallback。
//
// 设计:
//   1. ELF 自动探测 libUE4.so / libUnreal.so (基类 GetUnrealELF 已经支持)
//   2. 通过扫描 ELF rodata 中 "++UE4+Release-X.YY" 字符串自动识别 UE 版本,
//      然后选用 UE_DefaultOffsets::UE4_xx_yy / UE5_xx
//   3. 对 GUObjectArrayPtr / GNamesPtr 走"段反扫 + UClass IsA + 已知名锚点"
//      这套通用算法,而不依赖每个游戏的具体 ADRP+LDR pattern
//   4. 由于 AutoFixProfile 只是骨架,Matrix/Physx/FrameCount 留作可选 (返回 0)
//
// 触发条件: executable.cpp 在所有专用 profile 都没匹配上时,把这个 profile 投出来跑。

class AutoFixProfile : public IGameProfile
{
public:
    AutoFixProfile();
    explicit AutoFixProfile(const std::string &targetPackage);

    bool ArchSupprted() const override;
    std::string GetAppName() const override;
    std::vector<std::string> GetAppIDs() const override;

    bool isUsingCasePreservingName() const override { return _casePreserving; }
    bool IsUsingFNamePool() const override { return _useFNamePool; }
    bool isUsingOutlineNumberName() const override { return _outlineNumber; }

    UE_Offsets *GetOffsets() const override;

protected:
    uintptr_t GetGUObjectArrayPtr() const override;
    uintptr_t GetMatrix() const override;
    uintptr_t GetPhysx() const override;
    uintptr_t GetFrameCount() const override;
    uintptr_t GetNamesPtr() const override;

private:
    enum class UEFamily : uint8_t
    {
        Unknown = 0,
        UE4_00_17,
        UE4_18_19,
        UE4_20,
        UE4_21,
        UE4_22,
        UE4_23_24,
        UE4_25_27,
        UE5_00_02,
        UE5_03,
    };

    void DetectVersion() const;
    void EnsureOffsetsInited() const;
    UEFamily ScanVersionString() const;
    UEFamily GuessMajorFromSoName() const;

private:
    std::string _packageHint;

    mutable bool _versionDetected = false;
    mutable UEFamily _family = UEFamily::Unknown;
    mutable std::string _versionStr;

    mutable bool _useFNamePool = true;
    mutable bool _casePreserving = false;
    mutable bool _outlineNumber = false;

    mutable UE_Offsets _offsets;
    mutable bool _offsetsInited = false;
};
