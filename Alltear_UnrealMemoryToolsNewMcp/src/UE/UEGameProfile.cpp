#include "UEGameProfile.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <initializer_list>
#include <vector>

#include "UEMemory.hpp"
#include "UEWrappers.hpp"

using namespace UEMemory;

void IGameProfile::SetAddressOverrides(const UEAddressOverrides &overrides)
{
    UE_Offsets *offsets = GetOffsets();
    if (offsets)
    {
        if (!_hasOffsetsBackup)
        {
            _baseOffsetsBackup = *offsets;
            _hasOffsetsBackup = true;
        }
        else
        {
            *offsets = _baseOffsetsBackup;
        }
    }
    _addressOverrides = overrides;
}

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

    // ---- 多锚点判定：UE 引擎内建名 ----
    // 所有 UE 游戏的名字表开头都是这一批，与具体游戏无关。
    // 原为 BruteForceDeltaForceNames() 内的局部 static，此处提升为文件作用域以便通用路径共用。
    const char *const kKnownNames[] = {
        "None", "ByteProperty", "IntProperty", "BoolProperty",
        "FloatProperty", "ObjectProperty", "NameProperty",
        "StructProperty", "ArrayProperty", "Object", "Class", "Function"
    };
    constexpr size_t kKnownNamesCount = sizeof(kKnownNames) / sizeof(kKnownNames[0]);

    // 窗口扫描的通过阈值：命中几个"不同的"引擎名才算确认
    constexpr int kMinAnchorHits = 2;

    // 单次读取的窗口大小（一次远程读覆盖全部候选偏移，避免多次远程读）
    constexpr size_t kAnchorWindowSize = 128;

    // FNamePool 内首个名字条目的候选偏移（条目头长度随 UE 版本变化）
    const uintptr_t kPoolEntryOffsets[] = {0x8, 0x6, 0xA, 0x4, 0xC, 0x10};
    constexpr size_t kPoolEntryOffsetsCount =
        sizeof(kPoolEntryOffsets) / sizeof(kPoolEntryOffsets[0]);

    // GNames 二级解引用后条目头的候选偏移
    const uintptr_t kGNamesEntryOffsets[] = {0x24, 0x2C, 0x1C, 0x14, 0x34};
    constexpr size_t kGNamesEntryOffsetsCount =
        sizeof(kGNamesEntryOffsets) / sizeof(kGNamesEntryOffsets[0]);

    // 返回命中的引擎名索引；未命中返回 -1
    int MatchEngineName(const char *s)
    {
        if (!s || !*s)
            return -1;

        for (size_t i = 0; i < kKnownNamesCount; ++i)
        {
            if (std::strncmp(s, kKnownNames[i], std::strlen(kKnownNames[i])) == 0)
                return static_cast<int>(i);
        }
        return -1;
    }

    struct AnchorScanResult
    {
        bool hit = false;
        bool exact = false;   // true = Tier1 精确偏移命中；false = Tier2 窗口兜底
        uintptr_t offset = 0; // 精确命中时的偏移
        int hits = 0;         // 窗口扫描命中的不同引擎名数量
    };

    // 只做 1 次远程读，随后在本地跑两级判定：
    //   Tier 1 精确偏移匹配（快、准）→ 失败则 Tier 2 窗口扫描（宽松、抗条目头长度变化）
    AnchorScanResult ScanEngineNameAnchors(uintptr_t base, char *buf, size_t bufCap,
                                           const uintptr_t *offsets, size_t count)
    {
        AnchorScanResult r;
        if (!buf || bufCap < 32)
            return r;

        std::memset(buf, 0, bufCap);
        if (!vm_rpm_ptr((void *)base, buf, bufCap))
            return r;

        // Tier 1：候选偏移逐个精确匹配
        for (size_t k = 0; k < count; ++k)
        {
            if (offsets[k] + 16 > bufCap)
                continue;
            if (MatchEngineName(buf + offsets[k]) >= 0)
            {
                r.hit = true;
                r.exact = true;
                r.offset = offsets[k];
                r.hits = 1;
                return r;
            }
        }

        // Tier 2：窗口扫描，统计出现的不同引擎名数量
        bool seen[kKnownNamesCount] = {};
        for (size_t i = 0; i + 1 < bufCap; ++i)
        {
            const int idx = MatchEngineName(buf + i);
            if (idx >= 0 && !seen[idx])
            {
                seen[idx] = true;
                ++r.hits;
            }
        }
        if (r.hits >= kMinAnchorHits)
            r.hit = true;

        return r;
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
        // kKnownNames 已提升至文件作用域（通用路径与此处共用同一份）

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

    if (_addressOverrides.hasNameLayout)
    {
        pOffsets->FNamePool.Stride = _addressOverrides.nameStride;
        pOffsets->FNamePool.BlocksBit = _addressOverrides.nameBlocksBit;
        pOffsets->FNamePool.BlocksOff = _addressOverrides.nameBlocksOff;
        pOffsets->FNamePoolEntry.Header = _addressOverrides.nameHeaderOff;
        const uintptr_t lengthShift = _addressOverrides.nameLengthShift;
        pOffsets->FNamePoolEntry.GetLength = [lengthShift](uint16_t header) -> size_t
        {
            return static_cast<size_t>(header >> lengthShift);
        };
    }
    if (_addressOverrides.hasObjectLayout)
    {
        pOffsets->FUObjectArray.ObjObjects = _addressOverrides.objObjectsOff;
        pOffsets->TUObjectArray.Objects = _addressOverrides.objectsOff;
        pOffsets->TUObjectArray.NumElements = _addressOverrides.numElementsOff;
        pOffsets->TUObjectArray.NumElementsPerChunk = _addressOverrides.numElementsPerChunk;
        pOffsets->FUObjectItem.Object = _addressOverrides.itemObjectOff;
        pOffsets->FUObjectItem.Size = _addressOverrides.itemSize;
        pOffsets->UObject.ClassPrivate = _addressOverrides.classPrivateOff;
        pOffsets->UObject.NamePrivate = _addressOverrides.namePrivateOff;
        pOffsets->UObject.OuterPrivate = _addressOverrides.outerPrivateOff;
    }

    _UEVars.NamesPtr = _addressOverrides.hasNamesPtr ? _addressOverrides.namesPtr : GetNamesPtr();
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

    _UEVars.GUObjectsArrayPtr = _addressOverrides.hasGUObjectArrayPtr
        ? _addressOverrides.guObjectArrayPtr
        : GetGUObjectArrayPtr();
    if (!kPtrValidator.isPtrReadable(_UEVars.GUObjectsArrayPtr))
        return UEVarsInitStatus::ERROR_INIT_GUOBJECTARRAY;

    if (!_addressOverrides.hasObjectLayout)
        BootstrapCoreObjectArrayOffsets(_UEVars.pGetNameByID, pOffsets, _UEVars.GUObjectsArrayPtr);

    _UEVars.ObjObjectsPtr = _UEVars.GUObjectsArrayPtr + pOffsets->FUObjectArray.ObjObjects;

    if (!vm_rpm_ptr((void *)(_UEVars.ObjObjectsPtr + pOffsets->TUObjectArray.Objects),
                    &_UEVars.ObjObjects_Objects, sizeof(uintptr_t)))
        return UEVarsInitStatus::ERROR_INIT_OBJOBJECTS;
    if (!kPtrValidator.isPtrReadable(_UEVars.ObjObjects_Objects, sizeof(uintptr_t)))
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
    // Do not report Probe success until at least one real UObject can be
    // traversed with the selected layout.  This catches stale/incorrect
    // overrides before SEARCH_CLASSES and DUMP consume them.
    bool hasObjectSample = false;
    if (auto *objects = UEWrappers::GetObjects())
    {
        const int32_t total = objects->GetNumElements();
        const int32_t sampleLimit = std::min<int32_t>(total, 64);
        for (int32_t i = 0; i < sampleLimit; ++i)
        {
            if (objects->GetObjectPtr(i)) { hasObjectSample = true; break; }
        }
    }
    if (!hasObjectSample)
    {
        LOGE("[Bootstrap] object array layout validated but no readable UObject sample");
        return UEVarsInitStatus::ERROR_INIT_OBJOBJECTS;
    }
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

    // FNAME_OUTLINE_NUMBER：FName 是 Outline Number（含偏移），不是直接 Index。
    // 真正的 NameIndex = id >> 18。此处解包，后续 FNamePool 查找使用真实 Index。
    int32_t resolvedId = id;
    if (IsUsingFNamePool() && isUsingOutlineNumberName())
    {
        static const int FNAME_OUTLINE_SHIFT = 18;
        const int32_t extracted = id >> FNAME_OUTLINE_SHIFT;
        if (extracted > 0 && extracted != id)
        {
            resolvedId = extracted;
        }
    }

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

    uintptr_t block_offset = ((resolvedId >> blockBit) * sizeof(void *));
    uintptr_t chunck_offset = ((resolvedId & chunckMask) * stride);

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

        // 自动识别 FNameEntryHeader 布局（保持 UE4.22-UE5 全兼容）：
        //   UE4.22-4.25 : uint32 header, Len = header >> 6（GetLength 已自动识别）
        //   UE4.26+     : uint16 header, Len = header >> 1
        //   FNAME_OUTLINE_NUMBER（含腾讯魔改 UE4，如 LetsGo/元梦之星）：
        //     Len==0 的 entry 是"外链"记录，布局为 Header(2)+NextEntryId(4)+Number(4)，
        //     真实名字由 NextEntryId 指向。此前被 isUsingOutlineNumberName() 开关
        //     挡住（UE4.25-4.27 档案硬编码 false），改为纯结构自动识别。
        size_t tryLen = offsets->FNamePoolEntry.GetLength(header);
        if (tryLen == 0 && (header >> 1) == 0)
        {
            // 结构上确认这是 FNAME_OUTLINE_NUMBER 的外链 entry：
            // 运行时把开关自愈置 true（仅 outline 模式的游戏才会走到这里，
            // 4.26+ 非 outline 游戏不受影响），保证后续 UE_FName::GetName/
            // GetNumber 不再误读 FName.Number（LetsGo 的 FName 仅 4 字节）。
            if (!offsets->Config.isUsingOutlineNumberName)
            {
                offsets->Config.isUsingOutlineNumberName = true;
                LOGI("[Bootstrap] FNamePool: detected FNAME_OUTLINE_NUMBER outlined entry, enabling outline-name mode");
            }

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

        strLen = offsets->FNamePoolEntry.GetLength(header);
        if (strLen == 0)
            strLen = static_cast<size_t>(header >> 1);  // 16 位 header 兜底
        strLen = std::min<size_t>(strLen, kMAX_UENAME_BUFFER);
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

    auto ue_elf = GetUnrealELF();
    if (!ue_elf.isValid())
        return false; // ELF 未找到，无法判断，保守返回非模拟器

    for (auto &it : ue_elf.segments())
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
    const uintptr_t stableItemSize = (off->FUObjectItem.Size >= 0x18) ? off->FUObjectItem.Size : 0x18;

    static const uintptr_t kNameOffs[] = {0x18, 0x1c, 0x20, 0x24, 0x28,0x2c,0x30,0x34,0x38,0x3c,0x40,0x44,0x48,0x4c,0x50,0x54,0x58,0x5c,0x60,0x64,0x68,0x6c};

    // Scan multiple early slots to avoid false-negative when slot 0 has garbage.
    // FName pool stores the PLAIN package/class name, not the "/Script/"-prefixed
    // path: LetsGo(UE4.26) slot0 = Package "CoreUObject" (FName "CoreUObject"),
    // slot1 = Class CoreUObject.Object (FName "Object"). Accept both forms plus
    // generic "Package"/"Class" so slot0-null and slot0-package layouts verify.
    constexpr int kVerifySlots = 8;
    static const char *const kVerifyAnchors[] = {"CoreUObject", "/Script/CoreUObject", "Object", "Package", "Class"};

    // 双向扫描：GUObjectArray 可能在 namesScanBase 高地址或低地址方向，
    // 原实现只往高地址扫（namesScanBase + 8*i），低地址方向永远扫不到。
    static const int kMaxSearchDist = 0x300000;

    auto verifyCandidate = [&](uintptr_t candObjAddr, const char* direction) -> uintptr_t
    {
        uintptr_t objects = vm_rpm_ptr<uintptr_t>((void *)(candObjAddr + objObjectsOff));
        if (objects < 0x10000 || !kPtrValidator.isPtrReadable(objects, sizeof(uintptr_t)))
            return 0;

        uintptr_t firstObj = 0;
        uintptr_t chunk0 = 0;
        if (numChunks > 0)
        {
            chunk0 = vm_rpm_ptr<uintptr_t>((void *)objects);
            if (kPtrValidator.isPtrReadable(chunk0, sizeof(uintptr_t)))
                firstObj = vm_rpm_ptr<uintptr_t>((void *)(chunk0 + itemObj));
        }
        else
        {
            firstObj = vm_rpm_ptr<uintptr_t>((void *)(objects + itemObj));
        }
        if (firstObj < 0x10000 || !kPtrValidator.isPtrReadable(firstObj))
            return 0;

        // 多槽位验证：扫描前 kVerifySlots 个对象，检查是否有有效锚点
        // UE4.2x 的 slot 0 是保留空槽，真对象从 slot 1 开始
        int anchorHits = 0;
        for (int slot = 0; slot < kVerifySlots; ++slot)
        {
            uintptr_t objAddr = 0;
            if (numChunks > 0 && chunk0)
            {
                const int32_t chunkIdx = slot / static_cast<int32_t>(numChunks);
                const int32_t withinChunk = slot % static_cast<int32_t>(numChunks);
                const uintptr_t chunkPtr = vm_rpm_ptr<uintptr_t>((void *)(objects + chunkIdx * sizeof(uintptr_t)));
                if (chunkPtr && kPtrValidator.isPtrReadable(chunkPtr, sizeof(uintptr_t)))
                    objAddr = vm_rpm_ptr<uintptr_t>((void *)(chunkPtr + withinChunk * stableItemSize + itemObj));
            }
            else
            {
                objAddr = vm_rpm_ptr<uintptr_t>((void *)(objects + slot * stableItemSize + itemObj));
            }

            if (objAddr < 0x10000 || !kPtrValidator.isPtrReadable(objAddr))
                continue;  // 空槽，跳过

            // 验证此对象的可读性（class + name）
            if (off->UObject.ClassPrivate && off->UObject.NamePrivate)
            {
                const uintptr_t classPtr = vm_rpm_ptr<uintptr_t>((void *)(objAddr + off->UObject.ClassPrivate));
                if (classPtr && kPtrValidator.isPtrReadable(classPtr, 0x20))
                {
                    for (uintptr_t no : kNameOffs)
                    {
                        const int32_t id = vm_rpm_ptr<int32_t>((const void *)(objAddr + no));
                        if (id > 0 && id <= 0x200000)
                        {
                            const std::string nm = GetNameByID(id);
                            bool matched = false;
                            // Outline number 模式下 GetNameByID 可能返回 "CoreUObject_N"，
                            // 去掉 _N 后缀后再与锚点比较。
                            std::string nmClean = nm;
                            if (nmClean.size() > 2 && nmClean.substr(nmClean.size() - 2) == "_N")
                                nmClean.resize(nmClean.size() - 2);
                            for (const char *anchor : kVerifyAnchors)
                            {
                                if (nmClean == anchor)
                                {
                                    matched = true;
                                    break;
                                }
                            }
                            if (matched)
                            {
                                ++anchorHits;
                                if (no != namePrivateOff)
                                {
                                    LOGI("[Bootstrap] Adjusting UObject.NamePrivate 0x%lx -> 0x%lx",
                                         (unsigned long)namePrivateOff, (unsigned long)no);
                                    off->UObject.NamePrivate = no;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (anchorHits > 0)
        {
            LOGI("[Bootstrap] GUObject @ 0x%lx (%s/Objects=0x%lx, anchorHits=%d, verified %d slots)",
                 (unsigned long)candObjAddr, direction, (unsigned long)objects, anchorHits, kVerifySlots);
            return candObjAddr;
        }

        // 内存条件全部通过但锚点未命中：打印诊断（只打前 5 个，避免刷屏）。
        // 主要用途：真机复测时确认对象槽位地址 / FName index / 名字解析是否正确。
        static int sDiagCount = 0;
        if (sDiagCount < 5)
        {
            ++sDiagCount;
            const int32_t id0 = (firstObj >= 0x10000)
                ? vm_rpm_ptr<int32_t>((const void *)(firstObj + namePrivateOff)) : -1;
            const std::string s0 = (id0 > 0 && id0 <= 0x200000) ? GetNameByID(id0) : "";
            LOGI("[Bootstrap] verifyCandidate(mem-ok,name-fail): cand=0x%lx objects=0x%lx chunk0=0x%lx obj0=0x%lx id=%d name='%s'",
                 (unsigned long)candObjAddr, (unsigned long)objects,
                 (unsigned long)chunk0, (unsigned long)firstObj, id0, s0.c_str());
        }
        return 0;
    };

    for (int i = 0; i < kMaxSearchDist; ++i)
    {
        // 高地址方向
        {
            const uintptr_t candObjAddr = namesScanBase + 8ULL * static_cast<uintptr_t>(i);
            if (verifyCandidate(candObjAddr, "UP"))
                return candObjAddr;
        }
        // 低地址方向（跳过 i==0 避免重复）
        if (i > 0)
        {
            const uintptr_t candObjAddr = namesScanBase - 8ULL * static_cast<uintptr_t>(i);
            if (candObjAddr < 0x10000)
                continue;
            if (verifyCandidate(candObjAddr, "DOWN"))
                return candObjAddr;
        }
    }

    LOGE("[Bootstrap] 通用方式搜索 GUObject 失败");
    LOGI("[Bootstrap] 诊断: NamesPtr=0x%lx, scanBase=0x%lx, nameOff=0x%lx, objObjectsOff=0x%lx",
         (unsigned long)namesPtr, (unsigned long)namesScanBase,
         (unsigned long)namePrivateOff, (unsigned long)objObjectsOff);
    LOGI("[Bootstrap] 建议: 使用 SCAN_GNAMES 验证 NamesPtr，或使用 APPLY_PROBE_OVERRIDES 手动指定");
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

    // 多锚点判定由 ScanEngineNameAnchors() 完成（见文件顶部匿名 namespace），
    // 单次远程读 + 本地两级判定，替换原先"单锚点 ByteProperty + 单偏移"的做法。
    //
    // 宽松命中（Tier2 窗口兜底）不立即返回，先记在下面两个变量里继续扫描；
    // 只有整轮扫完都没有精确命中时，才退而使用宽松命中。
    // 否则一个靠前的弱结果会盖掉后面真正的精确命中——那比返回 0 更糟。
    uintptr_t weakPool = 0;
    uintptr_t weakGNames = 0;

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
                char strBuf0[kAnchorWindowSize] = {};
                char strBuf1[kAnchorWindowSize] = {};
                char strBuf2[kAnchorWindowSize] = {};

                auto tryFNamePool = [&]() -> uintptr_t
                {
                    const AnchorScanResult r =
                        ScanEngineNameAnchors(val, strBuf0, sizeof(strBuf0),
                                              kPoolEntryOffsets, kPoolEntryOffsetsCount);
                    if (!r.hit)
                        return 0;

                    uintptr_t poolBase = candidate;
                    if (off->FNamePool.BlocksOff && candidate >= off->FNamePool.BlocksOff)
                        poolBase -= off->FNamePool.BlocksOff;

                    if (!r.exact)
                    {
                        if (weakPool == 0)
                            weakPool = poolBase;
                        return 0; // 宽松命中：先记下，继续找精确命中
                    }

                    LOGI("[Bootstrap] FNamePool @ 0x%lx (slot @ 0x%lx, anchorOff=0x%lx, hits=%d)",
                         static_cast<unsigned long>(poolBase),
                         static_cast<unsigned long>(candidate),
                         static_cast<unsigned long>(r.offset),
                         r.hits);
                    return poolBase;
                };

                auto tryGNames = [&]() -> uintptr_t
                {
                    uintptr_t p0 = vm_rpm_ptr<uintptr_t>((void *)val);
                    uintptr_t p1 = p0 ? vm_rpm_ptr<uintptr_t>((void *)p0) : 0;

                    if (p1)
                    {
                        const AnchorScanResult r =
                            ScanEngineNameAnchors(p1, strBuf1, sizeof(strBuf1),
                                                  kGNamesEntryOffsets, kGNamesEntryOffsetsCount);
                        if (r.hit)
                        {
                            if (!r.exact)
                            {
                                if (weakGNames == 0)
                                    weakGNames = candidate;
                            }
                            else
                            {
                                LOGI("[Bootstrap] GNames @ 0x%lx (anchorOff=0x%lx, hits=%d)",
                                     static_cast<unsigned long>(candidate),
                                     static_cast<unsigned long>(r.offset),
                                     r.hits);
                                return candidate;
                            }
                        }
                    }

                    if (isPubgClassic)
                    {
                        uintptr_t q0 = vm_rpm_ptr<uintptr_t>((void *)(val + 0x110));
                        uintptr_t q1 = q0 ? vm_rpm_ptr<uintptr_t>((void *)q0) : 0;
                        uintptr_t q2 = q1 ? vm_rpm_ptr<uintptr_t>((void *)q1) : 0;
                        uintptr_t q3 = q2 ? vm_rpm_ptr<uintptr_t>((void *)q2) : 0;

                        if (q3)
                        {
                            const AnchorScanResult r =
                                ScanEngineNameAnchors(q3, strBuf2, sizeof(strBuf2),
                                                      kGNamesEntryOffsets, kGNamesEntryOffsetsCount);
                            if (r.hit)
                            {
                                if (!r.exact)
                                {
                                    if (weakGNames == 0)
                                        weakGNames = candidate;
                                }
                                else
                                {
                                    LOGI("[Bootstrap] PUBG-style GNames @ 0x%lx (anchorOff=0x%lx, hits=%d)",
                                         static_cast<unsigned long>(candidate),
                                         static_cast<unsigned long>(r.offset),
                                         r.hits);
                                    return candidate;
                                }
                            }
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

    // 整轮扫完都没有精确命中 → 退而使用宽松命中（若有）
    if (weakPool || weakGNames)
    {
        const uintptr_t result = preferFNamePool
                                     ? (weakPool ? weakPool : weakGNames)
                                     : (weakGNames ? weakGNames : weakPool);

        LOGW("[Bootstrap] 仅命中窗口兜底（非精确偏移）名字表 @ 0x%lx，建议用 probe 结果复核",
             static_cast<unsigned long>(result));
        return result;
    }

    LOGE("[Bootstrap] 自动搜索 GNames/NamePool 失败");
    LOGI("[Bootstrap] 诊断: ELF基址=0x%lx, ELF大小=%zu, 可读段数=%zu",
         (unsigned long)ue_elf.base(), 0, ue_elf.segments().size());
    for (const auto &seg : ue_elf.segments())
        LOGI("[Bootstrap]   段: base=0x%lx size=%zu readable=%d writable=%d",
             (unsigned long)seg.startAddress, seg.length, seg.readable, seg.writeable);
    LOGI("[Bootstrap] 建议: 使用 SCAN_GNAMES 命令手动扫描，或检查游戏是否修改了 ELF");
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
