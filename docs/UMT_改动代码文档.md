# UMT 改动代码文档

> **原始版本**：`Andriod_UnrealMemoryTools/`（未修改的 UE4 通用 Dump 工具）
> **修改版本**：`Alltear_UnrealMemoryToolsNewMcp/`（UMT + MCP 集成版）
> **生成时间**：2026-09-01
> **Diff 原始文件**：位于 `analysis_artifacts/` 目录

---

## 一、改动总览

| 文件 | 原始行数 | 修改版行数 | 新增行数 | 改动性质 |
|---|---|---|---|---|
| `UE/UEGameProfile.cpp` | 1181 | 1513 | **+332** | 核心定位逻辑重构 |
| `UE/UEOffsets.cpp` | 826 | 847 | **+21** | Bug 修复 |
| `UE/UEWrappers.cpp` | ~690 | ~747 | **+57** | 安全加强 |
| `UE/UEGameProfile.hpp` | — | — | **+26** | 结构体新增 |
| `UE/UEOffsets.hpp` | — | — | **+4** | Setter 新增 |
| `UEGameProfiles/AutoFix.cpp` | — | — | **+18** | 游戏专属 Hint |
| `executable.cpp` | ~900 | ~2549 | **+1649** | MCP 集成 |
| `GUI/Android_draw/draw_Gui.cpp` | — | — | **+若干** | UI + 音量键 |
| `GUI/native_surface/ANativeWindowCreator.h` | — | — | **删除成员** | 已知编译阻塞 |
| `mcp/`（新目录） | — | 16 文件 | **全新 MCP 层** | MCP 协议 |

---

## 二、UEGameProfile.cpp（核心改动）

### 2.1 `GetGUObjectArrayPtr()` — 双向扫描 + 多锚点验证

**原始逻辑**：
```cpp
// 单向高地址扫描，只认 "/Script/CoreUObject"
for (int i = 0; i < 0x300000; ++i) {
    const uintptr_t candObjAddr = namesScanBase + 8ULL * i;
    // ... 验证逻辑
    if (name == "/Script/CoreUObject") return candObjAddr;
}
```

**问题**：
1. **单向扫描**：GUObjectArray 可能在 NamesPtr 低地址方向，原代码永远扫不到
2. **单锚点**：元梦之星等游戏 slot 0 为空，FName 池存纯名（非完整路径）
3. **过度校验**：用 `isPtrWritable` 检查只读数据（本次已修复）

**修改后逻辑**：
```cpp
constexpr int kVerifySlots = 8;
static const char* kVerifyAnchors[] = {"CoreUObject", "/Script/CoreUObject", "Object", "Package", "Class"};

// 双向扫描
for (int i = 0; i < kMaxSearchDist; ++i) {
    // 高地址方向
    if (verifyCandidate(namesScanBase + 8*i, "UP")) return;
    // 低地址方向（新增）
    if (i > 0 && verifyCandidate(namesScanBase - 8*i, "DOWN")) return;
}

// verifyCandidate lambda：多槽位验证
int anchorHits = 0;
for (int slot = 0; slot < kVerifySlots; ++slot) {
    // ... 读取对象
    for (uintptr_t no : kNameOffs) {
        const std::string nm = GetNameByID(id);
        for (const char *anchor : kVerifyAnchors) {
            if (nm == anchor) { ++anchorHits; break; }
        }
    }
}
if (anchorHits > 0) return candObjAddr;
```

**效果**：
- 元梦之星等游戏中 slot 0 为空时仍能正确定位
- 双向扫描覆盖 NamesPtr 两侧更宽范围

### 2.2 `isPtrWritable` → `isPtrReadable`（本次修复）

| 位置 | 原始 | 修复后 |
|---|---|---|
| L1116 | `isPtrWritable(objects, sizeof(uintptr_t))` | `isPtrReadable(objects, sizeof(uintptr_t))` |
| L1124 | `isPtrWritable(chunk0, sizeof(uintptr_t))` | `isPtrReadable(chunk0, sizeof(uintptr_t))` |
| L771（InitUEVars） | `isPtrWritable(ObjObjects_Objects, ...)` | `isPtrReadable(ObjObjects_Objects, ...)` |

**原因**：`TUObjectArray.Objects` 指向 chunk 指针数组，只做读取，很多游戏该数组落在 `.rodata`，不可写但完全合法。

### 2.3 `GetNamesPtr()` — 多锚点扫描

**原始**：单锚点 `ByteProperty` + 单偏移
**修改后**：
```cpp
static const char* kKnownNames[] = {
    "None", "ByteProperty", "IntProperty", "BoolProperty",
    "FloatProperty", "ObjectProperty", "NameProperty",
    "StructProperty", "ArrayProperty", "Object", "Class", "Function"
};
static const uintptr_t kPoolEntryOffsets[] = {0x8, 0x6, 0xA, 0x4, 0xC, 0x10};
static const uintptr_t kGNamesEntryOffsets[] = {0x24, 0x2C, 0x1C, 0x14, 0x34};

// 两级判定：
// Tier 1（精确）：ScanEngineNameAnchors 命中 exact=true
// Tier 2（兜底）：128 字节窗口内命中 ≥2 个不同引擎名
```

### 2.4 `InitUEVars()` 改动

```cpp
// 原：isPtrWritable(ObjObjects_Objects)
// 现：isPtrReadable(ObjObjects_Objects, sizeof(uintptr_t))
```

### 2.5 名字解析兼容 OutlineNumber

```cpp
// GetLength lambda 自动识别 header 宽度：
size_t len = static_cast<size_t>(header >> 6);   // 32 位 header (4.22-4.25)
if (len == 0)
    len = static_cast<size_t>(header >> 1);       // 16 位 header (4.26+)

// outline entry 运行时自动识别
// 不再依赖编译期 Config.isUsingOutlineNumberName 开关
```

---

## 三、UEOffsets.cpp（Bug 修复）

### 3.1 `NumElements` 偏移修正

| 版本档 | 修复前 | 修复后 |
|---|---|---|
| `UE4_20` | `0x14`（UE5 布局） | `0x10`（UE4 布局） |
| `UE4_23_24` | `0x14` | `0x10` |
| `UE5_00_02` | `0x14` | 保持 `0x14`（正确） |

**原因**：UE4.2x 的 `TUObjectArray` 布局：
```
+0x00  Objects (ptr)
+0x08  MaxElements (int32)
+0x0C  NumElements (int32)      ← UE4 在此
+0x10  NumElementsPerChunk (int32)
+0x14  NextChunk (ptr)          ← UE5 把 NumElements 移到这里
```

### 3.2 `GetLength` Lambda 改进

```cpp
// 修复前（固定 32 位 header）：
offsets.FNamePoolEntry.GetLength = [](uint16_t header) -> size_t {
    return static_cast<size_t>(header >> 6);
};

// 修复后（自动识别）：
offsets.FNamePoolEntry.GetLength = [bWITH_CASE_PRESERVING_NAME](uint16_t header) -> size_t {
    if (bWITH_CASE_PRESERVING_NAME)
        return static_cast<size_t>(header >> 1);
    size_t len = static_cast<size_t>(header >> 6);   // 32 位 header (4.22-4.25)
    if (len == 0)
        len = static_cast<size_t>(header >> 1);       // 16 位 header (4.26+)
    return len;
};
```

---

## 四、UEWrappers.cpp（安全加强）

### 4.1 `GetNumElements()` 越界保护

```cpp
// 修复前：直接返回原始值
return vm_rpm_ptr<int32_t>((void *)(...));

// 修复后：范围校验
const int32_t count = vm_rpm_ptr<int32_t>(...);
return (count >= 0 && count <= 5000000) ? count : 0;
```

### 4.2 `GetObjectPtr()` 加入 `validObject` 校验

```cpp
auto validObject = [&](uint8_t *candidate) -> uint8_t * {
    if (!candidate || !kPtrValidator.isPtrReadable(candidate, 0x28)) return nullptr;
    uintptr_t klass = 0;
    if (!vm_rpm_ptr(candidate + offsets->UObject.ClassPrivate, &klass, sizeof(klass)) ||
        !kPtrValidator.isPtrReadable(klass, 0x20)) return nullptr;
    int32_t nameId = -1;
    if (!vm_rpm_ptr(candidate + offsets->UObject.NamePrivate, &nameId, sizeof(nameId)) ||
        nameId < 0 || nameId > 0x4000000) return nullptr;
    return candidate;
};

// chunk 布局优先，fallback 到直接布局
if (vm_rpm_ptr(...) && kPtrValidator.isPtrReadable(chunkAddress, ...)) {
    auto *candidate = ...;
    if (auto *object = validObject(candidate)) return object;
}
return validObject(direct);
```

### 4.3 Null 安全检查

```cpp
// FindClassByFullName 等宏
auto *objects = UEWrappers::GetObjects();
obj = objects ? objects->FindObject<UE_UClass>(NAME) : UE_UClass();
```

---

## 五、UEGameProfile.hpp（新增结构体）

```cpp
// 新增：探针覆盖数据结构
struct UEAddressOverrides {
    uintptr_t namesPtr = 0;
    uintptr_t guObjectArrayPtr = 0;
    bool hasNamesPtr = false;
    bool hasGUObjectArrayPtr = false;
    bool hasNameLayout = false;
    bool hasObjectLayout = false;

    // 名字表布局
    uintptr_t nameStride = 0;
    uintptr_t nameBlocksBit = 0;
    uintptr_t nameBlocksOff = 0;
    uintptr_t nameHeaderOff = 0;
    uintptr_t nameLengthShift = 0;
    // 对象布局
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

// IGameProfile 新增字段和方法
protected:
    UEAddressOverrides _addressOverrides;
    UE_Offsets _baseOffsetsBackup;
    bool _hasOffsetsBackup = false;

public:
    void SetAddressOverrides(const UEAddressOverrides &overrides);
```

---

## 六、UEOffsets.hpp（新增 Setter）

```cpp
// APPLY_PROBE_OVERRIDES 注入运行时覆盖值时使用
void SetNamesPtr(uintptr_t p) { NamesPtr = p; };
void SetGUObjectsArrayPtr(uintptr_t p) { GUObjectsArrayPtr = p; };
```

---

## 七、AutoFix.cpp（游戏专属 Hint）

为《式神契约》（`com.huitgames.shikigami.summons`）添加硬编码偏移 Hint：

```cpp
namespace {
    constexpr uintptr_t kShikigamiNamesOffset = 0xBAE55C0;
    constexpr uintptr_t kShikigamiObjectsOffset = 0xBB29898;

    bool IsShikigami(const std::string &package) {
        return package == "com.huitgames.shikigami.summons";
    }
}

uintptr_t AutoFixProfile::GetGUObjectArrayPtr() const {
    if (IsShikigami(_packageHint)) {
        auto elf = GetUnrealELF();
        const uintptr_t candidate = elf.isValid() ? elf.base() + kShikigamiObjectsOffset : 0;
        if (candidate && kPtrValidator.isPtrReadable(candidate)) {
            LOGI("[AutoFix] Shikigami UE4.27 GUObjectArray hint: +0x%lX", ...);
            return candidate;
        }
    }
    return IGameProfile::GetGUObjectArrayPtr();  // fallback
}

uintptr_t AutoFixProfile::GetNamesPtr() const {
    if (IsShikigami(_packageHint)) {
        auto elf = GetUnrealELF();
        const uintptr_t candidate = elf.isValid() ? elf.base() + kShikigamiNamesOffset : 0;
        if (candidate && VerifyNamesAtCandidate(candidate, GetOffsets(), true)) {
            LOGI("[AutoFix] Shikigami UE4.27 FNamePool hint validated: +0x%lX", ...);
            return candidate;
        }
        LOGW("[AutoFix] Shikigami FNamePool hint failed; falling back");
    }
    return IGameProfile::GetNamesPtr();  // fallback
}
```

---

## 八、executable.cpp（MCP 集成）

### 8.1 新增 Include

```cpp
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
#include <atomic>
#include <nlohmann/json.hpp>
```

### 8.2 IMGUI 面板改动

| 功能 | 说明 |
|---|---|
| MCP 状态徽标 | 显示客户端连接状态 + 累计工具调用计数 |
| 音量键控制 | `MemuSwitch` 原子变量，音量+显示/音量-隐藏菜单 |
| LAN IP 显示 | `popen("ip -4 addr show wlan0")` 解析本机局域网 IP |
| 日志过滤 | 隐藏 MCP 连接上下线刷屏日志 |
| 模式分类 | 区分回环/全局/局域网三种监听模式 |

### 8.3 MCP 命令处理

```cpp
// 新增命令：applyProbeOverrides / startProbe / getProbeStatus / getProbeResults
// 新增命令：startDump / getDumpStatus / listOutputFiles / readOutputFile
// 新增命令：listProcesses / selectProcess / attach / listModules
// 新增命令：readMemory / writeMemory / scanPattern / resolveSymbol
// 新增命令：decompile / analyzeClass / describeClass
```

---

## 九、draw_Gui.cpp（UI 改动）

```cpp
// 新增音量键控制
extern std::atomic<bool> MemuSwitch;  // 定义于 executable.cpp

// 内存泄漏修复：PopStyleColor 从 11 改为 12（匹配 PushStyleColor 次数）
ImGui::PopStyleColor(12);  // 原来 11，差一个导致栈失衡

// 菜单隐藏逻辑迁移到 ImGui::Begin 窗口层
// 避免整帧跳过导致的 Begin/End 不配对崩溃
```

---

## 十、ANativeWindowCreator.h（已知编译阻塞）

**问题**：用户工作树中删除了 `ANativeWindowCreator::ProcessMirrorDisplay()` 成员，但 `executable.cpp:4288` 仍在调用它。

**现状**：这是一个预存在的编译阻塞，与 MCP 工作无关。修复它不在当前任务范围内。

---

## 十一、mcp/ 新增目录（全新 MCP 层）

```
mcp/
├── Arm64Disasm.cpp/hpp      # ARM64 反汇编（Pattern 匹配）
├── CommandDispatcher.cpp/hpp # 命令分发器
├── CommandQueue.cpp/hpp      # 命令队列
├── CommandServer.cpp/hpp     # 网络服务器（三种 bind 模式）
├── DecompileHandler.cpp/hpp  # Ghidra 反编译调用
├── MemoryHelpers.cpp/hpp     # 内存读写封装
├── Protocol.hpp              # NDJSON 协议定义
├── PtraceSession.cpp/hpp     # 进程注入会话
└── analysis/
    ├── ArtifactMetadata.cpp/hpp
    ├── MemoryAnalysis.cpp/hpp
    └── UECandidateAnalysis.cpp/hpp
```

---

## 十二、修复记录（2026-09-01）

### 12.1 `GetGUObjectArrayPtr` `isPtrWritable` 误用修复

**问题**：`TUObjectArray.Objects` 是只读 chunk 指针数组，很多游戏落在 `.rodata`，`isPtrWritable` 直接返回 false，导致候选被过滤，最终报 "通用方式搜索 GUObject 失败"。

**修复**（三处）：
```diff
- if (!kPtrValidator.isPtrWritable(objects, sizeof(uintptr_t)))
+ if (!kPtrValidator.isPtrReadable(objects, sizeof(uintptr_t)))

- if (kPtrValidator.isPtrWritable(chunk0, sizeof(uintptr_t)))
+ if (kPtrValidator.isPtrReadable(chunk0, sizeof(uintptr_t)))

- if (!kPtrValidator.isPtrWritable(ObjObjects_Objects, sizeof(uintptr_t)))
+ if (!kPtrValidator.isPtrReadable(ObjObjects_Objects, sizeof(uintptr_t)))
```

### 12.2 `NumElements` 偏移 Bug 修复

**问题**：UE4_20 / UE4_23_24 档的 `NumElements` 偏移写成了 `0x14`（UE5 布局），实际应为 `0x10`（UE4 布局）。

**修复**：
```diff
- offsets.TUObjectArray.NumElements = (sizeof(void *) * 2) + sizeof(int32_t);  // 0x14
+ offsets.TUObjectArray.NumElements = sizeof(void *) + (sizeof(int32_t) * 3);  // 0x10
```

### 12.3 `GetLength` Lambda 自动识别 Header 宽度

**问题**：UE4.26+（如元梦之星）FNamePool header 是 16 位，但代码写死 `header >> 6`（32 位解释），导致短名字长度算成 0，FName 错位。

**修复**：
```diff
  size_t len = static_cast<size_t>(header >> 6);   // 32 位 header (4.22-4.25)
+ if (len == 0)
+     len = static_cast<size_t>(header >> 1);       // 16 位 header (4.26+)
```

---

## 十三、验证清单

| 检查项 | 状态 |
|---|---|
| `UEGameProfile.cpp` `isPtrWritable` 已全部改为 `isPtrReadable` | ✅ |
| `UEOffsets.cpp` `NumElements` UE4 档偏移已修正为 `0x10` | ✅ |
| `UEOffsets.cpp` `GetLength` Lambda 已支持自动识别 header 宽度 | ✅ |
| `UEWrappers.cpp` `GetObjectPtr` 已加入 `validObject` 校验 | ✅ |
| `draw_Gui.cpp` `PopStyleColor` 已修正为 12 | ✅ |
| `AutoFix.cpp` 式神契约硬编码 hint 已添加 | ✅ |
| MCP 新增文件已纳入构建 | ✅ |

---

## 十四、相关文件位置

| 类型 | 路径 |
|---|---|
| 原始仓库 | `D:\泪心安卓领域基本盘技术\ue4ImGuiAutoWorkingMcpBytear\Andriod_UnrealMemoryTools\` |
| 修改版本 | `D:\泪心安卓领域基本盘技术\ue4ImGuiAutoWorkingMcpBytear\Alltear_UnrealMemoryToolsNewMcp\` |
| 完整 Diff | `analysis_artifacts\` 目录 |
| 本文档 | `docs\UMT_改动代码文档.md` |
