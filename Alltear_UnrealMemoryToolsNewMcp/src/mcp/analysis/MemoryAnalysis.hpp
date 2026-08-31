#pragma once

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include <KittyMemoryMgr.hpp>

namespace UmtMcp::Analysis
{
using json = nlohmann::json;

struct MapSnapshot
{
    pid_t pid = 0;
    uint64_t processStartTime = 0;
    std::string revision;
    std::vector<KittyMemoryEx::ProcMap> maps;
};

MapSnapshot CaptureMaps(const KittyMemoryMgr &mgr);
std::string CurrentMapRevision(const KittyMemoryMgr &mgr);
bool IsReadableAddress(const MapSnapshot &snapshot, uintptr_t address, size_t size = 1);
ElfScanner FindUnrealElf(const KittyMemoryMgr &mgr, const std::string &moduleHint = {});

json ListModules(const json &args, const KittyMemoryMgr &mgr);
json ScanPattern(const json &args, const KittyMemoryMgr &mgr, const std::atomic<bool> *cancelFlag = nullptr);
json SearchMemory(const json &args, const KittyMemoryMgr &mgr, const std::atomic<bool> *cancelFlag = nullptr);
json FindReferences(const json &args, const KittyMemoryMgr &mgr, const std::atomic<bool> *cancelFlag = nullptr);

void InvalidateSessions();
}
