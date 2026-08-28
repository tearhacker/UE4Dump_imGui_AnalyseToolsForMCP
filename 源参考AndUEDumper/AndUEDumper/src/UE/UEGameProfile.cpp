#include "UEGameProfile.hpp"

#include "UEMemory.hpp"
#include "UEWrappers.hpp"

using namespace UEMemory;

UEVarsInitStatus IGameProfile::InitUEVars()
{
    bool is32Bit = KittyMemoryEx::getMaps(kMgr.processID(), EProcMapFilter::EndWith, "/linker64").empty();
    if (is32Bit)
    {
        if (sizeof(void *) != 4)
        {
            LOGE("Dumper is 64bit while target process is 32bit. Please use the correct architecture.");
            return UEVarsInitStatus::ERROR_ARCH_MISMATCH;
        }
    }
    else
    {
        if (sizeof(void *) != 8)
        {
            LOGE("Dumper is 32bit while target process is 64bit. Please use the correct architecture.");
            return UEVarsInitStatus::ERROR_ARCH_MISMATCH;
        }
    }

    auto ue_elf = GetUnrealELF();
    if (!ue_elf.isValid())
    {
        LOGE("Couldn't find a valid UE ELF in target process maps.");
        return UEVarsInitStatus::ERROR_LIB_NOT_FOUND;
    }

    if (!ArchSupprted())
    {
        if (GetUnrealELF().header().e_machine > 0 && !ue_elf.isFixedBySoInfo())
        {
            LOGE("Architecture ( 0x%x ) is not supported for this game.", ue_elf.header().e_machine);
            return UEVarsInitStatus::ARCH_NOT_SUPPORTED;
        }
        else
        {
            LOGW("UE ELF Header might have been removed or modified!");
        }
    }

    LOGI("Library: %s", ue_elf.realPath().c_str());
    LOGI("BaseAddress: %p", (void *)ue_elf.base());
    LOGI("==========================");

    kPtrValidator.setPID(kMgr.processID());
    kPtrValidator.setUseCache(true);
    kPtrValidator.refreshRegionCache();
    if (kPtrValidator.cachedRegions().empty())
        return UEVarsInitStatus::ERROR_INIT_PTR_VALIDATOR;

    _UEVars.BaseAddress = ue_elf.base();

    UE_Offsets *pOffsets = GetOffsets();
    if (!pOffsets)
        return UEVarsInitStatus::ERROR_INIT_OFFSETS;

    _UEVars.Offsets = pOffsets;

    _UEVars.NamesPtr = GetNamesPtr();
    if (IsUsingFNamePool())
    {
        if (!kPtrValidator.isPtrReadable(_UEVars.NamesPtr))
            return UEVarsInitStatus::ERROR_INIT_NAMEPOOL;
    }
    else
    {
        if (!kPtrValidator.isPtrReadable(_UEVars.NamesPtr))
            return UEVarsInitStatus::ERROR_INIT_GNAMES;
    }

    _UEVars.pGetNameByID = [this](int32_t id) -> std::string
    {
        return GetNameByID(id);
    };

    _UEVars.GUObjectsArrayPtr = GetGUObjectArrayPtr();
    if (!kPtrValidator.isPtrReadable(_UEVars.GUObjectsArrayPtr))
        return UEVarsInitStatus::ERROR_INIT_GUOBJECTARRAY;

    _UEVars.ObjObjectsPtr = _UEVars.GUObjectsArrayPtr + pOffsets->FUObjectArray.ObjObjects;

    if (!vm_rpm_ptr((void *)(_UEVars.ObjObjectsPtr + pOffsets->TUObjectArray.Objects),
                    &_UEVars.ObjObjects_Objects, sizeof(uintptr_t)))
        return UEVarsInitStatus::ERROR_INIT_OBJOBJECTS;

    UEWrappers::Init(GetUEVars());

    return UEVarsInitStatus::SUCCESS;
}

uint8_t *IGameProfile::GetNameEntry(int32_t id) const
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

    uintptr_t block_offset = ((id >> blockBit) * sizeof(void *));
    uintptr_t chunck_offset = ((id & chunckMask) * stride);

    uint8_t *chunck = vm_rpm_ptr<uint8_t *>((void *)(namesPtr + blocks + block_offset));
    if (!chunck)
        return nullptr;

    return (chunck + chunck_offset);
}

std::string IGameProfile::GetNameEntryString(uint8_t *entry) const
{
    if (!entry)
        return "";

    UE_Offsets *offsets = GetOffsets();

    uint8_t *pStr = nullptr;
    // don't care for now
    // bool isWide = false;
    size_t strLen = 0;
    int strNumber = 0;

    if (!IsUsingFNamePool())
    {
        int32_t name_index = 0;
        if (!vm_rpm_ptr(entry + offsets->FNameEntry.Index, &name_index,
                        sizeof(int32_t)))
            return "";

        pStr = entry + offsets->FNameEntry.Name;
        // isWide = offsets->FNameEntry.GetIsWide(name_index)
        strLen = kMAX_UENAME_BUFFER;
    }
    else
    {
        uint16_t header = 0;
        if (!vm_rpm_ptr(entry + offsets->FNamePoolEntry.Header, &header,
                        sizeof(int16_t)))
            return "";

        if (isUsingOutlineNumberName() &&
            offsets->FNamePoolEntry.GetLength(header) == 0)
        {
            const uintptr_t stringOff =
                offsets->FNamePoolEntry.Header + sizeof(int16_t);
            const uintptr_t entryIdOff = stringOff + ((stringOff == 6) * 2);
            const int32_t nextEntryId = vm_rpm_ptr<int32_t>(entry + entryIdOff);
            if (nextEntryId <= 0)
                return "";

            strNumber = vm_rpm_ptr<int32_t>(entry + entryIdOff + sizeof(int32_t));
            entry = GetNameEntry(nextEntryId);
            if (!vm_rpm_ptr(entry + offsets->FNamePoolEntry.Header, &header,
                            sizeof(int16_t)))
                return "";
        }

        strLen = std::min<size_t>(offsets->FNamePoolEntry.GetLength(header), kMAX_UENAME_BUFFER);
        if (strLen <= 0)
            return "";

        // isWide = offsets->FNamePoolEntry.GetIsWide(header);
        pStr = entry + offsets->FNamePoolEntry.Header + sizeof(int16_t);
    }

    std::string result = vm_rpm_str(pStr, strLen);

    if (strNumber > 0)
        result += '_' + std::to_string(strNumber - 1);

    return result;
}

std::string IGameProfile::GetNameByID(int32_t id) const
{
    return GetNameEntryString(GetNameEntry(id));
}

std::vector<std::string> IGameProfile::GetUESoNames() const
{
    return {"libUE4.so",
            "libUnreal.so"};
}

ElfScanner IGameProfile::GetUnrealELF() const
{
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    static const std::vector<std::string> cUELibNames = GetUESoNames();

    static ElfScanner ue_elf{};
    if (ue_elf.isValid())
        return ue_elf;

    // find via linker or nativebridge solist
    // some games like farlight remove ELF header from lib
    for (const auto &lib : cUELibNames)
    {
        auto nativeSo = kMgr.linkerScanner.findSoInfo(lib);
        if (nativeSo.ptr)
        {
            ue_elf = kMgr.elfScanner.createWithSoInfo(nativeSo);
            if (ue_elf.isValid())
                return ue_elf;
        }

        auto emulatedSo = kMgr.nbScanner.findSoInfo(lib);
        if (emulatedSo.ptr)
        {
            ue_elf = kMgr.elfScanner.createWithSoInfo(emulatedSo);
            if (ue_elf.isValid())
                return ue_elf;
        }
    }

    // find from /maps
    for (const auto &lib : cUELibNames)
    {
        ue_elf = kMgr.elfScanner.findElf(lib);
        if (ue_elf.isValid())
            return ue_elf;
    }

    return ue_elf;
}

bool IGameProfile::findProcessEvent(uint8_t *uObject, uintptr_t *pe_address_out, int *pe_index_out) const
{
    // for arm64 only for now
#ifdef __LP64__

    auto pe_sym = GetUnrealELF().findSymbol("_ZN7UObject12ProcessEventEP9UFunctionPv");
    auto vft = vm_rpm_ptr<uint8_t *>(uObject);

    std::array<uintptr_t, 100> vft_ptrs;
    vm_rpm_ptr(vft, vft_ptrs.data(), vft_ptrs.size() * sizeof(uintptr_t));

    // ADRP GUObjectArray
    // LDR/LDRSW UObject->Index
    // MOV FUObjectItem->Size
    // LDRB UFunction->Flags+1
    // LDR/LDRSH UStruct->PropertiesSize
    // ADD/LDRSH UFunction->ParamSize
    // LDR UStruct->ChildProperties/UStruct->Children;
    // LDRB UFunction->Flags+2
    // read 0x200 bytes from each virt func

    auto offs = GetOffsets();
    auto objArrayPtr = GetUEVars()->GetGUObjectsArrayPtr();
    auto objObjectsPtr = GetUEVars()->GetObjObjectsPtr();
    int bestScore = 0;
    int bestScoreIdx = -1;

    for (size_t i = 0; i < vft_ptrs.size(); i++)
    {
        int score = 0;
        std::array<bool, 10> oks = {false, false, false, false, false, false, false, false, false, false};

        if (pe_sym != 0 && vft_ptrs[i] == pe_sym)
        {
            bestScore = oks.size();
            bestScoreIdx = i;
            break;
        }

        std::vector<uint32_t> instrs(0x200 / 4, 0);
        vm_rpm_ptr((void *)vft_ptrs[i], instrs.data(), instrs.size() * 4);

        for (size_t j = 0; j < instrs.size(); j++)
        {
            auto insn = KittyArm64::decodeInsn(instrs[j], vft_ptrs[i] + (j * 4));
            if (!insn.isValid())
                continue;

            if (!oks[0] && (insn.type == EKittyInsnTypeArm64::ADRP || insn.type == EKittyInsnTypeArm64::ADR))
            {
                uintptr_t adrp_adr = insn.target;
                for (size_t k = 1; k < 8 && k < instrs.size(); k++)
                {
                    auto insn2 = KittyArm64::decodeInsn(instrs[j + k]);
                    if (insn2.isValid() && insn2.immediate != 0 && insn.rd == insn2.rn)
                    {
                        adrp_adr += insn2.immediate;
                        break;
                    }
                }

                uintptr_t adrp_adr_deref = vm_rpm_ptr<uintptr_t>((void *)adrp_adr);
                if (adrp_adr == objArrayPtr || adrp_adr_deref == objArrayPtr || adrp_adr == objObjectsPtr || adrp_adr_deref == objObjectsPtr )
                    oks[0] = true;
            }

            // LDR
            if (insn.immediate == (int64_t)offs->UObject.InternalIndex)
                oks[1] = true;

            // MOV
            if (insn.immediate == (int64_t)offs->FUObjectItem.Size)
                oks[2] = true;

            // LDRB
            if (insn.immediate == (int64_t)offs->UFunction.EFunctionFlags + 1)
                oks[3] = true;

            // LDR
            if (insn.immediate == (int64_t)offs->UStruct.PropertiesSize)
                oks[4] = true;

            // LDR TWICE FOR MEMSET AND MEMCPY
            if (insn.immediate == (int64_t)offs->UFunction.ParamSize)
                oks[5] = true;

            if (oks[5] && insn.immediate == (int64_t)offs->UFunction.ParamSize)
                oks[6] = true;

            // LDR
            uintptr_t children = offs->UStruct.ChildProperties ? offs->UStruct.ChildProperties : offs->UStruct.Children;
            if (insn.immediate == (int64_t)children)
                oks[7] = true;

            // LDRB
            if (insn.immediate == (int64_t)offs->UFunction.EFunctionFlags + 2)
                oks[8] = true;

            // LDR
            if (oks[7] && insn.immediate == (int64_t)children)
                oks[9] = true;
        }

        for (const auto &ok : oks)
            if (ok) score++;

        if (score > bestScore)
        {
            bestScoreIdx = i;
            bestScore = score;
        }

        /*LOGI("VFT[%zd] (%p) score: %d [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d]", i, (void *)(vft_ptrs[i] - GetUEVars()->GetBaseAddress()), score,
             oks[0], oks[1], oks[2], oks[3], oks[4], oks[5], oks[6], oks[7], oks[8], oks[9]);*/
    }

    if (bestScoreIdx >= 0)
    {
        if (pe_address_out)
            *pe_address_out = vft_ptrs[bestScoreIdx];

        if (pe_index_out)
            *pe_index_out = bestScoreIdx;

        //LOGI("VFT[%d] (%p) has best the score(%d)", bestScoreIdx, (void *)(vft_ptrs[bestScoreIdx] - GetUEVars()->GetBaseAddress()), bestScore);

        return true;
    }

#else
    ((void)uObject);
    ((void)pe_address_out);
    ((void)pe_index_out);
#endif

    return false;
}

bool IGameProfile::isEmulator() const
{
    const auto elf = GetUnrealELF();
    return (elf.isValid() && kMgr.elfScanner.isElfEmulated(elf)) || kMgr.nbScanner.isValid();
}

uintptr_t IGameProfile::findIdaPattern(PATTERN_MAP_TYPE map_type,
                                       const std::string &pattern,
                                       const int step,
                                       uint32_t skip_result) const
{
    ElfScanner ue_elf = GetUnrealELF();
    std::vector<KittyMemoryEx::ProcMap> search_segments;
    bool hasBSS = ue_elf.bssSegments().size() > 0;

    if (map_type == PATTERN_MAP_TYPE::BSS)
    {
        if (!hasBSS)
            return 0;

        for (auto &it : ue_elf.bssSegments())
            search_segments.push_back(it);
    }
    else
    {
        for (auto &it : ue_elf.segments())
        {
            if (!it.readable || !it.is_private)
                continue;

            if (map_type == PATTERN_MAP_TYPE::ANY_X && !it.executable)
                continue;
            else if (map_type == PATTERN_MAP_TYPE::ANY_W && !it.writeable)
                continue;

            search_segments.push_back(it);
        }
    }

    LOGD("search_segments count = %p", (void *)search_segments.size());

    uintptr_t insn_address = 0;

    for (auto &it : search_segments)
    {
        if (skip_result > 0)
        {
            auto adr_list = kMgr.memScanner.findIdaPatternAll(it.startAddress,
                                                              it.endAddress, pattern);
            if (adr_list.size() > skip_result)
            {
                insn_address = adr_list[skip_result];
            }
        }
        else
        {
            insn_address = kMgr.memScanner.findIdaPatternFirst(
                it.startAddress, it.endAddress, pattern);
        }
        if (insn_address)
            break;
    }
    return (insn_address ? (insn_address + step) : 0);
}

std::vector<std::string> IGameProfile::GetExcludedObjects() const
{
    // full name
    /*return {
        "ScriptStruct CoreUObject.Vector",
        "ScriptStruct CoreUObject.Vector2D"
    };*/
    return {};
}

std::string IGameProfile::GetUserTypesHeader() const
{
    return R"(#pragma once
    
#include <cstdio>
#include <string>
#include <cstdint>

template <class T>
class TArray
{
protected:
    T *Data;
    int32_t NumElements;
    int32_t MaxElements;

public:
    TArray(const TArray &) = default;
    TArray(TArray &&) = default;

    inline TArray() : Data(nullptr), NumElements(0), MaxElements(0) {}
    inline TArray(int size) : NumElements(0), MaxElements(size), Data(reinterpret_cast<T *>(calloc(1, sizeof(T) * size))) {}

    TArray &operator=(TArray &&) = default;
    TArray &operator=(const TArray &) = default;

    inline T &operator[](int i) { return (IsValid() && IsValidIndex(i)) ? Data[i] : T(); };
    inline const T &operator[](int i) const { (IsValid() && IsValidIndex(i)) ? Data[i] : T(); }

    inline explicit operator bool() const { return IsValid(); };

    inline bool IsValid() const { return Data != nullptr; }
    inline bool IsValidIndex(int index) const { return index >= 0 && index < NumElements; }

    inline int Slack() const { return MaxElements - NumElements; }

    inline int Num() const { return NumElements; }
    inline int Max() const { return MaxElements; }

    inline T *GetData() const { return Data; }
    inline T *GetDataAt(int index) const { return Data + index; }

    inline bool Add(const T &element)
    {
        if (Slack() <= 0) return false;

        Data[NumElements] = element;
        NumElements++;
        return true;
    }

    inline bool RemoveAt(int index)
    {
        if (!IsValidIndex(index)) return false;

        NumElements--;

        for (int i = index; i < NumElements; i++)
        {
            Data[i] = Data[i + 1];
        }

        return true;
    }

    inline void Clear()
    {
        NumElements = 0;
        if (Data) memset(Data, 0, sizeof(T) * MaxElements);
    }
};

class FString : public TArray<wchar_t>
{
public:
    FString() = default;
    inline FString(const wchar_t *wstr)
    {
        MaxElements = NumElements = (wstr && *wstr) ? int32_t(std::wcslen(wstr)) + 1 : 0;
        if (NumElements) Data = const_cast<wchar_t *>(wstr);
    }

    inline FString operator=(const wchar_t *&&other) { return FString(other); }

    inline std::wstring ToWString() const { return IsValid() ? Data : L""; }
};

template <typename KeyType, typename ValueType>
class TPair
{
private:
    KeyType First;
    ValueType Second;

public:
    TPair() = default;
    inline TPair(KeyType Key, ValueType Value) : First(Key), Second(Value) {}

    inline KeyType &Key() { return First; }
    inline const KeyType &Key() const { return First; }
    inline ValueType &Value() { return Second; }
    inline const ValueType &Value() const { return Second; }
};)";
}