#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>

#include <KittyMemoryMgr.hpp>

namespace UmtMcp::Analysis
{
using json = nlohmann::json;

json ScanGNamesCandidates(const json &args, const KittyMemoryMgr &mgr, const std::atomic<bool> *cancelFlag = nullptr);
json SampleGNamesCandidate(const json &args, const KittyMemoryMgr &mgr);
json ScanObjectCandidates(const json &args, const KittyMemoryMgr &mgr, const std::atomic<bool> *cancelFlag = nullptr);
json SampleObjectCandidate(const json &args, const KittyMemoryMgr &mgr);

bool ValidateCandidateBinding(const std::string &kind, const std::string &sessionId,
                              int candidateId, pid_t pid, uint64_t processStartTime,
                              const std::string &mapRevision,
                              uintptr_t address, std::string &reason);
void InvalidateCandidateSessions();
}
