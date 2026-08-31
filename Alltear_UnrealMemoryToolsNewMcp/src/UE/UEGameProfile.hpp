#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "../Utils/Logger.hpp"

#include "UEMemory.hpp"
#include "UEOffsets.hpp"

enum class PATTERN_MAP_TYPE : int8_t
{
    ANY_R,  // Search in any private readable map

    ANY_X,  // Search in any private readable & executable map

    ANY_W,  // Search in any private readable & writeable map

    BSS,  // Search in .bss maps
};

struct UEAddressOverrides
{
    uintptr_t namesPtr = 0;
    uintptr_t guObjectArrayPtr = 0;
    bool hasNamesPtr = false;
    bool hasGUObjectArrayPtr = false;
    bool hasNameLayout = false;
    bool hasObjectLayout = false;

    uintptr_t nameStride = 0;
    uintptr_t nameBlocksBit = 0;
    uintptr_t nameBlocksOff = 0;
    uintptr_t nameHeaderOff = 0;
    uintptr_t nameLengthShift = 0;
    uintptr_t objObjectsOff = 0;
    uintptr_t objectsOff = 0;
    uintptr_t numElementsOff = 0;
    uintptr_t numElementsPerChunk = 0;
    uintptr_t itemObjectOff = 0;
    uintptr_t itemSize = 0;
    uintptr_t classPrivateOff = 0;
    uintptr_t namePrivateOff = 0;
    uintptr_t outerPrivateOff = 0;
};

class IGameProfile
{
public:
protected:
    UEVars _UEVars;
    UEAddressOverrides _addressOverrides;
    UE_Offsets _baseOffsetsBackup;
    bool _hasOffsetsBackup = false;

public:
    virtual ~IGameProfile() = default;

    UEVarsInitStatus InitUEVars();
    const UEVars *GetUEVars() const { return &_UEVars; }
    void SetAddressOverrides(const UEAddressOverrides &overrides);

    virtual ElfScanner GetUnrealELF() const;

    // arch support check
    virtual bool ArchSupprted() const = 0;

    virtual std::string GetAppName() const = 0;

    virtual std::vector<std::string> GetAppIDs() const = 0;

    virtual bool isUsingCasePreservingName() const = 0;

    virtual bool IsUsingFNamePool() const = 0;

    virtual bool isUsingOutlineNumberName() const = 0;

    virtual UE_Offsets *GetOffsets() const = 0;

protected:
    virtual uintptr_t GetGUObjectArrayPtr() const;
    virtual uintptr_t GetMatrix()  const = 0;
    virtual uintptr_t GetPhysx() const = 0;
    virtual uintptr_t GetFrameCount() const = 0;
    virtual uintptr_t GetStaticFindObject() const;
    virtual uintptr_t GetNativeAndroidApp() const;
    virtual uintptr_t GetProcessEvent() const;
    // GNames / NamePoolData
    virtual uintptr_t GetNamesPtr() const;

    virtual uint8_t *GetNameEntry(int32_t id) const;
    // can override if decryption is needed
    virtual std::string GetNameEntryString(uint8_t *entry) const;
    virtual std::string GetNameByID(int32_t id) const;

    virtual bool isEmulator() const;

    virtual uintptr_t findIdaPattern(PATTERN_MAP_TYPE map_type,
                                     const std::string &pattern, const int step,
                                     uint32_t skip_result = 0) const;


};
