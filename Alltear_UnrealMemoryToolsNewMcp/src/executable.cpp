#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// 查询设备当前 Wi-Fi 局域网 IP（root 环境下直接调原生 ip 命令；用于模式 1 连接提示）
static std::string GetLanIp();

#include "Utils/Logger.hpp"
#include "Utils/ProgressUtils.hpp"

#include "Dumper.hpp"
#include "SDKExplorer.hpp"

#include "UE/UEMemory.hpp"
#include "UE/UEGameProfile.hpp"

#include "UE/UEGameProfiles/ArenaBreakout.hpp"
#include "UE/UEGameProfiles/AutoFix.hpp"
#include "UE/UEGameProfiles/DeltaForce.hpp"
#include "UE/UEGameProfiles/Farlight.hpp"
#include "UE/UEGameProfiles/NRC.hpp"
#include "UE/UEGameProfiles/PUBGMHD.hpp"
#include "UE/UEGameProfiles/PUBG.hpp"
#include "UE/UEGameProfiles/Sfps2.hpp"
#include "UE/UEGameProfiles/Valorant.hpp"

#include "Android_draw/draw.h"
#include "Android_Graphics/GraphicsManager.h"

#include "mcp/CommandDispatcher.hpp"
#include "mcp/CommandQueue.hpp"
#include "mcp/CommandServer.hpp"
#include "mcp/Protocol.hpp"
#include "mcp/MemoryHelpers.hpp"
#include "mcp/Arm64Disasm.hpp"
#include "mcp/PtraceSession.hpp"
#include "mcp/DecompileHandler.hpp"
#include "mcp/analysis/MemoryAnalysis.hpp"
#include "mcp/analysis/UECandidateAnalysis.hpp"
#include "mcp/analysis/ArtifactMetadata.hpp"
#include "UE/UEWrappers.hpp"
#include <sys/mman.h>
#include <cstdlib>

#include <nlohmann/json.hpp>

    // 日志面板过滤开关：勾选后隐藏 MCP 连接上下线（[MCP·连接]）这类刷屏日志
    bool gHideMcpConnLogs = false;

std::vector<IGameProfile *> UE_Games = {
    new ArenaBreakoutProfile(),
    new DeltaForceProfile(),
    new FarlightProfile(),
    new ShuishaProfile(),
    new ValorantProfile(),
    new NRCProfile(),
    new PUBGMHDProfile(),
    new PUBGProfile(),
};

#define kUEDUMPER_VERSION "1.0.0"

namespace
{
    constexpr const char *kOutputDirectory = "/sdcard/UnrealMemoryTools";
    constexpr size_t kMaxLogLines = 1500;

    enum class UiLang { ZH = 0, EN = 1 };
    UiLang gUiLang = UiLang::ZH;

    inline const char *Tr(const char *zh, const char *en)
    {
        return gUiLang == UiLang::ZH ? zh : en;
    }

    struct AutoProcessCandidate
    {
        pid_t pid = 0;
        std::string package;
        std::string profileName;
        bool dedicated = false;
    };

    struct ProbeOffsetEntry
    {
        std::string name;
        uintptr_t value = 0;
        uintptr_t relative = 0;
        bool found = false;
    };

    struct StructFieldEntry
    {
        std::string name;
        std::string type;
        uintptr_t offset = 0;
        bool found = false;
        std::string description;
    };

    struct StructGroup
    {
        std::string name;
        std::vector<StructFieldEntry> fields;
    };

    struct ProbeResult
    {
        bool valid = false;
        bool success = false;
        pid_t pid = 0;
        std::string package;
        std::string profileName;
        bool dedicated = false;
        uintptr_t baseAddress = 0;
        std::vector<ProbeOffsetEntry> offsets;
        std::vector<StructGroup> structGroups;
        nlohmann::json fieldSources = nlohmann::json::object();
        nlohmann::json overrideEvidence = nlohmann::json::array();
        std::string mapRevision;
        uint64_t processStartTime = 0;
        IGameProfile *profile = nullptr;
        std::unique_ptr<AutoFixProfile> autoProfileOwner;
    };

    struct DumpUiState
    {
        std::mutex mutex;
        std::vector<std::string> logLines;
        std::string phase = "空闲";
        std::string activePackage;
        std::string resultPath;
        std::string lastError;
        bool probeRunning = false;
        bool probeFinished = false;
        bool probeSuccess = false;
        bool dumpRunning = false;
        bool dumpFinished = false;
        bool dumpSuccess = false;
        bool soDumpRunning = false;
        bool soDumpFinished = false;
        bool soDumpSuccess = false;
        std::string soDumpPath;
        nlohmann::json soDumpArtifact = nlohmann::json::object();
        int objectsPercent = 0;
        int dumpPercent = 0;
        std::vector<ProbeOffsetEntry> probeOffsets;
        std::vector<StructGroup> probeStructGroups;
        std::string probedPackage;
        std::string probedProfileName;
        pid_t probedPid = 0;
    };

    DumpUiState gDumpUiState;
    std::vector<AutoProcessCandidate> gCandidates;
    int gSelectedIndex = 0;
    std::thread gWorkerThread;
    ProbeResult gProbeResult;
    std::atomic<bool> gCancelRequested{false};
    static std::mutex gOverrideMutex;
    static std::unordered_map<std::string, uintptr_t> gProbeOverrides;
    static std::unordered_map<std::string, nlohmann::json> gProbeOverrideLayouts;
    static pid_t gProbeOverridePid = 0;
    static uint64_t gProbeOverrideStartTime = 0;
    static std::string gProbeOverrideRevision;
    static std::chrono::steady_clock::time_point gProbeOverrideExpiresAt{};

    struct PendingProbeOverrideGuard
    {
        ~PendingProbeOverrideGuard()
        {
            std::lock_guard<std::mutex> lock(gOverrideMutex);
            gProbeOverrides.clear();
            gProbeOverrideLayouts.clear();
            gProbeOverridePid = 0;
            gProbeOverrideStartTime = 0;
            gProbeOverrideRevision.clear();
            gProbeOverrideExpiresAt = {};
        }
    };

    // ── JobRegistry:支持 CANCEL_JOB 和 jobId 轮询(轻量实现)
    struct JobEntry
    {
        std::string jobId;
        std::string type; // "probe" / "dump"
        std::chrono::steady_clock::time_point createdAt;
        bool running = false;
        int progress = 0;
        std::string lastError;
        nlohmann::json result = nullptr;
    };
    static std::mutex gJobMutex;
    static std::vector<JobEntry> gJobs;
    static std::string genJobId()
    {
        static std::atomic<uint64_t> sJobSeq{0};
        return "job_" + std::to_string(++sJobSeq) + "_" +
               std::to_string(static_cast<uint32_t>(
                   std::chrono::duration_cast<std::chrono::seconds>(
                       std::chrono::system_clock::now().time_since_epoch()).count()));
    }
    static std::string StartJob(const std::string &type)
    {
        std::lock_guard<std::mutex> lock(gJobMutex);
        JobEntry j;
        j.jobId = genJobId();
        j.type = type;
        j.running = true;
        j.createdAt = std::chrono::steady_clock::now();
        gJobs.push_back(j);
        return j.jobId;
    }
    static void FinishJob(const std::string &jobId, bool success, const std::string &error = {})
    {
        std::lock_guard<std::mutex> lock(gJobMutex);
        for (auto &j : gJobs)
        {
            if (j.jobId == jobId)
            {
                j.running = false;
                j.progress = success ? 100 : 0;
                j.lastError = error;
                break;
            }
        }
    }
    static void UpdateJobProgress(const std::string &jobId, int pct)
    {
        std::lock_guard<std::mutex> lock(gJobMutex);
        for (auto &j : gJobs)
            if (j.jobId == jobId) { j.progress = pct; break; }
    }

    static bool HasRunningJob()
    {
        std::lock_guard<std::mutex> lock(gJobMutex);
        return std::any_of(gJobs.begin(), gJobs.end(),
                           [](const JobEntry &job) { return job.running; });
    }

    static bool HasRunningUiTask()
    {
        std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
        return gDumpUiState.probeRunning || gDumpUiState.dumpRunning ||
               gDumpUiState.soDumpRunning;
    }

    static void RequireIdleWorker()
    {
        if (HasRunningJob() || HasRunningUiTask())
            throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady,
                                       "已有长任务运行中，请先轮询状态或取消任务");
    }

    static void FinishJobWithResult(const std::string &jobId, nlohmann::json result)
    {
        std::lock_guard<std::mutex> lock(gJobMutex);
        for (auto &job : gJobs)
        {
            if (job.jobId != jobId) continue;
            job.running = false;
            job.progress = 100;
            job.lastError.clear();
            job.result = std::move(result);
            break;
        }
    }

    static nlohmann::json SnapshotJobs()
    {
        std::lock_guard<std::mutex> lock(gJobMutex);
        nlohmann::json jobs = nlohmann::json::array();
        const size_t begin = gJobs.size() > 8 ? gJobs.size() - 8 : 0;
        for (size_t i = begin; i < gJobs.size(); ++i)
        {
            const auto &job = gJobs[i];
            const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - job.createdAt).count();
            nlohmann::json item = {{"jobId", job.jobId}, {"type", job.type},
                            {"status", job.running ? "running" : job.lastError.empty() ? "succeeded" : "failed"},
                            {"running", job.running}, {"progress", job.progress},
                            {"elapsedMs", elapsedMs},
                            {"lastError", job.lastError.empty() ? nlohmann::json(nullptr)
                                                                  : nlohmann::json(job.lastError)}};
            if (i + 1 == gJobs.size()) item["result"] = job.result;
            jobs.push_back(std::move(item));
        }
        return jobs;
    }

    static UEAddressOverrides BuildProfileOverrides()
    {
        std::lock_guard<std::mutex> lock(gOverrideMutex);
        UEAddressOverrides out;
        auto names = gProbeOverrides.find("names");
        if (names != gProbeOverrides.end())
        {
            out.namesPtr = names->second;
            out.hasNamesPtr = true;
        }
        auto objects = gProbeOverrides.find("objects");
        if (objects != gProbeOverrides.end())
        {
            out.guObjectArrayPtr = objects->second;
            out.hasGUObjectArrayPtr = true;
        }
        auto apply = [](const nlohmann::json &layout, const char *key, uintptr_t &field)
        {
            if (layout.contains(key) && layout[key].is_number_integer())
                field = static_cast<uintptr_t>(std::max<int64_t>(0, layout[key].get<int64_t>()));
        };
        auto nameLayout = gProbeOverrideLayouts.find("names");
        if (nameLayout != gProbeOverrideLayouts.end())
        {
            out.hasNameLayout = true;
            apply(nameLayout->second, "stride", out.nameStride);
            apply(nameLayout->second, "blocksBit", out.nameBlocksBit);
            apply(nameLayout->second, "blocksOff", out.nameBlocksOff);
            apply(nameLayout->second, "headerOff", out.nameHeaderOff);
            apply(nameLayout->second, "lengthShift", out.nameLengthShift);
        }
        auto objectLayout = gProbeOverrideLayouts.find("objects");
        if (objectLayout != gProbeOverrideLayouts.end())
        {
            out.hasObjectLayout = true;
            apply(objectLayout->second, "objObjectsOff", out.objObjectsOff);
            apply(objectLayout->second, "objectsOff", out.objectsOff);
            apply(objectLayout->second, "numElementsOff", out.numElementsOff);
            if (objectLayout->second.value("chunked", true))
                apply(objectLayout->second, "numElementsPerChunk", out.numElementsPerChunk);
            else
                out.numElementsPerChunk = 0;
            apply(objectLayout->second, "itemObjectOff", out.itemObjectOff);
            apply(objectLayout->second, "itemSize", out.itemSize);
            apply(objectLayout->second, "classPrivateOff", out.classPrivateOff);
            apply(objectLayout->second, "namePrivateOff", out.namePrivateOff);
            apply(objectLayout->second, "outerPrivateOff", out.outerPrivateOff);
            if (objectLayout->second.contains("namePrivateOffsets") &&
                objectLayout->second["namePrivateOffsets"].is_array() &&
                !objectLayout->second["namePrivateOffsets"].empty())
                out.namePrivateOff = objectLayout->second["namePrivateOffsets"][0].get<uintptr_t>();
        }
        return out;
    }

    // ── 目录列表辅助(用 readdir,与 FindAutoProcessCandidates 风格一致)
    static std::vector<nlohmann::json> ListDirectory(const std::string &path)
    {
        std::vector<nlohmann::json> files;
        DIR *dir = opendir(path.c_str());
        if (!dir) return files;
        dirent *entry = nullptr;
        while ((entry = readdir(dir)) != nullptr)
        {
            nlohmann::json f;
            f["name"] = entry->d_name;
            f["type"] = (entry->d_type == DT_DIR) ? "directory" : "file";
            files.push_back(f);
        }
        closedir(dir);
        std::sort(files.begin(), files.end(),
                  [](const nlohmann::json &a, const nlohmann::json &b)
                  { return a["name"].get<std::string>() < b["name"].get<std::string>(); });
        return files;
    }

    bool IsNumericName(const char *s)
    {
        if (!s || !*s) return false;
        for (const char *p = s; *p; ++p)
        {
            if (*p < '0' || *p > '9')
                return false;
        }
        return true;
    }

    bool HasUnrealLib(pid_t pid)
    {
        auto maps = KittyMemoryEx::getAllMaps(pid);
        for (const auto &m : maps)
        {
            if (m.pathname.find("libUE4.so") != std::string::npos ||
                m.pathname.find("libUnreal.so") != std::string::npos)
                return true;
        }
        return false;
    }

    std::vector<AutoProcessCandidate> FindAutoProcessCandidates()
    {
        std::unordered_map<std::string, AutoProcessCandidate> candidates;

        for (auto *profile : UE_Games)
        {
            for (const auto &pkg : profile->GetAppIDs())
            {
                auto pids = KittyMemoryEx::getPIDsOf(pkg);
                for (pid_t pid : pids)
                    candidates[pkg] = {pid, pkg, profile->GetAppName(), true};
            }
        }

        DIR *dir = opendir("/proc");
        if (!dir) return {};

        dirent *entry = nullptr;
        while ((entry = readdir(dir)) != nullptr)
        {
            if (!IsNumericName(entry->d_name))
                continue;

            pid_t pid = static_cast<pid_t>(atoi(entry->d_name));
            if (pid <= 0)
                continue;

            std::string processName = KittyMemoryEx::getProcessName(pid);
            if (processName.empty() || candidates.count(processName) > 0)
                continue;
            if (!HasUnrealLib(pid))
                continue;

            candidates[processName] = {pid, processName, "自动识别 (UE4/UE5 通用)", false};
        }

        closedir(dir);

        std::vector<AutoProcessCandidate> result;
        result.reserve(candidates.size());
        for (const auto &it : candidates)
            result.push_back(it.second);

        std::sort(result.begin(), result.end(), [](const AutoProcessCandidate &a, const AutoProcessCandidate &b)
        {
            if (a.dedicated != b.dedicated)
                return a.dedicated > b.dedicated;
            return a.package < b.package;
        });
        return result;
    }

    std::string BuildDumpFileBanner(const std::string &fileName)
    {
        if (fileName.size() >= 5 && fileName.substr(fileName.size() - 5) == ".json")
            return {};

        return "// 创作者: 曦曦(DreamFekk) https://github.com/DreamFekk\n"
               "// 禁止圈钱盗卖\n\n";
    }



    void PushUiLog(char level, const std::string &message)
    {
        std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
        gDumpUiState.logLines.push_back(std::string(1, level) + ": " + message);
        if (gDumpUiState.logLines.size() > kMaxLogLines)
        {
            gDumpUiState.logLines.erase(
                gDumpUiState.logLines.begin(),
                gDumpUiState.logLines.begin() + (gDumpUiState.logLines.size() - kMaxLogLines));
        }
    }

    void LoggerSink(char level, const char *message)
    {
        PushUiLog(level, message ? message : "");
    }

    void SetDumpPhase(const std::string &phase)
    {
        std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
        gDumpUiState.phase = phase;
    }

    void SetProgress(int objectsPercent, int dumpPercent)
    {
        std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
        if (objectsPercent >= 0)
            gDumpUiState.objectsPercent = objectsPercent;
        if (dumpPercent >= 0)
            gDumpUiState.dumpPercent = dumpPercent;
    }

    void BeginProbeState(const AutoProcessCandidate &candidate)
    {
        std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
        gDumpUiState.probeRunning = true;
        gDumpUiState.probeFinished = false;
        gDumpUiState.probeSuccess = false;
        gDumpUiState.dumpRunning = false;
        gDumpUiState.dumpFinished = false;
        gDumpUiState.dumpSuccess = false;
        gDumpUiState.phase = "探针准备中";
        gDumpUiState.activePackage = candidate.package;
        gDumpUiState.probedPackage = candidate.package;
        gDumpUiState.probedProfileName = candidate.profileName;
        gDumpUiState.probedPid = candidate.pid;
        gDumpUiState.resultPath.clear();
        gDumpUiState.lastError.clear();
        gDumpUiState.objectsPercent = 0;
        gDumpUiState.dumpPercent = 0;
        gDumpUiState.probeOffsets.clear();
        gDumpUiState.probeStructGroups.clear();
        gDumpUiState.logLines.clear();
    }

    void InvalidateProbeReuse(const std::string &reason)
    {
        gProbeResult = ProbeResult{};

        std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
        gDumpUiState.probeFinished = false;
        gDumpUiState.probeSuccess = false;
        gDumpUiState.probeOffsets.clear();
        gDumpUiState.probeStructGroups.clear();
        gDumpUiState.probedPackage.clear();
        gDumpUiState.probedProfileName.clear();
        gDumpUiState.probedPid = 0;
        gDumpUiState.dumpFinished = false;
        gDumpUiState.dumpSuccess = false;
        gDumpUiState.soDumpFinished = false;
        gDumpUiState.soDumpSuccess = false;
        if (!reason.empty())
            gDumpUiState.logLines.push_back("I: " + reason);
    }

    void FinishProbeState(bool success, const std::vector<ProbeOffsetEntry> &offsets,
                          const std::vector<StructGroup> &structGroups, const std::string &lastError)
    {
        std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
        gDumpUiState.probeRunning = false;
        gDumpUiState.probeFinished = true;
        gDumpUiState.probeSuccess = success;
        gDumpUiState.phase = success ? "探针完成" : "探针失败";
        gDumpUiState.probeOffsets = offsets;
        gDumpUiState.probeStructGroups = structGroups;
        gDumpUiState.lastError = lastError;
    }

    void BeginDumpState(const std::string &package)
    {
        std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
        gDumpUiState.dumpRunning = true;
        gDumpUiState.dumpFinished = false;
        gDumpUiState.dumpSuccess = false;
        gDumpUiState.phase = "Dump 准备中";
        gDumpUiState.activePackage = package;
        gDumpUiState.resultPath.clear();
        gDumpUiState.lastError.clear();
        gDumpUiState.objectsPercent = 0;
        gDumpUiState.dumpPercent = 0;
    }

    void FinishDumpState(bool success, const std::string &resultPath, const std::string &lastError)
    {
        std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
        gDumpUiState.dumpRunning = false;
        gDumpUiState.dumpFinished = true;
        gDumpUiState.dumpSuccess = success;
        gDumpUiState.phase = success ? "完成" : "失败";
        gDumpUiState.resultPath = resultPath;
        gDumpUiState.lastError = lastError;
        if (success)
        {
            gDumpUiState.objectsPercent = 100;
            gDumpUiState.dumpPercent = 100;
        }
    }

    void RefreshCandidates()
    {
        const std::string previousPackage =
            (gSelectedIndex >= 0 && gSelectedIndex < static_cast<int>(gCandidates.size()))
                ? gCandidates[gSelectedIndex].package
                : std::string();

        gCandidates = FindAutoProcessCandidates();
        gSelectedIndex = 0;

        if (!previousPackage.empty())
        {
            for (size_t i = 0; i < gCandidates.size(); ++i)
            {
                if (gCandidates[i].package == previousPackage)
                {
                    gSelectedIndex = static_cast<int>(i);
                    break;
                }
            }
        }

        if (gCandidates.empty())
        {
            std::string knownGames;
            for (size_t i = 0; i < UE_Games.size(); ++i)
            {
                if (i)
                    knownGames += "、";
                knownGames += UE_Games[i]->GetAppName();
            }
            PushUiLog('W',
                      "未检测到正在运行的 Unreal Engine 进程。已自动扫描以下内置游戏,但当前均未运行:"
                      + knownGames
                      + "。若已启动游戏仍无显示,请确认游戏处于前台运行,且本工具有权限读取其 "
                        "/proc/<pid>/cmdline(其它 UE4/UE5 通用进程也会走通用扫描分支)。");
        }
        else
            PushUiLog('I', "检测到 " + std::to_string(gCandidates.size()) + " 个 Unreal Engine 进程。");

        if (gProbeResult.valid)
        {
            bool foundSameProcess = false;
            for (const auto &candidate : gCandidates)
            {
                if (candidate.package == gProbeResult.package && candidate.pid == gProbeResult.pid)
                {
                    foundSameProcess = true;
                    break;
                }
            }
            if (!foundSameProcess)
                InvalidateProbeReuse("进程列表已变化，已清除旧探针结果，请重新探测。");
        }
    }

    bool SaveDumpBuffers(const std::unordered_map<std::string, BufferFmt> &dumpbuffersMap, const std::string &dumpGameDir)
    {
        LOGI("正在保存文件...");
        for (const auto &it : dumpbuffersMap)
        {
            if (it.first.empty())
                continue;

            std::string path = KittyUtils::String::Fmt("%s/%s", dumpGameDir.c_str(), it.first.c_str());
            std::string banner = BuildDumpFileBanner(it.first);
            if (banner.empty())
            {
                if (!it.second.writeBufferToFile(path))
                    return false;
            }
            else
            {
                BufferFmt finalBuf;
                finalBuf.append("{}", banner);
                finalBuf.append("{}", it.second.read());
                if (!finalBuf.writeBufferToFile(path))
                    return false;
            }
        }
        return true;
    }

    std::vector<ProbeOffsetEntry> CollectProbeOffsets(IGameProfile *profile)
    {
        std::vector<ProbeOffsetEntry> offsets;
        if (!profile)
            return offsets;

        const auto *vars = profile->GetUEVars();
        if (!vars)
            return offsets;

        const uintptr_t base = vars->GetBaseAddress();
        auto add = [&](const char *name, uintptr_t value)
        {
            ProbeOffsetEntry e;
            e.name = name;
            e.value = value;
            e.relative = (value && base && value >= base) ? (value - base) : 0;
            e.found = value != 0;
            offsets.push_back(std::move(e));
        };

        add("BaseAddress", base);
        add("GUObjectArray", vars->GetGUObjectsArrayPtr());
        add("ObjObjects", vars->GetObjObjectsPtr());
        add(profile->IsUsingFNamePool() ? "FNamePool" : "GNames", vars->GetNamesPtr());
        add("ProcessEvent", vars->GetProcessEvent());
        add("StaticFindObject", vars->GetStaticFindObject());
        add("FrameCount", vars->GetFrameCount());
        add("Matrix", vars->GetMatrix());
        add("Physx", vars->GetPhysx());
        add("NativeAndroidApp", vars->GetNativeAndroidApp());
        return offsets;
    }

    std::vector<StructGroup> CollectStructGroups(IGameProfile *profile)
    {
        std::vector<StructGroup> groups;
        if (!profile)
            return groups;

        const auto *vars = profile->GetUEVars();
        if (!vars)
            return groups;

        UE_Offsets *off = vars->GetOffsets();
        if (!off)
            return groups;

        auto pushField = [](StructGroup &g, const char *name, const char *type,
                            uintptr_t offset, const char *desc, bool allowZero = false)
        {
            StructFieldEntry e;
            e.name = name;
            e.type = type;
            e.offset = offset;
            e.found = allowZero ? true : (offset != 0);
            e.description = desc ? desc : "";
            g.fields.push_back(std::move(e));
        };

        // 基础指针组
        {
            StructGroup g;
            g.name = "基础";
            const uintptr_t base = vars->GetBaseAddress();
            auto pushAbs = [&](const char *name, const char *type, uintptr_t value, const char *desc)
            {
                StructFieldEntry e;
                e.name = name;
                e.type = type;
                e.offset = (value && base && value >= base) ? (value - base) : value;
                e.found = value != 0;
                e.description = desc;
                g.fields.push_back(std::move(e));
            };
            pushAbs("BaseAddress", "uintptr_t", base, "可执行模块基址");
            pushAbs("GUObjectArray", "FUObjectArray*", vars->GetGUObjectsArrayPtr(), "全局对象数组指针 (相对基址)");
            pushAbs("ObjObjects", "FChunkedFixedUObjectArray*", vars->GetObjObjectsPtr(), "ObjObjects 指针 (相对基址)");
            pushAbs(profile->IsUsingFNamePool() ? "FNamePool" : "GNames",
                    profile->IsUsingFNamePool() ? "FNamePool*" : "TStaticIndirectArrayThreadSafeRead*",
                    vars->GetNamesPtr(), "名称池/全局名称表 (相对基址)");
            pushAbs("ProcessEvent", "void*", vars->GetProcessEvent(), "ProcessEvent 函数地址 (相对基址)");
            pushAbs("StaticFindObject", "void*", vars->GetStaticFindObject(), "StaticFindObject 函数地址 (相对基址)");
            pushAbs("FrameCount", "uint64*", vars->GetFrameCount(), "GFrameCounter 地址 (相对基址)");
            pushAbs("Matrix", "FMatrix*", vars->GetMatrix(), "ViewProjection 矩阵地址 (相对基址)");
            pushAbs("Physx", "void*", vars->GetPhysx(), "PhysX 全局指针 (相对基址)");
            pushAbs("NativeAndroidApp", "android_app*", vars->GetNativeAndroidApp(), "GNativeAndroidApp (相对基址)");
            groups.push_back(std::move(g));
        }

        // UObject
        {
            StructGroup g;
            g.name = "UObject";
            pushField(g, "VTable", "void*", 0, "虚函数表 (固定 0)", true);
            pushField(g, "ObjectFlags", "EObjectFlags", off->UObject.ObjectFlags, "对象标志", true);
            pushField(g, "InternalIndex", "int32", off->UObject.InternalIndex, "GUObjectArray 中的索引", true);
            pushField(g, "ClassPrivate", "UClass*", off->UObject.ClassPrivate, "对象所属类");
            pushField(g, "NamePrivate", "FName", off->UObject.NamePrivate, "对象名称");
            pushField(g, "OuterPrivate", "UObject*", off->UObject.OuterPrivate, "外部 (Outer) 对象");
            groups.push_back(std::move(g));
        }

        // UField
        {
            StructGroup g;
            g.name = "UField";
            pushField(g, "Next", "UField*", off->UField.Next, "链表下一个 UField");
            groups.push_back(std::move(g));
        }

        // UStruct
        {
            StructGroup g;
            g.name = "UStruct";
            pushField(g, "SuperStruct", "UStruct*", off->UStruct.SuperStruct, "父结构 (基类)");
            pushField(g, "Children", "UField*", off->UStruct.Children, "子字段链表");
            pushField(g, "ChildProperties", "FField*", off->UStruct.ChildProperties, "子属性链表 (UE5)");
            pushField(g, "PropertiesSize", "int32", off->UStruct.PropertiesSize, "属性总大小");
            pushField(g, "MinAlignment", "int32", off->UStruct.MinAlignment, "最小对齐");
            groups.push_back(std::move(g));
        }

        // UClass
        {
            StructGroup g;
            g.name = "UClass";
            pushField(g, "(继承自 UStruct)", "-", 0, "UClass 复用 UStruct 全部偏移", true);
            pushField(g, "ClassDefaultObject", "UObject*", off->UClass.ClassDefaultObject, "类默认对象");
            pushField(g, "ImplementedInterfaces", "TArray<FImplementedInterface>", off->UClass.ImplementedInterfaces, "接口实现列表");
            pushField(g, "CastFlags", "uint64", off->UClass.CastFlags, "类 CastFlags");
            pushField(g, "ClassFlags", "uint32", off->UClass.ClassFlags, "类标志");
            groups.push_back(std::move(g));
        }

        // UScriptStruct
        {
            StructGroup g;
            g.name = "UScriptStruct";
            pushField(g, "(继承自 UStruct)", "-", 0, "UScriptStruct 复用 UStruct 布局", true);
            pushField(g, "StructFlags", "uint32", off->UScriptStruct.StructFlags, "脚本结构标志");
            groups.push_back(std::move(g));
        }

        // UFunction
        {
            StructGroup g;
            g.name = "UFunction";
            pushField(g, "EFunctionFlags", "EFunctionFlags", off->UFunction.EFunctionFlags, "函数标志");
            pushField(g, "NumParams", "uint8", off->UFunction.NumParams, "参数个数");
            pushField(g, "ParamSize", "uint16", off->UFunction.ParamSize, "参数总大小");
            pushField(g, "Func", "void*", off->UFunction.Func, "函数指针 (Native)");
            groups.push_back(std::move(g));
        }

        // FField + FProperty (UE 4.25+ / UE5)
        {
            StructGroup g;
            g.name = "FField+FProperty";
            pushField(g, "FField.ClassPrivate", "FFieldClass*", off->FField.ClassPrivate, "FField 所属类");
            pushField(g, "FField.Owner", "FFieldVariant", off->FField.Owner, "FField 所属者");
            pushField(g, "FField.Next", "FField*", off->FField.Next, "下一个 FField");
            pushField(g, "FField.NamePrivate", "FName", off->FField.NamePrivate, "FField 名称");
            pushField(g, "FField.FlagsPrivate", "EObjectFlags", off->FField.FlagsPrivate, "FField 标志");
            pushField(g, "FFieldClass.Name", "FName", off->FFieldClass.Name, "FFieldClass 名称");
            pushField(g, "FFieldClass.SuperClass", "FFieldClass*", off->FFieldClass.SuperClass, "FFieldClass 父类");
            pushField(g, "FFieldClass.CastFlags", "uint64", off->FFieldClass.CastFlags, "FFieldClass CastFlags");
            pushField(g, "FProperty.ArrayDim", "int32", off->FProperty.ArrayDim, "数组维度");
            pushField(g, "FProperty.ElementSize", "int32", off->FProperty.ElementSize, "单元素大小");
            pushField(g, "FProperty.PropertyFlags", "uint64", off->FProperty.PropertyFlags, "属性标志");
            pushField(g, "FProperty.Offset_Internal", "int32", off->FProperty.Offset_Internal, "属性内部偏移");
            pushField(g, "FProperty.Size", "size_t", off->FProperty.Size, "FProperty 总大小");
            pushField(g, "UProperty.ArrayDim", "int32", off->UProperty.ArrayDim, "(旧版) 数组维度");
            pushField(g, "UProperty.ElementSize", "int32", off->UProperty.ElementSize, "(旧版) 单元素大小");
            pushField(g, "UProperty.PropertyFlags", "uint64", off->UProperty.PropertyFlags, "(旧版) 属性标志");
            pushField(g, "UProperty.Offset_Internal", "int32", off->UProperty.Offset_Internal, "(旧版) 属性内部偏移");
            pushField(g, "UProperty.Size", "size_t", off->UProperty.Size, "(旧版) UProperty 总大小");
            groups.push_back(std::move(g));
        }

        // FName / FNameEntry / FNamePool
        {
            StructGroup g;
            g.name = "FName";
            pushField(g, "FName.ComparisonIndex", "int32", off->FName.ComparisonIndex, "比较索引", true);
            pushField(g, "FName.DisplayIndex", "int32", off->FName.DisplayIndex, "显示索引");
            pushField(g, "FName.Number", "int32", off->FName.Number, "Number 字段");
            pushField(g, "FName.Size", "size_t", off->FName.Size, "FName 结构大小");
            pushField(g, "FNameEntry.Index", "int32", off->FNameEntry.Index, "FNameEntry 索引");
            pushField(g, "FNameEntry.Name", "char/wchar*", off->FNameEntry.Name, "FNameEntry 字符串");
            pushField(g, "FNamePool.Stride", "uint32", off->FNamePool.Stride, "Pool 步长");
            pushField(g, "FNamePool.BlocksBit", "uint32", off->FNamePool.BlocksBit, "Block 位宽");
            pushField(g, "FNamePool.BlocksOff", "uint32", off->FNamePool.BlocksOff, "Block 起始偏移");
            pushField(g, "FNamePoolEntry.Header", "uint16", off->FNamePoolEntry.Header, "PoolEntry 头");
            groups.push_back(std::move(g));
        }

        {
            StructGroup g;
            g.name = "ULevel+UDataTable";
            pushField(g, "ULevel.Actors", "TArray<AActor*>", off->ULevel.Actors, "关卡中的 Actor 列表");
            pushField(g, "UDataTable.RowMap", "TMap<FName,uint8*>", off->UDataTable.RowMap, "数据表行映射");
            groups.push_back(std::move(g));
        }

        // FUObjectArray / TUObjectArray / FUObjectItem
        {
            StructGroup g;
            g.name = "ObjectArray";
            pushField(g, "FUObjectArray.ObjObjects", "FChunkedFixedUObjectArray", off->FUObjectArray.ObjObjects, "ObjObjects 偏移", true);
            pushField(g, "TUObjectArray.Objects", "void**", off->TUObjectArray.Objects, "对象指针表", true);
            pushField(g, "TUObjectArray.NumElements", "int32", off->TUObjectArray.NumElements, "元素数量");
            pushField(g, "TUObjectArray.NumElementsPerChunk", "int32", off->TUObjectArray.NumElementsPerChunk, "每块元素数量");
            pushField(g, "FUObjectItem.Object", "UObject*", off->FUObjectItem.Object, "对象指针", true);
            pushField(g, "FUObjectItem.Size", "size_t", off->FUObjectItem.Size, "ObjectItem 大小");
            groups.push_back(std::move(g));
        }

        // UEnum
        {
            StructGroup g;
            g.name = "UEnum";
            pushField(g, "Names", "TArray<TPair<FName,int64>>", off->UEnum.Names, "枚举名称数组");
            groups.push_back(std::move(g));
        }

        return groups;
    }

    void ExecuteProbe(const AutoProcessCandidate candidate)
    {
        PendingProbeOverrideGuard consumeOverrides;
        BeginProbeState(candidate);

        LOGI("当前使用 UE Dumper %s", kUEDUMPER_VERSION);
        LOGI("目标包名: %s", candidate.package.c_str());
        LOGI("进程 ID: %d", candidate.pid);
        LOGI("==========================");

        SetDumpPhase("初始化内存");
        LOGI("正在初始化内存...");
        if (!kMgr.initialize(candidate.pid, EK_MEM_OP_SYSCALL, false) &&
            !kMgr.initialize(candidate.pid, EK_MEM_OP_IO, false))
        {
            LOGE("初始化 KittyMemoryMgr 失败。");
            FinishProbeState(false, {}, {}, "ERROR_INIT_MEMORY");
            return;
        }

        gProbeResult = ProbeResult{};
        gProbeResult.pid = candidate.pid;
        gProbeResult.package = candidate.package;
        gProbeResult.profileName = candidate.profileName;
        gProbeResult.dedicated = candidate.dedicated;

        IGameProfile *matchedProfile = nullptr;
        for (auto *profile : UE_Games)
        {
            for (const auto &pkg : profile->GetAppIDs())
            {
                if (candidate.package == pkg)
                {
                    matchedProfile = profile;
                    break;
                }
            }
            if (matchedProfile)
                break;
        }

        std::string lastError;
        bool initOk = false;
        UEDumper probeDumper{};
        const UEAddressOverrides addressOverrides = BuildProfileOverrides();
        {
            const auto snapshot = UmtMcp::Analysis::CaptureMaps(kMgr);
            gProbeResult.mapRevision = snapshot.revision;
            gProbeResult.processStartTime = snapshot.processStartTime;
            std::lock_guard<std::mutex> lock(gOverrideMutex);
            for (const char *key : {"names", "objects", "world"})
            {
                auto value = gProbeOverrides.find(key);
                const bool overridden = value != gProbeOverrides.end();
                gProbeResult.fieldSources[key] = overridden ? "OVERRIDE" : "AUTO";
                if (overridden)
                {
                    nlohmann::json evidence = {{"field", key}, {"source", "OVERRIDE"},
                                               {"address", UmtMcp::FormatAddress(value->second)}};
                    auto layout = gProbeOverrideLayouts.find(key);
                    if (layout != gProbeOverrideLayouts.end()) evidence["layout"] = layout->second;
                    gProbeResult.overrideEvidence.push_back(std::move(evidence));
                    LOGI("[ProbeSource] %s=OVERRIDE address=%s", key,
                         UmtMcp::FormatAddress(value->second).c_str());
                }
                else
                    LOGI("[ProbeSource] %s=AUTO", key);
            }
        }

        if (matchedProfile)
        {
            SetDumpPhase("探测专用 Profile");
            LOGI("识别到专用 Profile: %s", matchedProfile->GetAppName().c_str());
            matchedProfile->SetAddressOverrides(addressOverrides);
            initOk = probeDumper.Init(matchedProfile);
            if (initOk)
            {
                gProbeResult.profile = matchedProfile;
            }
            else
            {
                std::string err = probeDumper.GetLastError();
                LOGW("专用 Profile 初始化失败 (%s)，回退到自动 Profile。",
                     err.empty() ? "未知原因，可能版本偏移不匹配" : err.c_str());
            }
        }

        if (!initOk)
        {
            SetDumpPhase("探测自动 Profile");
            LOGI("使用自动 Profile (UE4/UE5 通用) 进行探测。");
            gProbeResult.autoProfileOwner = std::make_unique<AutoFixProfile>(candidate.package);
            gProbeResult.autoProfileOwner->SetAddressOverrides(addressOverrides);
            initOk = probeDumper.Init(gProbeResult.autoProfileOwner.get());
            if (initOk)
            {
                gProbeResult.profile = gProbeResult.autoProfileOwner.get();
                gProbeResult.profileName = matchedProfile
                    ? std::string("自动识别 (专用 Profile [") + matchedProfile->GetAppName() + "] 失败回退)"
                    : std::string("自动识别 (UE4/UE5 通用)");
                gProbeResult.dedicated = false;
            }
        }

        if (!initOk)
        {
            lastError = probeDumper.GetLastError();
            if (lastError.empty())
                lastError = "ERROR_PROBE_INIT_FAILED";
            LOGE("探针失败: %s", lastError.c_str());
            FinishProbeState(false, {}, {}, lastError);
            return;
        }

        auto offsets = CollectProbeOffsets(gProbeResult.profile);
        auto structGroups = CollectStructGroups(gProbeResult.profile);
        // 取消检查点
        if (gCancelRequested)
        {
            LOGW("探针已被取消。");
            FinishProbeState(false, {}, {}, "CANCELLED");
            return;
        }
        gProbeResult.offsets = offsets;
        gProbeResult.structGroups = structGroups;
        gProbeResult.baseAddress = gProbeResult.profile && gProbeResult.profile->GetUEVars()
                                       ? gProbeResult.profile->GetUEVars()->GetBaseAddress()
                                       : 0;
        gProbeResult.valid = true;
        gProbeResult.success = true;

        LOGI("探针完成，已识别核心偏移：");
        for (const auto &entry : offsets)
        {
            if (entry.found)
                LOGI("  %s: <Base> + 0x%lX", entry.name.c_str(),
                     static_cast<unsigned long>(entry.relative));
            else
                LOGI("  %s: <未识别>", entry.name.c_str());
        }
        LOGI("==========================");
        FinishProbeState(true, offsets, structGroups, probeDumper.GetLastError());
    }

    void ExecuteDump(const AutoProcessCandidate candidate)
    {
        BeginDumpState(candidate.package);

        if (!gProbeResult.valid || !gProbeResult.success || !gProbeResult.profile)
        {
            LOGE("请先完成探针流程。");
            FinishDumpState(false, {}, "ERROR_NO_PROBE_RESULT");
            return;
        }
        if (gProbeResult.package != candidate.package || gProbeResult.pid != candidate.pid)
        {
            LOGE("探针目标 (%s, pid=%d) 与 Dump 目标 (%s, pid=%d) 不一致，请重新探测。",
                 gProbeResult.package.c_str(), gProbeResult.pid,
                 candidate.package.c_str(), candidate.pid);
            FinishDumpState(false, {}, "ERROR_PROBE_MISMATCH");
            return;
        }

        const std::string dumpGameDir = std::string(kOutputDirectory) + "/" + candidate.package;
        IOUtils::delete_directory(dumpGameDir);
        if (IOUtils::mkdir_recursive(dumpGameDir, 0777) == -1)
        {
            const int err = errno;
            LOGE("创建输出目录失败 [\"%s\"]，错误=%d | %s。", kOutputDirectory, err, strerror(err));
            FinishDumpState(false, dumpGameDir, "ERROR_CREATE_OUTPUT_DIR");
            return;
        }

        UEDumper uEDumper{};
        uEDumper.setDumpExeInfoNotify([](bool finished)
        {
            if (!finished) { SetDumpPhase("导出可执行信息"); LOGI("正在导出可执行信息..."); }
        });
        uEDumper.setDumpNamesInfoNotify([](bool finished)
        {
            if (!finished) { SetDumpPhase("导出名称信息"); LOGI("正在导出名称信息..."); }
        });
        uEDumper.setDumpObjectsInfoNotify([](bool finished)
        {
            if (!finished) { SetDumpPhase("导出对象信息"); LOGI("正在导出对象信息..."); }
        });
        uEDumper.setDumpOffsetsInfoNotify([](bool finished)
        {
            if (!finished) { SetDumpPhase("导出偏移信息"); LOGI("正在导出偏移信息..."); }
        });
        uEDumper.setObjectsProgressCallback([](const SimpleProgressBar &progress)
        {
            SetProgress(progress.getPercentage(), -1);
        });
        uEDumper.setDumpProgressCallback([](const SimpleProgressBar &progress)
        {
            SetProgress(-1, progress.getPercentage());
        });

        SetDumpPhase("初始化 Dumper");
        LOGI("正在初始化 Dumper...");
        LOGI("Dump 将复用探测阶段已初始化的 Profile/Offsets。");
        if (!uEDumper.Init(gProbeResult.profile, true))
        {
            std::string err = uEDumper.GetLastError();
            if (err.empty()) err = "ERROR_INIT_DUMPER";
            LOGE("初始化 Dumper 失败: %s", err.c_str());
            FinishDumpState(false, dumpGameDir, err);
            return;
        }

        std::unordered_map<std::string, BufferFmt> dumpbuffersMap;
        const auto dumpStart = std::chrono::steady_clock::now();

        SetDumpPhase("开始 Dump");
        // 取消检查点
        if (gCancelRequested)
        {
            LOGW("Dump 已被取消。");
            FinishDumpState(false, dumpGameDir, "CANCELLED");
            return;
        }
        bool dumpSuccess = uEDumper.Dump(&dumpbuffersMap);

        if (!dumpSuccess && uEDumper.GetLastError().empty())
        {
            LOGE("当前游戏暂不支持，请检查包名。");
            FinishDumpState(false, dumpGameDir, "ERROR_UNSUPPORTED_GAME");
            return;
        }

        if (dumpbuffersMap.empty())
        {
            LOGE("导出失败，错误 <缓冲区为空>");
            LOGE("导出状态 <%s>", uEDumper.GetLastError().c_str());
            FinishDumpState(false, dumpGameDir, uEDumper.GetLastError());
            return;
        }

        SetDumpPhase("保存文件");
        if (!SaveDumpBuffers(dumpbuffersMap, dumpGameDir))
        {
            LOGE("保存导出文件失败。");
            FinishDumpState(false, dumpGameDir, "ERROR_SAVE_FILES");
            return;
        }
        const auto dumpEnd = std::chrono::steady_clock::now();
        const std::chrono::duration<float, std::milli> dumpDurationMS = dumpEnd - dumpStart;
        if (!uEDumper.GetLastError().empty())
            LOGI("导出状态: %s", uEDumper.GetLastError().c_str());
        LOGI("导出耗时: %.2fms", dumpDurationMS.count());
        LOGI("导出位置: %s", dumpGameDir.c_str());

        FinishDumpState(true, dumpGameDir, uEDumper.GetLastError());
    }

    void ExecuteDumpUnrealLib(const AutoProcessCandidate candidate, const std::string source)
    {
        {
            std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
            gDumpUiState.soDumpRunning = true;
            gDumpUiState.soDumpFinished = false;
            gDumpUiState.soDumpSuccess = false;
            gDumpUiState.soDumpPath.clear();
            gDumpUiState.soDumpArtifact = nlohmann::json::object();
            gDumpUiState.phase = "Dump 动态库准备中";
        }

        nlohmann::json artifact = nlohmann::json::object();
        auto finish = [&](bool ok, const std::string &path)
        {
            std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
            gDumpUiState.soDumpRunning = false;
            gDumpUiState.soDumpFinished = true;
            gDumpUiState.soDumpSuccess = ok;
            gDumpUiState.soDumpPath = path;
            gDumpUiState.soDumpArtifact = artifact;
            gDumpUiState.phase = ok ? "动态库 Dump 完成" : "动态库 Dump 失败";
        };

        if (!kMgr.isMemValid() || kMgr.processID() != candidate.pid)
        {
            LOGE("动态库 Dump 需要先对当前目标执行 ATTACH。");
            finish(false, {});
            return;
        }

        SetDumpPhase("Dump 动态库");
        const std::string dumpGameDir = std::string(kOutputDirectory) + "/" + candidate.package;
        if (IOUtils::mkdir_recursive(dumpGameDir, 0777) == -1 && errno != EEXIST)
        {
            const int err = errno;
            LOGE("创建输出目录失败 [\"%s\"]，错误=%d | %s。", dumpGameDir.c_str(), err, strerror(err));
            finish(false, {});
            return;
        }

        auto ue_elf = UmtMcp::Analysis::FindUnrealElf(kMgr);
        if (!ue_elf.isValid())
        {
            LOGE("未找到有效的 UE ELF (libUE4.so / libUnreal.so)。");
            finish(false, {});
            return;
        }

        std::string elfPath = ue_elf.filePath();
        std::string elfName;
        if (!elfPath.empty())
        {
            size_t slash = elfPath.find_last_of('/');
            elfName = (slash == std::string::npos) ? elfPath : elfPath.substr(slash + 1);
        }
        if (elfName.empty())
            elfName = "libUE4.so";

        std::string dumpSoPath = dumpGameDir + "/" + elfName;
        const auto t0 = std::chrono::steady_clock::now();
        LOGI("正在转储 %s (base=0x%lX, size=0x%lX) -> %s",
             elfName.c_str(),
             (unsigned long)ue_elf.base(),
             (unsigned long)(ue_elf.end() - ue_elf.base()),
             dumpSoPath.c_str());

        bool dumped = false;
        std::string sourceUsed = "MEMORY";
        if (source != "MEMORY" && !elfPath.empty() && ue_elf.baseSegment().offset == 0)
        {
            std::ifstream input(elfPath, std::ios::binary);
            std::ofstream output(dumpSoPath, std::ios::binary | std::ios::trunc);
            if (input && output)
            {
                output << input.rdbuf();
                dumped = (input.good() || input.eof()) && output.good();
                if (dumped) sourceUsed = "FILE";
            }
        }
        if (!dumped && source != "FILE")
            dumped = kMgr.dumpMemELF(ue_elf, dumpSoPath);
        if (!dumped)
        {
            LOGE("动态库转储失败 (写入 %s 失败)。", dumpSoPath.c_str());
            finish(false, dumpSoPath);
            return;
        }

        const auto t1 = std::chrono::steady_clock::now();
        const std::chrono::duration<float, std::milli> ms = t1 - t0;
        LOGI("动态库转储完成，source=%s，耗时 %.2fms。", sourceUsed.c_str(), ms.count());
        LOGI("文件位置: %s", dumpSoPath.c_str());
        artifact = UmtMcp::Analysis::InspectArtifactFile(dumpSoPath, sourceUsed);
        finish(true, dumpSoPath);
    }

    std::string StartProbeSelected()
    {
        if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
        {
            PushUiLog('E', "请先选择目标进程。");
            return {};
        }
        if (gWorkerThread.joinable())
            gWorkerThread.join();
        const std::string jobId = StartJob("probe");
        const AutoProcessCandidate candidate = gCandidates[gSelectedIndex];
        gWorkerThread = std::thread([candidate, jobId]() {
            ExecuteProbe(candidate);
            const bool success = gProbeResult.valid && gProbeResult.success;
            FinishJob(jobId, success, success ? std::string() : "PROBE_FAILED");
        });
        return jobId;
    }

    std::string StartDumpAfterProbe()
    {
        if (!gProbeResult.valid || !gProbeResult.success)
        {
            PushUiLog('E', "请先成功完成探针流程，再开始 Dump。");
            return {};
        }
        if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
        {
            PushUiLog('E', "请先选择目标进程。");
            return {};
        }
        if (gWorkerThread.joinable())
            gWorkerThread.join();
        const std::string jobId = StartJob("dump");
        const AutoProcessCandidate candidate = gCandidates[gSelectedIndex];
        gWorkerThread = std::thread([candidate, jobId]() {
            ExecuteDump(candidate);
            bool success = false;
            { std::lock_guard<std::mutex> lock(gDumpUiState.mutex); success = gDumpUiState.dumpSuccess; }
            FinishJob(jobId, success, success ? std::string() : "DUMP_FAILED");
        });
        return jobId;
    }

    std::string StartDumpUnrealLib(const std::string &source = "AUTO")
    {
        if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
        {
            PushUiLog('E', "请先选择目标进程。");
            return {};
        }
        if (!kMgr.isMemValid() || kMgr.processID() != gCandidates[gSelectedIndex].pid)
        {
            PushUiLog('E', "请先 ATTACH 当前目标进程。");
            return {};
        }
        if (HasRunningJob() || HasRunningUiTask()) return {};
        if (gWorkerThread.joinable())
            gWorkerThread.join();
        const std::string jobId = StartJob("dump_unreal_library");
        const AutoProcessCandidate candidate = gCandidates[gSelectedIndex];
        gWorkerThread = std::thread([candidate, source, jobId]()
        {
            ExecuteDumpUnrealLib(candidate, source);
            bool success = false;
            {
                std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
                success = gDumpUiState.soDumpSuccess;
            }
            FinishJob(jobId, success, success ? std::string() : "DUMP_UNREAL_LIBRARY_FAILED");
        });
        return jobId;
    }

    std::string StartProbeAndDump()
    {
        if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
            return {};
        if (HasRunningJob() || HasRunningUiTask()) return {};
        if (gWorkerThread.joinable()) gWorkerThread.join();
        const std::string jobId = StartJob("dump_sdk");
        const AutoProcessCandidate candidate = gCandidates[gSelectedIndex];
        gWorkerThread = std::thread([candidate, jobId]()
        {
            UpdateJobProgress(jobId, 5);
            ExecuteProbe(candidate);
            if (!gProbeResult.valid || !gProbeResult.success || gCancelRequested)
            {
                FinishJob(jobId, false, gCancelRequested ? "CANCELLED" : "PROBE_FAILED");
                return;
            }
            UpdateJobProgress(jobId, 50);
            ExecuteDump(candidate);
            bool success = false;
            std::string error;
            {
                std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
                success = gDumpUiState.dumpSuccess;
                error = gDumpUiState.lastError;
            }
            FinishJob(jobId, success, success ? std::string() :
                (error.empty() ? std::string("DUMP_FAILED") : error));
        });
        return jobId;
    }

    std::string StartAnalysisJob(const std::string &type,
                                 std::function<nlohmann::json()> work)
    {
        if (HasRunningJob()) return {};
        {
            std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
            if (gDumpUiState.probeRunning || gDumpUiState.dumpRunning || gDumpUiState.soDumpRunning)
                return {};
        }
        if (gWorkerThread.joinable()) gWorkerThread.join();
        gCancelRequested = false;
        const std::string jobId = StartJob(type);
        gWorkerThread = std::thread([jobId, work = std::move(work)]() mutable
        {
            try
            {
                FinishJobWithResult(jobId, work());
            }
            catch (const UmtMcp::HandlerError &error)
            {
                FinishJob(jobId, false, error.code + std::string(": ") + error.what());
            }
            catch (const std::exception &error)
            {
                FinishJob(jobId, false, std::string("E_INTERNAL: ") + error.what());
            }
        });
        return jobId;
    }

    // ---------------------------------------------------------------------
    // MCP 命令注册（服务端）
    //
    // 全部命令都在主线程 Layout_tick_UI 之后由 CommandDispatcher::PollOnce() 执行——
    // UMT 全局非线程安全，放主线程即与 UI 操作天然串行，无需加锁。
    //
    // ⚠️ isFast 目前是【死参数】：Register() 把它存进 CommandDispatcher::fastFlags_，
    //    但 PollOnce() 从不读取——所有命令一律当场同步执行，跟 isFast 取什么值无关。
    //
    //    【后果是什么】耗时命令会同步占满主线程 → ImGui 渲染循环停 → **界面卡住**。
    //    但 🔴 心跳不会停：心跳由 CommandServer 线程在 HandleFrame 的等待循环里发
    //    （CommandServer.cpp:255），跟主线程忙不忙无关。所以 PC 侧**不会**误判假死，
    //    只是设备端界面冻住，且命令若超过 kCommandTimeoutSec(120s) 会被掐断返 E_TIMEOUT。
    //
    //    【哪些命令真慢】不要照协议文档 §6/§7 的字面描述判断，要看实现：
    //      · SCAN_PATTERN  扫最多 512MB → 真慢（已标 false）
    //      · SEARCH_CLASSES 遍历整个 GUObjectArray → 真慢（已标 false）
    //      · SCAN_CANDIDATES 按 maps 扫区域 → 真慢
    //      · LOCATE_ENGINE_GLOBALS 多步编排 → 真慢（已标 false）
    //    而 SCAN_GNAMES / SCAN_OBJECTS **并不慢**：它们复用 START_PROBE 已解析出的
    //    指针，只采样 16 条做自校验，是毫秒级。协议文档说它们"分钟级"是按
    //    "从零全网扫描"的语义写的，与当前实现不符 —— 标 true 是对的。
    //
    //    isFast 的【预期】语义保留为：true=同步执行；false=应投递 gWorkerThread。
    //    要让它真正生效，需在 PollOnce() 里按 fastFlags_ 路由（见 docs/api/12 §5.3.1）。
    //    在此之前，改这个标志位没有任何运行时效果，别误以为改了就修好了。
    //
    // 本函数定义在匿名 namespace 内部，因此可以直接访问 FindAutoProcessCandidates
    // 与 gDumpUiState。
    // ---------------------------------------------------------------------
    // G 组引擎语义命令的前置检查：attach 完成 + probe 完成 + UEWrappers 已初始化。
    // 复用 START_PROBE 已建好的运行时（UEWrappers::Init 在 profile 初始化时调用），不重写探测逻辑。
    static UE_UObjectArray *RequireObjects()
    {
        if (!UEMemory::kMgr.isMemValid())
            throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
        if (!gProbeResult.valid || !gProbeResult.profile)
            throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "请先 START_PROBE 完成探针流程");
        auto *objects = UEWrappers::GetObjects();
        if (!objects)
            throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "对象数组未初始化（探针未完成）");
        return objects;
    }

    // ── F 组远程调用的共用辅助 ────────────────────────────────────────────────

    // 解析单个调用参数：支持 "0x..."（地址/指针）与 "i:123"（十进制或 0x 整数）。
    bool ParseCallArg(const std::string &s, uintptr_t &out)
    {
        if (s.size() > 2 && s[0] == 'i' && s[1] == ':')
        {
            errno = 0;
            char *end = nullptr;
            const long long v = std::strtoll(s.c_str() + 2, &end, 0);
            if (errno != 0 || !end || *end != '\0') return false;
            out = static_cast<uintptr_t>(v);
            return true;
        }
        return UmtMcp::ParseAddress(s, out);
    }

    // 按 returnKind 解释 64 位返回值；同时保留原始值供 AI 校验。
    // 注意：json 别名声明在 namespace UmtMcp 内（CommandDispatcher.hpp），此处在全局匿名
    // 命名空间中，必须显式限定 UmtMcp::json（SetupMcpCommands 内部才有局部 using json）。
    UmtMcp::json FormatReturnValue(uintptr_t raw, const std::string &kind)
    {
        if (kind == "void") return nullptr;
        if (kind == "i32") return static_cast<int32_t>(raw);
        if (kind == "i64") return static_cast<int64_t>(raw);
        if (kind == "f32")
        {
            const uint32_t lo = static_cast<uint32_t>(raw & 0xFFFFFFFFu);
            float f = 0.f;
            std::memcpy(&f, &lo, sizeof(f));
            return f;
        }
        return UmtMcp::FormatAddress(raw);  // 默认 ptr
    }

    // KittyTraceMgr::callFunctionFrom 是变参函数，按实参个数分发（aarch64 最多 8 个寄存器参数）。
    uintptr_t InvokeRemoteFunction(uintptr_t trapAddress, uintptr_t functionAddress,
                                   const std::vector<uintptr_t> &a)
    {
        const auto &t = UEMemory::kMgr.trace;
        switch (a.size())
        {
            case 0: return t.callFunctionFrom(trapAddress, functionAddress, 0);
            case 1: return t.callFunctionFrom(trapAddress, functionAddress, 1, a[0]);
            case 2: return t.callFunctionFrom(trapAddress, functionAddress, 2, a[0], a[1]);
            case 3: return t.callFunctionFrom(trapAddress, functionAddress, 3, a[0], a[1], a[2]);
            case 4: return t.callFunctionFrom(trapAddress, functionAddress, 4, a[0], a[1], a[2], a[3]);
            case 5: return t.callFunctionFrom(trapAddress, functionAddress, 5, a[0], a[1], a[2], a[3], a[4]);
            case 6: return t.callFunctionFrom(trapAddress, functionAddress, 6, a[0], a[1], a[2], a[3], a[4], a[5]);
            case 7: return t.callFunctionFrom(trapAddress, functionAddress, 7, a[0], a[1], a[2], a[3], a[4], a[5], a[6]);
            case 8: return t.callFunctionFrom(trapAddress, functionAddress, 8, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
            default: return 0;
        }
    }

    void SetupMcpCommands()
    {
        using json = nlohmann::json;

        // PING —— 连通性与版本验证，不触碰任何 UMT 状态
        UmtMcp::CommandDispatcher::Register("PING",
                                            [](const json &) -> json
                                            {
                                                return {{"build", kUEDUMPER_VERSION},
                                                        {"protocol", UmtMcp::kProtocolVersion},
                                                        {"connected", true}};
                                            },
                                            true);

        // LIST_PROCESSES —— 外迁 FindAutoProcessCandidates()
        UmtMcp::CommandDispatcher::Register("LIST_PROCESSES",
                                            [](const json &args) -> json
                                            {
                                                const bool dedicatedOnly = args.value("dedicatedOnly", false);

                                                json arr = json::array();
                                                for (const auto &c : FindAutoProcessCandidates())
                                                {
                                                    if (dedicatedOnly && !c.dedicated)
                                                        continue;
                                                    arr.push_back({{"pid", c.pid},
                                                                   {"package", c.package},
                                                                   {"profileName", c.profileName},
                                                                   {"dedicated", c.dedicated}});
                                                }
                                                return {{"processes", arr}};
                                            },
                                            true);

        // GET_LOGS —— 读 gDumpUiState.logLines 环形缓冲（必须持 mutex）
        UmtMcp::CommandDispatcher::Register("GET_LOGS",
                                            [](const json &args) -> json
                                            {
                                                const int sinceIndex = args.value("sinceIndex", 0);
                                                int maxLines = args.value("maxLines", 50);
                                                if (maxLines <= 0)
                                                    maxLines = 50;
                                                if (maxLines > static_cast<int>(kMaxLogLines))
                                                    maxLines = static_cast<int>(kMaxLogLines);

                                                json lines = json::array();
                                                int total = 0;
                                                {
                                                    std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
                                                    total = static_cast<int>(gDumpUiState.logLines.size());

                                                    for (int i = sinceIndex;
                                                         i < total && static_cast<int>(lines.size()) < maxLines;
                                                         ++i)
                                                    {
                                                        const std::string &raw = gDumpUiState.logLines[i];
                                                        // PushUiLog 格式："<level>: <message>"
                                                        std::string level;
                                                        std::string message = raw;
                                                        if (raw.size() >= 2 && raw[1] == ':')
                                                        {
                                                            level = raw.substr(0, 1);
                                                            message = (raw.size() > 3) ? raw.substr(3) : std::string();
                                                        }
                                                        lines.push_back({{"index", i},
                                                                         {"level", level},
                                                                         {"timestamp", ""},
                                                                         {"message", message}});
                                                    }
                                                }
                                                return {{"lines", lines}, {"totalLines", total}};
                                            },
                                            true);
        // ── GET_CAPABILITIES
        UmtMcp::CommandDispatcher::Register("GET_CAPABILITIES",
            [](const json &args) -> json
            {
                auto cmds = UmtMcp::CommandDispatcher::RegisteredCommands();
                return {{"commands", cmds}, {"build", kUEDUMPER_VERSION}, {"protocol", UmtMcp::kProtocolVersion}};
            }, true);

        // ── MEMORY_READ
        UmtMcp::CommandDispatcher::Register("MEMORY_READ",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                std::string addrStr = args.value("address", "");
                uintptr_t addr = 0;
                if (!UmtMcp::ParseAddress(addrStr, addr))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "address 格式无效: " + addrStr);
                int size = args.value("size", 0);
                if (size < 1 || size > 4096)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "size 须在 [1, 4096] 范围内");
                std::vector<uint8_t> buf(static_cast<size_t>(size));
                size_t bytesRead = UEMemory::kMgr.readMem(addr, buf.data(), static_cast<size_t>(size));
                if (bytesRead != static_cast<size_t>(size))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kReadFailed,
                        "读取失败: 请求 " + std::to_string(size) + " 字节, 实际 " + std::to_string(bytesRead));
                return {{"address", UmtMcp::FormatAddress(addr)}, {"size", size},
                        {"hex", UmtMcp::BytesToHex(buf.data(), static_cast<size_t>(size))}};
            }, true);

        // ── MEMORY_READ_VALUE
        UmtMcp::CommandDispatcher::Register("MEMORY_READ_VALUE",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                std::string addrStr = args.value("address", "");
                uintptr_t addr = 0;
                if (!UmtMcp::ParseAddress(addrStr, addr))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "address 格式无效: " + addrStr);
                std::string valueType = args.value("valueType", "");
                size_t typeSize = UmtMcp::ValueTypeSize(valueType);
                if (typeSize == 0)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "未知 valueType: " + valueType);
                std::vector<uint8_t> buf(typeSize);
                size_t bytesRead = UEMemory::kMgr.readMem(addr, buf.data(), typeSize);
                if (bytesRead != typeSize)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kReadFailed,
                        "读取失败: 请求 " + std::to_string(typeSize) + " 字节, 实际 " + std::to_string(bytesRead));
                json value;
                if (valueType == "bool") { value = (buf[0] != 0); }
                else if (valueType == "i8") { value = static_cast<int8_t>(buf[0]); }
                else if (valueType == "u8") { value = static_cast<uint8_t>(buf[0]); }
                else if (valueType == "i16") { int16_t v=0; memcpy(&v,buf.data(),sizeof(v)); value=v; }
                else if (valueType == "u16") { uint16_t v=0; memcpy(&v,buf.data(),sizeof(v)); value=v; }
                else if (valueType == "i32") { int32_t v=0; memcpy(&v,buf.data(),sizeof(v)); value=v; }
                else if (valueType == "u32") { uint32_t v=0; memcpy(&v,buf.data(),sizeof(v)); value=v; }
                else if (valueType == "i64") { int64_t v=0; memcpy(&v,buf.data(),sizeof(v)); value=v; }
                else if (valueType == "u64") { uint64_t v=0; memcpy(&v,buf.data(),sizeof(v)); value=v; }
                else if (valueType == "f32") { float v=0.f; memcpy(&v,buf.data(),sizeof(v)); value=v; }
                else if (valueType == "f64") { double v=0.; memcpy(&v,buf.data(),sizeof(v)); value=v; }
                else if (valueType == "ptr32") { uint32_t v=0; memcpy(&v,buf.data(),sizeof(v)); value=UmtMcp::FormatAddress(static_cast<uintptr_t>(v)); }
                else if (valueType == "ptr64") { uintptr_t v=0; memcpy(&v,buf.data(),sizeof(v)); value=UmtMcp::FormatAddress(v); }
                return {{"address", UmtMcp::FormatAddress(addr)}, {"valueType", valueType},
                        {"value", value}, {"rawHex", UmtMcp::BytesToHex(buf.data(), typeSize)}};
            }, true);

        // ── READ_STRING
        UmtMcp::CommandDispatcher::Register("READ_STRING",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                std::string addrStr = args.value("address", "");
                uintptr_t addr = 0;
                if (!UmtMcp::ParseAddress(addrStr, addr))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "address 格式无效: " + addrStr);
                int maxLen = args.value("maxLen", 256);
                if (maxLen < 1 || maxLen > 4096)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "maxLen 须在 [1, 4096] 范围内");
                bool isWide = args.value("isWide", false);
                std::string result;
                bool truncated = false;
                if (isWide)
                {
                    std::vector<wchar_t> wbuf(static_cast<size_t>(maxLen) + 1);
                    size_t read = UEMemory::kMgr.readMem(addr, wbuf.data(), static_cast<size_t>(maxLen) * sizeof(wchar_t));
                    if (read != static_cast<size_t>(maxLen) * sizeof(wchar_t))
                        throw UmtMcp::HandlerError(UmtMcp::Err::kReadFailed, "宽字符串读取失败");
                    wbuf[static_cast<size_t>(maxLen)] = L'\0';
                    std::wstring ws(wbuf.data());
                    truncated = (ws.size() == static_cast<size_t>(maxLen));
                    result = UmtMcp::WstringToUtf8(ws);
                }
                else
                {
                    std::vector<char> cbuf(static_cast<size_t>(maxLen) + 1);
                    size_t read = UEMemory::kMgr.readMem(addr, cbuf.data(), static_cast<size_t>(maxLen));
                    if (read != static_cast<size_t>(maxLen))
                        throw UmtMcp::HandlerError(UmtMcp::Err::kReadFailed, "字符串读取失败");
                    cbuf[static_cast<size_t>(maxLen)] = '\0';
                    std::string s(cbuf.data());
                    truncated = (s.size() == static_cast<size_t>(maxLen));
                    result = s;
                }
                return {{"address", UmtMcp::FormatAddress(addr)}, {"value", result},
                        {"isWide", isWide}, {"truncated", truncated}};
            }, true);

        // ── LIST_MODULES
        UmtMcp::CommandDispatcher::Register("LIST_MODULES",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                return UmtMcp::Analysis::ListModules(args, UEMemory::kMgr);
            }, true);

        // ── DECODE_ADRL
        UmtMcp::CommandDispatcher::Register("DECODE_ADRL",
            [](const json &args) -> json
            {
                std::string addrStr = args.value("address", "");
                uintptr_t addr = 0;
                if (!UmtMcp::ParseAddress(addrStr, addr))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "address 格式无效: " + addrStr);
                uintptr_t target = UEMemory::Arm64::DecodeADRL(addr);
                if (target == 0)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kDecodeFailed, "ADRL 解码失败, 地址: " + addrStr);
                return {{"instructionAddress", UmtMcp::FormatAddress(addr)},
                        {"targetAddress", UmtMcp::FormatAddress(target)}};
            }, true);

        // ── WRITE_MEMORY（D 组 · 内存原语 · 写入）
        UmtMcp::CommandDispatcher::Register("WRITE_MEMORY",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                std::string addrStr = args.value("address", "");
                uintptr_t addr = 0;
                if (!UmtMcp::ParseAddress(addrStr, addr))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "address 格式无效: " + addrStr);
                std::string hex = args.value("hex", "");
                if (hex.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "hex 不能为空");
                std::vector<uint8_t> bytes;
                if (!UmtMcp::HexToBytes(hex, bytes))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "hex 格式无效: " + hex);
                if (bytes.empty() || bytes.size() > 4096)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "写入长度须在 [1, 4096] 范围内");
                size_t written = UEMemory::kMgr.writeMem(addr, bytes.data(), bytes.size());
                if (written != bytes.size())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kWriteFailed,
                        "写入失败: 请求 " + std::to_string(bytes.size()) + " 字节, 实际 " + std::to_string(written));
                return {{"address", UmtMcp::FormatAddress(addr)}, {"written", (int)written},
                        {"hex", UmtMcp::BytesToHex(bytes.data(), bytes.size())}};
            }, true);

        // ── SCAN_PATTERN（D 组 · 内存原语 · 特征码扫描）
        UmtMcp::CommandDispatcher::Register("SCAN_PATTERN",
            [](const json &args) -> json
            {
                if (args.value("async", false))
                {
                    json workArgs = args;
                    workArgs.erase("async");
                    const std::string jobId = StartAnalysisJob("scan_pattern", [workArgs]()
                    {
                        return UmtMcp::Analysis::ScanPattern(workArgs, UEMemory::kMgr, &gCancelRequested);
                    });
                    if (jobId.empty()) throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "已有长任务运行中");
                    return {{"jobId", jobId}, {"status", "running"}, {"suggestedWaitMs", 2000}};
                }
                return UmtMcp::Analysis::ScanPattern(args, UEMemory::kMgr, &gCancelRequested);
            }, false); // 扫描可能慢，标为重活

        UmtMcp::CommandDispatcher::Register("SEARCH_MEMORY",
            [](const json &args) -> json
            {
                if (args.value("async", false))
                {
                    json workArgs = args;
                    workArgs.erase("async");
                    const std::string jobId = StartAnalysisJob("search_memory", [workArgs]()
                    {
                        return UmtMcp::Analysis::SearchMemory(workArgs, UEMemory::kMgr, &gCancelRequested);
                    });
                    if (jobId.empty()) throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "已有长任务运行中");
                    return {{"jobId", jobId}, {"status", "running"}, {"suggestedWaitMs", 2000}};
                }
                return UmtMcp::Analysis::SearchMemory(args, UEMemory::kMgr, &gCancelRequested);
            }, false);

        UmtMcp::CommandDispatcher::Register("FIND_REFERENCES",
            [](const json &args) -> json
            {
                if (args.value("async", false))
                {
                    json workArgs = args;
                    workArgs.erase("async");
                    const std::string jobId = StartAnalysisJob("find_references", [workArgs]()
                    {
                        return UmtMcp::Analysis::FindReferences(workArgs, UEMemory::kMgr, &gCancelRequested);
                    });
                    if (jobId.empty()) throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "已有长任务运行中");
                    return {{"jobId", jobId}, {"status", "running"}, {"suggestedWaitMs", 2000}};
                }
                return UmtMcp::Analysis::FindReferences(args, UEMemory::kMgr, &gCancelRequested);
            }, false);

        // ── DISASSEMBLE（E 组 · 理解层 · ARM64 反汇编）
        UmtMcp::CommandDispatcher::Register("DISASSEMBLE",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                std::string addrStr = args.value("address", "");
                uintptr_t addr = 0;
                if (!UmtMcp::ParseAddress(addrStr, addr))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "address 格式无效: " + addrStr);
                int count = args.value("count", 16);
                if (count < 1 || count > 256)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "count 须在 [1, 256] 范围内");
                const size_t totalBytes = (size_t)count * 4;
                std::vector<uint8_t> code(totalBytes);
                size_t read = UEMemory::kMgr.readMem(addr, code.data(), totalBytes);
                if (read < 4)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kReadFailed, "无法读取指令流");
                json lines = json::array();
                uintptr_t pc = addr;
                size_t off = 0;
                while (off + 4 <= read && lines.size() < (size_t)count)
                {
                    uint32_t insn = 0;
                    memcpy(&insn, code.data() + off, 4); // 小端设备直接组装
                    std::string text = UmtMcp::DisassembleArm64(pc, insn);
                    lines.push_back({{"address", UmtMcp::FormatAddress(pc)},
                                     {"bytes", UmtMcp::BytesToHex(code.data() + off, 4)},
                                     {"text", text}});
                    pc += 4; off += 4;
                }
                return {{"address", UmtMcp::FormatAddress(addr)}, {"count", (int)lines.size()},
                        {"instructions", lines}};
            }, true);

        // ── DECOMPILE（E 组 · 理解层 · ARM64 反编译）
        UmtMcp::CommandDispatcher::Register("DECOMPILE",
            [](const json &args) -> json
            {
                return UmtMcp::DecompileHandler::Handle(args);
            }, false);

        // ── DECOMPILER_STATUS（E 组 · 理解层 · 查询反编译器状态）
        UmtMcp::CommandDispatcher::Register("DECOMPILER_STATUS",
            [](const json &) -> json
            {
                return UmtMcp::DecompileHandler::Status();
            }, true);

        // ── F 组 远程调用（ptrace，风险最高）
        // 复用 KittyTraceMgr::callFunctionFrom（KittyTrace.cpp:140，此前 src/ 零引用）+ findRemoteOfSymbol，
        // 会话生命周期由 UmtMcp::PtraceSessionRegistry 管理（四重兜底）。
        //
        // ⚠️ 返回陷阱靠 SIGSEGV/SIGILL 判定，无法区分"陷阱正常触发"与"被调函数自己崩了"，
        //    因此每个结果都附带 rawHex，AI 必须自行校验返回值合理性。
        //
        // TODO(契约对齐): GET_CAPABILITIES 应按 ptraceAvailable 动态摘除 F 组命令；
        //   探测 ptrace 可用性需要真实 attach（会短暂冻结目标进程），不适合在 capabilities 里做，
        //   改由调用时返回 E_PTRACE_FAILED 表达不可用。

        // ── BEGIN_ATTACH_SESSION（F 组：建立冻结会话，四重兜底由 PtraceSessionRegistry 保证）
        UmtMcp::CommandDispatcher::Register("BEGIN_ATTACH_SESSION",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                int maxHoldMs = args.value("maxHoldMs", UmtMcp::kSessionDefaultHoldMs);
                if (maxHoldMs < 1 || maxHoldMs > UmtMcp::kSessionMaxHoldLimitMs)
                    maxHoldMs = UmtMcp::kSessionDefaultHoldMs;
                std::string id;
                try
                {
                    id = UmtMcp::PtraceSessionRegistry::Instance().Begin(maxHoldMs);
                }
                catch (const std::exception &e)
                {
                    // 异常兜底：attach 失败也要确保不残留冻结状态
                    UmtMcp::PtraceSessionRegistry::Instance().ForceDetachAll();
                    throw UmtMcp::HandlerError(UmtMcp::Err::kPtraceFailed, e.what());
                }
                return {{"sessionId", id}, {"maxHoldMs", maxHoldMs},
                        {"note", "会话期间目标进程被冻结；务必调用 END_ATTACH_SESSION，否则看门狗将在超时后强制 detach"}};
            }, true);

        // ── END_ATTACH_SESSION（F 组：结束会话并恢复目标进程）
        UmtMcp::CommandDispatcher::Register("END_ATTACH_SESSION",
            [](const json &args) -> json
            {
                std::string sessionId = args.value("sessionId", "");
                if (sessionId.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "sessionId 不能为空");
                const int64_t elapsed = UmtMcp::PtraceSessionRegistry::Instance().End(sessionId);
                if (elapsed < 0)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound, "无效或已结束的 sessionId: " + sessionId);
                return {{"ok", true}, {"elapsedMs", elapsed}};
            }, true);

        // ── CALL_REMOTE_FUNCTION（F 组：在会话内调用单个函数）
        UmtMcp::CommandDispatcher::Register("CALL_REMOTE_FUNCTION",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                if (args.value("confirmDangerous", false) != true)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs,
                        "远程调用会在目标进程内执行代码。请先用 SCAN_PATTERN / DISASSEMBLE 验证函数地址，"
                        "并显式传 confirmDangerous=true 确认");
                std::string sessionId = args.value("sessionId", "");
                if (!UmtMcp::PtraceSessionRegistry::Instance().Touch(sessionId))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound,
                        "无效或已超时的 sessionId: " + sessionId + "（需先 BEGIN_ATTACH_SESSION）");
                std::string addrStr = args.value("address", "");
                uintptr_t fn = 0;
                if (!UmtMcp::ParseAddress(addrStr, fn))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "address 格式无效: " + addrStr);
                std::vector<uintptr_t> callArgs;
                if (args.contains("args"))
                {
                    if (!args["args"].is_array())
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "args 须为数组");
                    for (const auto &a : args["args"])
                    {
                        uintptr_t v = 0;
                        const std::string s = a.is_string() ? a.get<std::string>() : "";
                        if (!ParseCallArg(s, v))
                            throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "参数格式无效: " + s);
                        callArgs.push_back(v);
                    }
                }
                if (callArgs.size() > 8)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "参数最多 8 个（aarch64 寄存器传参上限）");
                uintptr_t trap = 0;  // 返回陷阱：执行 0 地址必触发 SIGSEGV
                const std::string trapStr = args.value("trapAddress", "");
                if (!trapStr.empty() && !UmtMcp::ParseAddress(trapStr, trap))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "trapAddress 格式无效: " + trapStr);
                const std::string returnKind = args.value("returnKind", "ptr");

                uintptr_t raw = 0;
                try
                {
                    raw = InvokeRemoteFunction(trap, fn, callArgs);
                }
                catch (...)
                {
                    // 异常兜底：任何异常先 detach 再返回错误
                    UmtMcp::PtraceSessionRegistry::Instance().End(sessionId);
                    throw UmtMcp::HandlerError(UmtMcp::Err::kPtraceFailed, "远程调用异常，已强制结束会话");
                }
                return {{"ok", true},
                        {"value", FormatReturnValue(raw, returnKind)},
                        {"rawHex", UmtMcp::FormatAddress(raw)},
                        {"note", "返回陷阱靠 SIGSEGV/SIGILL 判定，无法区分陷阱触发与被调函数崩溃，请校验 value 合理性"}};
            }, true);

        // ── CALL_REMOTE_FUNCTION_BATCH（F 组主推：无状态，内部 attach→N次→detach，不可能泄漏会话）
        UmtMcp::CommandDispatcher::Register("CALL_REMOTE_FUNCTION_BATCH",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                if (args.value("confirmDangerous", false) != true)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs,
                        "远程调用会在目标进程内执行代码。请先用 SCAN_PATTERN / DISASSEMBLE 验证函数地址，"
                        "并显式传 confirmDangerous=true 确认");
                if (UmtMcp::PtraceSessionRegistry::Instance().IsActive())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs,
                        "已有活动会话 " + UmtMcp::PtraceSessionRegistry::Instance().ActiveId() +
                        "；批量调用是无状态的，请先 END_ATTACH_SESSION");
                std::string addrStr = args.value("address", "");
                uintptr_t fn = 0;
                if (!UmtMcp::ParseAddress(addrStr, fn))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "address 格式无效: " + addrStr);
                if (!args.contains("argSets") || !args["argSets"].is_array())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "argSets 须为数组（每组一次调用）");
                const std::string returnKind = args.value("returnKind", "ptr");
                int maxHoldMs = args.value("maxHoldMs", UmtMcp::kSessionDefaultHoldMs);
                if (maxHoldMs < 1 || maxHoldMs > UmtMcp::kSessionMaxHoldLimitMs)
                    maxHoldMs = UmtMcp::kSessionDefaultHoldMs;
                uintptr_t trap = 0;
                const std::string trapStr = args.value("trapAddress", "");
                if (!trapStr.empty() && !UmtMcp::ParseAddress(trapStr, trap))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "trapAddress 格式无效: " + trapStr);

                const auto t0 = std::chrono::steady_clock::now();
                if (!UEMemory::kMgr.trace.Attach())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kPtraceFailed,
                        "PTRACE_ATTACH 失败（ptrace 不可用 / 权限不足 / 目标进程已退出）");

                json results = json::array();
                int callCount = 0;
                try
                {
                    for (const auto &set : args["argSets"])
                    {
                        // 总时长预算：超过 maxHoldMs 立即停止（避免长时间冻结目标进程）
                        const auto heldMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                                                std::chrono::steady_clock::now() - t0).count();
                        if (heldMs > maxHoldMs) break;

                        if (!set.is_array())
                            throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "argSets 每一项须为参数数组");
                        std::vector<uintptr_t> callArgs;
                        for (const auto &a : set)
                        {
                            uintptr_t v = 0;
                            const std::string s = a.is_string() ? a.get<std::string>() : "";
                            if (!ParseCallArg(s, v))
                                throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "参数格式无效: " + s);
                            callArgs.push_back(v);
                        }
                        if (callArgs.size() > 8)
                            throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "单次调用参数最多 8 个");
                        const uintptr_t raw = InvokeRemoteFunction(trap, fn, callArgs);
                        ++callCount;
                        results.push_back({{"ok", true},
                                           {"value", FormatReturnValue(raw, returnKind)},
                                           {"rawHex", UmtMcp::FormatAddress(raw)}});
                    }
                }
                catch (...)
                {
                    UEMemory::kMgr.trace.Detach();  // 异常兜底：先 detach 再抛
                    throw;
                }
                UEMemory::kMgr.trace.Detach();

                const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                                         std::chrono::steady_clock::now() - t0).count();
                return {{"results", results}, {"elapsedMs", elapsed}, {"callCount", callCount},
                        {"note", "无状态：本次调用内部已完成 attach/detach。返回陷阱靠 SIGSEGV/SIGILL 判定，请校验 value"}};
            }, false);

        // ── ALLOC_SCRATCH（F 组：在目标进程内 mmap 可写缓冲区）
        // 用 KittyMemoryMgr::findRemoteOfSymbol(KT_LOCAL_SYMBOL(mmap)) 解析目标进程 mmap 地址后远程调用。
        UmtMcp::CommandDispatcher::Register("ALLOC_SCRATCH",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                const int size = args.value("size", 0);
                if (size < 1 || size > 65536)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "size 须在 [1, 65536]");
                if (UmtMcp::PtraceSessionRegistry::Instance().IsActive())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs,
                        "已有活动会话 " + UmtMcp::PtraceSessionRegistry::Instance().ActiveId() +
                        "；ALLOC_SCRATCH 是无状态的，请先 END_ATTACH_SESSION");

                const uintptr_t remoteMmap = UEMemory::kMgr.findRemoteOfSymbol(KT_LOCAL_SYMBOL(mmap));
                if (!remoteMmap)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound, "无法解析目标进程的 mmap 符号");

                if (!UEMemory::kMgr.trace.Attach())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kPtraceFailed, "PTRACE_ATTACH 失败");

                uintptr_t addr = 0;
                try
                {
                    // mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)
                    addr = UEMemory::kMgr.trace.callFunctionFrom(0, remoteMmap, 6,
                        static_cast<uintptr_t>(0), static_cast<uintptr_t>(size),
                        static_cast<uintptr_t>(PROT_READ | PROT_WRITE),
                        static_cast<uintptr_t>(MAP_PRIVATE | MAP_ANONYMOUS),
                        static_cast<uintptr_t>(-1), static_cast<uintptr_t>(0));
                }
                catch (...)
                {
                    UEMemory::kMgr.trace.Detach();  // 异常兜底
                    throw UmtMcp::HandlerError(UmtMcp::Err::kPtraceFailed, "目标进程 mmap 调用异常");
                }
                UEMemory::kMgr.trace.Detach();

                if (addr == static_cast<uintptr_t>(-1) || addr == 0)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kPtraceFailed,
                        "目标进程 mmap 返回失败 (MAP_FAILED)");

                UmtMcp::PtraceSessionRegistry::Instance().RecordAlloc(addr, static_cast<size_t>(size));
                return {{"address", UmtMcp::FormatAddress(addr)}, {"size", size}};
            }, false);

        // ── G 组 引擎语义（实时交互，直接复用 UEWrappers 运行时设施，不重写稳定逻辑）
        // 这些命令把原版 START_DUMP 落盘的 GNames.txt / Objects.txt / SDK 能力，做成可被 PC 侧
        // 实时查询的交互接口（对应 docs/api/12 外迁清单 G 组 与 mcp-protocol.md §6）。

        // ── SCAN_GNAMES（G 组：定位 GNames/FNamePool 候选并自校验）
        // 复用 START_PROBE 已解析出的 GNames 指针；扫描语义 = 报告权威候选 + 采样若干 name 自校验。
        // 若需"从零全网扫描"应改为 isFast=false 投 worker（见 MemoryHelpers.hpp TODO），当前探针已缓存权威指针。
        UmtMcp::CommandDispatcher::Register("SCAN_GNAMES",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                const std::string source = args.value("source", "AUTO");
                if (source == "CANDIDATE" || args.contains("sessionId") ||
                    !gProbeResult.valid || !gProbeResult.profile)
                {
                    if (args.value("async", false))
                    {
                        json workArgs = args;
                        workArgs.erase("async");
                        const std::string jobId = StartAnalysisJob("scan_gnames", [workArgs]()
                        {
                            return UmtMcp::Analysis::ScanGNamesCandidates(
                                workArgs, UEMemory::kMgr, &gCancelRequested);
                        });
                        if (jobId.empty()) throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "已有长任务运行中");
                        return {{"jobId", jobId}, {"status", "running"}, {"suggestedWaitMs", 2000}};
                    }
                    return UmtMcp::Analysis::ScanGNamesCandidates(args, UEMemory::kMgr, &gCancelRequested);
                }
                if (!gProbeResult.valid || !gProbeResult.profile)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "请先 START_PROBE 完成探针流程");
                const auto *vars = gProbeResult.profile->GetUEVars();
                if (!vars || !vars->GetNamesPtr())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "GNames 指针未解析（探针未完成）");
                uintptr_t namesPtr = vars->GetNamesPtr();
                bool useFNamePool = gProbeResult.profile->IsUsingFNamePool();
                bool casePreserving = gProbeResult.profile->isUsingCasePreservingName();
                json samples = json::array();
                bool anyValid = false;
                for (int32_t i = 0; i < 16; ++i)
                {
                    std::string nm = UEWrappers::GetNameByID(i);
                    bool valid = !nm.empty() && nm.find('\0') == std::string::npos;
                    if (valid) anyValid = true;
                    samples.push_back({{"index", i}, {"name", nm}, {"valid", valid}});
                }
                return {{"namesPtr", UmtMcp::FormatAddress(namesPtr)},
                        {"layout", useFNamePool ? "FNamePool" : "FNameEntryArray"},
                        {"casePreserving", casePreserving},
                        {"valid", anyValid},
                        {"sampleNames", samples}};
            }, true);

        // ── SAMPLE_GNAMES（G 组：采样任意 name 索引，确认某段确实是真实 GNames）
        UmtMcp::CommandDispatcher::Register("SAMPLE_GNAMES",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                const std::string source = args.value("source", args.contains("sessionId") ? "CANDIDATE" : "PROBE_RESULT");
                if (source == "CANDIDATE")
                    return UmtMcp::Analysis::SampleGNamesCandidate(args, UEMemory::kMgr);
                if (!gProbeResult.valid || !gProbeResult.profile)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "请先 START_PROBE 完成探针流程");
                int32_t startIndex = args.value("startIndex", 0);
                int32_t count = args.value("count", 32);
                if (startIndex < 0)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "startIndex 须 >= 0");
                if (count < 1 || count > 2000)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "count 须在 [1, 2000]");
                json samples = json::array();
                for (int32_t i = startIndex; i < startIndex + count; ++i)
                {
                    std::string nm = UEWrappers::GetNameByID(i);
                    bool valid = !nm.empty() && nm.find('\0') == std::string::npos;
                    samples.push_back({{"index", i}, {"name", nm}, {"valid", valid}});
                }
                return {{"startIndex", startIndex}, {"count", count}, {"samples", samples}};
            }, true);

        // ── SCAN_OBJECTS（G 组：定位 GUObjectArray 候选并自校验）
        UmtMcp::CommandDispatcher::Register("SCAN_OBJECTS",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                const std::string source = args.value("source", "AUTO");
                if (source == "CANDIDATE" || args.contains("sessionId") ||
                    !gProbeResult.valid || !gProbeResult.success || !UEWrappers::GetObjects())
                {
                    if (args.value("async", false))
                    {
                        json workArgs = args;
                        workArgs.erase("async");
                        const std::string jobId = StartAnalysisJob("scan_objects", [workArgs]()
                        {
                            return UmtMcp::Analysis::ScanObjectCandidates(
                                workArgs, UEMemory::kMgr, &gCancelRequested);
                        });
                        if (jobId.empty()) throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "已有长任务运行中");
                        return {{"jobId", jobId}, {"status", "running"}, {"suggestedWaitMs", 2000}};
                    }
                    return UmtMcp::Analysis::ScanObjectCandidates(args, UEMemory::kMgr, &gCancelRequested);
                }
                auto *objects = RequireObjects();
                uintptr_t arrPtr = 0;
                const auto *vars = UEWrappers::GetUEVars();
                if (vars) arrPtr = vars->GetGUObjectsArrayPtr();
                int32_t total = objects->GetNumElements();
                const int32_t kSample = 16;
                json samples = json::array();
                for (int32_t i = 0; i < kSample && i < total; ++i)
                {
                    UE_UObject obj = objects->GetObjectPtr(i);
                    if (!obj)
                    {
                        samples.push_back({{"index", i}, {"valid", false}});
                        continue;
                    }
                    samples.push_back({{"index", i},
                                      {"name", obj.GetName()},
                                      {"className", obj.GetClass().GetName()},
                                      {"valid", true}});
                }
                return {{"objectArrayPtr", UmtMcp::FormatAddress(arrPtr)},
                        {"totalObjects", total},
                        {"sampleObjects", samples}};
            }, true);

        // ── SAMPLE_OBJECTS（G 组：采样对象数组条目，读取其名/类名/Outer）
        UmtMcp::CommandDispatcher::Register("SAMPLE_OBJECTS",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                const std::string source = args.value("source", args.contains("sessionId") ? "CANDIDATE" : "PROBE_RESULT");
                if (source == "CANDIDATE")
                    return UmtMcp::Analysis::SampleObjectCandidate(args, UEMemory::kMgr);
                auto *objects = RequireObjects();
                int32_t startIndex = args.value("startIndex", 0);
                int32_t count = args.value("count", 32);
                int32_t total = objects->GetNumElements();
                if (startIndex < 0 || startIndex >= total)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "startIndex 越界");
                if (count < 1 || count > 2000)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "count 须在 [1, 2000]");
                json samples = json::array();
                for (int32_t i = startIndex; i < startIndex + count && i < total; ++i)
                {
                    UE_UObject obj = objects->GetObjectPtr(i);
                    if (!obj)
                    {
                        samples.push_back({{"index", i}, {"valid", false}});
                        continue;
                    }
                    UE_UObject outer = obj.GetOuter();
                    samples.push_back({{"index", i},
                                      {"name", obj.GetName()},
                                      {"className", obj.GetClass().GetName()},
                                      {"outerName", outer ? outer.GetName() : ""},
                                      {"valid", true}});
                }
                return {{"startIndex", startIndex}, {"count", (int32_t)samples.size()},
                        {"totalObjects", total}, {"samples", samples}};
            }, true);

        // ── SEARCH_CLASSES（G 组：按名搜类，服务端过滤；遍历量可能大，标 isFast=false）
        UmtMcp::CommandDispatcher::Register("SEARCH_CLASSES",
            [](const json &args) -> json
            {
                auto *objects = RequireObjects();
                std::string filter = args.value("nameFilter", "");
                if (filter.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "nameFilter 不能为空");
                int maxResults = args.value("maxResults", 100);
                if (maxResults < 1 || maxResults > 1000) maxResults = 100;
                bool caseSensitive = args.value("caseSensitive", false);
                std::string lf = filter;
                if (!caseSensitive) std::transform(lf.begin(), lf.end(), lf.begin(), ::tolower);
                json results = json::array();
                objects->ForEachObjectOfClass(UE_UClass::StaticClass(),
                    [&](UE_UObject obj) -> bool
                    {
                        if ((int)results.size() >= maxResults) return false;
                        std::string n = obj.GetName();
                        std::string cmp = n;
                        if (!caseSensitive) std::transform(cmp.begin(), cmp.end(), cmp.begin(), ::tolower);
                        if (cmp.find(lf) != std::string::npos)
                        {
                            UE_UClass cls = obj.Cast<UE_UClass>();
                            results.push_back({{"address", UmtMcp::FormatAddress(reinterpret_cast<uintptr_t>(obj.GetAddress()))},
                                               {"name", n},
                                               {"cppName", obj.GetCppName()},
                                               {"classSize", cls.GetSize()}});
                        }
                        return true;
                    });
                return {{"nameFilter", filter}, {"count", (int)results.size()}, {"results", results}};
            }, false);

        // ── DESCRIBE_CLASS（G 组：类结构描述，同时兼容 UE4 UProperty 与 UE5 FProperty 两条属性链）
        UmtMcp::CommandDispatcher::Register("DESCRIBE_CLASS",
            [](const json &args) -> json
            {
                // Address-based class analysis must work independently of the
                // global object iterator. Name lookup still requires it below.
                auto *objects = UEWrappers::GetObjects();
                UE_UClass cls;
                std::string addrStr = args.value("address", "");
                std::string name = args.value("name", "");
                if (!addrStr.empty())
                {
                    uintptr_t addr = 0;
                    if (!UmtMcp::ParseAddress(addrStr, addr))
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "address 格式无效: " + addrStr);
                    cls = UE_UClass(reinterpret_cast<uint8_t *>(addr));
                }
                else if (!name.empty())
                {
                    if (!objects)
                        throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "对象数组未初始化，无法按名称查类");
                    UE_UObject found = objects->FindObjectFast(name);
                    if (!found) throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound, "未找到类: " + name);
                    if (!found.IsA(UE_UClass::StaticClass()))
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, name + " 不是 UClass");
                    cls = found.Cast<UE_UClass>();
                }
                else
                {
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "需提供 address 或 name");
                }
                if (!cls) throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound, "无效的类地址");

                json props = json::array();
                int cap = 256;
                // UE5: FField / FProperty 链
                for (UE_FField f = cls.GetChildProperties(); f && cap > 0; f = f.GetNext(), --cap)
                {
                    UE_FProperty fp = f.Cast<UE_FProperty>();
                    auto t = fp.GetType();
                    props.push_back({{"name", f.GetName()},
                                     {"type", t.second},
                                     {"offset", fp.GetOffset()},
                                     {"size", fp.GetSize()},
                                     {"arrayDim", fp.GetArrayDim()},
                                     {"flags", (uint64_t)fp.GetPropertyFlags()}});
                }
                // UE4: UField / UProperty 链
                for (UE_UField f = cls.GetChildren(); f && cap > 0; f = f.GetNext(), --cap)
                {
                    UE_UProperty up = f.Cast<UE_UProperty>();
                    auto t = up.GetType();
                    props.push_back({{"name", f.GetName()},
                                     {"type", t.second},
                                     {"offset", up.GetOffset()},
                                     {"size", up.GetSize()},
                                     {"arrayDim", up.GetArrayDim()},
                                     {"flags", (uint64_t)up.GetPropertyFlags()}});
                }
                UE_UStruct super = cls.GetSuper();
                UE_UObject cdo = cls.GetClassDefaultObject();
                return {{"address", UmtMcp::FormatAddress(reinterpret_cast<uintptr_t>(cls.GetAddress()))},
                        {"name", cls.GetName()},
                        {"cppName", cls.GetCppName()},
                        {"fullName", cls.GetFullName()},
                        {"superName", super ? super.GetName() : ""},
                        {"classSize", cls.GetSize()},
                        {"classFlags", (uint32_t)cls.GetClassFlags()},
                        {"cdoAddress", cdo ? UmtMcp::FormatAddress(reinterpret_cast<uintptr_t>(cdo.GetAddress())) : ""},
                        {"propertyCount", (int)props.size()},
                        {"properties", props}};
            }, true);

        // ── INSPECT_OBJECT（G 组：对象实例检视，读取其标识/类/Outer/索引/标志）
        UmtMcp::CommandDispatcher::Register("INSPECT_OBJECT",
            [](const json &args) -> json
            {
                // Inspecting a caller-supplied address only needs an attached
                // memory context. Requiring a working GUObjectArray here made
                // direct runtime inspection impossible when object enumeration
                // was the only broken subsystem.
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                std::string addrStr = args.value("address", "");
                if (addrStr.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "address 不能为空");
                uintptr_t addr = 0;
                if (!UmtMcp::ParseAddress(addrStr, addr))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "address 格式无效: " + addrStr);
                UE_UObject obj(reinterpret_cast<uint8_t *>(addr));
                if (!obj) throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound, "无效对象地址: " + addrStr);
                UE_UClass cls = obj.GetClass();
                UE_UObject outer = obj.GetOuter();
                const bool canResolveCppName = UEWrappers::GetObjects() != nullptr;
                return {{"address", UmtMcp::FormatAddress(reinterpret_cast<uintptr_t>(obj.GetAddress()))},
                        {"name", obj.GetName()},
                        {"cppName", canResolveCppName ? obj.GetCppName() : ""},
                        {"fullName", obj.GetFullName()},
                        {"className", cls ? cls.GetName() : ""},
                        {"outerName", outer ? outer.GetName() : ""},
                        {"index", obj.GetIndex()},
                        {"flags", (uint32_t)obj.GetFlags()}};
            }, true);

        // ── RESOLVE_SYMBOL
        UmtMcp::CommandDispatcher::Register("RESOLVE_SYMBOL",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                std::string symbolName = args.value("symbol", "");
                if (symbolName.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "symbol 不能为空");
                const std::string module = args.value("module", "");
                const std::string matchMode = args.value("match", "EXACT_THEN_FUZZY");
                const bool includeDebug = args.value("includeDebug", true);
                const int maxResults = std::clamp(args.value("maxResults", 20), 1, 50);
                auto elf = UmtMcp::Analysis::FindUnrealElf(UEMemory::kMgr, module);
                if (!elf.isValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound,
                        "未找到有效 Unreal ELF: " + (module.empty() ? std::string("libUE4.so/libUnreal.so") : module));

                json matches = json::array();
                std::unordered_set<std::string> seen;
                auto append = [&](const std::string &name, uintptr_t address, const char *table)
                {
                    if (!address || matches.size() >= static_cast<size_t>(maxResults)) return;
                    const std::string key = name + ":" + UmtMcp::FormatAddress(address);
                    if (!seen.insert(key).second) return;
                    matches.push_back({{"name", name}, {"address", UmtMcp::FormatAddress(address)},
                                       {"moduleOffset", UmtMcp::FormatAddress(address - elf.base())},
                                       {"symbolType", "UNKNOWN"}, {"table", table}});
                };

                append(symbolName, elf.findSymbol(symbolName), "DYNSYM");
                if (includeDebug) append(symbolName, elf.findDebugSymbol(symbolName), "SYMTAB");
                if (matches.empty() && matchMode != "EXACT")
                {
                    for (const auto &[name, address] : elf.symbols())
                        if (name.find(symbolName) != std::string::npos) append(name, address, "DYNSYM");
                    if (includeDebug)
                        for (const auto &[name, address] : elf.dsymbols())
                            if (name.find(symbolName) != std::string::npos) append(name, address, "SYMTAB");
                }
                if (matches.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound,
                        "符号未找到: " + symbolName);
                const json &first = matches[0];
                return {{"symbol", symbolName}, {"address", first["address"]},
                        {"moduleOffset", first["moduleOffset"]}, {"module", elf.filePath()},
                        {"matches", matches}};
            }, true);

        // ── FOLLOW_POINTER_CHAIN
        UmtMcp::CommandDispatcher::Register("FOLLOW_POINTER_CHAIN",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                std::string baseStr = args.value("baseAddress", "");
                uintptr_t baseAddr = 0;
                if (!UmtMcp::ParseAddress(baseStr, baseAddr))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "baseAddress 格式无效: " + baseStr);
                auto offsets = args.value("offsets", json::array());
                if (offsets.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "offsets 不能为空");
                // 缓存 maps 用于模块归属查询
                auto maps = KittyMemoryEx::getAllMaps(UEMemory::kMgr.processID());
                // 第一步: baseAddress 是存储根指针的地址, 需先解引用
                uintptr_t current = 0;
                size_t ptrSize = sizeof(uintptr_t);
                size_t read = UEMemory::kMgr.readMem(baseAddr, &current, ptrSize);
                if (read != ptrSize)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kReadFailed,
                        "baseAddress 解引用失败: " + baseStr);
                json steps = json::array();
                steps.push_back({{"step", 0}, {"address", UmtMcp::FormatAddress(baseAddr)},
                    {"value", UmtMcp::FormatAddress(current)}, {"action", "deref_base"}});
                // 逐步跟踪偏移链
                for (size_t i = 0; i < offsets.size(); i++)
                {
                    std::string offStr = offsets[i].get<std::string>();
                    UmtMcp::ParsedOffset po;
                    if (!UmtMcp::ParseOffset(offStr, po))
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs,
                            "offset 格式无效: " + offStr);
                    // 加偏移
                    uintptr_t addr = current + static_cast<uintptr_t>(po.offset);
                    // 数组步进
                    if (po.arrayIndex >= 0)
                        addr += static_cast<uintptr_t>(po.arrayIndex) * ptrSize;
                    // 读取下一级指针
                    uintptr_t next = 0;
                    read = UEMemory::kMgr.readMem(addr, &next, ptrSize);
                    if (read != ptrSize)
                    {
                        // 链断裂
                        auto region = KittyMemoryEx::getAddressMap(maps, addr);
                        steps.push_back({{"step", i + 1}, {"address", UmtMcp::FormatAddress(addr)},
                            {"offset", offStr}, {"error", "读取失败"},
                            {"module", region.pathname}});
                        return {{"baseAddress", UmtMcp::FormatAddress(baseAddr)},
                            {"steps", steps}, {"brokenAtStep", i + 1},
                            {"finalAddress", nullptr}};
                    }
                    // 指针有效性检查
                    bool valid = (next != 0);
                    auto reg = KittyMemoryEx::getAddressMap(maps, next);
                    steps.push_back({{"step", i + 1}, {"address", UmtMcp::FormatAddress(addr)},
                        {"offset", offStr}, {"value", UmtMcp::FormatAddress(next)},
                        {"valid", valid}, {"module", reg.pathname}});
                    current = next;
                }
                auto finalRegion = KittyMemoryEx::getAddressMap(maps, current);
                return {{"baseAddress", UmtMcp::FormatAddress(baseAddr)},
                    {"steps", steps}, {"brokenAtStep", nullptr},
                    {"finalAddress", UmtMcp::FormatAddress(current)},
                    {"finalModule", finalRegion.pathname}};
            }, true);

        // ── SELECT_PROCESS（状态入口：选目标进程，为 ATTACH/PROBE 准备）
        UmtMcp::CommandDispatcher::Register("SELECT_PROCESS",
            [](const json &args) -> json
            {
                RequireIdleWorker();
                RefreshCandidates();
                const int targetPid = args.value("pid", 0);
                const std::string targetPkg = args.value("package", "");
                int found = -1;
                if (targetPid > 0)
                {
                    for (size_t i = 0; i < gCandidates.size(); ++i)
                        if (gCandidates[i].pid == targetPid) { found = static_cast<int>(i); break; }
                }
                else if (!targetPkg.empty())
                {
                    for (size_t i = 0; i < gCandidates.size(); ++i)
                        if (gCandidates[i].package == targetPkg) { found = static_cast<int>(i); break; }
                }
                else if (!gCandidates.empty())
                {
                    found = 0;
                }
                if (found < 0)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound,
                        "未找到匹配的目标进程，请先确认设备上有 UE 进程在运行");
                gSelectedIndex = found;
                const auto &c = gCandidates[found];
                InvalidateProbeReuse("已切换目标进程，清除旧探针结果");
                gProbeResult.pid = c.pid;
                gProbeResult.package = c.package;
                gProbeResult.profileName = c.profileName;
                gProbeResult.dedicated = c.dedicated;
                gProbeResult.valid = false;
                gProbeResult.success = false;
                UmtMcp::Analysis::InvalidateSessions();
                UmtMcp::Analysis::InvalidateCandidateSessions();
                {
                    std::lock_guard<std::mutex> lock(gOverrideMutex);
                    gProbeOverrides.clear();
                    gProbeOverrideLayouts.clear();
                    gProbeOverridePid = 0;
                    gProbeOverrideStartTime = 0;
                    gProbeOverrideRevision.clear();
                    gProbeOverrideExpiresAt = {};
                }
                return {{"selectedIndex", found}, {"pid", c.pid}, {"package", c.package},
                        {"profileName", c.profileName}, {"dedicated", c.dedicated}};
            }, false);

        // ── ATTACH（状态入口：将 KittyMemoryMgr 附着到选中进程；PROBE 也会重做，此处供分步校验）
        UmtMcp::CommandDispatcher::Register("ATTACH",
            [](const json &args) -> json
            {
                RequireIdleWorker();
                if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "请先 SELECT_PROCESS 选定目标进程");
                const auto &c = gCandidates[gSelectedIndex];
                const bool ok = UEMemory::kMgr.initialize(c.pid, EK_MEM_OP_SYSCALL, false) ||
                                UEMemory::kMgr.initialize(c.pid, EK_MEM_OP_IO, false);
                const auto snapshot = ok ? UmtMcp::Analysis::CaptureMaps(UEMemory::kMgr)
                                         : UmtMcp::Analysis::MapSnapshot{};
                return {{"attached", ok}, {"pid", c.pid},
                        {"isMemValid", UEMemory::kMgr.isMemValid()},
                        {"processStartTime", ok ? json(std::to_string(snapshot.processStartTime)) : json(nullptr)},
                        {"mapRevision", ok ? json(snapshot.revision) : json(nullptr)}};
            }, false);

        // ── START_PROBE（状态入口：触发完整探针，重活投 worker 线程，用 GET_PROBE_STATUS 轮询）
        //    命令名严格对齐 docs/mcp-protocol.md §6（startProbe → START_PROBE）
        UmtMcp::CommandDispatcher::Register("START_PROBE",
            [](const json &args) -> json
            {
                RequireIdleWorker();
                if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "请先 SELECT_PROCESS 选定目标进程");
                {
                    std::lock_guard<std::mutex> lock(gOverrideMutex);
                    if (gProbeOverridePid && gProbeOverridePid != gCandidates[gSelectedIndex].pid)
                        throw UmtMcp::HandlerError(UmtMcp::Err::kSessionStale, "override 绑定的 pid 与当前进程不一致");
                    if (gProbeOverridePid && std::chrono::steady_clock::now() > gProbeOverrideExpiresAt)
                        throw UmtMcp::HandlerError(UmtMcp::Err::kSessionStale, "override 已超过 TTL，请重新验证候选");
                    if (gProbeOverrideStartTime && UEMemory::kMgr.isMemValid())
                    {
                        const auto currentStart = UmtMcp::Analysis::CaptureMaps(UEMemory::kMgr).processStartTime;
                        if (currentStart != gProbeOverrideStartTime)
                            throw UmtMcp::HandlerError(UmtMcp::Err::kSessionStale, "override 绑定的进程 start time 已变化");
                    }
                    if (!gProbeOverrideRevision.empty() && UEMemory::kMgr.isMemValid() &&
                        gProbeOverrideRevision != UmtMcp::Analysis::CurrentMapRevision(UEMemory::kMgr))
                        throw UmtMcp::HandlerError(UmtMcp::Err::kMapStale, "override 绑定的 maps revision 已变化");
                }
                const std::string jobId = StartProbeSelected();
                return {{"started", true}, {"jobId", jobId}, {"probeId", jobId},
                        {"phase", "probing"}, {"suggestedWaitMs", args.value("waitMs", 5000)}};
            }, false);

        // ── DETECT_UE_VERSION（状态入口：读探针结果推断 UE 版本 / 基址 / profile）
        UmtMcp::CommandDispatcher::Register("DETECT_UE_VERSION",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                if (gProbeResult.valid)
                {
                    const std::string pn = gProbeResult.profileName;
                    std::string ue = "unknown";
                    if (pn.find("UE5") != std::string::npos) ue = "UE5";
                    else if (pn.find("UE4") != std::string::npos) ue = "UE4";
                    return {{"profileName", pn},
                            {"baseAddress", UmtMcp::FormatAddress(gProbeResult.baseAddress)},
                            {"ueVersion", ue}, {"source", "PROBE_RESULT"},
                            {"appName", gProbeResult.profile ? gProbeResult.profile->GetAppName() : std::string()}};
                }
                auto elf = UmtMcp::Analysis::FindUnrealElf(UEMemory::kMgr);
                if (!elf.isValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound, "未找到 libUE4.so/libUnreal.so");
                const bool ue5 = elf.filePath().find("libUnreal.so") != std::string::npos;
                return {{"profileName", nullptr}, {"baseAddress", UmtMcp::FormatAddress(elf.base())},
                        {"ueVersion", ue5 ? "UE5" : "UE4"}, {"source", "ELF_NAME"},
                        {"appName", ""}};
            }, true);

        // ── START_DUMP（业务工具：触发完整 dump，重活投 worker，用 GET_DUMP_STATUS 轮询）
        UmtMcp::CommandDispatcher::Register("START_DUMP",
            [](const json &args) -> json
            {
                RequireIdleWorker();
                if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "请先 SELECT_PROCESS 选定目标进程");
                if (!gProbeResult.valid || !gProbeResult.success)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "请先 START_PROBE 完成探针流程");
                const std::string probeId = args.value("probeId", "");
                if (!probeId.empty()) {
                    std::lock_guard<std::mutex> lock(gJobMutex);
                    bool matched = false;
                    for (const auto &job : gJobs)
                        if (job.jobId == probeId && job.type == "probe" && !job.running && job.lastError.empty()) matched = true;
                    if (!matched) throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "probeId 不匹配或探针尚未成功");
                }
                const std::string jobId = StartDumpAfterProbe();
                return {{"started", true}, {"jobId", jobId}, {"suggestedWaitMs", args.value("waitMs", 5000)}};
            }, false);

        // ── DUMP_UNREAL_LIBRARY（业务工具：转储 libUE4.so / libUnreal.so）
        UmtMcp::CommandDispatcher::Register("DUMP_UNREAL_LIBRARY",
            [](const json &args) -> json
            {
                if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "请先 SELECT_PROCESS 选定目标进程");
                if (!UEMemory::kMgr.isMemValid() || UEMemory::kMgr.processID() != gCandidates[gSelectedIndex].pid)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "请先 ATTACH 当前目标进程");
                const std::string source = args.value("source", "AUTO");
                if (source != "AUTO" && source != "FILE" && source != "MEMORY")
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "source 必须是 AUTO/FILE/MEMORY");
                const std::string jobId = StartDumpUnrealLib(source);
                if (jobId.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "已有长任务运行中");
                return {{"started", true}, {"jobId", jobId}, {"source", source}};
            }, false);

        // ── GET_PROBE_RESULTS（业务工具：返回探针识别的核心偏移与结构体布局）
        UmtMcp::CommandDispatcher::Register("GET_PROBE_RESULTS",
            [](const json &) -> json
            {
                if (!gProbeResult.valid)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "请先 START_PROBE 完成探针流程");
                json offsets = json::array();
                for (const auto &e : gProbeResult.offsets)
                    offsets.push_back({{"name", e.name},
                                       {"value", UmtMcp::FormatAddress(e.value)},
                                       {"relative", UmtMcp::FormatAddress(e.relative)},
                                       {"found", e.found}});
                json groups = json::array();
                for (const auto &g : gProbeResult.structGroups)
                {
                    json fields = json::array();
                    for (const auto &f : g.fields)
                        fields.push_back({{"name", f.name}, {"type", f.type},
                                          {"offset", UmtMcp::FormatAddress(f.offset)},
                                          {"found", f.found}, {"description", f.description}});
                    groups.push_back({{"name", g.name}, {"fields", fields}});
                }
                return {{"pid", gProbeResult.pid},
                        {"processStartTime", std::to_string(gProbeResult.processStartTime)},
                        {"mapRevision", gProbeResult.mapRevision},
                        {"profileName", gProbeResult.profileName},
                        {"fieldSources", gProbeResult.fieldSources},
                        {"overrideEvidence", gProbeResult.overrideEvidence},
                        {"offsets", offsets}, {"structGroups", groups}};
            }, true);

        // ── GET_PROBE_STATUS（业务工具：探针进度/状态，持 mutex 读 DumpUiState）
        UmtMcp::CommandDispatcher::Register("GET_PROBE_STATUS",
            [](const json &args) -> json
            {
                std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
                const auto jobs = SnapshotJobs();
                const std::string requested = args.value("jobId", "");
                return {{"jobId", requested}, {"phase", gDumpUiState.phase},
                        {"objectsPercent", gDumpUiState.objectsPercent},
                        {"running", gDumpUiState.probeRunning},
                        {"finished", gDumpUiState.probeFinished},
                        {"success", gDumpUiState.probeSuccess}, {"jobs", jobs}};
            }, true);

        // ── GET_DUMP_STATUS（业务工具：dump / soDump 进度与产物路径，持 mutex 读 DumpUiState）
        UmtMcp::CommandDispatcher::Register("GET_DUMP_STATUS",
            [](const json &) -> json
            {
                std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
                return {{"phase", gDumpUiState.phase},
                        {"dumpPercent", gDumpUiState.dumpPercent},
                        {"running", gDumpUiState.dumpRunning},
                        {"finished", gDumpUiState.dumpFinished},
                        {"success", gDumpUiState.dumpSuccess},
                        {"resultPath", gDumpUiState.resultPath},
                        {"soDumpRunning", gDumpUiState.soDumpRunning},
                        {"soDumpFinished", gDumpUiState.soDumpFinished},
                        {"soDumpSuccess", gDumpUiState.soDumpSuccess},
                        {"soDumpPath", gDumpUiState.soDumpPath},
                        {"soDumpArtifact", gDumpUiState.soDumpArtifact},
                        {"jobs", SnapshotJobs()}};
            }, true);

        // ── H 组:LOCATE_ENGINE_GLOBALS(一键定位 GNames/GUObjectArray/GWorld 等引擎全局指针)
        // 优先从 gProbeResult.profile->GetUEVars() 读已解析的权威指针,
        // 对未解析的补用 SCAN_PATTERN 扫描,返回所有结果供 AI 复核。
        const auto locateEngineGlobalsImpl =
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                const int waitMs = args.value("waitMs", 30000);
                if (waitMs < 1 || waitMs > 60000)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "waitMs 须在 [1, 60000] 内");

                const auto snapshot = UmtMcp::Analysis::CaptureMaps(UEMemory::kMgr);
                auto elf = UmtMcp::Analysis::FindUnrealElf(UEMemory::kMgr);
                if (!elf.isValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound, "未找到有效 Unreal ELF");

                auto globalEntry = [&](uintptr_t slot, uintptr_t value, int indirection,
                                       const std::string &source, int score) -> json
                {
                    return {{"slotAddress", UmtMcp::FormatAddress(slot)},
                            {"valueAddress", UmtMcp::FormatAddress(value)},
                            {"moduleOffset", slot >= elf.base()
                                ? json(UmtMcp::FormatAddress(slot - elf.base())) : json(nullptr)},
                            {"indirection", indirection}, {"source", source},
                            {"score", score}, {"confidence", score >= 70 ? "HIGH" : score >= 40 ? "MEDIUM" : "LOW"}};
                };
                auto findSymbolAny = [&](std::initializer_list<const char *> names) -> uintptr_t
                {
                    for (const char *name : names)
                    {
                        uintptr_t address = elf.findSymbol(name);
                        if (!address) address = elf.findDebugSymbol(name);
                        if (address) return address;
                    }
                    return 0;
                };

                auto *vars = UEWrappers::GetUEVars();
                json globals = json::object();
                json evidence = json::array();
                json failedSteps = json::array();
                json suggested = json::object();

                if (vars && gProbeResult.valid)
                {
                    if (vars->GetNamesPtr() != 0)
                    {
                        globals["FNamePool"] = globalEntry(vars->GetNamesPtr(), vars->GetNamesPtr(), 0,
                                                           "PROBE_RESULT", 100);
                        globals["gNamesPtr"] = UmtMcp::FormatAddress(vars->GetNamesPtr());
                        globals["gNamesPtrAddr"] = UmtMcp::FormatAddress(vars->GetNamesPtr());
                        evidence.push_back("FNamePool recovered from ProbeResult");
                    }
                    if (vars->GetGUObjectsArrayPtr() != 0)
                    {
                        globals["GUObjectArray"] = globalEntry(vars->GetGUObjectsArrayPtr(),
                            vars->GetGUObjectsArrayPtr(), 0, "PROBE_RESULT", 100);
                        globals["guObjectArrayPtr"] = UmtMcp::FormatAddress(vars->GetGUObjectsArrayPtr());
                        globals["guObjectArrayPtrAddr"] = UmtMcp::FormatAddress(vars->GetGUObjectsArrayPtr());
                        evidence.push_back("GUObjectArray recovered from ProbeResult");
                    }
                    if (vars->GetObjObjects_Objects() != 0)
                        globals["objObjectsPtr"] = UmtMcp::FormatAddress(vars->GetObjObjects_Objects());
                }

                if (!globals.contains("FNamePool"))
                {
                    const uintptr_t symbol = findSymbolAny({"NamePoolData", "GNames", "GName"});
                    if (symbol)
                    {
                        globals["FNamePool"] = globalEntry(symbol, symbol, 0, "ELF_SYMBOL", 90);
                        globals["gNamesPtr"] = UmtMcp::FormatAddress(symbol);
                        globals["gNamesPtrAddr"] = UmtMcp::FormatAddress(symbol);
                        evidence.push_back("FNamePool resolved from ELF symbol");
                    }
                    else failedSteps.push_back({{"step", "resolveSymbol(FNamePool)"}, {"code", "E_NOT_FOUND"}});
                }

                if (!globals.contains("GUObjectArray"))
                {
                    const uintptr_t symbol = findSymbolAny({"GUObjectArray", "GObjectArray"});
                    if (symbol)
                    {
                        globals["GUObjectArray"] = globalEntry(symbol, symbol, 0, "ELF_SYMBOL", 90);
                        globals["guObjectArrayPtr"] = UmtMcp::FormatAddress(symbol);
                        globals["guObjectArrayPtrAddr"] = UmtMcp::FormatAddress(symbol);
                        evidence.push_back("GUObjectArray resolved from ELF symbol");
                    }
                    else failedSteps.push_back({{"step", "resolveSymbol(GUObjectArray)"}, {"code", "E_NOT_FOUND"}});
                }

                uintptr_t gWorldSlot = 0;
                uintptr_t worldObjectHint = 0;
                {
                    std::lock_guard<std::mutex> lock(gOverrideMutex);
                    auto world = gProbeOverrides.find("world");
                    if (world != gProbeOverrides.end()) gWorldSlot = world->second;
                }
                std::string worldSource = "PROBE_OVERRIDE";
                if (!gWorldSlot)
                {
                    // Verified Shikigami UE4.27 build hint; validate slot/object below.
                    const bool shikigami = gSelectedIndex >= 0 &&
                        gSelectedIndex < static_cast<int>(gCandidates.size()) &&
                        gCandidates[gSelectedIndex].package == "com.huitgames.shikigami.summons";
                    if (shikigami && elf.isValid())
                    {
                        constexpr uintptr_t kShikigamiGWorldOffset = 0xBCA0910;
                        const uintptr_t hintedSlot = elf.base() + kShikigamiGWorldOffset;
                        if (UmtMcp::Analysis::IsReadableAddress(snapshot, hintedSlot, sizeof(uintptr_t)))
                        {
                            gWorldSlot = hintedSlot;
                            worldSource = "SHIKIGAMI_UE427_HINT";
                            evidence.push_back("GWorld slot from verified Shikigami UE4.27 relative offset");
                        }
                    }
                }
                if (!gWorldSlot)
                {
                    gWorldSlot = findSymbolAny({"GWorld"});
                    worldSource = "ELF_SYMBOL";
                }
                if (!gWorldSlot && gProbeResult.success && UEWrappers::GetObjects())
                {
                    auto *objects = UEWrappers::GetObjects();
                    UE_UClass worldClass = objects->FindObject("Class Engine.World").Cast<UE_UClass>();
                    if (worldClass)
                    {
                        const auto segments = elf.segments();
                        objects->ForEachObject([&](UE_UObject object)
                        {
                            if (!object || object.HasFlags(EObjectFlags::ClassDefaultObject) ||
                                !object.IsA(worldClass)) return false;
                            for (auto it = segments.rbegin(); it != segments.rend(); ++it)
                            {
                                if (!it->is_rw) continue;
                                const uintptr_t slot = UEMemory::FindAlignedPointerRefrence(
                                    it->startAddress, it->length, object.GetAddress());
                                if (!slot) continue;
                                gWorldSlot = slot;
                                worldObjectHint = reinterpret_cast<uintptr_t>(object.GetAddress());
                                worldSource = "OBJECT_REFERENCE";
                                return true;
                            }
                            return false;
                        });
                        if (gWorldSlot)
                            evidence.push_back("GWorld slot recovered from a validated World object reference");
                        else
                            failedSteps.push_back({{"step", "enumerateWorldObjects"},
                                                   {"reason", "World instance or rw reference not found"}});
                    }
                    else failedSteps.push_back({{"step", "findObject(Class Engine.World)"},
                                                {"code", "E_NOT_FOUND"}});
                }
                if (gWorldSlot)
                {
                    uintptr_t worldObject = 0, secondRead = 0;
                    const bool firstReadable = UEMemory::kMgr.readMem(gWorldSlot, &worldObject, sizeof(worldObject)) == sizeof(worldObject);
                    const bool stable = firstReadable &&
                        UEMemory::kMgr.readMem(gWorldSlot, &secondRead, sizeof(secondRead)) == sizeof(secondRead) &&
                        secondRead == worldObject;
                    const bool readable = stable &&
                        UmtMcp::Analysis::IsReadableAddress(snapshot, worldObject, sizeof(uintptr_t));
                    const bool expectedObject = !worldObjectHint || worldObjectHint == worldObject;
                    bool classChecked = false;
                    bool classMatches = true;
                    if (readable && gProbeResult.success && UEWrappers::GetObjects())
                    {
                        UE_UClass worldClass = UEWrappers::GetObjects()->FindObject(
                            "Class Engine.World").Cast<UE_UClass>();
                        if (worldClass)
                        {
                            classChecked = true;
                            classMatches = UE_UObject(reinterpret_cast<uint8_t *>(worldObject)).IsA(worldClass);
                        }
                    }
                    globals["GWorld"] = globalEntry(gWorldSlot, readable ? worldObject : 0, 1, worldSource,
                        readable && expectedObject && classMatches ? 90 : readable ? 50 : 25);
                    globals["GWorld"]["validation"] = {
                        {"slotReadable", firstReadable}, {"stableAcrossReads", stable},
                        {"objectReadable", readable}, {"matchesWorldCandidate", expectedObject},
                        {"classChecked", classChecked}, {"classMatchesWorld", classMatches}};
                    globals["gWorldPtrAddr"] = UmtMcp::FormatAddress(gWorldSlot);
                    globals["worldObject"] = readable ? json(UmtMcp::FormatAddress(worldObject)) : json(nullptr);
                    if (!readable || !expectedObject || !classMatches)
                        failedSteps.push_back({{"step", "validateGWorld"},
                            {"reason", !stable ? "slot value unstable/unreadable" :
                                       !readable ? "world object unreadable" :
                                       !expectedObject ? "slot does not match World candidate" :
                                                         "object class is not Engine.World"}});
                }
                else failedSteps.push_back({{"step", "resolveSymbol(GWorld)"}, {"code", "E_NOT_FOUND"}});

                if (!globals.contains("FNamePool"))
                {
                    try
                    {
                        json scanArgs = {{"region", "ELF_SEGMENTS"}, {"maxCandidates", 5},
                                         {"maxScanBytes", 16 * 1024 * 1024}};
                        json result = UmtMcp::Analysis::ScanGNamesCandidates(scanArgs, UEMemory::kMgr, &gCancelRequested);
                        if (!result["candidates"].empty())
                        {
                            const json best = result["candidates"][0];
                            globals["FNamePool"] = best;
                            globals["gNamesPtr"] = best["poolAddress"];
                            globals["gNamesPtrAddr"] = best["slotAddress"];
                            suggested["namesPtr"] = {{"address", best["poolAddress"]},
                                {"semantics", "POOL_BASE"}, {"layout", best["layout"]},
                                {"sessionId", result["sessionId"]}, {"candidateId", best["candidateId"]}};
                            evidence.push_back("FNamePool candidate produced by structural scan");
                        }
                        else failedSteps.push_back({{"step", "scanGNames"}, {"reason", "no scored candidate"}});
                    }
                    catch (const std::exception &error)
                    {
                        failedSteps.push_back({{"step", "scanGNames"}, {"reason", error.what()}});
                    }
                }

                if (!globals.contains("GUObjectArray"))
                {
                    try
                    {
                        json scanArgs = {{"region", "MODULE_RW"}, {"maxCandidates", 5},
                                         {"maxDistanceBytes", 16 * 1024 * 1024}};
                        if (suggested.contains("namesPtr"))
                        {
                            scanArgs["namesSessionId"] = suggested["namesPtr"]["sessionId"];
                            scanArgs["namesCandidateId"] = suggested["namesPtr"]["candidateId"];
                        }
                        json result = UmtMcp::Analysis::ScanObjectCandidates(scanArgs, UEMemory::kMgr, &gCancelRequested);
                        if (!result["candidates"].empty())
                        {
                            const json best = result["candidates"][0];
                            globals["GUObjectArray"] = best;
                            globals["guObjectArrayPtr"] = best["arrayAddress"];
                            globals["guObjectArrayPtrAddr"] = best["slotAddress"];
                            suggested["guObjectArrayPtr"] = {{"address", best["arrayAddress"]},
                                {"semantics", "FUOBJECTARRAY_BASE"}, {"layout", best["layout"]},
                                {"sessionId", result["sessionId"]}, {"candidateId", best["candidateId"]}};
                            evidence.push_back("GUObjectArray candidate produced by structural scan");
                        }
                        else failedSteps.push_back({{"step", "scanObjects"}, {"reason", "no scored candidate"}});
                    }
                    catch (const std::exception &error)
                    {
                        failedSteps.push_back({{"step", "scanObjects"}, {"reason", error.what()}});
                    }
                }

                const bool namesFound = globals.contains("FNamePool");
                const bool objectsFound = globals.contains("GUObjectArray");
                const bool worldFound = globals.contains("GWorld") && !globals["GWorld"]["valueAddress"].is_null() &&
                                        globals["GWorld"]["valueAddress"] != "0x0" &&
                                        globals["GWorld"]["validation"]["matchesWorldCandidate"].get<bool>() &&
                                        globals["GWorld"]["validation"]["classMatchesWorld"].get<bool>();
                return {{"success", namesFound && objectsFound},
                        {"partial", !(namesFound && objectsFound && worldFound)},
                        {"mapRevision", snapshot.revision},
                        {"globals", globals},
                        {"method", gProbeResult.success ? "PROBE_RESULT" : "ATTACH_ONLY_FALLBACK"},
                        {"evidence", evidence}, {"failedSteps", failedSteps},
                        {"suggestedOverrides", suggested},
                        {"nextAction", namesFound && objectsFound ? "APPLY_PROBE_OVERRIDES_THEN_START_PROBE" : "REVIEW_CANDIDATES"}};
            };
        UmtMcp::CommandDispatcher::Register("LOCATE_ENGINE_GLOBALS",
            [locateEngineGlobalsImpl](const json &args) -> json
            {
                if (args.value("async", false))
                {
                    json workArgs = args;
                    workArgs.erase("async");
                    const std::string jobId = StartAnalysisJob("locate_engine_globals",
                        [locateEngineGlobalsImpl, workArgs]()
                        {
                            return locateEngineGlobalsImpl(workArgs);
                        });
                    if (jobId.empty()) throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "已有长任务运行中");
                    return {{"jobId", jobId}, {"status", "running"}, {"suggestedWaitMs", 5000}};
                }
                return locateEngineGlobalsImpl(args);
            }, false);

        // ── H 组:DUMP_SDK(一键完成探测→转储→产出 SDK 全流程,走 JobRegistry)
        UmtMcp::CommandDispatcher::Register("DUMP_SDK",
            [](const json &args) -> json
            {
                RequireIdleWorker();
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "请先 SELECT_PROCESS");
                const int waitMs = args.value("waitMs", 10000);
                if (waitMs < 1 || waitMs > 60000)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "waitMs 须在 [1, 60000] 内");

                std::string jobId = StartProbeAndDump();
                if (jobId.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotReady, "已有长任务运行中");
                return {{"jobId", jobId}, {"status", "started"},
                        {"suggestedWaitMs", waitMs},
                        {"note", "已启动 Probe -> Dump;轮询 GET_DUMP_STATUS.jobs 中对应 jobId"}};
            }, false);

        // ── H 组:ANALYZE_CLASS(类分析,输出字段解读+可信度分级)
        UmtMcp::CommandDispatcher::Register("ANALYZE_CLASS",
            [](const json &args) -> json
            {
                RequireObjects();
                std::string className = args.value("className", "");
                if (className.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "className 不能为空");
                bool includeRuntimeSample = args.value("includeRuntimeSample", false);

                // 找类(复用 DESCRIBE_CLASS 逻辑)
                auto *objects = UEWrappers::GetObjects();
                UE_UClass cls;
                if (className.substr(0, 2) == "0x")
                {
                    // 地址定位
                    uintptr_t addr = 0;
                    if (!UmtMcp::ParseAddress(className, addr))
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "className 格式无效: " + className);
                    UE_UObject uo(reinterpret_cast<void *>(addr));
                    if (!objects->IsObject(uo))
                        throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound, "地址不在对象数组内: " + className);
                    cls = uo.Cast<UE_UClass>();
                }
                else
                {
                    UE_UObject found = objects->FindObjectFast(className);
                    if (!found) throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound, "未找到类: " + className);
                    cls = found.Cast<UE_UClass>();
                }
                if (!cls) throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound, "无效的类地址: " + className);

                json props = json::array();
                json fieldKinds = json::array();
                json conclusions = json::array();
                int cap = 256;
                // UE5: FField / FProperty 链
                for (UE_FField f = cls.GetChildProperties(); f && cap > 0; f = f.GetNext(), --cap)
                {
                    UE_FProperty fp = f.Cast<UE_FProperty>();
                    auto t = fp.GetType();
                    const std::string typeName = t.second;
                    uintptr_t off = fp.GetOffset();
                    uint32_t size = fp.GetSize();
                    json p = {{"name", f.GetName()}, {"type", typeName},
                              {"offset", UmtMcp::FormatAddress(off)},
                              {"size", size}, {"arrayDim", fp.GetArrayDim()},
                              {"flags", (uint64_t)fp.GetPropertyFlags()}};
                    // 判断属性类型→分类
                    std::string kind;
                    if (typeName.find("StructProperty") != std::string::npos) kind = "inline_struct";
                    else if (typeName.find("ObjectProperty") != std::string::npos || typeName.find("WeakPtrProperty") != std::string::npos) kind = "pointer";
                    else if (typeName.find("BoolProperty") != std::string::npos) kind = "value";
                    else if (typeName.find("IntProperty") != std::string::npos) kind = "value";
                    else if (typeName.find("FloatProperty") != std::string::npos) kind = "value";
                    else kind = "other";
                    fieldKinds.push_back({{"name", f.GetName()}, {"kind", kind}, {"type", typeName}});
                    // A 级结论:Dump 可直接证明的类型
                    if (kind == "inline_struct")
                        conclusions.push_back({{"claim", f.GetName() + " 是内嵌结构体(offset=" +
                                               std::to_string(off) + ",size=" + std::to_string(size) + ")"},
                                              {"grade", "A"},
                                              {"evidence", "UE_FProperty::GetType() 返回 StructProperty"}});
                    else if (kind == "pointer")
                        conclusions.push_back({{"claim", f.GetName() + " 是指针(offset=" +
                                               std::to_string(off) + ")"},
                                              {"grade", "A"},
                                              {"evidence", "UE_FProperty::GetType() 返回 ObjectProperty/Base"}});
                    props.push_back(p);
                }
                // UE4: UField / UProperty 链
                cap = 256;
                for (UE_UField f = cls.GetChildren(); f && cap > 0; f = f.GetNext(), --cap)
                {
                    UE_UProperty up = f.Cast<UE_UProperty>();
                    auto t = up.GetType();
                    const std::string typeName = t.second;
                    uintptr_t off = up.GetOffset();
                    uint32_t size = up.GetSize();
                    json p = {{"name", f.GetName()}, {"type", typeName},
                              {"offset", UmtMcp::FormatAddress(off)},
                              {"size", size}, {"arrayDim", up.GetArrayDim()},
                              {"flags", (uint64_t)up.GetPropertyFlags()}};
                    std::string kind;
                    if (typeName.find("StructProperty") != std::string::npos) kind = "inline_struct";
                    else if (typeName.find("ObjectProperty") != std::string::npos || typeName.find("WeakPtrProperty") != std::string::npos) kind = "pointer";
                    else if (typeName.find("BoolProperty") != std::string::npos) kind = "value";
                    else if (typeName.find("IntProperty") != std::string::npos) kind = "value";
                    else if (typeName.find("FloatProperty") != std::string::npos) kind = "value";
                    else kind = "other";
                    fieldKinds.push_back({{"name", f.GetName()}, {"kind", kind}, {"type", typeName}});
                    props.push_back(p);
                }

                // 运行时采样(可选):读该类的第一个实例,验证属性值
                if (includeRuntimeSample)
                {
                    UE_UObject sample = objects->FindObjectFast(className);
                    if (sample && sample.IsA(cls))
                    {
                        conclusions.push_back({{"claim", "类 " + cls.GetFullName() + " 存在运行时实例"},
                                              {"grade", "B"},
                                              {"evidence", UmtMcp::FormatAddress(reinterpret_cast<uintptr_t>(sample.GetAddress()))}});
                    }
                    else
                    {
                        conclusions.push_back({{"claim", "未找到类 " + className + " 的运行时实例(可能未加载)"},
                                              {"grade", "C"},
                                              {"evidence", "FindObjectFast 返回空"}});
                    }
                }

                // 总结信息
                UE_UStruct super = cls.GetSuper();
                json classInfo = {{"address", UmtMcp::FormatAddress(reinterpret_cast<uintptr_t>(cls.GetAddress()))},
                                  {"name", cls.GetName()}, {"cppName", cls.GetCppName()},
                                  {"fullName", cls.GetFullName()},
                                  {"superName", super ? super.GetName() : ""},
                                  {"classSize", cls.GetSize()},
                                  {"propertyCount", (int)props.size()}};

                return {{"classInfo", classInfo},
                        {"fieldKinds", fieldKinds},
                        {"properties", props},
                        {"conclusions", conclusions},
                        {"note", "A级=Dump可直接证明;B级=需多文件交叉证明;C级=必须运行时实测"}};
            }, true);

        // ── I 组:SCAN_CANDIDATES(批量内存扫描+KittyPtrValidator落点校验,返回sessionId缓存)
        // 按 region 枚举内存页,对每个候选点用 KittyPtrValidator 校验可读性,返回分页 sessionId
        UmtMcp::CommandDispatcher::Register("SCAN_CANDIDATES",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");

                std::string regionFilter = args.value("region", "");  // 可选,如 "libUE4.so"
                int alignment = args.value("alignment", 8);
                if (alignment <= 0 || alignment > 256) alignment = 8;
                int maxCandidates = args.value("maxCandidates", 200);
                if (maxCandidates < 1 || maxCandidates > 2000) maxCandidates = 200;

                auto maps = KittyMemoryEx::getAllMaps(UEMemory::kMgr.processID());
                std::vector<KittyMemoryEx::ProcMap> validRegions;
                for (const auto &m : maps)
                {
                    if (!regionFilter.empty() && m.pathname.find(regionFilter) == std::string::npos)
                        continue;
                    if (!(m.protection & PROT_READ)) continue;
                    validRegions.push_back(m);
                }
                if (validRegions.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kReadFailed, "无可读的内存区域(请先 ATTACH)");

                // 批量读:每次读 4KB,步长 alignment
                std::vector<uintptr_t> candidates;
                size_t scannedBytes = 0;
                for (const auto &reg : validRegions)
                {
                    uintptr_t start = static_cast<uintptr_t>(reg.startAddress);
                    uintptr_t end = static_cast<uintptr_t>(reg.endAddress);
                    if (start % alignment != 0) start = ((start + alignment - 1) / alignment) * alignment;
                    for (uintptr_t addr = start; addr + sizeof(uintptr_t) <= end && (int)candidates.size() < maxCandidates; addr += alignment)
                    {
                        uintptr_t val = 0;
                        if (UEMemory::kMgr.readMem(addr, &val, sizeof(val)) == sizeof(val))
                        {
                            // 用 KittyPtrValidator 验证候选值是否可读
                            if (val != 0 && kPtrValidator.isPtrInAddressSpace(val))
                            {
                                candidates.push_back(val);
                                scannedBytes += alignment;
                            }
                        }
                    }
                }

                // 生成 sessionId(10 分钟过期)
                std::string sessionId = "scan_" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count());
                // TODO:将 candidates 存入 SessionRegistry(当前简单返回首屏,后续可扩展为分页)
                json resultCandidates = json::array();
                int preview = std::min((int)candidates.size(), 50);
                for (int i = 0; i < preview; ++i)
                    resultCandidates.push_back(UmtMcp::FormatAddress(candidates[i]));

                return {{"sessionId", sessionId},
                        {"scannedBytes", (int)scannedBytes},
                        {"candidateCount", (int)candidates.size()},
                        {"preview", resultCandidates},
                        {"totalCandidates", (int)candidates.size()},
                        {"note", "完整候选集暂只返回前 50 条;建议结合 DESCIBLE_CLASS + FOLLOW_POINTER_CHAIN 进一步分析"}};
            }, false);

        // ── C 组:LIST_OUTPUT_FILES(列出 /sdcard/UnrealMemoryTools/<package>/ 产物目录)
        UmtMcp::CommandDispatcher::Register("LIST_OUTPUT_FILES",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                std::string package = args.value("package", "");
                if (package.empty())
                {
                    if (gSelectedIndex >= 0 && gSelectedIndex < static_cast<int>(gCandidates.size()))
                        package = gCandidates[gSelectedIndex].package;
                    else
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "package 不能为空,或未 SELECT_PROCESS");
                }
                std::string dumpDir = std::string(kOutputDirectory) + "/" + package;
                auto files = ListDirectory(dumpDir);
                json result = json::array();
                for (const auto &f : files)
                    result.push_back({{"name", f["name"].get<std::string>()},
                                      {"type", f["type"].get<std::string>()}});
                return {{"package", package}, {"path", dumpDir}, {"files", result}};
            }, true);

        // ── C 组:READ_OUTPUT_FILE(读取 dump 产物中小文件,回传 base64 编码内容)
        // 大文件(>1MB)只返回元信息,提示 PC 侧 adb pull
        UmtMcp::CommandDispatcher::Register("READ_OUTPUT_FILE",
            [](const json &args) -> json
            {
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                std::string filename = args.value("filename", "");
                if (filename.empty())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "filename 不能为空");
                std::string package = args.value("package", "");
                if (package.empty())
                {
                    if (gSelectedIndex >= 0 && gSelectedIndex < static_cast<int>(gCandidates.size()))
                        package = gCandidates[gSelectedIndex].package;
                    else
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "package 不能为空");
                }
                std::string filePath = std::string(kOutputDirectory) + "/" + package + "/" + filename;

                // 读文件大小
                struct stat st {};
                if (stat(filePath.c_str(), &st) != 0)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotFound, "文件不存在: " + filePath);
                const size_t fileSize = static_cast<size_t>(st.st_size);
                const size_t kLargeFileThreshold = 1 * 1024 * 1024; // 1MB
                const size_t offset = args.value("offset", static_cast<size_t>(0));
                const size_t limit = std::min(args.value("limit", static_cast<size_t>(64 * 1024)), static_cast<size_t>(256 * 1024));

                if (fileSize > kLargeFileThreshold)
                    return {{"filename", filename}, {"path", filePath},
                            {"sizeBytes", (int)fileSize},
                            {"content", nullptr}, {"truncated", false},
                            {"note", "文件超过 1MB,请使用 adb pull 拉取"},
                            {"adbPullCmd", "adb pull \"" + filePath + "\" ./outputs/" + package + "/"}};

                // 小文件:直接读内容并以 base64 编码返回(socket 安全传输)
                std::FILE *fp = std::fopen(filePath.c_str(), "rb");
                if (!fp)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kReadFailed, "无法打开文件: " + filePath);
                std::fseek(fp, 0, SEEK_END);
                long fsize = std::ftell(fp);
                std::fseek(fp, 0, SEEK_SET);
                if (offset > static_cast<size_t>(fsize)) { std::fclose(fp); throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "offset 超出文件大小"); }
                std::fseek(fp, static_cast<long>(offset), SEEK_SET);
                const size_t readSize = std::min(limit, static_cast<size_t>(fsize) - offset);
                std::vector<char> buf(readSize);
                std::fread(buf.data(), 1, readSize, fp);
                std::fclose(fp);

                // base64 编码
                static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
                std::string b64;
                b64.reserve(((fsize + 2) / 3) * 4);
                for (size_t i = 0; i < buf.size(); i += 3)
                {
                    uint32_t n = (static_cast<uint8_t>(buf[i]) << 16) |
                                 (i + 1 < buf.size() ? static_cast<uint8_t>(buf[i + 1]) << 8 : 0) |
                                 (i + 2 < buf.size() ? static_cast<uint8_t>(buf[i + 2]) : 0);
                    b64 += table[(n >> 18) & 0x3f];
                    b64 += table[(n >> 12) & 0x3f];
                    b64 += (i + 1 < buf.size()) ? table[(n >> 6) & 0x3f] : '=';
                    b64 += (i + 2 < buf.size()) ? table[n & 0x3f] : '=';
                }

                return {{"filename", filename}, {"path", filePath},
                        {"sizeBytes", (int)fileSize},
                        {"offset", offset}, {"returnedBytes", buf.size()}, {"nextOffset", offset + buf.size()},
                        {"hasMore", offset + buf.size() < fileSize}, {"content", b64}, {"truncated", offset + buf.size() < fileSize}};
            }, true);

        // ── C 组:CANCEL_JOB(取消正在运行的长任务:probe/dump)
        UmtMcp::CommandDispatcher::Register("CANCEL_JOB",
            [](const json &args) -> json
            {
                gCancelRequested = true;
                // 通知所有 job
                {
                    std::lock_guard<std::mutex> lock(gJobMutex);
                    for (auto &j : gJobs)
                        if (j.running) { j.running = false; j.lastError = "用户取消"; j.progress = -1; }
                }
                // 如果 worker 线程还在跑,等它自然退出(它会在检查点看到 gCancelRequested)
                if (gWorkerThread.joinable())
                    gWorkerThread.join();
                gCancelRequested = false;
                return {{"ok", true}, {"cancelled", true},
                        {"note", "worker 线程已停止,请轮询 GET_PROBE_STATUS / GET_DUMP_STATUS 确认最终状态"}};
            }, true);

        // ── C 组:APPLY_PROBE_OVERRIDES(在下次 START_PROBE 前注入自定义偏移)
        // 将 key→value 对存入 gProbeOverrides,START_PROBE 开头合并进 profile
        UmtMcp::CommandDispatcher::Register("APPLY_PROBE_OVERRIDES",
            [](const json &args) -> json
            {
                RequireIdleWorker();
                if (!UEMemory::kMgr.isMemValid())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kNotAttached, "未 attach 到目标进程");
                if (!args.contains("overrides") || !args["overrides"].is_object())
                    throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "overrides 须为对象");
                const pid_t currentPid = UEMemory::kMgr.processID();
                const int boundPid = args.value("pid", static_cast<int>(currentPid));
                if (boundPid != currentPid)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kSessionStale, "override pid 与当前 attach 进程不一致");
                const auto snapshot = UmtMcp::Analysis::CaptureMaps(UEMemory::kMgr);
                const std::string currentRevision = snapshot.revision;
                uint64_t requestedStartTime = snapshot.processStartTime;
                if (args.contains("processStartTime"))
                {
                    const json &value = args["processStartTime"];
                    if (value.is_number_unsigned())
                    {
                        requestedStartTime = value.get<uint64_t>();
                    }
                    else if (value.is_number_integer() && value.get<int64_t>() >= 0)
                    {
                        requestedStartTime = static_cast<uint64_t>(value.get<int64_t>());
                    }
                    else if (value.is_string())
                    {
                        const std::string raw = value.get<std::string>();
                        char *end = nullptr;
                        errno = 0;
                        const unsigned long long parsed = std::strtoull(raw.c_str(), &end, 10);
                        if (errno != 0 || end == raw.c_str() || !end || *end != '\0')
                            throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs,
                                "processStartTime 必须是非负整数或十进制字符串");
                        requestedStartTime = static_cast<uint64_t>(parsed);
                    }
                    else
                    {
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs,
                            "processStartTime 必须是非负整数或十进制字符串");
                    }
                }
                if (requestedStartTime != snapshot.processStartTime)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kSessionStale,
                        "override 绑定的进程 start time 与当前进程不一致");
                const std::string requestedRevision = args.value("mapRevision", currentRevision);
                if (requestedRevision != currentRevision)
                    throw UmtMcp::HandlerError(UmtMcp::Err::kMapStale, "override maps revision 与当前进程映射不一致");
                const int ttlSeconds = std::clamp(args.value("ttlSeconds", 300), 1, 3600);
                const bool validateBeforeApply = args.value("validateBeforeApply", true);

                auto canonicalKey = [](std::string key) -> std::string
                {
                    std::transform(key.begin(), key.end(), key.begin(),
                                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                    if (key == "names" || key == "gnames" || key == "namesptr") return "names";
                    if (key == "objects" || key == "guobjectarray" || key == "guobjectarrayptr") return "objects";
                    if (key == "world" || key == "gworld" || key == "gworldptr") return "world";
                    return {};
                };

                std::lock_guard<std::mutex> lock(gOverrideMutex);
                int applied = 0;
                json accepted = json::array();
                for (auto it = args["overrides"].begin(); it != args["overrides"].end(); ++it)
                {
                    const std::string key = canonicalKey(it.key());
                    if (key.empty())
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "未知 override key: " + it.key());
                    const json spec = it.value().is_object()
                        ? it.value()
                        : json{{"address", it.value()}, {"semantics", "DIRECT_BASE"}};
                    if (!spec.contains("address") || !spec["address"].is_string())
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "override " + it.key() + " 缺少 address");
                    uintptr_t addr = 0;
                    if (!UmtMcp::ParseAddress(spec["address"].get<std::string>(), addr))
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs, "override " + it.key() + " 格式无效");
                    const std::string semantics = spec.value("semantics", key == "world" ? "SLOT_ADDRESS" : "DIRECT_BASE");
                    if (key != "world" && (semantics == "SLOT_ADDRESS" || semantics == "POINTER_SLOT"))
                    {
                        uintptr_t resolved = 0;
                        if (UEMemory::kMgr.readMem(addr, &resolved, sizeof(resolved)) != sizeof(resolved) || !resolved)
                            throw UmtMcp::HandlerError(UmtMcp::Err::kReadFailed,
                                "override " + it.key() + " 槽位无法解引用");
                        addr = resolved;
                    }
                    if (!UmtMcp::Analysis::IsReadableAddress(snapshot, addr, sizeof(uintptr_t)))
                        throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs,
                            "override " + it.key() + " 地址不在可读 map");

                    const std::string sessionId = spec.value("sessionId", "");
                    const int candidateId = spec.value("candidateId", -1);
                    if (validateBeforeApply && (!sessionId.empty() || candidateId >= 0))
                    {
                        if (sessionId.empty() || candidateId < 0 || key == "world")
                            throw UmtMcp::HandlerError(UmtMcp::Err::kBadArgs,
                                "候选绑定需要 sessionId + candidateId，且仅支持 names/objects");
                        std::string reason;
                        if (!UmtMcp::Analysis::ValidateCandidateBinding(key, sessionId, candidateId,
                                currentPid, snapshot.processStartTime, currentRevision, addr, reason))
                            throw UmtMcp::HandlerError(UmtMcp::Err::kSessionStale,
                                "override 候选证据校验失败: " + reason);
                    }

                    gProbeOverrides[key] = addr;
                    if (spec.contains("layout") && spec["layout"].is_object())
                        gProbeOverrideLayouts[key] = spec["layout"];
                    accepted.push_back({{"key", key}, {"address", UmtMcp::FormatAddress(addr)},
                                        {"semantics", semantics}, {"sessionId", sessionId},
                                        {"candidateId", candidateId}});
                    ++applied;
                }
                gProbeOverridePid = currentPid;
                gProbeOverrideStartTime = snapshot.processStartTime;
                gProbeOverrideRevision = currentRevision;
                gProbeOverrideExpiresAt = std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds);
                return {{"applied", applied}, {"overrides", (int)gProbeOverrides.size()},
                        {"pid", currentPid}, {"processStartTime", std::to_string(snapshot.processStartTime)},
                        {"mapRevision", currentRevision}, {"ttlSeconds", ttlSeconds},
                        {"validateBeforeApply", validateBeforeApply},
                        {"accepted", accepted},
                        {"note", "覆盖在 InitUEVars 定位 Names/GUObjectArray 前生效;随后调用 START_PROBE"}};
            }, true);
    }
} // namespace

void RenderAutoUEDumpPanel(bool *main_thread_flag)
{
    bool probeRunning = false;
    bool probeFinished = false;
    bool probeSuccess = false;
    bool dumpRunning = false;
    bool dumpFinished = false;
    bool dumpSuccess = false;
    bool soDumpRunning = false;
    bool soDumpFinished = false;
    bool soDumpSuccess = false;
    std::string soDumpPath;
    int objectsPercent = 0;
    int dumpPercent = 0;
    std::string phase;
    std::string activePackage;
    std::string resultPath;
    std::string lastError;
    std::string probedPackage;
    std::string probedProfileName;
    pid_t probedPid = 0;
    std::vector<ProbeOffsetEntry> probeOffsets;
    std::vector<StructGroup> probeStructGroups;
    std::vector<std::string> logLines;

    {
        std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
        probeRunning = gDumpUiState.probeRunning;
        probeFinished = gDumpUiState.probeFinished;
        probeSuccess = gDumpUiState.probeSuccess;
        dumpRunning = gDumpUiState.dumpRunning;
        dumpFinished = gDumpUiState.dumpFinished;
        dumpSuccess = gDumpUiState.dumpSuccess;
        soDumpRunning = gDumpUiState.soDumpRunning;
        soDumpFinished = gDumpUiState.soDumpFinished;
        soDumpSuccess = gDumpUiState.soDumpSuccess;
        soDumpPath = gDumpUiState.soDumpPath;
        objectsPercent = gDumpUiState.objectsPercent;
        dumpPercent = gDumpUiState.dumpPercent;
        phase = gDumpUiState.phase;
        activePackage = gDumpUiState.activePackage;
        resultPath = gDumpUiState.resultPath;
        lastError = gDumpUiState.lastError;
        probedPackage = gDumpUiState.probedPackage;
        probedProfileName = gDumpUiState.probedProfileName;
        probedPid = gDumpUiState.probedPid;
        probeOffsets = gDumpUiState.probeOffsets;
        probeStructGroups = gDumpUiState.probeStructGroups;
        logLines = gDumpUiState.logLines;
    }

    enum NavPage
    {
        NavOverview = 0,
        NavProcess,
        NavStructs,
        NavExplorer,
        NavMcp,
        NavLogs,
        NavSettings
    };

    static int navPage = NavOverview;
    static int selectedStructIndex = 0;
    static int themeIndex = 1;

    const bool busy = probeRunning || dumpRunning || soDumpRunning;
    if (!busy && gWorkerThread.joinable())
        gWorkerThread.join();

    const bool hasSelection = !gCandidates.empty() &&
                              gSelectedIndex >= 0 &&
                              gSelectedIndex < static_cast<int>(gCandidates.size());
    const std::string selectedPackage = hasSelection ? gCandidates[gSelectedIndex].package : std::string();
    const pid_t selectedPid = hasSelection ? gCandidates[gSelectedIndex].pid : 0;
    const bool probeMatchesSelection = probeFinished && probeSuccess &&
                                       !selectedPackage.empty() &&
                                       selectedPackage == probedPackage &&
                                       selectedPid == probedPid;
    const bool hasData = probeFinished && probeSuccess && !probeStructGroups.empty();
    if (!hasData)
        selectedStructIndex = 0;
    else if (selectedStructIndex >= static_cast<int>(probeStructGroups.size()))
        selectedStructIndex = static_cast<int>(probeStructGroups.size()) - 1;

    auto applyTheme = [](int idx)
    {
        switch (idx)
        {
            case 0: ImGui::StyleColorsLight(); break;
            case 2: ImGui::StyleColorsClassic(); break;
            default: ImGui::StyleColorsDark(); break;
        }
    };

    auto drawNavButton = [](const char *label, bool active) -> bool
    {
        ImGui::PushStyleColor(ImGuiCol_Button,
                              active ? ImVec4(0.23f, 0.40f, 0.70f, 0.95f)
                                     : ImVec4(0.11f, 0.14f, 0.20f, 0.90f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              active ? ImVec4(0.27f, 0.47f, 0.82f, 1.0f)
                                     : ImVec4(0.18f, 0.24f, 0.35f, 0.95f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              active ? ImVec4(0.20f, 0.36f, 0.63f, 1.0f)
                                     : ImVec4(0.16f, 0.22f, 0.31f, 1.0f));
        const bool clicked = ImGui::Button(label, ImVec2(-1.0f, 42.0f));
        ImGui::PopStyleColor(3);
        return clicked;
    };

    auto drawStatusChip = [](const char *label, const ImVec4 &color)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
        ImGui::Button(label);
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();
    };

    auto drawSectionHeader = [](const char *title, const char *subtitle)
    {
        ImGui::Text("%s", title);
        if (subtitle && *subtitle)
            ImGui::TextDisabled("%s", subtitle);
        ImGui::Dummy(ImVec2(0.0f, 2.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
    };

    auto drawActionButtons = [&]()
    {
        if (busy)
        {
            ImGui::BeginDisabled();
            const char *label = Tr("进行中...", "Working...");
            if (probeRunning)        label = Tr("探针进行中...", "Probing...");
            else if (dumpRunning)    label = Tr("Dump 进行中...", "Dumping...");
            else if (soDumpRunning)  label = Tr("动态库 Dump 进行中...", "Dumping library...");
            ImGui::Button(label, ImVec2(-1.0f, 42.0f));
            ImGui::EndDisabled();
        }
        else
        {
            const bool canProbe = hasSelection;
            if (!canProbe) ImGui::BeginDisabled();
            if (ImGui::Button(Tr("开始探测", "Start Probe"), ImVec2(-1.0f, 42.0f)))
                StartProbeSelected();
            if (!canProbe) ImGui::EndDisabled();

            const bool canDump = probeMatchesSelection;
            if (!canDump) ImGui::BeginDisabled();
            if (ImGui::Button(Tr("开始 Dump", "Start Dump"), ImVec2(-1.0f, 42.0f)))
                StartDumpAfterProbe();
            if (!canDump) ImGui::EndDisabled();

            const bool canDumpSo = probeMatchesSelection;
            if (!canDumpSo) ImGui::BeginDisabled();
            if (ImGui::Button(Tr("Dump 动态库", "Dump Library"), ImVec2(-1.0f, 42.0f)))
                StartDumpUnrealLib();
            if (!canDumpSo) ImGui::EndDisabled();
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("%s", Tr("从内存转储 libUE4.so / libUnreal.so",
                                      "Dump libUE4.so / libUnreal.so from memory"));
                ImGui::TextDisabled("%s: %s/<package>/<lib*.so>",
                                    Tr("输出", "Output"), kOutputDirectory);
                ImGui::EndTooltip();
            }
        }
    };

    auto drawProcessListPane = [&](const char *childId, const char *title, const char *subtitle)
    {
        ImGui::PushID(childId);
        ImGui::BeginChild("##panel", ImVec2(0.0f, 0.0f), true);
        drawSectionHeader(title, subtitle);
        if (ImGui::Button(Tr("刷新进程", "Refresh Processes"), ImVec2(-1.0f, 40.0f)) && !busy)
            RefreshCandidates();
        ImGui::Dummy(ImVec2(0.0f, 4.0f));
        if (ImGui::BeginChild("##process_list_card", ImVec2(0.0f, 0.0f), false))
        {
            for (int i = 0; i < static_cast<int>(gCandidates.size()); ++i)
            {
                const auto &candidate = gCandidates[i];
                std::string label = candidate.package + "\nPID " + std::to_string(candidate.pid) +
                                    "  |  " + candidate.profileName;
                if (ImGui::Selectable(label.c_str(), gSelectedIndex == i, 0, ImVec2(0.0f, 52.0f)))
                {
                    const bool changed = gSelectedIndex != i;
                    gSelectedIndex = i;
                    if (changed && !busy &&
                        (!gProbeResult.valid ||
                         gProbeResult.package != candidate.package ||
                         gProbeResult.pid != candidate.pid))
                    {
                        InvalidateProbeReuse("已切换到新的进程实例，旧探针结果已失效，请重新探测。");
                    }
                }
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s: %s", Tr("包名", "Package"), candidate.package.c_str());
                    ImGui::Text("PID: %d", candidate.pid);
                    ImGui::Text("Profile: %s", candidate.profileName.c_str());
                    ImGui::Text("%s: %s", Tr("模式", "Mode"),
                                candidate.dedicated ? Tr("专用", "Dedicated")
                                                    : Tr("自动", "Auto"));
                    ImGui::EndTooltip();
                }
            }
            if (gCandidates.empty())
            {
                ImGui::TextWrapped("%s",
                    Tr("当前没有找到正在运行的 Unreal Engine 进程，请点击上方按钮刷新。",
                       "No running Unreal Engine process was found. Click the button above to refresh."));
            }
        }
        ImGui::EndChild();
        ImGui::EndChild();
        ImGui::PopID();
    };

    auto drawProbeSummaryPane = [&](const char *title, const char *subtitle, bool withJumpToStructs)
    {
        ImGui::PushID(title);
        ImGui::BeginChild("##panel", ImVec2(0.0f, 0.0f), true);
        drawSectionHeader(title, subtitle);
        if (probeFinished && probeSuccess)
        {
            ImGui::TextWrapped("%s: %s", Tr("已探测目标", "Probed"), probedPackage.c_str());
            ImGui::TextWrapped("Profile: %s", probedProfileName.c_str());
            ImGui::Text("PID: %d", probedPid);
            ImGui::Dummy(ImVec2(0.0f, 8.0f));

            if (!probeOffsets.empty() &&
                ImGui::BeginTable("##probe_offsets", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn(Tr("名称", "Name"), ImGuiTableColumnFlags_WidthStretch, 1.3f);
                ImGui::TableSetupColumn(Tr("偏移", "Offset"), ImGuiTableColumnFlags_WidthStretch, 1.0f);
                ImGui::TableSetupColumn(Tr("状态", "State"), ImGuiTableColumnFlags_WidthStretch, 0.8f);
                ImGui::TableHeadersRow();
                for (const auto &entry : probeOffsets)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(entry.name.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("0x%lX", static_cast<unsigned long>(entry.relative));
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextColored(entry.found ? ImVec4(0.36f, 0.92f, 0.45f, 1.0f)
                                                   : ImVec4(1.0f, 0.43f, 0.43f, 1.0f),
                                       "%s", entry.found ? Tr("已识别", "OK") : Tr("缺失", "Missing"));
                }
                ImGui::EndTable();
            }

            if (!probeStructGroups.empty())
            {
                ImGui::Dummy(ImVec2(0.0f, 8.0f));
                ImGui::Text("%s", Tr("结构摘要", "Struct Summary"));
                ImGui::Dummy(ImVec2(0.0f, 2.0f));
                for (int i = 0; i < static_cast<int>(probeStructGroups.size()) && i < 8; ++i)
                {
                    const auto &group = probeStructGroups[i];
                    std::string summary = group.name + "  |  " +
                                          std::to_string(group.fields.size()) + Tr(" 字段", " fields");
                    if (ImGui::Selectable(summary.c_str(), selectedStructIndex == i))
                    {
                        selectedStructIndex = i;
                        if (withJumpToStructs)
                            navPage = NavStructs;
                    }
                }
            }
        }
        else if (probeFinished)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.43f, 0.43f, 1.0f), "%s",
                Tr("探针失败，请查看日志后重新尝试。",
                   "Probe failed. Check the logs and try again."));
        }
        else
        {
            ImGui::TextWrapped("%s",
                Tr("尚未执行探针。选择进程后可在右侧直接开始。",
                   "No probe has been run yet. Select a process and start from the right column."));
        }
        ImGui::EndChild();
        ImGui::PopID();
    };

    auto drawActionPanel = [&](const char *title, const char *subtitle, bool showRecentLogs)
    {
        ImGui::PushID(title);
        ImGui::BeginChild("##panel", ImVec2(0.0f, 0.0f), true);
        drawSectionHeader(title, subtitle);

        if (hasSelection)
        {
            const auto &candidate = gCandidates[gSelectedIndex];
            ImGui::TextWrapped("%s: %s", Tr("已选包名", "Selected"), candidate.package.c_str());
            ImGui::Text("PID: %d", candidate.pid);
            ImGui::TextWrapped("Profile: %s", candidate.profileName.c_str());
            ImGui::Text("%s: %s", Tr("模式", "Mode"),
                        candidate.dedicated ? Tr("专用", "Dedicated")
                                            : Tr("自动", "Auto"));
        }
        else
        {
            ImGui::TextWrapped("%s",
                Tr("请先在左侧列表中选择一个进程。",
                   "Select a process from the left list first."));
        }

        ImGui::Dummy(ImVec2(0.0f, 8.0f));
        drawActionButtons();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 6.0f));
        ImGui::Text("%s: %s", Tr("阶段", "Phase"), phase.c_str());
        if (probeFinished)
        {
            ImGui::TextColored(probeSuccess ? ImVec4(0.36f, 0.92f, 0.45f, 1.0f)
                                            : ImVec4(1.0f, 0.43f, 0.43f, 1.0f),
                               "%s", probeSuccess ? Tr("探针成功", "Probe OK")
                                                  : Tr("探针失败", "Probe Failed"));
        }
        if (dumpFinished)
        {
            ImGui::TextColored(dumpSuccess ? ImVec4(0.36f, 0.92f, 0.45f, 1.0f)
                                           : ImVec4(1.0f, 0.43f, 0.43f, 1.0f),
                               "%s", dumpSuccess ? Tr("Dump 成功", "Dump OK")
                                                 : Tr("Dump 失败", "Dump Failed"));
        }
        if (soDumpFinished)
        {
            ImGui::TextColored(soDumpSuccess ? ImVec4(0.36f, 0.92f, 0.45f, 1.0f)
                                             : ImVec4(1.0f, 0.43f, 0.43f, 1.0f),
                               "%s", soDumpSuccess ? Tr("动态库导出成功", "Library dump OK")
                                                   : Tr("动态库导出失败", "Library dump failed"));
        }
        if (!resultPath.empty() && dumpSuccess)
            ImGui::TextWrapped("%s: %s", Tr("结果路径", "Output Path"), resultPath.c_str());
        if (!soDumpPath.empty() && soDumpSuccess)
            ImGui::TextWrapped("%s: %s", Tr("动态库路径", "Library Path"), soDumpPath.c_str());
        if (!lastError.empty())
            ImGui::TextWrapped("%s: %s", Tr("状态信息", "Status"), lastError.c_str());

        if (objectsPercent > 0)
        {
            std::string label = std::string(Tr("对象扫描", "Objects scan")) + " " +
                                std::to_string(objectsPercent) + "%";
            ImGui::ProgressBar(objectsPercent / 100.0f, ImVec2(-1.0f, 0.0f), label.c_str());
        }
        if (dumpPercent > 0)
        {
            std::string label = std::string(Tr("Dump 进度", "Dump progress")) + " " +
                                std::to_string(dumpPercent) + "%";
            ImGui::ProgressBar(dumpPercent / 100.0f, ImVec2(-1.0f, 0.0f), label.c_str());
        }

        if (showRecentLogs && !logLines.empty())
        {
            ImGui::Dummy(ImVec2(0.0f, 8.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 6.0f));
            ImGui::Text("%s", Tr("最近日志", "Recent Logs"));
            if (ImGui::BeginChild("##recent_logs", ImVec2(0.0f, 160.0f), false, ImGuiWindowFlags_HorizontalScrollbar))
            {
                const int begin = static_cast<int>(logLines.size()) > 8
                                    ? static_cast<int>(logLines.size()) - 8
                                    : 0;
                for (int i = begin; i < static_cast<int>(logLines.size()); ++i)
                    ImGui::TextUnformatted(logLines[i].c_str());
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
        ImGui::PopID();
    };

    auto drawStructListPane = [&](const char *title, const char *subtitle)
    {
        ImGui::PushID(title);
        ImGui::BeginChild("##panel", ImVec2(0.0f, 0.0f), true);
        drawSectionHeader(title, subtitle);
        if (hasData && ImGui::BeginChild("##struct_list_scroll", ImVec2(0.0f, 0.0f), false))
        {
            for (int i = 0; i < static_cast<int>(probeStructGroups.size()); ++i)
            {
                const auto &group = probeStructGroups[i];
                std::string item = group.name + "\n" +
                                   std::to_string(group.fields.size()) + Tr(" 个字段", " fields");
                if (ImGui::Selectable(item.c_str(), selectedStructIndex == i, 0, ImVec2(0.0f, 54.0f)))
                    selectedStructIndex = i;
            }
            ImGui::EndChild();
        }
        else
        {
            ImGui::TextWrapped("%s",
                Tr("尚未探测到可用结构，请先完成探针流程。",
                   "No structure data yet. Complete the probe first."));
        }
        ImGui::EndChild();
        ImGui::PopID();
    };

    auto drawStructDetailPane = [&](const char *title, const char *subtitle)
    {
        ImGui::PushID(title);
        ImGui::BeginChild("##panel", ImVec2(0.0f, 0.0f), true);
        drawSectionHeader(title, subtitle);
        if (hasData && selectedStructIndex >= 0 &&
            selectedStructIndex < static_cast<int>(probeStructGroups.size()))
        {
            const auto &group = probeStructGroups[selectedStructIndex];
            ImGui::Text("%s", group.name.c_str());
            ImGui::TextDisabled("%s: %d", Tr("字段数量", "Field count"), static_cast<int>(group.fields.size()));
            ImGui::Dummy(ImVec2(0.0f, 8.0f));
            if (ImGui::BeginTable("##fields_detail", 5,
                                  ImGuiTableFlags_Borders |
                                  ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_Resizable |
                                  ImGuiTableFlags_ScrollY |
                                  ImGuiTableFlags_SizingStretchProp,
                                  ImVec2(0.0f, 0.0f)))
            {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableSetupColumn(Tr("字段", "Field"),  ImGuiTableColumnFlags_WidthStretch, 1.5f);
                ImGui::TableSetupColumn(Tr("类型", "Type"),   ImGuiTableColumnFlags_WidthStretch, 1.3f);
                ImGui::TableSetupColumn(Tr("偏移", "Offset"), ImGuiTableColumnFlags_WidthStretch, 0.9f);
                ImGui::TableSetupColumn(Tr("状态", "Status"), ImGuiTableColumnFlags_WidthStretch, 0.8f);
                ImGui::TableSetupColumn(Tr("说明", "Notes"),  ImGuiTableColumnFlags_WidthStretch, 2.2f);
                ImGui::TableHeadersRow();

                for (const auto &field : group.fields)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(field.name.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(field.type.c_str());
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("0x%lX", static_cast<unsigned long>(field.offset));
                    ImGui::TableSetColumnIndex(3);
                    ImGui::TextColored(field.found ? ImVec4(0.36f, 0.92f, 0.45f, 1.0f)
                                                   : ImVec4(1.0f, 0.43f, 0.43f, 1.0f),
                                       "%s", field.found ? Tr("已识别", "Identified")
                                                         : Tr("未识别", "Unknown"));
                    ImGui::TableSetColumnIndex(4);
                    ImGui::TextWrapped("%s", field.description.c_str());
                }
                ImGui::EndTable();
            }
        }
        else
        {
            ImGui::TextWrapped("%s",
                Tr("左侧还没有可显示的结构数据。",
                   "There is no structure data to display yet."));
        }
        ImGui::EndChild();
        ImGui::PopID();
    };

    auto drawOverviewCard = [&](const char *childId, float width, const char *title, const char *value, const char *subtitle)
    {
        ImGui::PushID(childId);
        ImGui::BeginChild("##metric", ImVec2(width, 92.0f), true);
        ImGui::TextDisabled("%s", title);
        ImGui::Text("%s", value);
        if (subtitle && *subtitle)
            ImGui::TextDisabled("%s", subtitle);
        ImGui::EndChild();
        ImGui::PopID();
    };

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 12.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 9.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 14.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 14.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 10.0f);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.07f, 0.09f, 0.13f, 0.96f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.11f, 0.13f, 0.19f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.16f, 0.22f, 0.32f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.18f, 0.26f, 0.39f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.27f, 0.42f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.24f, 0.38f, 0.58f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.19f, 0.33f, 0.53f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.14f, 0.22f, 0.34f, 0.90f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.19f, 0.31f, 0.47f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.16f, 0.27f, 0.42f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.28f, 0.40f, 0.90f));
    ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.20f, 0.26f, 0.37f, 0.90f));

    const float sidebarWidth = 180.0f;
    const float leftColumnWidth = 320.0f;
    const float middleColumnWidth = 310.0f;

    ImGui::BeginChild("##tool_root", ImVec2(0.0f, 0.0f), false);

    ImGui::BeginChild("##sidebar", ImVec2(sidebarWidth, 0.0f), true);
    ImGui::Text("UnrealMemoryTools");
    ImGui::TextDisabled("UnrealEngine4.1x-5.0x");
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    if (drawNavButton(Tr("总览", "Overview"), navPage == NavOverview)) navPage = NavOverview;
    if (drawNavButton(Tr("进程", "Processes"), navPage == NavProcess)) navPage = NavProcess;
    if (drawNavButton(Tr("结构", "Structs"), navPage == NavStructs)) navPage = NavStructs;
    if (drawNavButton(Tr("SDK", "SDK"), navPage == NavExplorer)) navPage = NavExplorer;
    if (drawNavButton("MCP", navPage == NavMcp)) navPage = NavMcp;
    if (drawNavButton(Tr("日志", "Logs"), navPage == NavLogs)) navPage = NavLogs;
    if (drawNavButton(Tr("设置", "Settings"), navPage == NavSettings)) navPage = NavSettings;

    const float sidebarFooter = ImGui::GetWindowHeight() - 96.0f;
    if (ImGui::GetCursorPosY() < sidebarFooter)
        ImGui::SetCursorPosY(sidebarFooter);
    ImGui::Separator();
    ImGui::TextDisabled("%s %s", Tr("版本", "Version"), kUEDUMPER_VERSION);
    ImGui::TextDisabled("%s", Tr("泪心二改版", "TearHacker Fork"));
    ImGui::EndChild();

    ImGui::SameLine(0.0f, 14.0f);

    ImGui::BeginChild("##workspace", ImVec2(0.0f, 0.0f), false);
    if (navPage == NavMcp)
    {
        ImGui::BeginChild("##mcp_workspace", ImVec2(0.0f, 0.0f), true);
        drawSectionHeader(Tr("MCP 命令服务", "MCP Command Service"),
                          Tr("连接信息与 PC 侧配置集中显示在此页",
                             "Connection details for the PC client are shown here"));

        ImGui::TextDisabled("%s", Tr("服务状态", "Service Status"));
        if (UmtMcp::CommandServer::IsRunning())
        {
            ImGui::TextColored(ImVec4(0.36f, 0.92f, 0.45f, 1.0f), "%s",
                               Tr("MCP 服务已启动", "MCP service running"));
        }
        else
        {
            ImGui::TextDisabled("%s", Tr("MCP 服务未运行", "MCP service not running"));
        }

        // 客户端实时状态：一行展示 在线/离线 + 累计工具调用数（不换行、无 emoji）
        ImGui::Dummy(ImVec2(0.0f, 6.0f));
        if (UmtMcp::CommandServer::IsClientConnected())
        {
            ImGui::TextColored(ImVec4(0.36f, 0.92f, 0.45f, 1.0f),
                               "%s   %s: %llu",
                               Tr("客户端: 已连接", "Client: Connected"),
                               Tr("累计工具调用", "Tool calls"),
                               (unsigned long long)UmtMcp::CommandServer::ToolCallCount());
        }
        else
        {
            ImGui::TextColored(ImVec4(0.95f, 0.78f, 0.36f, 1.0f),
                               "%s   %s: %llu",
                               Tr("客户端: 未连接", "Client: Disconnected"),
                               Tr("累计工具调用", "Tool calls"),
                               (unsigned long long)UmtMcp::CommandServer::ToolCallCount());
        }

        ImGui::Dummy(ImVec2(0.0f, 12.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 12.0f));
        ImGui::TextDisabled("%s", Tr("监听地址", "Listen Address"));
        // 运行期实际 bind 地址：默认 127.0.0.1，写过 mcp_bind.conf 后变为配置值
        const std::string &bindAddr = UmtMcp::CommandServer::GetBindAddress();
        ImGui::Text("%s:%d", bindAddr.c_str(), static_cast<int>(UmtMcp::kDefaultPort));

        // 三种监听模式按 bind 地址判定当前生效项（设备侧只能看到 bind 地址，adb 隧道是 PC 端概念）
        //  - 回环 127.0.0.1 / localhost / ::1      → 模式 3 本机 Operit 同机直连，无需 adb，最安全
        //  - 全局 0.0.0.0 / ::                     → 模式 2 全局监听，连蜂窝/rmnet 也暴露，最危险
        //  - 其它具体地址（如 192.168.x.x）         → 模式 1 局域网/指定网卡直连
        // UI 上把三种模式都列出来，并用颜色 + "◀ 当前生效" 标出当前那一个
        const bool isLoopback = (bindAddr == "127.0.0.1" || bindAddr == "localhost" ||
                                 bindAddr == "::1" || bindAddr == "0:0:0:0:0:0:0:1");
        const bool isGlobal = (bindAddr == "0.0.0.0" || bindAddr == "::" ||
                               bindAddr == "::0" || bindAddr == "[::]");
        const int activeMode = isLoopback ? 3 : (isGlobal ? 2 : 1);
        auto drawModeRow = [&](int mode, const char *zh, const char *en, const ImVec4 &color) {
            if (mode == activeMode)
                ImGui::TextColored(color, "%s   ◀ %s", Tr(zh, en),
                                   Tr("当前生效", "active"));
            else
                ImGui::TextDisabled("%s", Tr(zh, en));
        };
        drawModeRow(3, "模式 3 · 本机回环（Operit 同机直连，无需 ADB 隧道）",
                       "Mode 3 - Loopback (same-device Operit, no ADB tunnel)",
                       ImVec4(0.36f, 0.92f, 0.45f, 1.0f));
        drawModeRow(2, "模式 2 · 全局监听 0.0.0.0（含蜂窝网络，仅绝对可信环境使用）",
                       "Mode 2 - Global 0.0.0.0 (incl. cellular; trusted env only)",
                       ImVec4(0.92f, 0.36f, 0.36f, 1.0f));
        drawModeRow(1, "模式 1 · 局域网/指定地址直连（仅该网卡可达，注意局域网内其它设备）",
                       "Mode 1 - LAN/specified-address direct (one NIC only; mind other LAN devices)",
                       ImVec4(0.95f, 0.78f, 0.36f, 1.0f));

        // 设备当前局域网 IP：模式 1 时 PC 端客户端要填的连接地址（root 直接查）
        ImGui::Dummy(ImVec2(0.0f, 6.0f));
        ImGui::TextDisabled("%s", Tr("本机局域网 IP（模式 1 连接地址）",
                                     "Device LAN IP (Mode 1 connect address)"));
        const std::string lanIp = GetLanIp();
        if (lanIp.empty())
            ImGui::TextColored(ImVec4(0.95f, 0.78f, 0.36f, 1.0f), "%s",
                               Tr("未获取到，请确认 Wi-Fi 已连接", "Not available; ensure Wi-Fi is up"));
        else
            ImGui::Text("%s", lanIp.c_str());
        ImGui::TextDisabled("%s", Tr("把以上地址写入 mcp_bind.conf 即切换为模式 1",
                                     "Write this address into mcp_bind.conf to switch to Mode 1"));

        ImGui::Dummy(ImVec2(0.0f, 12.0f));
        ImGui::TextDisabled("%s", Tr("连接方式", "Connection"));
        ImGui::TextColored(ImVec4(0.36f, 0.92f, 0.45f, 1.0f), "%s",
                           Tr("无需 Token，客户端配置完成后可直接调用",
                              "No token required; configured clients can call directly"));
        ImGui::EndChild();
    }
    else if (navPage == NavExplorer)
    {
        ImGui::BeginChild("##explorer_card", ImVec2(0.0f, 0.0f), true);
        drawSectionHeader(Tr("SDK 浏览器", "SDK Explorer"),
                          Tr("此区域保持原有浏览功能，但外层布局改为卡片式容器",
                             "The explorer remains intact inside the new card layout"));
        SDKExplorer::SetLanguage(gUiLang == UiLang::ZH ? 0 : 1);
        SDKExplorer::Render();
        ImGui::EndChild();
    }
    else if (navPage == NavLogs)
    {
        ImGui::BeginChild("##logs_card", ImVec2(0.0f, 0.0f), true);
        drawSectionHeader(Tr("运行日志", "Logs"),
                          Tr("这里汇总了探测、Dump 与动态库导出的实时输出",
                             "Probe, dump and library export messages are shown here"));
        if (ImGui::Button(Tr("清空日志", "Clear Logs"), ImVec2(160.0f, 0.0f)))
        {
            std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
            gDumpUiState.logLines.clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox(Tr("隐藏 MCP 连接上下线", "Hide MCP connect/disconnect"),
                        &gHideMcpConnLogs);
        ImGui::Dummy(ImVec2(0.0f, 6.0f));
        if (ImGui::BeginChild("##logs_scroll", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_HorizontalScrollbar))
        {
            for (const auto &line : logLines)
            {
                // 勾选「隐藏连接上下线」后，过滤掉 [MCP·连接] 这类刷屏行
                if (gHideMcpConnLogs && line.find("[MCP·连接]") != std::string::npos)
                    continue;
                ImGui::TextUnformatted(line.c_str());
            }

            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
        ImGui::EndChild();
    }
    else if (navPage == NavSettings)
    {
        ImGui::BeginChild("##settings_card", ImVec2(0.0f, 0.0f), true);
        drawSectionHeader(Tr("设置中心", "Settings"),
                          Tr("语言、主题与项目信息集中在此页",
                             "Language, theme and project information live here"));

        ImGui::Text("%s", Tr("语言", "Language"));
        if (ImGui::Button("中文", ImVec2(120.0f, 0.0f))) gUiLang = UiLang::ZH;
        ImGui::SameLine(0.0f, 10.0f);
        if (ImGui::Button("English", ImVec2(120.0f, 0.0f))) gUiLang = UiLang::EN;

        ImGui::Dummy(ImVec2(0.0f, 8.0f));
        ImGui::Text("%s", Tr("主题", "Theme"));
        if (ImGui::Combo("##theme_combo", &themeIndex,
                         Tr("白色主题\0蓝色主题\0紫色主题\0",
                            "Light\0Dark\0Classic\0")))
        {
            applyTheme(themeIndex);
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 8.0f));
        ImGui::Text("%s: 曦曦(DreamFekk)    %s: 泪心(tearhacker)", Tr("创作者", "Author"), Tr("贡献者", "Contributor"));
        ImGui::TextWrapped("GitHub: https://github.com/DreamFekk");
        ImGui::TextWrapped("%s: %s", Tr("输出目录", "Output"), kOutputDirectory);
        ImGui::Text("%s: %s    %s: %.1f", Tr("渲染接口", "Renderer"), graphics->RenderName, Tr("当前 FPS", "FPS"), ImGui::GetIO().Framerate);

        ImGui::Dummy(ImVec2(0.0f, 12.0f));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 8.0f));
        if (ImGui::Button(Tr("退出应用", "Exit App"), ImVec2(-1.0f, 48.0f)))
            *main_thread_flag = false;
        ImGui::EndChild();
    }
    else if (navPage == NavOverview)
    {
        const float metricSpacing = 14.0f;
        const float metricWidth = (ImGui::GetContentRegionAvail().x - metricSpacing * 2.0f) / 3.0f;
        std::string processCountText = std::to_string(gCandidates.size());
        std::string structCountText = std::to_string(probeStructGroups.size());
        std::string phaseText = phase.empty() ? std::string(Tr("空闲", "Idle")) : phase;

        drawOverviewCard("##overview_metric_process", metricWidth, Tr("进程数量", "Processes"), processCountText.c_str(),
                         Tr("已识别的 UE 目标实例", "Detected Unreal Engine targets"));
        ImGui::SameLine(0.0f, metricSpacing);
        drawOverviewCard("##overview_metric_struct", metricWidth, Tr("结构分组", "Struct Groups"), structCountText.c_str(),
                         Tr("探针成功后可浏览的结构组", "Groups available after a successful probe"));
        ImGui::SameLine(0.0f, metricSpacing);
        drawOverviewCard("##overview_metric_phase", metricWidth, Tr("当前阶段", "Current Phase"), phaseText.c_str(),
                         busy ? Tr("任务正在执行", "A task is running")
                              : Tr("当前没有后台任务", "No background task is running"));

        ImGui::Dummy(ImVec2(0.0f, 12.0f));

        ImGui::BeginChild("##overview_left", ImVec2(leftColumnWidth + 80.0f, 0.0f), false);
        drawProbeSummaryPane(Tr("探针总览", "Probe Overview"),
                             Tr("总览页聚合当前探针状态和结构摘要",
                                "Overview aggregates the current probe state and struct summary"),
                             true);
        ImGui::EndChild();

        ImGui::SameLine(0.0f, 14.0f);
        drawActionPanel(Tr("快捷操作", "Quick Actions"),
                        Tr("总览页只保留最常用的动作和状态",
                           "Only the most common actions and status are kept here"),
                        true);
    }
    else if (navPage == NavProcess)
    {
        ImGui::BeginChild("##process_left", ImVec2(leftColumnWidth, 0.0f), false);
        drawProcessListPane("##process_manage_left", Tr("进程库", "Process Library"),
                            Tr("专门用于进程选择与切换",
                               "Dedicated to process selection and switching"));
        ImGui::EndChild();

        ImGui::SameLine(0.0f, 14.0f);

        ImGui::BeginChild("##process_middle", ImVec2(middleColumnWidth, 0.0f), false);
        ImGui::BeginChild("##process_detail_card", ImVec2(0.0f, 0.0f), true);
        drawSectionHeader(Tr("当前进程", "Current Process"),
                          Tr("这里专门显示选中进程的身份信息，不再和总览页混用",
                             "Identity details live here instead of being mixed into overview"));
        if (hasSelection)
        {
            const auto &candidate = gCandidates[gSelectedIndex];
            ImGui::TextWrapped("%s: %s", Tr("包名", "Package"), candidate.package.c_str());
            ImGui::Text("PID: %d", candidate.pid);
            ImGui::TextWrapped("Profile: %s", candidate.profileName.c_str());
            ImGui::Text("%s: %s", Tr("模式", "Mode"),
                        candidate.dedicated ? Tr("专用", "Dedicated")
                                            : Tr("自动", "Auto"));
            ImGui::Dummy(ImVec2(0.0f, 8.0f));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0.0f, 8.0f));
            ImGui::Text("%s", Tr("探针匹配状态", "Probe Match"));
            ImGui::TextColored(probeMatchesSelection ? ImVec4(0.36f, 0.92f, 0.45f, 1.0f)
                                                     : ImVec4(1.0f, 0.43f, 0.43f, 1.0f),
                               "%s", probeMatchesSelection ? Tr("当前探针结果可直接复用", "Current probe can be reused")
                                                          : Tr("当前探针结果与所选进程不匹配", "Probe result does not match selection"));
        }
        else
        {
            ImGui::TextWrapped("%s",
                Tr("还没有选中进程，请先在左侧列表中点击一个目标。",
                   "No process selected yet. Pick one from the left list."));
        }
        ImGui::EndChild();
        ImGui::EndChild();

        ImGui::SameLine(0.0f, 14.0f);
        drawActionPanel(Tr("进程操作", "Process Actions"),
                        Tr("进程页专注于探测、Dump 和状态回显",
                           "The process page focuses on probe, dump and status"),
                        true);
    }
    else if (navPage == NavStructs)
    {
        ImGui::BeginChild("##struct_left", ImVec2(260.0f, 0.0f), false);
        drawStructListPane(Tr("结构列表", "Struct List"),
                           Tr("这里只负责结构分组切换",
                              "This column only handles struct group selection"));
        ImGui::EndChild();

        ImGui::SameLine(0.0f, 14.0f);

        ImGui::BeginChild("##struct_mid", ImVec2(250.0f, 0.0f), false);
        drawProbeSummaryPane(Tr("结构摘要", "Struct Summary"),
                             Tr("补充展示当前探针成功后的结构摘要与偏移",
                                "Shows offset and struct summary after a successful probe"),
                             false);
        ImGui::EndChild();

        ImGui::SameLine(0.0f, 14.0f);
        drawStructDetailPane(Tr("字段详情", "Field Details"),
                             Tr("右侧只保留字段明细，不再混入操作按钮",
                                "The right pane now stays focused on field details"));
    }

    ImGui::EndChild();
    ImGui::EndChild();

    ImGui::PopStyleColor(11);
    ImGui::PopStyleVar(7);
}


// 查询设备当前 Wi-Fi 局域网 IP（root 环境下直接调用原生 ip 命令，无需手动查）
// 解析 `ip -4 addr show wlan0` 输出里的 "inet A.B.C.D/24"，提取 A.B.C.D
static std::string GetLanIp()
{
    std::string ip;
    FILE *fp = popen("ip -4 addr show wlan0 2>/dev/null", "r");
    if (fp)
    {
        char buf[512];
        while (fgets(buf, sizeof(buf), fp))
        {
            const char *p = strstr(buf, "inet ");
            if (!p) continue;
            p += 5;  // 跳过 "inet "
            const char *end = strchr(p, '/');
            if (!end) end = p + strcspn(p, " \t\r\n");
            const size_t len = static_cast<size_t>(end - p);
            if (len > 0 && len < 64)
            {
                ip.assign(p, len);
                break;
            }
        }
        pclose(fp);
    }
    return ip;
}

int main()
{
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    setbuf(stdin, nullptr);

    Logger::SetSink(LoggerSink);
    RefreshCandidates();

    // ---- MCP 命令服务（服务端，bind 127.0.0.1:35515）----
    {
        static UmtMcp::CommandQueue mcpQueue;
        UmtMcp::CommandDispatcher::BindQueue(&mcpQueue);
        SetupMcpCommands();

        if (UmtMcp::CommandServer::Start(UmtMcp::kDefaultPort, &mcpQueue, kUEDUMPER_VERSION))
        {
            LOGI("[MCP] 命令服务已就绪，客户端无需 Token 可直接连接");
        }
        else
        {
            LOGE("[MCP] 命令服务启动失败");
        }
    }

    ::graphics = GraphicsManager::getGraphicsInterface(GraphicsManager::VULKAN);
    if (!::graphics)
    {
        LOGE("创建图形后端失败。");
        Logger::SetSink(nullptr);
        return 1;
    }

    ::screen_config();
    ::native_window_screen_x = (::displayInfo.height > ::displayInfo.width ? ::displayInfo.height : ::displayInfo.width);
    ::native_window_screen_y = (::displayInfo.height > ::displayInfo.width ? ::displayInfo.height : ::displayInfo.width);
    ::abs_ScreenX = (::displayInfo.height > ::displayInfo.width ? ::displayInfo.height : ::displayInfo.width);
    ::abs_ScreenY = (::displayInfo.height < ::displayInfo.width ? ::displayInfo.height : ::displayInfo.width);

    ::window = android::ANativeWindowCreator::Create("UnrealMemoryTools", native_window_screen_x, native_window_screen_y, permeate_record);
    if (!::window)
    {
        LOGE("创建 ANativeWindow 失败。");
        Logger::SetSink(nullptr);
        return 1;
    }

    if (!graphics->Init_Render(::window, native_window_screen_x, native_window_screen_y))
    {
        LOGE("初始化图形渲染失败。");
        android::ANativeWindowCreator::Destroy(::window);
        Logger::SetSink(nullptr);
        return 1;
    }

    Touch::Init({(float)::abs_ScreenX, (float)::abs_ScreenY}, false);
    Touch::setOrientation(displayInfo.orientation);
    ::init_My_drawdata();

    bool flag = true;
    while (flag)
    {
        drawBegin();
        if (permeate_record == false)
            android::ANativeWindowCreator::ProcessMirrorDisplay();
        graphics->NewFrame();
        Layout_tick_UI(&flag);
        // MCP：每帧 poll 一次命令队列（主线程执行，保证与 UMT 全局状态串行）
        UmtMcp::CommandDispatcher::PollOnce();
        graphics->EndFrame();
    }

    UmtMcp::CommandServer::Stop();

    if (gWorkerThread.joinable())
        gWorkerThread.join();

    Touch::Close();
    graphics->Shutdown();
    android::ANativeWindowCreator::Destroy(::window);
    Logger::SetSink(nullptr);
    return 0;
}
