#include "UEGameProfile.hpp"

#include <algorithm>
#include <array>
#include <initializer_list>
#include <vector>

#include "UEMemory.hpp"
#include "UEWrappers.hpp"

using namespace UEMemory;

namespace
{
    constexpr uintptr_t kArm64PageSize = 0x1000;
    constexpr uintptr_t kMinReadablePtr = 0x10000;

    bool IsLikelyReadablePtr(uintptr_t value)
    {
        return value >= kMinReadablePtr && kPtrValidator.isPtrReadable(value);
    }

    bool IsLikelyObjectName(const std::string &name)
    {
        if (name.empty() || name.size() > 96)
            return false;

        for (char c : name)
        {
            const unsigned char uc = static_cast<unsigned char>(c);
            if (!(std::isalnum(uc) || c == '_' || c == '/' || c == '.'))
                return false;
        }

        return true;
    }


    bool HasAppId(const IGameProfile *profile, const char *appId)
    {
        if (!profile || !appId || !*appId)
            return false;
        for (const auto &id : profile->GetAppIDs())
        {
            if (id == appId)
                return true;
        }
        return false;
    }

    uintptr_t GetModuleReadableEnd(const ElfScanner &ue_elf)
    {
        uintptr_t end = ue_elf.base();
        for (const auto &seg : ue_elf.segments())
        {
            if (seg.readable)
                end = (end > static_cast<uintptr_t>(seg.endAddress))
                          ? end
                          : static_cast<uintptr_t>(seg.endAddress);
        }
        return end;
    }

    uintptr_t BruteForceDeltaForceNames(const IGameProfile *profile, const ElfScanner &ue_elf, const UE_Offsets *off)
    {
        if (!profile || !off || !ue_elf.isValid())
            return 0;

        // 已知的 Delta Force CN FName 解密参数（与 dump-7 保持一致）
        constexpr uintptr_t kFNameStride = 0x2;
        constexpr uintptr_t kFNameEntryToString = 0x2;
        constexpr uintptr_t kGNamesToFNamePool = 0x38;
        constexpr uintptr_t kFNamePoolToBlocks = 0x0;
        constexpr uintptr_t kSearchStartOff = 0x18000000;
        constexpr uintptr_t kSearchEndOff = 0x1E000000;
        constexpr uintptr_t kMinGamePtr = 0x4000000000ULL;
        constexpr uintptr_t kMaxGamePtr = 0x8000000000ULL;

        static const int kProbeIds[] = {0, 1, 2, 5, 10, 20, 50, 100};
        static const char *kKnownNames[] = {
            "None", "ByteProperty", "IntProperty", "BoolProperty",
            "FloatProperty", "ObjectProperty", "NameProperty",
            "StructProperty", "ArrayProperty", "Object", "Class", "Function"
        };

        const uintptr_t searchStart = ue_elf.base() + kSearchStartOff;
        const uintptr_t searchEnd = ue_elf.base() + kSearchEndOff;
        if (searchEnd <= searchStart)
            return 0;

        const size_t totalRange = (searchEnd - searchStart) / kArm64PageSize;
        size_t checked = 0;
        LOGI("[Bootstrap] DeltaForce GNames brute force start: base=0x%lx range=[0x%lx, 0x%lx) pages=%zu",
             static_cast<unsigned long>(ue_elf.base()),
             static_cast<unsigned long>(searchStart),
             static_cast<unsigned long>(searchEnd),
             totalRange);

        for (uintptr_t addr = searchStart; addr < searchEnd; addr += kArm64PageSize)
        {
            ++checked;
            const uintptr_t candidate = addr;
            if ((checked % 5000) == 0)
            {
                const size_t percent = totalRange ? ((checked * 100) / totalRange) : 0;
                LOGD("[Bootstrap] DeltaForce GNames progress: checked=%zu/%zu (%zu%%) current=0x%lx",
                     checked, totalRange, percent, static_cast<unsigned long>(candidate));
            }

            int validCount = 0;
            for (int testIdx : kProbeIds)
            {
                const uint32_t block = static_cast<uint32_t>(testIdx >> 18);
                const uint32_t offset = static_cast<uint32_t>(testIdx & 0x3FFFF);
                const uintptr_t fNamePool = candidate + kGNamesToFNamePool;
                const uintptr_t namePoolChunk = vm_rpm_ptr<uintptr_t>(
                    (void *)(fNamePool + kFNamePoolToBlocks + block * sizeof(uintptr_t)));

                if (namePoolChunk < kMinGamePtr || namePoolChunk > kMaxGamePtr)
                    continue;

                const uintptr_t fNameEntry = namePoolChunk + kFNameStride * offset;
                const uint16_t fNameEntryHeader = vm_rpm_ptr<uint16_t>((void *)fNameEntry);
                const bool isWide = (fNameEntryHeader & 1) != 0;
                const int strLength = (fNameEntryHeader >> 6) & 0x3FF;

                if (strLength > 0 && strLength < 100 && !isWide)
                {
                    const uintptr_t strPtr = fNameEntry + kFNameEntryToString;
                    uint8_t key = 0;
                    switch (strLength % 9)
                    {
                    case 0:
                        key = ((strLength & 0x1F) + strLength + 0x80) | 0x7F;
                        break;
                    case 1:
                        key = ((strLength ^ 0xDF) + strLength + 0x80) | 0x7F;
                        break;
                    case 2:
                        key = ((strLength | 0xCF) + strLength + 128) | 0x7F;
                        break;
                    case 3:
                        key = (33 * strLength + 128) | 0x7F;
                        break;
                    case 4:
                        key = (strLength + (strLength >> 2) + 0x80) | 0x7F;
                        break;
                    case 5:
                        key = (3 * strLength + 133) | 0x7F;
                        break;
                    case 6:
                        key = (((4 * strLength) | 5) + strLength + 128) | 0x7F;
                        break;
                    case 7:
                        key = (((strLength >> 4) | 7) + strLength + 128) | 0x7F;
                        break;
                    case 8:
                        key = ((strLength ^ 0xC) + strLength + 0x80) | 0x7F;
                        break;
                    default:
                        key = ((strLength ^ 0x40) + strLength + 128) | 0x7F;
                        break;
                    }

                    char encNameBuf[100] = {};
                    if (vm_rpm_ptr((void *)strPtr, encNameBuf, static_cast<size_t>(strLength)))
                    {
                        std::string decrypted(static_cast<size_t>(strLength), '\0');
                        for (int i = 0; i < strLength; ++i)
                            decrypted[static_cast<size_t>(i)] =
                                static_cast<char>(static_cast<uint8_t>(encNameBuf[i]) ^ key);

                        for (const char *known : kKnownNames)
                        {
                            if (decrypted == known)
                            {
                                LOGI("[Bootstrap] DeltaForce candidate hit: candidate=0x%lx testIdx=%d block=%u offset=0x%x len=%d key=0x%x text='%s'",
                                     static_cast<unsigned long>(candidate),
                                     testIdx,
                                     block,
                                     offset,
                                     strLength,
                                     static_cast<unsigned int>(key),
                                     decrypted.c_str());
                                ++validCount;
                                break;
                            }
                        }

                        if (validCount >= 1)
                        {
                            LOGI("[Bootstrap] DeltaForce GNames @ 0x%lx (validated with %d known names, checked=%zu/%zu)",
                                 static_cast<unsigned long>(candidate), validCount, checked, totalRange);
                            return candidate;
                        }
                    }
                }
            }
        }

        LOGW("[Bootstrap] DeltaForce GNames brute force failed after checking %zu/%zu pages",
             checked, totalRange);
        return 0;
    }
    std::vector<uintptr_t> MakeCandidateList(uintptr_t primary, std::initializer_list<uintptr_t> fallbacks)
    {
        std::vector<uintptr_t> result;
        result.reserve(fallbacks.size() + 1);

        auto addUnique = [&result](uintptr_t value)
        {
            if (std::find(result.begin(), result.end(), value) == result.end())
                result.push_back(value);
        };

        addUnique(primary);
        for (uintptr_t value : fallbacks)
            addUnique(value);

        return result;
    }

    uintptr_t ReadObjectByLayout(uintptr_t objects, bool chunked, int32_t index, int32_t numElementsPerChunk,
                                 uintptr_t itemObjectOff, uintptr_t itemSize)
    {
        if (!IsLikelyReadablePtr(objects) || itemSize == 0 || index < 0)
            return 0;

        if (!chunked)
        {
            const uintptr_t itemAddr = objects + (static_cast<uintptr_t>(index) * itemSize) + itemObjectOff;
            return vm_rpm_ptr<uintptr_t>((void *)itemAddr);
        }

        if (numElementsPerChunk <= 0)
            return 0;

        const int32_t chunkIndex = index / numElementsPerChunk;
        const int32_t withinChunkIndex = index % numElementsPerChunk;
        const uintptr_t chunk = vm_rpm_ptr<uintptr_t>((void *)(objects + (static_cast<uintptr_t>(chunkIndex) * sizeof(uintptr_t))));
        if (!IsLikelyReadablePtr(chunk))
            return 0;

        return vm_rpm_ptr<uintptr_t>((void *)(chunk + (static_cast<uintptr_t>(withinChunkIndex) * itemSize) + itemObjectOff));
    }

    int ScoreObjectArrayCandidate(const std::function<std::string(int32_t)> &resolveName, uintptr_t objObjectsPtr,
                                  uintptr_t objectsOff, uintptr_t numElementsOff, bool chunked,
                                  int32_t numElementsPerChunk, uintptr_t itemObjectOff, uintptr_t itemSize,
                                  const std::vector<uintptr_t> &nameOffsets)
    {
        if (!resolveName || !IsLikelyReadablePtr(objObjectsPtr))
            return 0;

        const uintptr_t objects = vm_rpm_ptr<uintptr_t>((void *)(objObjectsPtr + objectsOff));
        if (!IsLikelyReadablePtr(objects))
            return 0;

        const int32_t numElements = vm_rpm_ptr<int32_t>((void *)(objObjectsPtr + numElementsOff));
        if (numElements < 1024 || numElements > 5000000)
            return 0;

        if (chunked)
        {
            const uintptr_t firstChunk = vm_rpm_ptr<uintptr_t>((void *)objects);
            if (!IsLikelyReadablePtr(firstChunk))
                return 0;
        }

        int score = 0;
        const int32_t sampleCount = std::min<int32_t>(numElements, 32);
        for (int32_t i = 0; i < sampleCount; ++i)
        {
            const uintptr_t object = ReadObjectByLayout(objects, chunked, i, numElementsPerChunk, itemObjectOff, itemSize);
            if (!IsLikelyReadablePtr(object))
                continue;

            score += 1;
            for (uintptr_t nameOff : nameOffsets)
            {
                const int32_t nameId = vm_rpm_ptr<int32_t>((void *)(object + nameOff));
                if (nameId <= 0 || nameId > 0x4000000)
                    continue;

                const std::string name = resolveName(nameId);
                if (!IsLikelyObjectName(name))
                    continue;

                score += 4;
                if (name == "/Script/CoreUObject" || name == "Package" || name == "Class" || name == "Object")
                    score += 3;
                break;
            }

            if (score >= 12)
                break;
        }

        if (score <= 0)
            return 0;

        if (numElements > 30000)
            score += 2;
        else if (numElements > 1000)
            score += 1;

        return score;
    }

    void BootstrapCoreObjectArrayOffsets(const std::function<std::string(int32_t)> &resolveName, UE_Offsets *offsets,
                                         uintptr_t guObjectsArrayPtr)
    {
        if (!resolveName || !offsets || !IsLikelyReadablePtr(guObjectsArrayPtr))
            return;

        struct Candidate
        {
            int score = 0;
            uintptr_t objObjectsOff = 0;
            uintptr_t tuObjectsOff = 0;
            uintptr_t numElementsOff = 0;
            int32_t numElementsPerChunk = 0;
        } best;

        const auto objObjectsOffsets = MakeCandidateList(offsets->FUObjectArray.ObjObjects, {0x10, 0x18, 0x20, 0x8});
        const auto tuObjectsOffsets = MakeCandidateList(offsets->TUObjectArray.Objects, {0x0, 0x8, 0x10, 0x18, 0x20, 0x28});
        const auto numElementsOffsets = MakeCandidateList(offsets->TUObjectArray.NumElements, {0x4, 0x8, 0xC, 0x10, 0x14, 0x18, 0x1C, 0x20});
        const auto nameOffsets = MakeCandidateList(offsets->UObject.NamePrivate, {0x18, 0x1C, 0x20, 0x24, 0x28});
        const uintptr_t stableItemObjectOff = offsets->FUObjectItem.Object;
        const uintptr_t stableItemSize = (offsets->FUObjectItem.Size >= 0x18) ? offsets->FUObjectItem.Size : 0x18;
        const int32_t stableChunkSize = static_cast<int32_t>(offsets->TUObjectArray.NumElementsPerChunk > 0 ? offsets->TUObjectArray.NumElementsPerChunk : 0x10000);

        const auto evaluateCandidate = [&](uintptr_t objObjectsOff, uintptr_t tuObjectsOff, uintptr_t numElementsOff) -> Candidate
        {
            Candidate candidate;
            candidate.objObjectsOff = objObjectsOff;
            candidate.tuObjectsOff = tuObjectsOff;
            candidate.numElementsOff = numElementsOff;

            const uintptr_t objObjectsPtr = guObjectsArrayPtr + objObjectsOff;
            if (!IsLikelyReadablePtr(objObjectsPtr))
                return candidate;

            const int flatScore = ScoreObjectArrayCandidate(resolveName, objObjectsPtr, tuObjectsOff, numElementsOff, false,
                                                            0, stableItemObjectOff, stableItemSize, nameOffsets);
            const int chunkedScore = ScoreObjectArrayCandidate(resolveName, objObjectsPtr, tuObjectsOff, numElementsOff, true,
                                                               stableChunkSize, stableItemObjectOff, stableItemSize, nameOffsets);

            const bool chooseChunked = chunkedScore > flatScore;
            const int layoutScore = chooseChunked ? chunkedScore : flatScore;
            if (layoutScore <= 0)
                return candidate;

            candidate.score = layoutScore;
            candidate.numElementsPerChunk = chooseChunked ? stableChunkSize : 0;
            return candidate;
        };

        const auto considerCandidate = [&](const Candidate &candidate)
        {
            if (candidate.score > best.score)
            {
                best = candidate;
                return;
            }

            if (candidate.score == best.score && candidate.score > 0)
            {
                if (candidate.tuObjectsOff < best.tuObjectsOff ||
                    (candidate.tuObjectsOff == best.tuObjectsOff && candidate.objObjectsOff > best.objObjectsOff) ||
                    (candidate.tuObjectsOff == best.tuObjectsOff &&
                     candidate.objObjectsOff == best.objObjectsOff &&
                     candidate.numElementsOff < best.numElementsOff))
                {
                    best = candidate;
                }
            }
        };

        for (uintptr_t objObjectsOff : objObjectsOffsets)
        {
            for (uintptr_t tuObjectsOff : tuObjectsOffsets)
            {
                for (uintptr_t numElementsOff : numElementsOffsets)
                {
                    considerCandidate(evaluateCandidate(objObjectsOff, tuObjectsOff, numElementsOff));
                }
            }
        }

        if (best.score <= 0)
        {
            LOGW("[Bootstrap] Core object array offsets not detected, fallback to preset values");
            return;
        }

        offsets->FUObjectArray.ObjObjects = best.objObjectsOff;
        offsets->TUObjectArray.Objects = best.tuObjectsOff;
        offsets->TUObjectArray.NumElements = best.numElementsOff;
        offsets->TUObjectArray.NumElementsPerChunk = best.numElementsPerChunk;

        LOGI("[Bootstrap] Core object array offsets detected: ObjObjects=0x%lx TU.Objects=0x%lx TU.NumElements=0x%lx FUItem.Object=0x%lx FUItem.Size=0x%lx chunk=%d score=%d",
             static_cast<unsigned long>(offsets->FUObjectArray.ObjObjects),
             static_cast<unsigned long>(offsets->TUObjectArray.Objects),
             static_cast<unsigned long>(offsets->TUObjectArray.NumElements),
             static_cast<unsigned long>(stableItemObjectOff),
             static_cast<unsigned long>(stableItemSize),
             offsets->TUObjectArray.NumElementsPerChunk,
             best.score);
    }

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

    bool IsADD(uint32_t insn)
    {
        return (insn & 0x7F800000) == 0x11000000;
    }

    bool IsSubSP(uint32_t insn)
    {
        return (insn & 0xFFC00000) == 0xD1000000 &&
               ((insn >> 5) & 0x1F) == 31 &&
               (insn & 0x1F) == 31;
    }

    bool IsStpFpLr(uint32_t insn)
    {
        return (((insn & 0xFFC00000) == 0xA9000000) ||
                ((insn & 0xFFC00000) == 0xA9800000)) &&
               ((insn >> 5) & 0x1F) == 31;
    }

    bool IsStrInstruction(uint32_t insn)
    {
        return ((insn & 0xFFC00000) == 0xF8000000) ||
               ((insn & 0xFFC00000) == 0xFC000000) ||
               ((insn & 0xFFC00000) == 0xF9000000);
    }

    uintptr_t FixTaggedPtr(uintptr_t value)
    {
        if (value > 0xB400000000000000ULL && value < 0xB400008000000000ULL)
            return value & 0x7FFFFFFFFFULL;
        return value;
    }

    uintptr_t FindFunctionStart(uintptr_t addr, uintptr_t search_start)
    {
        if (!addr) return 0;
        if (search_start > addr) search_start = 0;

        for (uintptr_t pc = addr; ; pc -= 4)
        {
            uint32_t insn = vm_rpm_ptr<uint32_t>((void *)pc);
            if (IsStpFpLr(insn) || IsSubSP(insn))
                return pc;

            if (pc <= search_start || pc < 4)
                break;
        }

        return 0;
    }

    uintptr_t FindWideDataInSegments(const ElfScanner &ue_elf, const void *data, size_t size)
    {
        for (const auto &seg : ue_elf.segments())
        {
            if (!seg.readable) continue;
            uintptr_t hit = kMgr.memScanner.findDataFirst(seg.startAddress, seg.endAddress, data, size);
            if (hit) return hit;
        }
        return 0;
    }

    std::vector<uintptr_t> FindADRPForTarget(const ElfScanner &ue_elf, uintptr_t target_addr)
    {
        std::vector<uintptr_t> result;
        const uint64_t target_page = target_addr & ~0xFFFULL;
        std::vector<uint8_t> buf(kArm64PageSize);

        for (const auto &seg : ue_elf.segments())
        {
            if (!seg.readable || !seg.executable) continue;

            for (uintptr_t page = seg.startAddress; page < seg.endAddress; page += kArm64PageSize)
            {
                size_t to_read = std::min<size_t>(kArm64PageSize, seg.endAddress - page);
                if (to_read < sizeof(uint32_t)) continue;
                if (!vm_rpm_ptr((void *)page, buf.data(), to_read))
                    continue;

                for (size_t i = 0; i + sizeof(uint32_t) <= to_read; i += sizeof(uint32_t))
                {
                    uint32_t insn = 0;
                    memcpy(&insn, buf.data() + i, sizeof(uint32_t));
                    if ((insn & 0x9F000000) != 0x90000000)
                        continue;

                    uintptr_t pc = page + i;
                    if (DecodeADRP(pc, insn) == target_page)
                        result.push_back(pc);
                }
            }
        }

        return result;
    }

    uintptr_t FilterADRPWithADD(const std::vector<uintptr_t> &adrp_candidates, uintptr_t target_addr)
    {
        for (uintptr_t adrp_addr : adrp_candidates)
        {
            uint32_t adrp_insn = vm_rpm_ptr<uint32_t>((void *)adrp_addr);
            if (!adrp_insn) continue;

            uint64_t adrp_base = DecodeADRP(adrp_addr, adrp_insn);
            for (int i = 1; i <= 4; ++i)
            {
                uint32_t insn = vm_rpm_ptr<uint32_t>((void *)(adrp_addr + (i * 4)));
                if (!IsADD(insn)) continue;
                if ((adrp_base + DecodeADD(insn)) == target_addr)
                    return adrp_addr;
            }
        }
        return 0;
    }
}

UEVarsInitStatus IGameProfile::InitUEVars()
{
    _UEVars = UEVars{};

    bool is32Bit = KittyMemoryEx::getMapsEndWith(kMgr.processID(), "/linker64").empty();
    if (is32Bit)
    {
        if (sizeof(void *) != 4)
        {
            LOGE("当前 Dumper 为 64 位，但目标进程为 32 位，请使用对应架构版本。");
            return UEVarsInitStatus::ERROR_ARCH_MISMATCH;
        }
    }
    else
    {
        if (sizeof(void *) != 8)
        {
            LOGE("当前 Dumper 为 32 位，但目标进程为 64 位，请使用对应架构版本。");
            return UEVarsInitStatus::ERROR_ARCH_MISMATCH;
        }
    }

    auto ue_elf = GetUnrealELF();
    if (!ue_elf.isValid())
    {
        LOGE("在目标进程映射中未找到有效的 UE ELF。");
        return UEVarsInitStatus::ERROR_LIB_NOT_FOUND;
    }

    if (!ArchSupprted())
    {
        if (GetUnrealELF().header().e_machine > 0 && !ue_elf.isHeaderless())
        {
            LOGE("当前游戏的架构 (0x%x) 暂不支持。", ue_elf.header().e_machine);
            return UEVarsInitStatus::ARCH_NOT_SUPPORTED;
        }
        else
        {
            LOGW("UE ELF Header might have been removed or modified!");
        }
    }

    kPtrValidator.setPID(kMgr.processID());
    kPtrValidator.setUseCache(true);
    kPtrValidator.refreshRegionCache();
    if (kPtrValidator.regions().empty())
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

    BootstrapCoreObjectArrayOffsets(_UEVars.pGetNameByID, pOffsets, _UEVars.GUObjectsArrayPtr);

    _UEVars.ObjObjectsPtr = _UEVars.GUObjectsArrayPtr + pOffsets->FUObjectArray.ObjObjects;

    if (!vm_rpm_ptr((void *)(_UEVars.ObjObjectsPtr + pOffsets->TUObjectArray.Objects),
                    &_UEVars.ObjObjects_Objects, sizeof(uintptr_t)))
        return UEVarsInitStatus::ERROR_INIT_OBJOBJECTS;
    if (!kPtrValidator.isPtrReadable(_UEVars.ObjObjects_Objects))
        return UEVarsInitStatus::ERROR_INIT_OBJOBJECTS;

    LOGI("[Bootstrap] Runtime object array: GUObject=0x%lx ObjObjects=0x%lx Objects=0x%lx",
         static_cast<unsigned long>(_UEVars.GUObjectsArrayPtr),
         static_cast<unsigned long>(_UEVars.ObjObjectsPtr),
         static_cast<unsigned long>(_UEVars.ObjObjects_Objects));

    _UEVars.Matrix = GetMatrix();
    _UEVars.Physx = GetPhysx();
    _UEVars.FrameCount = GetFrameCount();
    _UEVars.StaticFindObject = GetStaticFindObject();
    _UEVars.NativeAndroidApp = GetNativeAndroidApp();
    UEWrappers::Init(GetUEVars());
    _UEVars.ProcessEvent = GetProcessEvent();

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
        static uintptr_t gNamesPtr = 0;
        if (gNames == 0 || gNamesPtr != namesPtr)
        {
            gNames = vm_rpm_ptr<uintptr_t>((void *)namesPtr);
            //printf("gNames = 0x%lx\n", (unsigned long)gNames);
            if (HasAppId(this, "com.tencent.ig"))
            {

                const uintptr_t level0 = vm_rpm_ptr<uintptr_t>((void *)(gNames + 0x110));
                const uintptr_t level1 = vm_rpm_ptr<uintptr_t>((void *)level0);
                gNames = vm_rpm_ptr<uintptr_t>((void *)level1);;
                //printf("level1 = 0x%lx\n", (unsigned long)level1);
                gNamesPtr = level0;
            }
            gNamesPtr = namesPtr;
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

ElfScanner IGameProfile::GetUnrealELF() const
{
    static const std::vector<std::string> cUELibNames = {"libUE4.so",
                                                         "libUnreal.so"};

    ElfScanner ue_elf{};
    for (const auto &lib : cUELibNames)
    {
        ue_elf = kMgr.findMemElf(lib);
        if (ue_elf.isValid())
        {
            //LOGI("[GetUnrealELF] %s found at base 0x%lx (pid=%d)",lib.c_str(), (unsigned long)ue_elf.base(), kMgr.processID());
            return ue_elf;
        }
    }

    // split config
    const auto maps = KittyMemoryEx::getAllMaps(kMgr.processID());
    for (const auto &lib : cUELibNames)
    {
        for (auto &it : maps)
        {
            if (KittyUtils::String::Contains(it.pathname, kMgr.processName()) &&
                KittyUtils::String::EndsWith(it.pathname, ".apk"))
            {
                ue_elf = kMgr.findMemElfInZip(it.pathname, lib);
                if (ue_elf.isValid())
                    return ue_elf;
            }
        }
    }

    // last resort, linker solist
    // some games like farlight and pubg remove ELF header from lib
    for (const auto &lib : cUELibNames)
    {
        ue_elf = kMgr.findMemElfFromLinker(lib);
        if (ue_elf.isValid())
            return ue_elf;
    }

    return ue_elf;
}

bool IGameProfile::isEmulator() const
{
    if (!KittyMemoryEx::getMapsContain(kMgr.processID(), "/arm/nb/").empty() ||
        !KittyMemoryEx::getMapsContain(kMgr.processID(), "/arm64/nb/").empty())
        return true;

    for (auto &it : GetUnrealELF().segments())
        if (it.executable)
            return false;

    return true;
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

uintptr_t IGameProfile::GetGUObjectArrayPtr() const
{
    // Generic Dumper-7 style GUObject scan:
    // walk nearby readable addresses, treat candidate+ObjObjects as the object
    // array entry point, then verify the first UObject resolves to the always
    // present "/Script/CoreUObject" package.
    UE_Offsets *off = GetOffsets();
    const uintptr_t namesPtr = GetNamesPtr();
    uintptr_t namesScanBase = namesPtr;
    if (HasAppId(this, "com.tencent.ig"))
    {
        const uintptr_t level0 = vm_rpm_ptr<uintptr_t>((void *)namesPtr);
        const uintptr_t level1 = vm_rpm_ptr<uintptr_t>((void *)(level0 + 0x110));
        if (level1)
            namesScanBase = vm_rpm_ptr<uintptr_t>((void *)(level1));
    }
    if (!off || !namesScanBase)
        return 0;

    const uintptr_t objObjectsOff = off->FUObjectArray.ObjObjects;
    const uintptr_t namePrivateOff = off->UObject.NamePrivate;
    const uintptr_t numChunks = off->TUObjectArray.NumElementsPerChunk;
    const uintptr_t itemObj = off->FUObjectItem.Object;
    (void)off->FUObjectItem.Size;

    static const uintptr_t kNameOffs[] = {0x18, 0x1c, 0x20, 0x24, 0x28,0x2c,0x30,0x34,0x38,0x3c,0x40,0x44,0x48,0x4c,0x50,0x54,0x58,0x5c,0x60,0x64,0x68,0x6c};

    for (int i = 0; i < 0x300000; ++i)
    {
        const uintptr_t candObjAddr = namesScanBase + 8ULL * static_cast<uintptr_t>(i);

        uintptr_t objects = vm_rpm_ptr<uintptr_t>((void *)(candObjAddr + objObjectsOff));
        if (objects < 0x10000 || !kPtrValidator.isPtrReadable(objects))
            continue;

        uintptr_t firstObj = 0;
        if (numChunks > 0)
        {
            const uintptr_t chunk0 = vm_rpm_ptr<uintptr_t>((void *)objects);
            if (!kPtrValidator.isPtrReadable(chunk0))
                continue;
            firstObj = vm_rpm_ptr<uintptr_t>((void *)(chunk0 + itemObj));
        }
        else
        {
            firstObj = vm_rpm_ptr<uintptr_t>((void *)(objects + itemObj));
        }

        if (firstObj < 0x10000 || !kPtrValidator.isPtrReadable(firstObj))
            continue;

        for (uintptr_t no : kNameOffs)
        {
            const int32_t id = vm_rpm_ptr<int32_t>((const void *)(firstObj + no));
            if (id <= 0 || id > 0x200000)
                continue;

            const std::string nm = GetNameByID(id);
            if (nm != "/Script/CoreUObject")
                continue;

            if (no != namePrivateOff)
            {
                LOGI("[Bootstrap] Adjusting UObject.NamePrivate 0x%lx -> 0x%lx",
                     (unsigned long)namePrivateOff, (unsigned long)no);
                off->UObject.NamePrivate = no;
            }

            LOGI("[Bootstrap] GUObject @ 0x%lx (Objects=0x%lx, FirstObj=0x%lx, anchor='/Script/CoreUObject')",
                 (unsigned long)candObjAddr, (unsigned long)objects, (unsigned long)firstObj);
            return candObjAddr;
        }
    }

    LOGE("[Bootstrap] 通用方式搜索 GUObject 失败");
    return 0;
}

uintptr_t IGameProfile::GetNamesPtr() const
{
    auto ue_elf = GetUnrealELF();
    if (!ue_elf.isValid())
    {
        LOGE("[Bootstrap] GetNamesPtr: 无效的 UE ELF (pid=%d)", kMgr.processID());
        return 0;
    }

    UE_Offsets *off = GetOffsets();
    if (!off)
        return 0;

    const bool isDeltaForce = HasAppId(this, "com.tencent.tmgp.dfm");
    if (isDeltaForce)
    {
        if (uintptr_t names = BruteForceDeltaForceNames(this, ue_elf, off))
            return names;
        LOGW("[Bootstrap] DeltaForce 专用 GNames 暴力搜索失败，回退到通用算法");
    }

    const bool preferFNamePool = IsUsingFNamePool();
    const bool isPubgClassic = HasAppId(this, "com.tencent.ig");
    constexpr size_t kPtrSize = sizeof(uintptr_t);
    constexpr uintptr_t kMinPtr = 0x4FFFFFFFFFULL;
    constexpr uintptr_t kMaxPtr = 0x7FFFFFFFFFULL;
    constexpr size_t kChunk = 0x100000;

    auto isByteProperty = [](const char *s) -> bool
    {
        return std::strncmp(s, "ByteProperty", 12) == 0;
    };

    for (const auto &seg : ue_elf.segments())
    {
        if (!seg.readable)
            continue;

        std::vector<uint8_t> buf(kChunk);
        for (size_t base = 0; base + kPtrSize <= seg.length; base += kChunk)
        {
            const size_t toRead = std::min<size_t>(kChunk, seg.length - base);
            if (!vm_rpm_ptr((void *)(seg.startAddress + base), buf.data(), toRead))
                continue;

            for (size_t i = 0; i + kPtrSize <= toRead; i += kPtrSize)
            {
                uintptr_t val = 0;
                memcpy(&val, buf.data() + i, kPtrSize);
                if (val < kMinPtr || val > kMaxPtr)
                    continue;

                const uintptr_t candidate = seg.startAddress + base + i;
                char strBuf0[64] = {};
                char strBuf1[64] = {};
                char strBuf2[64] = {};

                auto tryFNamePool = [&]() -> uintptr_t
                {
                    if (!vm_rpm_ptr((void *)(val + 0x8), strBuf0, 12) || !isByteProperty(strBuf0))
                        return 0;

                    uintptr_t poolBase = candidate;
                    if (off->FNamePool.BlocksOff && candidate >= off->FNamePool.BlocksOff)
                        poolBase -= off->FNamePool.BlocksOff;

                    LOGI("[Bootstrap] FNamePool @ 0x%lx (slot @ 0x%lx)",
                         static_cast<unsigned long>(poolBase), static_cast<unsigned long>(candidate));
                    return poolBase;
                };

                auto tryGNames = [&]() -> uintptr_t
                {
                    uintptr_t p0 = vm_rpm_ptr<uintptr_t>((void *)val);
                    uintptr_t p1 = p0 ? vm_rpm_ptr<uintptr_t>((void *)p0) : 0;
                    if (p1 && vm_rpm_ptr((void *)(p1 + 0x24), strBuf1, 12) && isByteProperty(strBuf1))
                    {
                        LOGI("[Bootstrap] GNames @ 0x%lx", static_cast<unsigned long>(candidate));
                        return candidate;
                    }

                    if (isPubgClassic)
                    {
                        uintptr_t q0 = vm_rpm_ptr<uintptr_t>((void *)(val + 0x110));
                        uintptr_t q1 = q0 ? vm_rpm_ptr<uintptr_t>((void *)q0) : 0;
                        uintptr_t q2 = q1 ? vm_rpm_ptr<uintptr_t>((void *)q1) : 0;
                        uintptr_t q3 = q2 ? vm_rpm_ptr<uintptr_t>((void *)q2) : 0;
                        if (q3 && vm_rpm_ptr((void *)(q3 + 0x24), strBuf2, 12) && isByteProperty(strBuf2))
                        {
                            LOGI("[Bootstrap] PUBG-style GNames @ 0x%lx", static_cast<unsigned long>(candidate));
                            return candidate;
                        }
                    }

                    return 0;
                };

                if (preferFNamePool)
                {
                    if (uintptr_t found = tryFNamePool()) return found;
                    if (uintptr_t found = tryGNames()) return found;
                }
                else
                {
                    if (uintptr_t found = tryGNames()) return found;
                    if (uintptr_t found = tryFNamePool()) return found;
                }
            }
        }
    }

    LOGE("[Bootstrap] 自动搜索 GNames/NamePool 失败");
    return 0;
}

uintptr_t IGameProfile::GetStaticFindObject() const
{
    static constexpr char16_t kNeedle[] = u"Illegal call to StaticFindObject() while serializing object data!";

    auto ue_elf = GetUnrealELF();
    if (!ue_elf.isValid()) return 0;

    uintptr_t wideStr = FindWideDataInSegments(ue_elf, kNeedle, sizeof(kNeedle) - sizeof(char16_t));
    if (!wideStr) return 0;

    std::vector<uintptr_t> adrpCandidates = FindADRPForTarget(ue_elf, wideStr);
    uintptr_t matched = FilterADRPWithADD(adrpCandidates, wideStr);
    if (!matched) return 0;

    uintptr_t fnStart = FindFunctionStart(matched, (matched > 0x1000) ? (matched - 0x1000) : 0);
    return fnStart ? fnStart : matched;
}

uintptr_t IGameProfile::GetNativeAndroidApp() const
{
    auto ue_elf = GetUnrealELF();
    if (!ue_elf.isValid()) return 0;

    constexpr uintptr_t kMinAppPtr = 0x4FFFFFFFFFULL;
    constexpr uintptr_t kMaxAppPtr = 0x7FFFFFFFFFULL;
    constexpr size_t kChunk = 0x100000;
    std::vector<uint8_t> buffer(kChunk);

    for (const auto &seg : ue_elf.segments())
    {
        if (!seg.readable) continue;

        for (size_t base = 0; base < seg.length; base += kChunk)
        {
            size_t toRead = std::min<size_t>(kChunk, seg.length - base);
            if (toRead < sizeof(uintptr_t)) continue;
            if (!vm_rpm_ptr((void *)(seg.startAddress + base), buffer.data(), toRead))
                continue;

            for (size_t i = 0; i + sizeof(uintptr_t) <= toRead; i += sizeof(uintptr_t))
            {
                uintptr_t val = 0;
                memcpy(&val, buffer.data() + i, sizeof(uintptr_t));
                uintptr_t decoded = FixTaggedPtr(val);
                if (decoded < kMinAppPtr || decoded > kMaxAppPtr)
                    continue;

                uintptr_t localeHolder = FixTaggedPtr(vm_rpm_ptr<uintptr_t>((void *)(decoded + 0x20)));
                if (!kPtrValidator.isPtrReadable(localeHolder + 0x8))
                    continue;

                char tag[5] = {};
                if (!vm_rpm_ptr((void *)(localeHolder + 0x8), tag, 4))
                    continue;

                if (memcmp(tag, "zhCN", 4) == 0)
                    return seg.startAddress + base + i;
            }
        }
    }

    return 0;
}

uintptr_t IGameProfile::GetProcessEvent() const
{
    auto *objects = UEWrappers::GetObjects();
    if (!objects) return 0;

    UE_UObject firstObject = objects->GetObjectPtr(0);
    if (!firstObject) return 0;

    uintptr_t vtable = vm_rpm_ptr<uintptr_t>((void *)firstObject.GetAddress());
    if (!kPtrValidator.isPtrReadable(vtable))
        return 0;

    for (uintptr_t i = 0; i < 500; ++i)
    {
        uintptr_t function = vm_rpm_ptr<uintptr_t>((void *)(vtable + i * sizeof(uintptr_t)));
        if (!kPtrValidator.isPtrReadable(function + 0x10))
            continue;

        uint32_t insn0 = vm_rpm_ptr<uint32_t>((void *)(function));
        uint32_t insn1 = vm_rpm_ptr<uint32_t>((void *)(function + 0x4));
        uint32_t insn2 = vm_rpm_ptr<uint32_t>((void *)(function + 0x8));
        if (i > 50 && i < 100 &&
            (IsStpFpLr(insn0) || IsStrInstruction(insn0)) &&
            (IsStpFpLr(insn1) || IsStrInstruction(insn1)) &&
            (IsStpFpLr(insn2) || IsStrInstruction(insn2)))
        {
            return function;
        }
    }

    return 0;
}