#include "MemoryAnalysis.hpp"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <limits>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

#include "../Arm64Disasm.hpp"
#include "../MemoryHelpers.hpp"
#include "../Protocol.hpp"

namespace UmtMcp::Analysis
{
namespace
{
constexpr size_t kScanChunk = 1024 * 1024;
constexpr size_t kMaxStoredHits = 5000;
constexpr int kMaxPageResults = 20;
constexpr size_t kMaxSessions = 32;

struct ScanRange
{
    uintptr_t start = 0;
    uintptr_t end = 0;
    KittyMemoryEx::ProcMap map;
    uintptr_t moduleBase = 0;
};

struct HitSession
{
    pid_t pid = 0;
    uint64_t processStartTime = 0;
    std::string revision;
    std::string kind;
    std::string queryType;
    std::vector<uintptr_t> hits;
    uint64_t scannedBytes = 0;
    uint64_t skippedBytes = 0;
    json readErrors = json::array();
    bool truncated = false;
};

struct ReferenceSession
{
    pid_t pid = 0;
    uint64_t processStartTime = 0;
    std::string revision;
    uintptr_t target = 0;
    std::vector<json> references;
    uint64_t scannedBytes = 0;
    uint64_t skippedBytes = 0;
    bool truncated = false;
};

std::mutex gSessionMutex;
std::unordered_map<std::string, HitSession> gSessions;
std::unordered_map<std::string, ReferenceSession> gReferenceSessions;
std::atomic<uint64_t> gSessionSeq{0};

uint64_t ReadProcessStartTime(pid_t pid)
{
    std::ifstream input("/proc/" + std::to_string(pid) + "/stat");
    std::string line;
    if (!std::getline(input, line)) return 0;
    const size_t close = line.rfind(')');
    if (close == std::string::npos || close + 2 >= line.size()) return 0;
    std::istringstream fields(line.substr(close + 2));
    std::string field;
    // The suffix begins at field 3 (state); process start time is field 22.
    for (int number = 3; number <= 22; ++number)
    {
        if (!(fields >> field)) return 0;
        if (number == 22)
        {
            try { return std::stoull(field); }
            catch (...) { return 0; }
        }
    }
    return 0;
}

void StoreHitSession(const std::string &id, const HitSession &session)
{
    std::lock_guard<std::mutex> lock(gSessionMutex);
    if (gSessions.size() >= kMaxSessions) gSessions.erase(gSessions.begin());
    gSessions[id] = session;
}

void StoreReferenceSession(const std::string &id, const ReferenceSession &session)
{
    std::lock_guard<std::mutex> lock(gSessionMutex);
    if (gReferenceSessions.size() >= kMaxSessions) gReferenceSessions.erase(gReferenceSessions.begin());
    gReferenceSessions[id] = session;
}

uint64_t Fnv1a(uint64_t hash, const void *data, size_t size)
{
    const auto *p = static_cast<const uint8_t *>(data);
    for (size_t i = 0; i < size; ++i)
    {
        hash ^= p[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t FnvString(uint64_t hash, const std::string &s)
{
    return Fnv1a(hash, s.data(), s.size());
}

std::string Hex(uint64_t value)
{
    std::ostringstream out;
    out << std::hex << std::nouppercase << value;
    return out.str();
}

std::string Lower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string BaseName(const std::string &path)
{
    const size_t slash = path.find_last_of('/');
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

bool IsAnonymousPath(const std::string &path)
{
    return path.empty() || path.front() == '[';
}

std::string MapId(const KittyMemoryEx::ProcMap &map)
{
    return "map:" + Hex(map.startAddress);
}

std::string ModuleId(const std::string &path)
{
    return "module:" + Hex(FnvString(1469598103934665603ULL, path));
}

std::string Perms(const KittyMemoryEx::ProcMap &map)
{
    return FormatPermissions(map.readable, map.writeable, map.executable);
}

bool PermissionMatches(const KittyMemoryEx::ProcMap &map, const json &values)
{
    if (!values.is_array() || values.empty())
        return true;
    const std::string actual = Perms(map);
    for (const auto &entry : values)
    {
        if (!entry.is_string()) continue;
        std::string wanted = entry.get<std::string>();
        if (wanted.size() > 3) wanted.resize(3);
        if (wanted == actual) return true;
    }
    return false;
}

uintptr_t ModuleBaseFor(const MapSnapshot &snapshot, const KittyMemoryEx::ProcMap &map)
{
    if (map.pathname.empty()) return 0;
    uintptr_t base = std::numeric_limits<uintptr_t>::max();
    for (const auto &candidate : snapshot.maps)
        if (candidate.pathname == map.pathname)
        {
            const uintptr_t start = static_cast<uintptr_t>(candidate.startAddress);
            if (candidate.offset == 0 || base == std::numeric_limits<uintptr_t>::max())
                base = std::min<uintptr_t>(base, start);
        }
    return base == std::numeric_limits<uintptr_t>::max() ? 0 : base;
}

const KittyMemoryEx::ProcMap *FindMap(const MapSnapshot &snapshot, uintptr_t address)
{
    for (const auto &map : snapshot.maps)
        if (map.contains(address)) return &map;
    return nullptr;
}

json HitLocation(const MapSnapshot &snapshot, uintptr_t address)
{
    json out = {{"address", FormatAddress(address)}};
    const auto *map = FindMap(snapshot, address);
    if (!map)
    {
        out["mapId"] = nullptr;
        out["module"] = nullptr;
        out["moduleOffset"] = nullptr;
        return out;
    }
    const uintptr_t moduleBase = ModuleBaseFor(snapshot, *map);
    out["mapId"] = MapId(*map);
    out["module"] = map->pathname.empty() ? json(nullptr) : json(BaseName(map->pathname));
    out["moduleOffset"] = moduleBase ? json(FormatAddress(address - moduleBase)) : json(nullptr);
    return out;
}

uintptr_t ParseRequiredAddress(const json &args, const char *key)
{
    uintptr_t out = 0;
    const std::string raw = args.value(key, "");
    if (!ParseAddress(raw, out))
        throw HandlerError(Err::kBadArgs, std::string(key) + " 地址格式无效: " + raw);
    return out;
}

size_t ParseNamedCursor(const json &args, const char *key)
{
    const std::string raw = args.value(key, "");
    if (raw.empty()) return 0;
    try
    {
        size_t used = 0;
        const unsigned long long value = std::stoull(raw, &used, 10);
        if (used != raw.size()) throw std::invalid_argument("trailing");
        return static_cast<size_t>(value);
    }
    catch (...)
    {
        throw HandlerError(Err::kBadArgs, std::string(key) + " 必须是十进制结果偏移");
    }
}

size_t ParseCursor(const json &args)
{
    return ParseNamedCursor(args, "cursor");
}

std::vector<ScanRange> SelectRanges(const json &args, const MapSnapshot &snapshot,
                                    const char *permissionKey, bool requireScope)
{
    const std::string module = args.value("module", "");
    const json mapIds = args.value("mapIds", json::array());
    const bool hasMapIds = mapIds.is_array() && !mapIds.empty();
    const bool hasStart = args.contains("start") && args["start"].is_string() &&
                          !args["start"].get<std::string>().empty();
    const bool hasEnd = args.contains("end") && args["end"].is_string() &&
                        !args["end"].get<std::string>().empty();
    const bool hasAddressRange = hasStart || hasEnd;
    const int sources = (!module.empty() ? 1 : 0) + (hasMapIds ? 1 : 0) + (hasAddressRange ? 1 : 0);
    if (sources > 1)
        throw HandlerError(Err::kBadArgs, "mapIds、module、start/end 三种范围来源互斥");
    if (hasAddressRange && (!hasStart || !hasEnd))
        throw HandlerError(Err::kBadArgs, "start 与 end 必须同时提供");
    if (requireScope && sources == 0)
        throw HandlerError(Err::kBadArgs, "需提供 mapIds、module 或 start+end");

    json permissions = args.value(permissionKey, json::array());
    if (permissions.empty() && std::string(permissionKey) != "permissions")
        permissions = args.value("permissions", json::array());

    std::unordered_set<std::string> wantedIds;
    if (hasMapIds)
        for (const auto &id : mapIds)
            if (id.is_string()) wantedIds.insert(id.get<std::string>());

    uintptr_t requestedStart = 0, requestedEnd = 0;
    if (hasAddressRange)
    {
        requestedStart = ParseRequiredAddress(args, "start");
        requestedEnd = ParseRequiredAddress(args, "end");
        if (requestedEnd <= requestedStart)
            throw HandlerError(Err::kBadArgs, "end 须大于 start");
    }

    const std::string moduleLower = Lower(module);
    std::vector<ScanRange> out;
    for (const auto &map : snapshot.maps)
    {
        if (!map.readable || !PermissionMatches(map, permissions)) continue;
        if (hasMapIds && !wantedIds.count(MapId(map))) continue;
        if (!module.empty() && Lower(map.pathname).find(moduleLower) == std::string::npos) continue;

        uintptr_t start = map.startAddress;
        uintptr_t end = map.endAddress;
        if (hasAddressRange)
        {
            start = std::max(start, requestedStart);
            end = std::min(end, requestedEnd);
            if (end <= start) continue;
        }
        out.push_back({start, end, map, ModuleBaseFor(snapshot, map)});
    }

    if (out.empty())
        throw HandlerError(Err::kNotFound, "指定范围内没有可读映射");
    return out;
}

std::string NewSessionId(const char *prefix)
{
    return std::string(prefix) + "-" + std::to_string(++gSessionSeq);
}

std::vector<uint8_t> ParseHexQuery(const std::string &raw)
{
    std::vector<uint8_t> bytes;
    if (!HexToBytes(raw, bytes) || bytes.empty())
        throw HandlerError(Err::kBadArgs, "HEX query 必须包含有效的十六进制字节");
    return bytes;
}

void AppendUtf16Le(const std::string &utf8, std::vector<uint8_t> &out)
{
    for (size_t i = 0; i < utf8.size();)
    {
        uint32_t cp = 0;
        const uint8_t c = static_cast<uint8_t>(utf8[i]);
        size_t count = 0;
        if (c < 0x80) { cp = c; count = 1; }
        else if ((c & 0xE0) == 0xC0) { cp = c & 0x1F; count = 2; }
        else if ((c & 0xF0) == 0xE0) { cp = c & 0x0F; count = 3; }
        else if ((c & 0xF8) == 0xF0) { cp = c & 0x07; count = 4; }
        else throw HandlerError(Err::kBadArgs, "UTF16LE query 含无效 UTF-8");
        if (i + count > utf8.size()) throw HandlerError(Err::kBadArgs, "UTF16LE query 截断");
        for (size_t j = 1; j < count; ++j)
        {
            const uint8_t cc = static_cast<uint8_t>(utf8[i + j]);
            if ((cc & 0xC0) != 0x80) throw HandlerError(Err::kBadArgs, "UTF16LE query 含无效 UTF-8");
            cp = (cp << 6) | (cc & 0x3F);
        }
        i += count;
        if (cp <= 0xFFFF)
        {
            const uint16_t u = static_cast<uint16_t>(cp);
            out.push_back(static_cast<uint8_t>(u));
            out.push_back(static_cast<uint8_t>(u >> 8));
        }
        else if (cp <= 0x10FFFF)
        {
            cp -= 0x10000;
            const uint16_t hi = static_cast<uint16_t>(0xD800 | (cp >> 10));
            const uint16_t lo = static_cast<uint16_t>(0xDC00 | (cp & 0x3FF));
            out.insert(out.end(), {static_cast<uint8_t>(hi), static_cast<uint8_t>(hi >> 8),
                                   static_cast<uint8_t>(lo), static_cast<uint8_t>(lo >> 8)});
        }
        else throw HandlerError(Err::kBadArgs, "UTF16LE query 超出 Unicode 范围");
    }
}

std::vector<uint8_t> BuildQuery(const std::string &type, const std::string &query)
{
    if (query.empty()) throw HandlerError(Err::kBadArgs, "query 不能为空");
    if (type == "ASCII")
    {
        for (unsigned char c : query)
            if (c > 0x7F) throw HandlerError(Err::kBadArgs, "ASCII query 只能包含 ASCII 字符");
        return std::vector<uint8_t>(query.begin(), query.end());
    }
    if (type == "UTF8") return std::vector<uint8_t>(query.begin(), query.end());
    if (type == "UTF16LE")
    {
        std::vector<uint8_t> out;
        AppendUtf16Le(query, out);
        return out;
    }
    if (type == "HEX") return ParseHexQuery(query);
    if (type == "U32" || type == "U64" || type == "POINTER")
    {
        uint64_t value = 0;
        try
        {
            size_t used = 0;
            value = std::stoull(query, &used, 0);
            if (used != query.size()) throw std::invalid_argument("trailing");
        }
        catch (...)
        {
            throw HandlerError(Err::kBadArgs, type + " query 必须是十进制或 0x 十六进制整数");
        }
        const size_t size = type == "U32" ? 4 : sizeof(uintptr_t);
        if (type == "U32" && value > std::numeric_limits<uint32_t>::max())
            throw HandlerError(Err::kBadArgs, "U32 query 超出范围");
        std::vector<uint8_t> out(size);
        std::memcpy(out.data(), &value, size);
        return out;
    }
    throw HandlerError(Err::kBadArgs, "queryType 必须是 ASCII/UTF8/UTF16LE/HEX/U32/U64/POINTER");
}

void ScanExact(const KittyMemoryMgr &mgr, const std::vector<ScanRange> &ranges,
               const std::vector<uint8_t> &needle, const std::vector<uint8_t> *mask,
               size_t alignment, HitSession &session, const std::atomic<bool> *cancelFlag)
{
    if (needle.empty()) return;
    std::vector<uint8_t> buffer(kScanChunk + needle.size());
    for (const auto &range : ranges)
    {
        for (uintptr_t cursor = range.start; cursor < range.end; cursor += kScanChunk)
        {
            if (cancelFlag && cancelFlag->load()) throw HandlerError(Err::kCancelled, "扫描已取消");
            const size_t primary = static_cast<size_t>(std::min<uintptr_t>(kScanChunk, range.end - cursor));
            const size_t overlap = std::min<size_t>(needle.size() - 1, range.end - cursor - primary);
            const size_t wanted = primary + overlap;
            const size_t got = mgr.readMem(cursor, buffer.data(), wanted);
            if (got < needle.size())
            {
                session.skippedBytes += primary;
                if (session.readErrors.size() < 16)
                    session.readErrors.push_back({{"start", FormatAddress(cursor)}, {"size", primary}, {"read", got}});
                continue;
            }
            session.scannedBytes += primary;
            const size_t scanLimit = std::min(primary, got - needle.size() + 1);
            for (size_t i = 0; i < scanLimit; ++i)
            {
                const uintptr_t address = cursor + i;
                if (alignment > 1 && address % alignment != 0) continue;
                bool matches = true;
                for (size_t j = 0; j < needle.size(); ++j)
                {
                    if (mask && !(*mask)[j]) continue;
                    if (buffer[i + j] != needle[j]) { matches = false; break; }
                }
                if (!matches) continue;
                if (session.hits.size() >= kMaxStoredHits)
                {
                    session.truncated = true;
                    return;
                }
                session.hits.push_back(address);
            }
        }
    }
}

void ParseIdaPattern(const std::string &raw, std::vector<uint8_t> &bytes, std::vector<uint8_t> &mask)
{
    std::istringstream in(raw);
    std::string token;
    while (in >> token)
    {
        if (token == "?" || token == "??")
        {
            bytes.push_back(0);
            mask.push_back(0);
            continue;
        }
        if (token.size() != 2 || !std::isxdigit(static_cast<unsigned char>(token[0])) ||
            !std::isxdigit(static_cast<unsigned char>(token[1])))
            throw HandlerError(Err::kBadArgs, "pattern 必须是 IDA 字节序列，例如 AA BB ? ? CC");
        bytes.push_back(static_cast<uint8_t>(std::stoul(token, nullptr, 16)));
        mask.push_back(1);
    }
    if (bytes.empty()) throw HandlerError(Err::kBadArgs, "pattern 不能为空");
}

json ContextHex(const KittyMemoryMgr &mgr, uintptr_t address, int before, int after)
{
    const uintptr_t start = address >= static_cast<uintptr_t>(before) ? address - before : address;
    const size_t size = static_cast<size_t>(before + after);
    if (size == 0) return "";
    std::vector<uint8_t> data(size);
    const size_t got = mgr.readMem(start, data.data(), size);
    return BytesToHex(data.data(), got);
}

json PageHits(const HitSession &session, const MapSnapshot &snapshot, const KittyMemoryMgr &mgr,
              const json &args, bool includeContext)
{
    const size_t cursor = ParseCursor(args);
    if (cursor > session.hits.size()) throw HandlerError(Err::kBadArgs, "cursor 超出结果范围");
    int requested = args.value("maxResults", kMaxPageResults);
    if (requested < 1) requested = kMaxPageResults;
    const size_t pageSize = static_cast<size_t>(std::min(requested, kMaxPageResults));
    const size_t end = std::min(session.hits.size(), cursor + pageSize);
    const int before = std::clamp(args.value("contextBefore", 0), 0, 64);
    const int after = std::clamp(args.value("contextAfter", 0), 0, 64);
    json hits = json::array();
    for (size_t i = cursor; i < end; ++i)
    {
        json hit = HitLocation(snapshot, session.hits[i]);
        if (includeContext && (before || after)) hit["contextHex"] = ContextHex(mgr, session.hits[i], before, after);
        hits.push_back(std::move(hit));
    }
    return {{"hits", hits},
            {"returned", hits.size()},
            {"totalHits", session.hits.size()},
            {"nextCursor", end < session.hits.size() ? json(std::to_string(end)) : json(nullptr)}};
}

json PageReferences(const std::string &sessionId, const ReferenceSession &session, const json &args)
{
    const size_t cursor = ParseCursor(args);
    if (cursor > session.references.size())
        throw HandlerError(Err::kBadArgs, "cursor 超出引用结果范围");
    int requested = args.value("maxResults", kMaxPageResults);
    if (requested < 1) requested = kMaxPageResults;
    const size_t pageSize = static_cast<size_t>(std::min(requested, kMaxPageResults));
    const size_t end = std::min(session.references.size(), cursor + pageSize);
    json references = json::array();
    for (size_t i = cursor; i < end; ++i) references.push_back(session.references[i]);
    return {{"sessionId", sessionId}, {"target", FormatAddress(session.target)},
            {"mapRevision", session.revision}, {"references", references},
            {"returned", references.size()}, {"totalReferences", session.references.size()},
            {"scannedBytes", session.scannedBytes}, {"skippedBytes", session.skippedBytes},
            {"truncated", session.truncated},
            {"nextCursor", end < session.references.size() ? json(std::to_string(end)) : json(nullptr)}};
}

bool WantsKind(const std::unordered_set<std::string> &kinds, const char *kind)
{
    return kinds.empty() || kinds.count(kind) != 0;
}

int64_t SignExtend(uint64_t value, int bits)
{
    const uint64_t sign = 1ULL << (bits - 1);
    return static_cast<int64_t>((value ^ sign) - sign);
}

bool DecodeAdrp(uintptr_t pc, uint32_t insn, int &rd, uintptr_t &page)
{
    if ((insn & 0x9F000000U) != 0x90000000U) return false;
    rd = insn & 0x1F;
    const uint64_t imm21 = ((static_cast<uint64_t>(insn >> 29) & 3) |
                            ((static_cast<uint64_t>(insn >> 5) & 0x7FFFF) << 2));
    page = static_cast<uintptr_t>((pc & ~uintptr_t(0xFFF)) + (SignExtend(imm21, 21) << 12));
    return true;
}

bool DecodeMovReg(uint32_t insn, int source, int &destination)
{
    if ((insn & 0xFFE0FFE0U) != 0xAA0003E0U) return false;
    const int rm = (insn >> 16) & 0x1F;
    if (rm != source) return false;
    destination = insn & 0x1F;
    return true;
}

bool DecodeAdd(uint32_t insn, int baseReg, uintptr_t page, uintptr_t &target)
{
    if ((insn & 0xFF000000U) != 0x91000000U) return false;
    if (static_cast<int>((insn >> 5) & 0x1F) != baseReg) return false;
    const uintptr_t imm = static_cast<uintptr_t>((insn >> 10) & 0xFFF) << (((insn >> 22) & 1) ? 12 : 0);
    target = page + imm;
    return true;
}

bool DecodeLdrUnsigned(uint32_t insn, int baseReg, uintptr_t page, uintptr_t &target)
{
    size_t scale = 0;
    if ((insn & 0xFFC00000U) == 0xF9400000U) scale = 8;
    else if ((insn & 0xFFC00000U) == 0xB9400000U) scale = 4;
    else return false;
    if (static_cast<int>((insn >> 5) & 0x1F) != baseReg) return false;
    target = page + static_cast<uintptr_t>((insn >> 10) & 0xFFF) * scale;
    return true;
}

bool DecodeLiteral(uintptr_t pc, uint32_t insn, uintptr_t &literalAddress)
{
    if ((insn & 0x3B000000U) != 0x18000000U) return false;
    const int64_t offset = SignExtend((insn >> 5) & 0x7FFFF, 19) << 2;
    literalAddress = static_cast<uintptr_t>(static_cast<int64_t>(pc) + offset);
    return true;
}
}

MapSnapshot CaptureMaps(const KittyMemoryMgr &mgr)
{
    MapSnapshot snapshot;
    snapshot.pid = mgr.processID();
    snapshot.processStartTime = ReadProcessStartTime(snapshot.pid);
    snapshot.maps = KittyMemoryEx::getAllMaps(snapshot.pid);
    uint64_t hash = 1469598103934665603ULL;
    hash = Fnv1a(hash, &snapshot.pid, sizeof(snapshot.pid));
    hash = Fnv1a(hash, &snapshot.processStartTime, sizeof(snapshot.processStartTime));
    for (const auto &map : snapshot.maps)
    {
        hash = Fnv1a(hash, &map.startAddress, sizeof(map.startAddress));
        hash = Fnv1a(hash, &map.endAddress, sizeof(map.endAddress));
        hash = Fnv1a(hash, &map.offset, sizeof(map.offset));
        hash = Fnv1a(hash, &map.inode, sizeof(map.inode));
        hash = FnvString(hash, map.pathname);
        const std::string perms = Perms(map);
        hash = FnvString(hash, perms);
    }
    snapshot.revision = std::to_string(snapshot.pid) + ":" + Hex(hash);
    return snapshot;
}

std::string CurrentMapRevision(const KittyMemoryMgr &mgr)
{
    return CaptureMaps(mgr).revision;
}

bool IsReadableAddress(const MapSnapshot &snapshot, uintptr_t address, size_t size)
{
    if (size == 0) return false;
    for (const auto &map : snapshot.maps)
        if (map.readable && address >= map.startAddress && address < map.endAddress &&
            size <= map.endAddress - address)
            return true;
    return false;
}

bool IsWritableAddress(const MapSnapshot &snapshot, uintptr_t address, size_t size)
{
    if (size == 0) return false;
    for (const auto &map : snapshot.maps)
        if (map.writeable && address >= map.startAddress && address < map.endAddress &&
            size <= map.endAddress - address)
            return true;
    return false;
}

ElfScanner FindUnrealElf(const KittyMemoryMgr &mgr, const std::string &moduleHint)
{
    std::vector<std::string> names;
    if (!moduleHint.empty()) names.push_back(moduleHint);
    names.push_back("libUE4.so");
    names.push_back("libUnreal.so");
    std::unordered_set<std::string> seen;
    for (const auto &name : names)
    {
        if (!seen.insert(name).second) continue;
        ElfScanner elf = mgr.findMemElf(name);
        if (elf.isValid()) return elf;
    }
    return {};
}

json ListModules(const json &args, const KittyMemoryMgr &mgr)
{
    const MapSnapshot snapshot = CaptureMaps(mgr);
    const std::string nameFilter = Lower(args.value("nameFilter", ""));
    const bool includeSegments = args.value("includeSegments", true);
    const bool includeAnonymous = args.value("includeAnonymous", false);
    const json permissions = args.value("permissions", json::array());
    const size_t cursor = ParseCursor(args);
    const size_t anonymousCursor = ParseNamedCursor(args, "anonymousCursor");
    const int requestedLimit = std::clamp(args.value("limit", kMaxPageResults), 1, 50);

    uintptr_t startFilter = 0, endFilter = std::numeric_limits<uintptr_t>::max();
    if (args.contains("start") || args.contains("end"))
    {
        startFilter = ParseRequiredAddress(args, "start");
        endFilter = ParseRequiredAddress(args, "end");
        if (endFilter <= startFilter) throw HandlerError(Err::kBadArgs, "end 须大于 start");
    }

    struct ModuleGroup { std::string path; uintptr_t base = 0; uintptr_t end = 0; std::vector<KittyMemoryEx::ProcMap> maps; };
    std::vector<ModuleGroup> groups;
    std::unordered_map<std::string, size_t> indexes;
    json anonymousAll = json::array();
    for (const auto &map : snapshot.maps)
    {
        if (!map.readable || !PermissionMatches(map, permissions)) continue;
        if (map.endAddress <= startFilter || map.startAddress >= endFilter) continue;
        if (!IsAnonymousPath(map.pathname))
        {
            if (!nameFilter.empty() && Lower(map.pathname).find(nameFilter) == std::string::npos) continue;
            auto [it, inserted] = indexes.emplace(map.pathname, groups.size());
            if (inserted) groups.push_back({map.pathname, map.startAddress, map.endAddress, {}});
            ModuleGroup &group = groups[it->second];
            group.base = std::min<uintptr_t>(group.base, map.startAddress);
            group.end = std::max<uintptr_t>(group.end, map.endAddress);
            group.maps.push_back(map);
        }
        else if (includeAnonymous && nameFilter.empty())
        {
            anonymousAll.push_back({{"mapId", MapId(map)},
                                 {"start", FormatAddress(map.startAddress)},
                                 {"end", FormatAddress(map.endAddress)},
                                 {"permissions", Perms(map)},
                                 {"fileOffset", FormatAddress(map.offset)},
                                 {"device", map.dev}, {"inode", map.inode},
                                 {"kind", map.writeable ? "BSS_OR_DATA" : "ANONYMOUS"}});
        }
    }

    std::sort(groups.begin(), groups.end(), [](const ModuleGroup &a, const ModuleGroup &b) { return a.base < b.base; });
    if (cursor > groups.size()) throw HandlerError(Err::kBadArgs, "cursor 超出模块结果范围");
    const size_t pageEnd = std::min(groups.size(), cursor + static_cast<size_t>(requestedLimit));
    if (anonymousCursor > anonymousAll.size())
        throw HandlerError(Err::kBadArgs, "anonymousCursor 超出匿名映射结果范围");
    const size_t anonymousEnd = std::min(anonymousAll.size(),
        anonymousCursor + static_cast<size_t>(requestedLimit));
    json anonymous = json::array();
    for (size_t i = anonymousCursor; i < anonymousEnd; ++i) anonymous.push_back(anonymousAll[i]);
    json modules = json::array();
    for (size_t i = cursor; i < pageEnd; ++i)
    {
        const ModuleGroup &group = groups[i];
        json segments = json::array();
        if (includeSegments)
        {
            for (size_t j = 0; j < group.maps.size(); ++j)
            {
                const auto &map = group.maps[j];
                segments.push_back({{"mapId", MapId(map)},
                                    {"start", FormatAddress(map.startAddress)},
                                    {"end", FormatAddress(map.endAddress)},
                                    {"permissions", Perms(map)},
                                    {"fileOffset", FormatAddress(map.offset)},
                                    {"device", map.dev}, {"inode", map.inode},
                                    {"kind", map.writeable ? "BSS_OR_DATA" : "ELF_LOAD"}});
            }
        }
        modules.push_back({{"moduleId", ModuleId(group.path)}, {"name", BaseName(group.path)},
                           {"path", group.path}, {"baseAddress", FormatAddress(group.base)},
                           {"endAddress", FormatAddress(group.end)}, {"segments", segments}});
    }
    return {{"pid", snapshot.pid}, {"processStartTime", std::to_string(snapshot.processStartTime)},
            {"mapRevision", snapshot.revision}, {"modules", modules},
            {"anonymousMaps", anonymous}, {"count", groups.size()},
            {"anonymousCount", anonymousAll.size()},
            {"nextCursor", pageEnd < groups.size() ? json(std::to_string(pageEnd)) : json(nullptr)},
            {"anonymousNextCursor", anonymousEnd < anonymousAll.size()
                ? json(std::to_string(anonymousEnd)) : json(nullptr)}};
}

json ScanPattern(const json &args, const KittyMemoryMgr &mgr, const std::atomic<bool> *cancelFlag)
{
    if (!mgr.isMemValid()) throw HandlerError(Err::kNotAttached, "未 attach 到目标进程");
    const MapSnapshot snapshot = CaptureMaps(mgr);
    const std::string existingId = args.value("sessionId", "");
    HitSession session;
    std::string sessionId = existingId;
    if (!existingId.empty())
    {
        std::lock_guard<std::mutex> lock(gSessionMutex);
        auto it = gSessions.find(existingId);
        if (it == gSessions.end() || it->second.pid != snapshot.pid ||
            it->second.processStartTime != snapshot.processStartTime)
            throw HandlerError(Err::kSessionStale, "pattern session 不存在或进程已切换");
        if (it->second.revision != snapshot.revision)
            throw HandlerError(Err::kMapStale, "pattern session 的 maps revision 已变化");
        session = it->second;
    }
    else
    {
        const std::string patternRaw = args.value("pattern", "");
        std::vector<uint8_t> bytes, mask;
        ParseIdaPattern(patternRaw, bytes, mask);
        const auto ranges = SelectRanges(args, snapshot, "segmentPermissions", true);
        session.pid = snapshot.pid;
        session.processStartTime = snapshot.processStartTime;
        session.revision = snapshot.revision;
        session.kind = "PATTERN";
        session.queryType = NormalizeIdaPattern(patternRaw);
        ScanExact(mgr, ranges, bytes, &mask, 1, session, cancelFlag);
        if (CurrentMapRevision(mgr) != snapshot.revision)
            throw HandlerError(Err::kMapStale, "pattern 扫描期间 maps revision 已变化");
        sessionId = NewSessionId("pattern");
        StoreHitSession(sessionId, session);
    }
    json page = PageHits(session, snapshot, mgr, args, false);
    page["sessionId"] = sessionId;
    page["pattern"] = session.queryType;
    page["mapRevision"] = snapshot.revision;
    page["scannedBytes"] = session.scannedBytes;
    page["skippedBytes"] = session.skippedBytes;
    page["readErrors"] = session.readErrors;
    page["truncated"] = session.truncated;
    return page;
}

json SearchMemory(const json &args, const KittyMemoryMgr &mgr, const std::atomic<bool> *cancelFlag)
{
    if (!mgr.isMemValid()) throw HandlerError(Err::kNotAttached, "未 attach 到目标进程");
    const MapSnapshot snapshot = CaptureMaps(mgr);
    const std::string existingId = args.value("sessionId", "");
    HitSession session;
    std::string sessionId = existingId;
    if (!existingId.empty())
    {
        std::lock_guard<std::mutex> lock(gSessionMutex);
        auto it = gSessions.find(existingId);
        if (it == gSessions.end() || it->second.pid != snapshot.pid ||
            it->second.processStartTime != snapshot.processStartTime)
            throw HandlerError(Err::kSessionStale, "search session 不存在或进程已切换");
        if (it->second.revision != snapshot.revision)
            throw HandlerError(Err::kMapStale, "search session 的 maps revision 已变化");
        session = it->second;
    }
    else
    {
        const std::string queryType = args.value("queryType", "");
        const std::string query = args.value("query", "");
        const auto needle = BuildQuery(queryType, query);
        const size_t alignment = static_cast<size_t>(std::clamp(args.value("alignment", 1), 1, 4096));
        const auto ranges = SelectRanges(args, snapshot, "permissions", false);
        session.pid = snapshot.pid;
        session.processStartTime = snapshot.processStartTime;
        session.revision = snapshot.revision;
        session.kind = "SEARCH";
        session.queryType = queryType;
        ScanExact(mgr, ranges, needle, nullptr, alignment, session, cancelFlag);
        if (CurrentMapRevision(mgr) != snapshot.revision)
            throw HandlerError(Err::kMapStale, "内存搜索期间 maps revision 已变化");
        sessionId = NewSessionId("search");
        StoreHitSession(sessionId, session);
    }
    json page = PageHits(session, snapshot, mgr, args, true);
    page["sessionId"] = sessionId;
    page["queryType"] = session.queryType;
    page["mapRevision"] = snapshot.revision;
    page["scannedBytes"] = session.scannedBytes;
    page["skippedBytes"] = session.skippedBytes;
    page["readErrors"] = session.readErrors;
    page["truncated"] = session.truncated;
    return page;
}

json FindReferences(const json &args, const KittyMemoryMgr &mgr, const std::atomic<bool> *cancelFlag)
{
    if (!mgr.isMemValid()) throw HandlerError(Err::kNotAttached, "未 attach 到目标进程");
    const MapSnapshot snapshot = CaptureMaps(mgr);
    const std::string existingId = args.value("sessionId", "");
    if (!existingId.empty())
    {
        std::lock_guard<std::mutex> lock(gSessionMutex);
        auto it = gReferenceSessions.find(existingId);
        if (it == gReferenceSessions.end() || it->second.pid != snapshot.pid ||
            it->second.processStartTime != snapshot.processStartTime)
            throw HandlerError(Err::kSessionStale, "reference session 不存在或进程已切换");
        if (it->second.revision != snapshot.revision)
            throw HandlerError(Err::kMapStale, "reference session 的 maps revision 已变化");
        return PageReferences(existingId, it->second, args);
    }
    const uintptr_t target = ParseRequiredAddress(args, "target");
    const auto ranges = SelectRanges(args, snapshot, "segmentPermissions", false);
    std::unordered_set<std::string> kinds;
    for (const auto &kind : args.value("kinds", json::array()))
        if (kind.is_string()) kinds.insert(kind.get<std::string>());
    const bool includeDisassembly = args.value("includeDisassembly", true);
    ReferenceSession session;
    session.pid = snapshot.pid;
    session.processStartTime = snapshot.processStartTime;
    session.revision = snapshot.revision;
    session.target = target;

    auto appendRef = [&](uintptr_t address, const char *kind, uintptr_t resolved, const json &instructions)
    {
        if (session.references.size() >= kMaxStoredHits) { session.truncated = true; return; }
        json ref = HitLocation(snapshot, address);
        ref["instructionAddress"] = ref["address"];
        ref.erase("address");
        ref["kind"] = kind;
        ref["resolvedTarget"] = FormatAddress(resolved);
        if (includeDisassembly) ref["instructions"] = instructions;
        session.references.push_back(std::move(ref));
    };

    if (WantsKind(kinds, "POINTER"))
    {
        std::vector<uint8_t> needle(sizeof(target));
        std::memcpy(needle.data(), &target, sizeof(target));
        HitSession pointerHits;
        ScanExact(mgr, ranges, needle, nullptr, sizeof(uintptr_t), pointerHits, cancelFlag);
        session.scannedBytes += pointerHits.scannedBytes;
        session.skippedBytes += pointerHits.skippedBytes;
        for (uintptr_t address : pointerHits.hits)
        {
            appendRef(address, "POINTER", target, json::array());
            if (session.truncated) break;
        }
    }

    if (!session.truncated && (WantsKind(kinds, "ADRP_ADD") || WantsKind(kinds, "ADRP_LDR") ||
                       WantsKind(kinds, "LITERAL_LOAD")))
    {
        std::vector<uint8_t> buffer(kScanChunk + 16);
        for (const auto &range : ranges)
        {
            if (!range.map.executable) continue;
            for (uintptr_t cursor = range.start; cursor < range.end && !session.truncated; cursor += kScanChunk)
            {
                if (cancelFlag && cancelFlag->load()) throw HandlerError(Err::kCancelled, "引用扫描已取消");
                const size_t primary = static_cast<size_t>(std::min<uintptr_t>(kScanChunk, range.end - cursor));
                const size_t overlap = std::min<size_t>(16, range.end - cursor - primary);
                const size_t got = mgr.readMem(cursor, buffer.data(), primary + overlap);
                if (got < 4) { session.skippedBytes += primary; continue; }
                session.scannedBytes += primary;
                for (size_t offset = (4 - (cursor & 3)) & 3; offset + 4 <= primary; offset += 4)
                {
                    uint32_t insn = 0;
                    std::memcpy(&insn, buffer.data() + offset, 4);
                    const uintptr_t pc = cursor + offset;
                    if (WantsKind(kinds, "LITERAL_LOAD"))
                    {
                        uintptr_t literal = 0;
                        if (DecodeLiteral(pc, insn, literal))
                        {
                            uintptr_t loaded = 0;
                            const bool readValue = mgr.readMem(literal, &loaded, sizeof(loaded)) == sizeof(loaded);
                            if (literal == target || (readValue && loaded == target))
                                appendRef(pc, "LITERAL_LOAD", target,
                                          json::array({DisassembleArm64(pc, insn)}));
                        }
                    }

                    int adrpReg = 0;
                    uintptr_t page = 0;
                    if (!DecodeAdrp(pc, insn, adrpReg, page)) continue;
                    int trackedReg = adrpReg;
                    json instructions = json::array({DisassembleArm64(pc, insn)});
                    for (size_t step = 1; step <= 3 && offset + (step + 1) * 4 <= got; ++step)
                    {
                        uint32_t next = 0;
                        std::memcpy(&next, buffer.data() + offset + step * 4, 4);
                        const uintptr_t nextPc = pc + step * 4;
                        instructions.push_back(DisassembleArm64(nextPc, next));
                        int moved = 0;
                        if (DecodeMovReg(next, trackedReg, moved)) { trackedReg = moved; continue; }
                        uintptr_t resolved = 0;
                        if (WantsKind(kinds, "ADRP_ADD") && DecodeAdd(next, trackedReg, page, resolved) && resolved == target)
                        {
                            appendRef(pc, "ADRP_ADD", resolved, instructions);
                            break;
                        }
                        if (WantsKind(kinds, "ADRP_LDR") && DecodeLdrUnsigned(next, trackedReg, page, resolved) && resolved == target)
                        {
                            appendRef(pc, "ADRP_LDR", resolved, instructions);
                            break;
                        }
                        if ((next & 0xFC000000U) == 0x14000000U || next == 0xD65F03C0U) break;
                    }
                    if (session.truncated) break;
                }
            }
        }
    }
    if (CurrentMapRevision(mgr) != snapshot.revision)
        throw HandlerError(Err::kMapStale, "引用扫描期间 maps revision 已变化");
    const std::string sessionId = NewSessionId("references");
    StoreReferenceSession(sessionId, session);
    return PageReferences(sessionId, session, args);
}

void InvalidateSessions()
{
    std::lock_guard<std::mutex> lock(gSessionMutex);
    gSessions.clear();
    gReferenceSessions.clear();
}
}
