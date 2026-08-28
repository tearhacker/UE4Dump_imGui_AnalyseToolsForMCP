#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <memory>
#include <mutex>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unordered_map>
#include <vector>

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
            PushUiLog('W', "未检测到正在运行的 Unreal Engine 进程。");
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

        if (matchedProfile)
        {
            SetDumpPhase("探测专用 Profile");
            LOGI("识别到专用 Profile: %s", matchedProfile->GetAppName().c_str());
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

    void ExecuteDumpUnrealLib(const AutoProcessCandidate candidate)
    {
        {
            std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
            gDumpUiState.soDumpRunning = true;
            gDumpUiState.soDumpFinished = false;
            gDumpUiState.soDumpSuccess = false;
            gDumpUiState.soDumpPath.clear();
            gDumpUiState.phase = "Dump 动态库准备中";
        }

        auto finish = [&](bool ok, const std::string &path)
        {
            std::lock_guard<std::mutex> lock(gDumpUiState.mutex);
            gDumpUiState.soDumpRunning = false;
            gDumpUiState.soDumpFinished = true;
            gDumpUiState.soDumpSuccess = ok;
            gDumpUiState.soDumpPath = path;
            gDumpUiState.phase = ok ? "动态库 Dump 完成" : "动态库 Dump 失败";
        };

        if (!gProbeResult.valid || !gProbeResult.success || !gProbeResult.profile)
        {
            LOGE("请先完成探针流程，再 Dump 动态库。");
            finish(false, {});
            return;
        }
        if (gProbeResult.package != candidate.package || gProbeResult.pid != candidate.pid)
        {
            LOGE("探针目标 (%s, pid=%d) 与动态库 Dump 目标 (%s, pid=%d) 不一致。",
                 gProbeResult.package.c_str(), gProbeResult.pid,
                 candidate.package.c_str(), candidate.pid);
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

        auto ue_elf = gProbeResult.profile->GetUnrealELF();
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

        if (!kMgr.dumpMemELF(ue_elf, dumpSoPath))
        {
            LOGE("动态库转储失败 (写入 %s 失败)。", dumpSoPath.c_str());
            finish(false, dumpSoPath);
            return;
        }

        const auto t1 = std::chrono::steady_clock::now();
        const std::chrono::duration<float, std::milli> ms = t1 - t0;
        LOGI("动态库转储完成，耗时 %.2fms。", ms.count());
        LOGI("文件位置: %s", dumpSoPath.c_str());
        finish(true, dumpSoPath);
    }

    void StartProbeSelected()
    {
        if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
        {
            PushUiLog('E', "请先选择目标进程。");
            return;
        }
        if (gWorkerThread.joinable())
            gWorkerThread.join();
        gWorkerThread = std::thread(ExecuteProbe, gCandidates[gSelectedIndex]);
    }

    void StartDumpAfterProbe()
    {
        if (!gProbeResult.valid || !gProbeResult.success)
        {
            PushUiLog('E', "请先成功完成探针流程，再开始 Dump。");
            return;
        }
        if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
        {
            PushUiLog('E', "请先选择目标进程。");
            return;
        }
        if (gWorkerThread.joinable())
            gWorkerThread.join();
        gWorkerThread = std::thread(ExecuteDump, gCandidates[gSelectedIndex]);
    }

    void StartDumpUnrealLib()
    {
        if (!gProbeResult.valid || !gProbeResult.success)
        {
            PushUiLog('E', "请先成功完成探针流程，再 Dump 动态库。");
            return;
        }
        if (gSelectedIndex < 0 || gSelectedIndex >= static_cast<int>(gCandidates.size()))
        {
            PushUiLog('E', "请先选择目标进程。");
            return;
        }
        if (gWorkerThread.joinable())
            gWorkerThread.join();
        gWorkerThread = std::thread(ExecuteDumpUnrealLib, gCandidates[gSelectedIndex]);
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
    const float topBarHeight = 92.0f;
    const float leftColumnWidth = 320.0f;
    const float middleColumnWidth = 310.0f;

    const char *pageTitle = Tr("总览面板", "Overview");
    const char *pageSubtitle = Tr("总览面板", "Overview");
    switch (navPage)
    {
        case NavProcess:
            pageTitle = Tr("进程管理", "Process Manager");
            pageSubtitle = Tr("选择当前游戏进程并执行探测或导出", "Select target process and run probe/dump");
            break;
        case NavStructs:
            pageTitle = Tr("结构浏览", "Struct Browser");
            pageSubtitle = Tr("查看 AutoFix 后的结构体字段结果", "Browse AutoFix struct field results");
            break;
        case NavExplorer:
            pageTitle = Tr("SDK 浏览器", "SDK Explorer");
            pageSubtitle = Tr("在单独工作区中查看导出的 SDK 内容", "Inspect SDK content in a focused workspace");
            break;
        case NavLogs:
            pageTitle = Tr("运行日志", "Logs");
            pageSubtitle = Tr("查看实时输出和错误信息", "Inspect runtime output and errors");
            break;
        case NavSettings:
            pageTitle = Tr("设置中心", "Settings");
            pageSubtitle = Tr("语言、主题和项目信息", "Language, theme and project information");
            break;
        default:
            break;
    }

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
    if (drawNavButton(Tr("日志", "Logs"), navPage == NavLogs)) navPage = NavLogs;
    if (drawNavButton(Tr("设置", "Settings"), navPage == NavSettings)) navPage = NavSettings;

    const float sidebarFooter = ImGui::GetWindowHeight() - 96.0f;
    if (ImGui::GetCursorPosY() < sidebarFooter)
        ImGui::SetCursorPosY(sidebarFooter);
    ImGui::Separator();
    ImGui::TextDisabled("%s %s", Tr("版本", "Version"), kUEDUMPER_VERSION);
    ImGui::TextDisabled("%s", Tr("禁止盗卖圈钱", "No reselling for profit"));
    ImGui::EndChild();

    ImGui::SameLine(0.0f, 14.0f);

    ImGui::BeginChild("##workspace", ImVec2(0.0f, 0.0f), false);
    ImGui::BeginChild("##topbar", ImVec2(0.0f, topBarHeight), true);
    ImGui::Text("%s", pageTitle);
    ImGui::TextDisabled("%s", pageSubtitle);
    if (!selectedPackage.empty())
    {
        ImGui::TextDisabled("%s: %s", Tr("当前目标", "Current target"), selectedPackage.c_str());
    }
    else
    {
        ImGui::TextDisabled("%s", Tr("当前未选择进程", "No process selected"));
    }

    const float rightStart = ImGui::GetWindowWidth() - 320.0f;
    if (rightStart > 0.0f)
        ImGui::SameLine(rightStart);
    if (busy)
        drawStatusChip(Tr("运行中", "Busy"), ImVec4(0.85f, 0.55f, 0.16f, 0.95f));
    else
        drawStatusChip(Tr("空闲", "Idle"), ImVec4(0.18f, 0.63f, 0.42f, 0.95f));
    ImGui::SameLine(0.0f, 10.0f);
    if (ImGui::Button("ZH", ImVec2(52.0f, 0.0f))) gUiLang = UiLang::ZH;
    ImGui::SameLine(0.0f, 6.0f);
    if (ImGui::Button("EN", ImVec2(52.0f, 0.0f))) gUiLang = UiLang::EN;
    ImGui::SameLine(0.0f, 6.0f);
    if (ImGui::Button(Tr("退出", "Exit"), ImVec2(78.0f, 0.0f)))
        *main_thread_flag = false;
    ImGui::EndChild();

    ImGui::Dummy(ImVec2(0.0f, 12.0f));

    if (navPage == NavExplorer)
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
        ImGui::Dummy(ImVec2(0.0f, 6.0f));
        if (ImGui::BeginChild("##logs_scroll", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_HorizontalScrollbar))
        {
            for (const auto &line : logLines)
                ImGui::TextUnformatted(line.c_str());

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
        ImGui::Text("%s: 曦曦(DreamFekk)", Tr("创作者", "Author"));
        ImGui::TextWrapped("GitHub: https://github.com/DreamFekk");
        ImGui::TextWrapped("%s: %s", Tr("输出目录", "Output"), kOutputDirectory);
        ImGui::TextWrapped("%s: %s", Tr("渲染接口", "Renderer"), graphics->RenderName);
        ImGui::Text("%s: %.1f", Tr("当前 FPS", "Current FPS"), ImGui::GetIO().Framerate);
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


int main()
{
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);
    setbuf(stdin, nullptr);

    Logger::SetSink(LoggerSink);
    RefreshCandidates();

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
        graphics->EndFrame();
    }

    if (gWorkerThread.joinable())
        gWorkerThread.join();

    Touch::Close();
    graphics->Shutdown();
    android::ANativeWindowCreator::Destroy(::window);
    Logger::SetSink(nullptr);
    return 0;
}
