#include "UECandidateAnalysis.hpp"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cstring>
#include <limits>
#include <mutex>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "MemoryAnalysis.hpp"
#include "../MemoryHelpers.hpp"
#include "../Protocol.hpp"

namespace UmtMcp::Analysis
{
namespace
{
constexpr size_t kChunk = 1024 * 1024;
constexpr size_t kDefaultScanBudget = 64 * 1024 * 1024;
constexpr size_t kMaxCandidateSessions = 32;
constexpr int kCandidatePageSize = 20;

struct NameLayout
{
    uint32_t stride = 2;
    uint32_t blocksBit = 16;
    uint32_t blocksOff = 0x40;
    uint32_t headerOff = 0;
    uint32_t lengthShift = 6;
};

struct NameCandidate
{
    int id = 0;
    uintptr_t poolAddress = 0;
    uintptr_t slotAddress = 0;
    uintptr_t block0 = 0;
    NameLayout layout;
    int score = 0;
    json evidence = json::array();
    json failedChecks = json::array();
};

struct NameSession
{
    pid_t pid = 0;
    uint64_t processStartTime = 0;
    std::string revision;
    std::string id;
    std::vector<NameCandidate> candidates;
    uint64_t scannedBytes = 0;
    uint64_t skippedBytes = 0;
    bool truncated = false;
};

struct ObjectLayout
{
    uint32_t objObjectsOff = 0x10;
    uint32_t objectsOff = 0;
    uint32_t numElementsOff = 0x14;
    bool chunked = true;
    uint32_t numElementsPerChunk = 65536;
    uint32_t itemObjectOff = 0;
    uint32_t itemSize = 0x18;
    uint32_t classPrivateOff = 0x10;
    uint32_t namePrivateOff = 0x18;
    uint32_t outerPrivateOff = 0x20;
};

struct ObjectCandidate
{
    int id = 0;
    uintptr_t arrayAddress = 0;
    uintptr_t objectsAddress = 0;
    int32_t numElements = 0;
    ObjectLayout layout;
    int score = 0;
    json evidence = json::array();
    json failedChecks = json::array();
    std::string namesSessionId;
    int namesCandidateId = -1;
};

struct ObjectSession
{
    pid_t pid = 0;
    uint64_t processStartTime = 0;
    std::string revision;
    std::string id;
    std::vector<ObjectCandidate> candidates;
    uint64_t scannedBytes = 0;
    uint64_t skippedBytes = 0;
    bool truncated = false;
};

std::mutex gCandidateMutex;
std::unordered_map<std::string, NameSession> gNameSessions;
std::unordered_map<std::string, ObjectSession> gObjectSessions;
std::atomic<uint64_t> gCandidateSeq{0};

std::string NewId(const char *prefix)
{
    return std::string(prefix) + "-" + std::to_string(++gCandidateSeq);
}

std::string Confidence(int score)
{
    if (score >= 70) return "HIGH";
    if (score >= 40) return "MEDIUM";
    return "LOW";
}

template <typename T>
bool ReadValue(const KittyMemoryMgr &mgr, uintptr_t address, T &value)
{
    return mgr.readMem(address, &value, sizeof(value)) == sizeof(value);
}

bool IsPrintableName(const std::string &name)
{
    if (name.empty() || name.size() > 127) return false;
    for (unsigned char c : name)
        if (c < 0x20 || c == 0x7F) return false;
    return true;
}

NameLayout ParseNameLayout(const json &value)
{
    NameLayout layout;
    if (!value.is_object()) return layout;
    layout.stride = std::clamp(value.value("stride", 2), 1, 16);
    layout.blocksBit = std::clamp(value.value("blocksBit", 16), 8, 24);
    layout.blocksOff = std::clamp(value.value("blocksOff", 0x40), 0, 0x1000);
    layout.headerOff = std::clamp(value.value("headerOff", 0), 0, 32);
    layout.lengthShift = std::clamp(value.value("lengthShift", 6), 1, 15);
    return layout;
}

json NameLayoutJson(const NameLayout &layout)
{
    return {{"kind", "FNamePool"}, {"stride", layout.stride},
            {"blocksBit", layout.blocksBit}, {"blocksOff", layout.blocksOff},
            {"headerOff", layout.headerOff}, {"lengthShift", layout.lengthShift}};
}

ObjectLayout ParseObjectLayout(const json &value)
{
    ObjectLayout layout;
    if (!value.is_object()) return layout;
    layout.objObjectsOff = std::clamp(value.value("objObjectsOff", 0x10), 0, 0x400);
    layout.objectsOff = std::clamp(value.value("objectsOff", 0), 0, 0x100);
    layout.numElementsOff = std::clamp(value.value("numElementsOff", 0x14), 0, 0x100);
    layout.chunked = value.value("chunked", true);
    layout.numElementsPerChunk = std::clamp(value.value("numElementsPerChunk", 65536), 1, 1048576);
    layout.itemObjectOff = std::clamp(value.value("itemObjectOff", 0), 0, 0x100);
    layout.itemSize = std::clamp(value.value("itemSize", 0x18), 8, 0x100);
    layout.classPrivateOff = std::clamp(value.value("classPrivateOff", 0x10), 0, 0x100);
    if (value.contains("namePrivateOffsets") && value["namePrivateOffsets"].is_array() &&
        !value["namePrivateOffsets"].empty())
        layout.namePrivateOff = std::clamp(value["namePrivateOffsets"][0].get<int>(), 0, 0x100);
    else
        layout.namePrivateOff = std::clamp(value.value("namePrivateOff", 0x18), 0, 0x100);
    layout.outerPrivateOff = std::clamp(value.value("outerPrivateOff", 0x20), 0, 0x100);
    return layout;
}

json ObjectLayoutJson(const ObjectLayout &layout)
{
    return {{"objObjectsOff", layout.objObjectsOff}, {"objectsOff", layout.objectsOff},
            {"numElementsOff", layout.numElementsOff}, {"chunked", layout.chunked},
            {"numElementsPerChunk", layout.numElementsPerChunk},
            {"itemObjectOff", layout.itemObjectOff}, {"itemSize", layout.itemSize},
            {"classPrivateOff", layout.classPrivateOff},
            {"namePrivateOffsets", json::array({layout.namePrivateOff})},
            {"outerPrivateOff", layout.outerPrivateOff}};
}

size_t ParseCandidateCursor(const json &args)
{
    const std::string raw = args.value("cursor", "");
    if (raw.empty()) return 0;
    try
    {
        size_t used = 0;
        const auto value = std::stoull(raw, &used, 10);
        if (used != raw.size()) throw std::invalid_argument("trailing");
        return static_cast<size_t>(value);
    }
    catch (...)
    {
        throw HandlerError(Err::kBadArgs, "cursor 必须是十进制候选偏移");
    }
}

json NameCandidateJson(const NameCandidate &candidate)
{
    return {{"candidateId", candidate.id},
            {"poolAddress", FormatAddress(candidate.poolAddress)},
            {"slotAddress", FormatAddress(candidate.slotAddress)},
            {"valueAddress", FormatAddress(candidate.poolAddress)},
            {"indirection", 0}, {"layout", NameLayoutJson(candidate.layout)},
            {"score", candidate.score}, {"confidence", Confidence(candidate.score)},
            {"evidence", candidate.evidence}, {"failedChecks", candidate.failedChecks},
            {"source", "STRUCTURAL_SCAN"}};
}

json ObjectCandidateJson(const ObjectCandidate &candidate)
{
    return {{"candidateId", candidate.id},
            {"arrayAddress", FormatAddress(candidate.arrayAddress)},
            {"slotAddress", FormatAddress(candidate.arrayAddress)},
            {"valueAddress", FormatAddress(candidate.arrayAddress)},
            {"indirection", 0}, {"objectsAddress", FormatAddress(candidate.objectsAddress)},
            {"numElements", candidate.numElements}, {"layout", ObjectLayoutJson(candidate.layout)},
            {"score", candidate.score}, {"confidence", Confidence(candidate.score)},
            {"evidence", candidate.evidence}, {"failedChecks", candidate.failedChecks},
            {"source", "STRUCTURAL_SCAN"}};
}

template <typename Session, typename Formatter>
json PageCandidates(const Session &session, const json &args, Formatter formatter)
{
    const size_t cursor = ParseCandidateCursor(args);
    if (cursor > session.candidates.size())
        throw HandlerError(Err::kBadArgs, "cursor 超出候选结果范围");
    const size_t limit = static_cast<size_t>(
        std::clamp(args.value("limit", kCandidatePageSize), 1, kCandidatePageSize));
    const size_t end = std::min(session.candidates.size(), cursor + limit);
    json candidates = json::array();
    for (size_t i = cursor; i < end; ++i) candidates.push_back(formatter(session.candidates[i]));
    return {{"sessionId", session.id}, {"pid", session.pid},
            {"processStartTime", std::to_string(session.processStartTime)},
            {"mapRevision", session.revision}, {"candidates", candidates},
            {"returned", candidates.size()}, {"totalCandidates", session.candidates.size()},
            {"scannedBytes", session.scannedBytes}, {"skippedBytes", session.skippedBytes},
            {"truncated", session.truncated},
            {"nextCursor", end < session.candidates.size() ? json(std::to_string(end)) : json(nullptr)}};
}

std::unordered_set<std::string> RequestedMapIds(const json &args)
{
    std::unordered_set<std::string> ids;
    for (const auto &id : args.value("mapIds", json::array()))
        if (id.is_string()) ids.insert(id.get<std::string>());
    return ids;
}

std::string MapId(const KittyMemoryEx::ProcMap &map)
{
    return "map:" + FormatAddress(map.startAddress).substr(2);
}

std::vector<KittyMemoryEx::ProcMap> CandidateMaps(const json &args, const MapSnapshot &snapshot,
                                                   bool names)
{
    const std::string region = args.value("region", names ? "ELF_SEGMENTS" : "MODULE_RW");
    const auto mapIds = RequestedMapIds(args);
    std::vector<KittyMemoryEx::ProcMap> maps;
    for (const auto &map : snapshot.maps)
    {
        if (!map.readable) continue;
        if (!mapIds.empty() && !mapIds.count(MapId(map))) continue;
        const bool ueModule = map.pathname.find("libUE4.so") != std::string::npos ||
                              map.pathname.find("libUnreal.so") != std::string::npos;
        const bool anonymous = map.pathname.empty() || map.pathname.front() == '[';
        bool use = false;
        if (!mapIds.empty()) use = true;
        else if (region == "ALL_READABLE") use = true;
        else if (region == "ELF_SEGMENTS") use = ueModule;
        else if (region == "BSS") use = map.writeable && (anonymous || ueModule);
        else if (region == "MODULE_RW") use = map.writeable && ueModule;
        else throw HandlerError(Err::kBadArgs, "未知 region: " + region);
        if (use) maps.push_back(map);
    }
    if (maps.empty()) throw HandlerError(Err::kNotFound, "候选扫描范围内没有可读映射");
    return maps;
}

bool DecodeNameAt(const KittyMemoryMgr &mgr, uintptr_t block, uint32_t offsetUnits,
                  const NameLayout &layout, std::string &name, std::string &failure)
{
    const uintptr_t entry = block + static_cast<uintptr_t>(offsetUnits) * layout.stride;
    uint16_t header = 0;
    if (!ReadValue(mgr, entry + layout.headerOff, header))
    {
        failure = "entry header unreadable";
        return false;
    }
    const size_t length = header >> layout.lengthShift;
    const bool wide = (header & 1) != 0;
    if (length < 1 || length > 127)
    {
        failure = "entry length out of range";
        return false;
    }
    const uintptr_t text = entry + layout.headerOff + sizeof(header);
    if (!wide)
    {
        std::vector<char> bytes(length);
        if (mgr.readMem(text, bytes.data(), length) != length)
        {
            failure = "entry bytes unreadable";
            return false;
        }
        name.assign(bytes.begin(), bytes.end());
    }
    else
    {
        std::vector<uint16_t> chars(length);
        if (mgr.readMem(text, chars.data(), length * sizeof(uint16_t)) != length * sizeof(uint16_t))
        {
            failure = "wide entry bytes unreadable";
            return false;
        }
        name.clear();
        for (uint16_t ch : chars)
        {
            if (ch > 0x7F) { failure = "wide non-ASCII sample"; return false; }
            name.push_back(static_cast<char>(ch));
        }
    }
    if (!IsPrintableName(name))
    {
        failure = "entry text is not printable";
        return false;
    }
    return true;
}

bool DecodeNameId(const KittyMemoryMgr &mgr, const NameCandidate &candidate, int32_t id,
                  std::string &name, std::string &failure)
{
    if (id < 0) { failure = "negative name id"; return false; }
    const uint32_t blockIndex = static_cast<uint32_t>(id) >> candidate.layout.blocksBit;
    const uint32_t offset = static_cast<uint32_t>(id) & ((1U << candidate.layout.blocksBit) - 1U);
    uintptr_t block = 0;
    if (!ReadValue(mgr, candidate.poolAddress + candidate.layout.blocksOff +
                         static_cast<uintptr_t>(blockIndex) * sizeof(uintptr_t), block) || !block)
    {
        failure = "block pointer unreadable";
        return false;
    }
    return DecodeNameAt(mgr, block, offset, candidate.layout, name, failure);
}

NameCandidate ValidateNameCandidate(const KittyMemoryMgr &mgr, const MapSnapshot &snapshot,
                                    uintptr_t pool, uintptr_t slot, const NameLayout &layout,
                                    const std::vector<uint32_t> &anchorOffsets,
                                    const std::unordered_set<std::string> &anchorNames)
{
    NameCandidate candidate;
    candidate.poolAddress = pool;
    candidate.slotAddress = slot;
    candidate.layout = layout;
    if (IsReadableAddress(snapshot, pool, layout.blocksOff + sizeof(uintptr_t)))
    {
        candidate.score += 5;
        candidate.evidence.push_back("pool address is readable");
    }
    else
    {
        candidate.failedChecks.push_back("pool address is not readable");
        return candidate;
    }
    if (!ReadValue(mgr, pool + layout.blocksOff, candidate.block0) ||
        !IsReadableAddress(snapshot, candidate.block0, 4))
    {
        candidate.failedChecks.push_back("Blocks[0] is unreadable");
        return candidate;
    }
    candidate.score += 10;
    candidate.evidence.push_back({{"check", "Blocks[0] readable"},
                                  {"address", FormatAddress(candidate.block0)}});

    int valid = 0, anchors = 0;
    for (uint32_t offset : anchorOffsets)
    {
        std::string name, failure;
        if (!DecodeNameAt(mgr, candidate.block0, offset, layout, name, failure)) continue;
        ++valid;
        if (candidate.evidence.size() < 5)
            candidate.evidence.push_back({{"offset", offset}, {"name", name}});
        if (name == "None") candidate.score += 15;
        if (anchorNames.count(name)) { ++anchors; candidate.score += 8; }
    }
    if (valid >= 3) candidate.score += 15;
    else candidate.failedChecks.push_back("fewer than three anchor offsets decoded");
    if (anchors >= 3) candidate.score += 20;
    else if (anchors == 0) candidate.failedChecks.push_back("no requested anchor name matched");
    return candidate;
}

const NameCandidate &GetNameCandidate(const std::string &sessionId, int candidateId,
                                      pid_t pid, uint64_t processStartTime,
                                      const std::string &revision)
{
    auto it = gNameSessions.find(sessionId);
    if (it == gNameSessions.end() || it->second.pid != pid ||
        it->second.processStartTime != processStartTime)
        throw HandlerError(Err::kSessionStale, "names candidate session 不存在或进程已切换");
    if (it->second.revision != revision)
        throw HandlerError(Err::kMapStale, "names candidate session 的 maps revision 已变化");
    for (const auto &candidate : it->second.candidates)
        if (candidate.id == candidateId) return candidate;
    throw HandlerError(Err::kSessionStale, "names candidateId 不存在");
}

const ObjectCandidate &GetObjectCandidate(const std::string &sessionId, int candidateId,
                                          pid_t pid, uint64_t processStartTime,
                                          const std::string &revision)
{
    auto it = gObjectSessions.find(sessionId);
    if (it == gObjectSessions.end() || it->second.pid != pid ||
        it->second.processStartTime != processStartTime)
        throw HandlerError(Err::kSessionStale, "objects candidate session 不存在或进程已切换");
    if (it->second.revision != revision)
        throw HandlerError(Err::kMapStale, "objects candidate session 的 maps revision 已变化");
    for (const auto &candidate : it->second.candidates)
        if (candidate.id == candidateId) return candidate;
    throw HandlerError(Err::kSessionStale, "objects candidateId 不存在");
}

bool ReadObjectAt(const KittyMemoryMgr &mgr, const ObjectCandidate &candidate, int32_t index,
                  uintptr_t &object)
{
    const ObjectLayout &layout = candidate.layout;
    uintptr_t itemBase = candidate.objectsAddress;
    if (layout.chunked)
    {
        const uint32_t chunkIndex = static_cast<uint32_t>(index) / layout.numElementsPerChunk;
        const uint32_t within = static_cast<uint32_t>(index) % layout.numElementsPerChunk;
        uintptr_t chunk = 0;
        if (!ReadValue(mgr, candidate.objectsAddress + chunkIndex * sizeof(uintptr_t), chunk) || !chunk)
            return false;
        itemBase = chunk + static_cast<uintptr_t>(within) * layout.itemSize;
    }
    else
        itemBase += static_cast<uintptr_t>(index) * layout.itemSize;
    return ReadValue(mgr, itemBase + layout.itemObjectOff, object) && object != 0;
}

ObjectCandidate ValidateObjectCandidate(const KittyMemoryMgr &mgr, const MapSnapshot &snapshot,
                                        uintptr_t array, const ObjectLayout &layout)
{
    ObjectCandidate candidate;
    candidate.arrayAddress = array;
    candidate.layout = layout;
    const uintptr_t objObjects = array + layout.objObjectsOff;
    if (!ReadValue(mgr, objObjects + layout.objectsOff, candidate.objectsAddress) ||
        !IsReadableAddress(snapshot, candidate.objectsAddress, sizeof(uintptr_t)))
    {
        candidate.failedChecks.push_back("Objects/Chunks pointer unreadable");
        return candidate;
    }
    candidate.score += 15;
    candidate.evidence.push_back({{"check", "Objects/Chunks pointer readable"},
                                  {"address", FormatAddress(candidate.objectsAddress)}});
    if (!ReadValue(mgr, objObjects + layout.numElementsOff, candidate.numElements) ||
        candidate.numElements < 1024 || candidate.numElements > 5000000)
    {
        candidate.failedChecks.push_back("NumElements outside 1024..5000000");
        return candidate;
    }
    candidate.score += 10;
    candidate.evidence.push_back({{"check", "NumElements plausible"},
                                  {"value", candidate.numElements}});

    int readableObjects = 0, readableClasses = 0;
    for (int32_t i = 0; i < std::min<int32_t>(candidate.numElements, 32); ++i)
    {
        uintptr_t object = 0;
        if (!ReadObjectAt(mgr, candidate, i, object) || !IsReadableAddress(snapshot, object, 8)) continue;
        ++readableObjects;
        uintptr_t klass = 0;
        if (ReadValue(mgr, object + layout.classPrivateOff, klass) && IsReadableAddress(snapshot, klass, 8))
            ++readableClasses;
    }
    if (readableObjects >= 3)
    {
        candidate.score += 15;
        candidate.evidence.push_back({{"check", "readable UObject samples"}, {"count", readableObjects}});
    }
    else candidate.failedChecks.push_back("fewer than three readable UObject samples");
    if (readableClasses >= 2)
    {
        candidate.score += 10;
        candidate.evidence.push_back({{"check", "readable ClassPrivate samples"}, {"count", readableClasses}});
    }
    else candidate.failedChecks.push_back("fewer than two readable ClassPrivate samples");
    return candidate;
}

void EnhanceObjectCandidateWithNames(const KittyMemoryMgr &mgr, ObjectCandidate &candidate,
                                     const NameCandidate &names)
{
    int decoded = 0;
    int anchors = 0;
    for (int32_t i = 0; i < std::min<int32_t>(candidate.numElements, 32); ++i)
    {
        uintptr_t object = 0;
        if (!ReadObjectAt(mgr, candidate, i, object)) continue;
        int32_t nameId = -1;
        if (!ReadValue(mgr, object + candidate.layout.namePrivateOff, nameId)) continue;
        std::string name, failure;
        if (!DecodeNameId(mgr, names, nameId, name, failure)) continue;
        ++decoded;
        if (name == "Object" || name == "Package" || name == "Class" ||
            name.find("CoreUObject") != std::string::npos)
            ++anchors;
        if (candidate.evidence.size() < 5)
            candidate.evidence.push_back({{"check", "name decoded"}, {"index", i}, {"name", name}});
    }
    if (decoded >= 3) candidate.score += 10;
    else candidate.failedChecks.push_back("fewer than three UObject names decoded");
    if (anchors > 0)
    {
        candidate.score += 20;
        if (candidate.evidence.size() < 5)
            candidate.evidence.push_back({{"check", "CoreUObject anchor names"}, {"count", anchors}});
    }
    else candidate.failedChecks.push_back("no Object/Package/Class/CoreUObject anchor name");
}
}

json ScanGNamesCandidates(const json &args, const KittyMemoryMgr &mgr, const std::atomic<bool> *cancelFlag)
{
    if (!mgr.isMemValid()) throw HandlerError(Err::kNotAttached, "未 attach 到目标进程");
    const MapSnapshot snapshot = CaptureMaps(mgr);
    const std::string existingId = args.value("sessionId", "");
    if (!existingId.empty())
    {
        std::lock_guard<std::mutex> lock(gCandidateMutex);
        auto it = gNameSessions.find(existingId);
        if (it == gNameSessions.end() || it->second.pid != snapshot.pid ||
            it->second.processStartTime != snapshot.processStartTime)
            throw HandlerError(Err::kSessionStale, "names candidate session 不存在或进程已切换");
        if (it->second.revision != snapshot.revision)
            throw HandlerError(Err::kMapStale, "names candidate session 的 maps revision 已变化");
        return PageCandidates(it->second, args, NameCandidateJson);
    }
    const auto maps = CandidateMaps(args, snapshot, true);
    std::vector<NameLayout> layouts;
    for (const auto &item : args.value("layouts", json::array())) layouts.push_back(ParseNameLayout(item));
    if (layouts.empty()) layouts.push_back({});
    std::vector<uint32_t> anchorOffsets = {0, 2, 4, 6, 8, 10, 12, 16};
    if (args.contains("anchorOffsets") && args["anchorOffsets"].is_array())
    {
        anchorOffsets.clear();
        for (const auto &off : args["anchorOffsets"])
            if (off.is_number_unsigned() || off.is_number_integer()) anchorOffsets.push_back(off.get<uint32_t>());
    }
    std::unordered_set<std::string> anchorNames = {"None", "ByteProperty", "IntProperty", "Object"};
    for (const auto &name : args.value("anchorNames", json::array()))
        if (name.is_string()) anchorNames.insert(name.get<std::string>());
    const int maxCandidates = std::clamp(args.value("maxCandidates", 50), 1, 200);
    const size_t budget = static_cast<size_t>(std::clamp<int64_t>(
        args.value("maxScanBytes", static_cast<int64_t>(kDefaultScanBudget)), 4096, 256LL * 1024 * 1024));
    uintptr_t minPtr = 0;
    uintptr_t maxPtr = std::numeric_limits<uintptr_t>::max();
    if (args.contains("minPtr") && !ParseAddress(args.value("minPtr", ""), minPtr))
        throw HandlerError(Err::kBadArgs, "minPtr 地址格式无效");
    if (args.contains("maxPtr") && !ParseAddress(args.value("maxPtr", ""), maxPtr))
        throw HandlerError(Err::kBadArgs, "maxPtr 地址格式无效");
    if (maxPtr <= minPtr) throw HandlerError(Err::kBadArgs, "maxPtr 须大于 minPtr");

    NameSession session;
    session.pid = snapshot.pid;
    session.processStartTime = snapshot.processStartTime;
    session.revision = snapshot.revision;
    session.id = NewId("names");
    std::unordered_set<std::string> seen;
    size_t scanned = 0, skipped = 0;
    bool truncated = false;
    std::vector<uint8_t> buffer(kChunk);
    for (const auto &map : maps)
    {
        for (uintptr_t cursor = map.startAddress; cursor < map.endAddress && scanned < budget; cursor += kChunk)
        {
            if (cancelFlag && cancelFlag->load()) throw HandlerError(Err::kCancelled, "FNamePool 候选扫描已取消");
            const size_t size = std::min<size_t>(kChunk, map.endAddress - cursor);
            const size_t got = mgr.readMem(cursor, buffer.data(), size);
            if (got < sizeof(uintptr_t)) { skipped += size; continue; }
            scanned += size;
            for (size_t off = 0; off + sizeof(uintptr_t) <= got; off += sizeof(uintptr_t))
            {
                uintptr_t value = 0;
                std::memcpy(&value, buffer.data() + off, sizeof(value));
                if (!IsReadableAddress(snapshot, value, 4)) continue;
                const uintptr_t slot = cursor + off;
                for (const auto &layout : layouts)
                {
                    uintptr_t pool = slot >= layout.blocksOff ? slot - layout.blocksOff : 0;
                    if (pool < minPtr || pool >= maxPtr) continue;
                    std::ostringstream key;
                    key << std::hex << pool << ':' << layout.stride << ':' << layout.blocksBit
                        << ':' << layout.blocksOff << ':' << layout.headerOff << ':' << layout.lengthShift;
                    if (!pool || !seen.insert(key.str()).second) continue;
                    NameCandidate candidate = ValidateNameCandidate(mgr, snapshot, pool, slot, layout,
                                                                    anchorOffsets, anchorNames);
                    if (candidate.score < 15) continue;
                    candidate.id = static_cast<int>(session.candidates.size());
                    session.candidates.push_back(std::move(candidate));
                    if (static_cast<int>(session.candidates.size()) >= maxCandidates)
                    {
                        truncated = true;
                        break;
                    }
                }
                if (truncated) break;
            }
            if (truncated) break;
        }
        if (truncated || scanned >= budget) break;
    }
    std::sort(session.candidates.begin(), session.candidates.end(),
              [](const NameCandidate &a, const NameCandidate &b) { return a.score > b.score; });
    if (CurrentMapRevision(mgr) != snapshot.revision)
        throw HandlerError(Err::kMapStale, "FNamePool 候选扫描期间 maps revision 已变化");
    for (size_t i = 0; i < session.candidates.size(); ++i) session.candidates[i].id = static_cast<int>(i);
    session.scannedBytes = scanned;
    session.skippedBytes = skipped;
    session.truncated = truncated || scanned >= budget;

    {
        std::lock_guard<std::mutex> lock(gCandidateMutex);
        if (gNameSessions.size() >= kMaxCandidateSessions) gNameSessions.erase(gNameSessions.begin());
        gNameSessions[session.id] = session;
    }
    return PageCandidates(session, args, NameCandidateJson);
}

json SampleGNamesCandidate(const json &args, const KittyMemoryMgr &mgr)
{
    if (!mgr.isMemValid()) throw HandlerError(Err::kNotAttached, "未 attach 到目标进程");
    const MapSnapshot snapshot = CaptureMaps(mgr);
    const std::string sessionId = args.value("sessionId", "");
    const int candidateId = args.value("candidateId", -1);
    const int32_t start = args.value("startIndex", 0);
    const int32_t count = std::clamp(args.value("count", 32), 1, 200);
    if (start < 0) throw HandlerError(Err::kBadArgs, "startIndex 须 >= 0");
    std::lock_guard<std::mutex> lock(gCandidateMutex);
    const NameCandidate &candidate = GetNameCandidate(sessionId, candidateId, snapshot.pid,
                                                       snapshot.processStartTime, snapshot.revision);
    json samples = json::array(), errors = json::array();
    for (int32_t id = start; id < start + count; ++id)
    {
        std::string name, failure;
        const bool valid = DecodeNameId(mgr, candidate, id, name, failure);
        samples.push_back({{"index", id}, {"name", name}, {"valid", valid}});
        if (!valid && errors.size() < 16) errors.push_back({{"index", id}, {"reason", failure}});
    }
    return {{"sessionId", sessionId}, {"candidateId", candidateId},
            {"poolAddress", FormatAddress(candidate.poolAddress)}, {"layout", NameLayoutJson(candidate.layout)},
            {"startIndex", start}, {"count", count}, {"samples", samples}, {"readErrors", errors}};
}

json ScanObjectCandidates(const json &args, const KittyMemoryMgr &mgr, const std::atomic<bool> *cancelFlag)
{
    if (!mgr.isMemValid()) throw HandlerError(Err::kNotAttached, "未 attach 到目标进程");
    const MapSnapshot snapshot = CaptureMaps(mgr);
    const std::string existingId = args.value("sessionId", "");
    if (!existingId.empty())
    {
        std::lock_guard<std::mutex> lock(gCandidateMutex);
        auto it = gObjectSessions.find(existingId);
        if (it == gObjectSessions.end() || it->second.pid != snapshot.pid ||
            it->second.processStartTime != snapshot.processStartTime)
            throw HandlerError(Err::kSessionStale, "objects candidate session 不存在或进程已切换");
        if (it->second.revision != snapshot.revision)
            throw HandlerError(Err::kMapStale, "objects candidate session 的 maps revision 已变化");
        return PageCandidates(it->second, args, ObjectCandidateJson);
    }
    const auto maps = CandidateMaps(args, snapshot, false);
    std::vector<ObjectLayout> layouts;
    for (const auto &item : args.value("layouts", json::array())) layouts.push_back(ParseObjectLayout(item));
    if (layouts.empty())
    {
        layouts.push_back({});
        ObjectLayout flat;
        flat.chunked = false;
        layouts.push_back(flat);
    }
    const int maxCandidates = std::clamp(args.value("maxCandidates", 50), 1, 200);
    const size_t budget = static_cast<size_t>(std::clamp<int64_t>(
        args.value("maxDistanceBytes", static_cast<int64_t>(kDefaultScanBudget)), 4096, 256LL * 1024 * 1024));

    const std::string direction = args.value("direction", "REGION");
    uintptr_t directionalStart = 0;
    uintptr_t directionalEnd = std::numeric_limits<uintptr_t>::max();
    if (direction != "REGION")
    {
        if (direction != "UP" && direction != "DOWN" && direction != "BOTH")
            throw HandlerError(Err::kBadArgs, "direction 必须是 UP/DOWN/BOTH/REGION");
        uintptr_t origin = 0;
        if (!ParseAddress(args.value("origin", ""), origin))
            throw HandlerError(Err::kBadArgs, "UP/DOWN/BOTH 必须提供有效 origin");
        if (direction == "UP" || direction == "BOTH")
            directionalStart = origin > budget ? origin - budget : 0;
        else directionalStart = origin;
        if (direction == "DOWN" || direction == "BOTH")
            directionalEnd = origin > std::numeric_limits<uintptr_t>::max() - budget
                ? std::numeric_limits<uintptr_t>::max() : origin + budget;
        else directionalEnd = origin;
    }

    ObjectSession session;
    session.pid = snapshot.pid;
    session.processStartTime = snapshot.processStartTime;
    session.revision = snapshot.revision;
    session.id = NewId("objects");
    const std::string namesSessionId = args.value("namesSessionId", "");
    const int namesCandidateId = args.value("namesCandidateId", -1);
    std::optional<NameCandidate> namesCandidate;
    if (!namesSessionId.empty() || namesCandidateId >= 0)
    {
        if (namesSessionId.empty() || namesCandidateId < 0)
            throw HandlerError(Err::kBadArgs, "namesSessionId 与 namesCandidateId 必须同时提供");
        std::lock_guard<std::mutex> lock(gCandidateMutex);
        namesCandidate = GetNameCandidate(namesSessionId, namesCandidateId,
                                          snapshot.pid, snapshot.processStartTime, snapshot.revision);
    }
    size_t scanned = 0, skipped = 0;
    bool truncated = false;
    std::unordered_set<std::string> seen;
    std::vector<uint8_t> buffer(kChunk + 0x500);
    for (const auto &map : maps)
    {
        const uintptr_t mapStart = std::max(static_cast<uintptr_t>(map.startAddress), directionalStart);
        const uintptr_t mapEnd = std::min(static_cast<uintptr_t>(map.endAddress), directionalEnd);
        if (mapEnd <= mapStart) continue;
        for (uintptr_t cursor = mapStart; cursor < mapEnd && scanned < budget; cursor += kChunk)
        {
            if (cancelFlag && cancelFlag->load())
                throw HandlerError(Err::kCancelled, "GUObjectArray 候选扫描已取消");
            const size_t primary = std::min<size_t>(kChunk, mapEnd - cursor);
            const size_t overlap = std::min<size_t>(0x500, mapEnd - cursor - primary);
            const size_t got = mgr.readMem(cursor, buffer.data(), primary + overlap);
            if (got < 0x20) { skipped += primary; continue; }
            scanned += primary;
            for (size_t local = 0; local + 0x20 <= primary; local += sizeof(uintptr_t))
            {
                const uintptr_t address = cursor + local;
                for (const auto &layout : layouts)
                {
                    const size_t objectsPos = local + layout.objObjectsOff + layout.objectsOff;
                    const size_t countPos = local + layout.objObjectsOff + layout.numElementsOff;
                    if (objectsPos + sizeof(uintptr_t) > got || countPos + sizeof(int32_t) > got) continue;
                    uintptr_t objects = 0;
                    int32_t count = 0;
                    std::memcpy(&objects, buffer.data() + objectsPos, sizeof(objects));
                    std::memcpy(&count, buffer.data() + countPos, sizeof(count));
                    if (count < 1024 || count > 5000000 ||
                        !IsReadableAddress(snapshot, objects, sizeof(uintptr_t)))
                        continue;
                    std::ostringstream key;
                    key << std::hex << address << ':' << layout.objObjectsOff << ':' << layout.objectsOff
                        << ':' << layout.numElementsOff << ':' << layout.chunked << ':'
                        << layout.numElementsPerChunk << ':' << layout.itemObjectOff << ':' << layout.itemSize
                        << ':' << layout.classPrivateOff << ':' << layout.namePrivateOff << ':' << layout.outerPrivateOff;
                    if (!seen.insert(key.str()).second) continue;
                    ObjectCandidate candidate = ValidateObjectCandidate(mgr, snapshot, address, layout);
                    if (candidate.score < 25) continue;
                    if (namesCandidate) EnhanceObjectCandidateWithNames(mgr, candidate, *namesCandidate);
                    candidate.id = static_cast<int>(session.candidates.size());
                    candidate.namesSessionId = namesSessionId;
                    candidate.namesCandidateId = namesCandidateId;
                    session.candidates.push_back(std::move(candidate));
                    if (static_cast<int>(session.candidates.size()) >= maxCandidates)
                    {
                        truncated = true;
                        break;
                    }
                }
                if (truncated) break;
            }
            if (truncated) break;
        }
        if (truncated || scanned >= budget) break;
    }
    std::sort(session.candidates.begin(), session.candidates.end(),
              [](const ObjectCandidate &a, const ObjectCandidate &b) { return a.score > b.score; });
    if (CurrentMapRevision(mgr) != snapshot.revision)
        throw HandlerError(Err::kMapStale, "GUObjectArray 候选扫描期间 maps revision 已变化");
    for (size_t i = 0; i < session.candidates.size(); ++i) session.candidates[i].id = static_cast<int>(i);
    session.scannedBytes = scanned;
    session.skippedBytes = skipped;
    session.truncated = truncated || scanned >= budget;
    {
        std::lock_guard<std::mutex> lock(gCandidateMutex);
        if (gObjectSessions.size() >= kMaxCandidateSessions) gObjectSessions.erase(gObjectSessions.begin());
        gObjectSessions[session.id] = session;
    }
    return PageCandidates(session, args, ObjectCandidateJson);
}

json SampleObjectCandidate(const json &args, const KittyMemoryMgr &mgr)
{
    if (!mgr.isMemValid()) throw HandlerError(Err::kNotAttached, "未 attach 到目标进程");
    const MapSnapshot snapshot = CaptureMaps(mgr);
    const std::string sessionId = args.value("sessionId", "");
    const int candidateId = args.value("candidateId", -1);
    const int32_t start = args.value("startIndex", 0);
    const int32_t count = std::clamp(args.value("count", 32), 1, 200);
    std::lock_guard<std::mutex> lock(gCandidateMutex);
    const ObjectCandidate &candidate = GetObjectCandidate(sessionId, candidateId, snapshot.pid,
                                                          snapshot.processStartTime, snapshot.revision);
    if (start < 0 || start >= candidate.numElements)
        throw HandlerError(Err::kBadArgs, "startIndex 越界");

    const NameCandidate *names = nullptr;
    if (!candidate.namesSessionId.empty() && candidate.namesCandidateId >= 0)
    {
        try { names = &GetNameCandidate(candidate.namesSessionId, candidate.namesCandidateId,
                                        snapshot.pid, snapshot.processStartTime, snapshot.revision); }
        catch (const HandlerError &) { names = nullptr; }
    }
    json samples = json::array(), errors = json::array();
    for (int32_t index = start; index < std::min(candidate.numElements, start + count); ++index)
    {
        uintptr_t object = 0, klass = 0, outer = 0;
        int32_t nameId = -1;
        const bool gotObject = ReadObjectAt(mgr, candidate, index, object);
        bool valid = gotObject && IsReadableAddress(snapshot, object, 8);
        if (valid)
        {
            ReadValue(mgr, object + candidate.layout.classPrivateOff, klass);
            ReadValue(mgr, object + candidate.layout.outerPrivateOff, outer);
            ReadValue(mgr, object + candidate.layout.namePrivateOff, nameId);
        }
        std::string name, failure;
        if (valid && names) DecodeNameId(mgr, *names, nameId, name, failure);
        samples.push_back({{"index", index},
                           {"objectAddress", gotObject ? json(FormatAddress(object)) : json(nullptr)},
                           {"nameId", nameId}, {"name", name},
                           {"classAddress", klass ? json(FormatAddress(klass)) : json(nullptr)},
                           {"outerAddress", outer ? json(FormatAddress(outer)) : json(nullptr)},
                           {"valid", valid}});
        if (!valid && errors.size() < 16) errors.push_back({{"index", index}, {"reason", "object unreadable"}});
    }
    return {{"sessionId", sessionId}, {"candidateId", candidateId},
            {"arrayAddress", FormatAddress(candidate.arrayAddress)}, {"numElements", candidate.numElements},
            {"layout", ObjectLayoutJson(candidate.layout)}, {"samples", samples}, {"readErrors", errors}};
}

bool ValidateCandidateBinding(const std::string &kind, const std::string &sessionId,
                              int candidateId, pid_t pid, uint64_t processStartTime,
                              const std::string &mapRevision,
                              uintptr_t address, std::string &reason)
{
    std::lock_guard<std::mutex> lock(gCandidateMutex);
    try
    {
        if (kind == "names")
        {
            const NameCandidate &candidate = GetNameCandidate(sessionId, candidateId, pid,
                                                               processStartTime, mapRevision);
            if (candidate.poolAddress != address) { reason = "candidate address mismatch"; return false; }
            return true;
        }
        if (kind == "objects")
        {
            const ObjectCandidate &candidate = GetObjectCandidate(sessionId, candidateId, pid,
                                                                   processStartTime, mapRevision);
            if (candidate.arrayAddress != address) { reason = "candidate address mismatch"; return false; }
            return true;
        }
        reason = "unknown candidate kind";
        return false;
    }
    catch (const HandlerError &error)
    {
        reason = error.what();
        return false;
    }
}

void InvalidateCandidateSessions()
{
    std::lock_guard<std::mutex> lock(gCandidateMutex);
    gNameSessions.clear();
    gObjectSessions.clear();
}
}
