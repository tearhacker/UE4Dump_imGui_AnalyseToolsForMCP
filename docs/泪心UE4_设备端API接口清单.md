# UMT 设备端 API 接口清单

> **本文档是全栈类比里的「后端 API 文档」**：源码有什么能力，对应什么函数，签名是什么，作用是什么。
> 开发 MCP 时照此实现 Service 层（`src/mcp/Commands/*.cpp`）与路由（命令分发器）。
>
> 与另两份文档的分工：
> - **本文档**：源码**有什么**（全量，不筛选）
> - 《MCP工具功能规格》：最终**暴露给 AI 哪些**（43 个，含 ATTACH/DISASSEMBLE）
> - 《开发必读架构》：**怎么写**（命名、协议、目录结构）

**现状标记**

| 标记 | 含义 | 开发动作 |
|---|---|---|
| ✅ 可直接调 | public、非匿名 namespace | 直接包装成命令 handler |
| 🔧 需外迁 | 已实现但锁在匿名 namespace | 移出并导出（约 200–300 行总量） |
| ⚠️ 需新建 | 源码没有 | 从零写 |

---

## 分层总览

```
L7 远程调用层   ptrace 在目标进程内执行函数      （高危，需护栏）
L6 查询服务层   SDK Explorer 的检索/容器/运行时视图
L5 批处理层     探测 / 转储 / 产出文件
L4 分析服务层   AutoFix 结构推断（本项目最硬的一块）
L3 领域模型层   UEWrappers —— UE 对象导航
L2 配置服务层   UEGameProfile / AutoFixProfile —— 引擎定位与偏移表
L1 原语层       UEMemory / KittyMemoryEx —— 读写、扫描、ELF
L0 会话层       进程枚举、目标选择、探针复用失效
```

---

## L0 会话层

| 接口 | 签名 | 作用 | 现状 |
|---|---|---|---|
| 进程枚举 | `std::vector<AutoProcessCandidate> FindAutoProcessCandidates()` | 枚举可分析的 UE 进程。两条来源：profile 的 `GetAppIDs()` 匹配 + 扫 `/proc` 判 maps 含 `libUE4.so`/`libUnreal.so` | 🔧 外迁（`executable.cpp:166-216`） |
| 候选结构 | `struct AutoProcessCandidate { pid_t pid = 0; std::string package; std::string profileName; bool dedicated = false; }` | 返回元素（带默认值初始化） | ✅（`executable.cpp:64-70`） |
| 刷新候选 | `RefreshCandidates()` | 重新枚举 | 🔧 外迁（`executable.cpp:345`） |
| 探针失效 | `InvalidateProbeReuse()` | 切换进程时作废旧探针结果，做跨进程隔离 | 🔧 外迁（`executable.cpp:282-300`） |

**8 个内置 profile**（类名，`executable.cpp:38-47`）：`ArenaBreakoutProfile` / `DeltaForceProfile` / `FarlightProfile` / `ShuishaProfile` / `ValorantProfile` / `NRCProfile` / `PUBGMHDProfile` / `PUBGProfile`
（⚠️ 中文对照未经源码核实：`NRCProfile` 疑为 NightCrows 夜鸦，早期文档所称"洛克王国:世界"**存疑**，以类名为准）

---

## L1 原语层 —— `src/UE/UEMemory.hpp`

### 内存读写

| 接口 | 签名 | 作用 |
|---|---|---|
| 读原始内存 | `bool vm_rpm_ptr(const void *address, void *result, size_t len)` | 读目标进程内存，失败返回 false |
| 读并转型 | `template<typename T> T vm_rpm_ptr(const void *address)` | 按类型读取并转换 |
| 读字符串 | `std::string vm_rpm_str(const void *address, size_t max_len = 1024)` | 读窄字符串 |
| 读宽字符串 | `std::wstring vm_rpm_strw(const void *address, size_t max_len = 1024)` | 读 UTF-16，**返回 `std::wstring` 不是 string**（FName 可能是宽字符） |

### 指针搜索与解码

| 接口 | 签名 | 作用 |
|---|---|---|
| 对齐指针反向引用 | `uintptr_t FindAlignedPointerRefrence(uintptr_t start, size_t range, uintptr_t ptr)` | 在范围里找"谁指向 ptr"，用于反查全局指针 |
| ADRP+ADR 解码 | `Arm64::DecodeADRL(uintptr_t adrp_address, uint32_t imm_insn_offset = 0)` | 解 ADRP/ADR 得绝对地址 |
| ADRP+ADD 解码 | `Arm64::Decode_ADRP_ADD(uintptr_t adrp_address, uint32_t add_offset = 4)` | 解 ADRP+ADD |
| ADRP+LDR 解码 | `Arm64::Decode_ADRP_LDR(uintptr_t adrp_address, uint32_t ldr_offset = 4)` | 解 ADRP+LDR |

### 文件工具

`get_filename` / `get_file_directory` / `get_file_extension` / `file_path_contains` / `remove_specials` / `replace_specials` / `delete_directory` / `path_is_directory` / `mkdir_recursive`

### KittyMemoryEx 依赖

| 能力 | 位置 | 备注 |
|---|---|---|
| ELF 扫描、读写、转储 | `KittyMemoryMgr.hpp` | ✅ 在用 |
| IDA 风 pattern 扫描 | `KittyScanner.hpp` `findIdaPatternAll/First` | ✅ 在用 |
| 指针可读性校验 | `KittyPtrValidator.hpp` | ✅ 在用（写内存前必校验） |
| **符号查找** | `KittyScanner.hpp:211` `ElfScanner::findSymbol` | 🔧 **src/ 零调用** |
| **ptrace 远程调用** | `KittyTrace.cpp:140` `callFunctionFrom` | 🔧 **src/ 零引用** |
| 内存备份/补丁 | `MemoryBackup.hpp` / `MemoryPatch.hpp` | ✅ 在用 |

---

## L2 配置服务层 —— 引擎定位

### `IGameProfile`（`src/UE/UEGameProfile.hpp`）

| 接口 | 签名 | 作用 | 现状 |
|---|---|---|---|
| 初始化引擎变量 | `UEVarsInitStatus InitUEVars()` | 主初始化流程：架构校验 → ELF → 偏移表 → Names → GUObjectArray → Bootstrap | ✅ |
| 取引擎变量 | `const UEVars *GetUEVars()` | 拿到全部定位结果 | ✅ |
| 取 UE ELF | `ElfScanner GetUnrealELF()` | 三级查找：maps → apk 内 zip（split config）→ linker solist | ✅ |
| 名字表定位 | `uintptr_t GetNamesPtr()` | 多锚点扫描 GNames / FNamePool | ✅（硬编码参数待参数化） |
| 对象数组定位 | `uintptr_t GetGUObjectArrayPtr()` | 扫描 GUObjectArray | ✅ **双向扫描已修**（原只往高地址单向扫，现已同时扫描低地址方向） |
| 名字条目 | `uint8_t *GetNameEntry(int32_t id)` | 按 id 取 FNameEntry 地址 | ✅ |
| 名字字符串 | `std::string GetNameEntryString(uint8_t *entry)` | 解出名字（可被覆写做解密） | ✅ |
| 按 id 取名 | `std::string GetNameByID(int32_t id)` | 组合上面两步 | ✅ |
| pattern 查找 | `uintptr_t findIdaPattern(PATTERN_MAP_TYPE, const std::string &pattern, int step, uint32_t skip_result = 0)` | IDA 风 pattern 搜索 | ✅ |
| 模拟器判定 | `bool isEmulator()` | 判断目标是否在模拟器 | ✅ **已修**：segments() 为空时返回 false（原直接 return true，真机误判模拟器） |

**纯虚（各 profile 必须实现）**：`ArchSupprted` / `GetAppName` / `GetAppIDs` / `isUsingCasePreservingName` / `IsUsingFNamePool` / `isUsingOutlineNumberName` / `GetOffsets`
**UMT 独有纯虚**（上游没有）：`GetMatrix` / `GetPhysx` / `GetFrameCount`

### `AutoFixProfile`（`src/UE/UEGameProfiles/AutoFix.cpp`）

| 接口 | 作用 | 现状 |
|---|---|---|
| `DetectVersion()`（`:140`，签名 `void AutoFixProfile::DetectVersion() const`） | 扫 rodata 的 `++UE4+Release-X.YY` 判版本 → 套 9 档 `UE_DefaultOffsets`（namespace 位于 `UEOffsets.hpp:214-250`，共 9 个函数：`UE4_00_17` / `UE4_18_19` / `UE4_20` / `UE4_21` / `UE4_22` / `UE4_23_24` / `UE4_25_27` / `UE5_00_02` / `UE5_03`，均带 `bool bWITH_CASE_PRESERVING_NAME` 参数，UE5 两档另带 `bFNAME_OUTLINE_NUMBER`） | ✅ |
| 通用搜索 | 搜 GNames / FNamePool / GUObjectArray；Matrix / Physx / FrameCount **恒为 0（不支持）** | ✅ |

---

## L3 领域模型层 —— `UEWrappers`（`src/UE/UEWrappers.hpp`）

> 这是**对象导航**的全部 API，AI 分析 SDK 结构主要靠这一层。

### 全局入口（namespace `UEWrappers`）

| 接口 | 签名 | 作用 |
|---|---|---|
| 初始化 | `void Init(const UEVars *vars)` | 注入定位结果 |
| 取对象数组 | `UE_UObjectArray *GetObjects()` | **一切对象导航的起点** |
| 取偏移表 | `UE_Offsets *GetOffsets()` | |
| 按 id 取名 | `std::string GetNameByID(int32_t id)` | |
| 取基址 | `uintptr_t GetBaseAddress()` | |
| 取初始化代数 | `uint32_t GetInitGeneration()` | 判断对象数组是否被重建 |

### `UE_UObject`（`:227`）

```cpp
EObjectFlags GetFlags() const;
int32_t      GetIndex() const;
UE_UClass    GetClass() const;
UE_UObject   GetOuter() const;
UE_UObject   GetPackageObject() const;
std::string  GetName() const;         // 短名
std::string  GetFullName() const;     // 全名，如 "Class Engine.World"
std::string  GetCppName() const;      // C++ 名
uint8_t     *GetAddress() const;
bool         IsA(UE_UClass cmp) const;
bool         HasFlags(EObjectFlags flags) const;
```

### `UE_UObjectArray`（`:265`）—— **检索主力**

```cpp
int32_t   GetNumElements() const;                    // 对象总数
uint8_t  *GetObjectPtr(int32_t id) const;            // 按下标取对象
void      ForEachObject(const std::function<bool(UE_UObject)> &callback) const;
void      ForEachObjectOfClass(const UE_UClass &cmp, const std::function<bool(UE_UObject)> &callback) const;
bool      IsObject(const UE_UObject &address) const;

template<typename T> T FindObject(const std::string &fullName) const;                    // :284 全名查找
template<typename T> T FindObjectFast(const std::string &name) const;                    // :298 短名查找
template<typename T> T FindObjectFastInOuter(const std::string &name, const std::string &outer); // :312 限定 Outer
```

### `UE_UField`（`:339`）→ `UE_UStruct`（`:455`）

```cpp
// UField
UE_UField GetNext() const;

// UStruct
UE_UStruct  GetSuper() const;
UE_FField   GetChildProperties() const;
UE_UField   GetChildren() const;
int32_t     GetSize() const;
int32_t     GetMinAlignment() const;
UE_FField   FindChildProp(const std::string &name) const;
UE_UField   FindChild(const std::string &name) const;
```

### `UE_UClass`（`:531`）

```cpp
UE_UObject GetClassDefaultObject() const;      // CDO
uintptr_t  GetImplementedInterfacesPtr() const;
uint64_t   GetCastFlags() const;
uint32_t   GetClassFlags() const;
```

### `UE_UFunction`（`:509`）

```cpp
uintptr_t   GetFunc() const;              // 函数地址
int8_t      GetNumParams() const;
int16_t     GetParamSize() const;
uint32_t    GetFunctionEFlags() const;
std::string GetFunctionFlags() const;     // 可读的 flags 串
```

### `UE_UScriptStruct`（`:523`）/ `UE_UEnum`（`:542`）

```cpp
uint32_t       GetStructFlags() const;    // UScriptStruct
TArray<uint8_t> GetNames() const;         // UEnum，TArray<TPair<FName,int64>>
std::string    GetName() const;           // UEnum
```

### 属性层

```cpp
// IProperty / IUProperty（:411/:427）
std::string   GetName() const;
int32_t       GetArrayDim() const;
int32_t       GetSize() const;
int32_t       GetOffset() const;
uint64_t      GetPropertyFlags() const;
UEPropTypeInfo GetType() const;
uint8_t       GetFieldMask() const;

// UE_FField（:815）
UE_FField   GetNext() const;
uintptr_t   GetOwnerPtr() const;
std::string GetName() const;
UE_FFieldClass GetClass() const;

// UE_FFieldClass（:790）
std::string    GetName() const;
UE_FName       GetFName() const;
UE_FFieldClass GetSuperClass() const;
uint64_t       GetCastFlags() const;

// UE_FProperty（:854）
int32_t  GetArrayDim() const;
int32_t  GetSize() const;
int32_t  GetOffset() const;
uint64_t GetPropertyFlags() const;
UEPropTypeInfo GetType() const;
uintptr_t FindSubFPropertyBaseOffset() const;
```

**属性子类型（每个都能拿到指向的目标类型）**

| 类 | 接口 | 作用 |
|---|---|---|
| `UE_FStructProperty` | `UE_UStruct GetStruct()` | 结构体属性的内层结构 |
| `UE_FObjectPropertyBase` | `UE_UClass GetPropertyClass()` | 对象属性的类 |
| `UE_FArrayProperty` | `UE_FProperty GetInner()` | 数组元素类型 |
| `UE_FByteProperty` | `UE_UEnum GetEnum()` | 字节属性绑定的枚举 |
| `UE_FBoolProperty` | （含位域信息） | 位域 bool |
| `UE_FEnumProperty` | `GetUnderlayingProp()` / `GetEnum()` | 枚举属性 |
| `UE_FSetProperty` | `GetElementProp()` | 集合元素 |
| `UE_FMapProperty` | `GetKeyProp()` / `GetValueProp()` | 映射键值 |
| `UE_FClassProperty` | `GetMetaClass()` | 类属性的元类 |
| `UE_FInterfaceProperty` | `GetInterfaceClass()` | 接口属性 |
| `UE_FDelegateProperty` | `GetSignatureFunction()` | 委托签名函数 |

### 容器与字符串

```cpp
template<class T> class TArray {     // :26
    T *Data; int32_t NumElements; int32_t MaxElements;
    T &operator[](int i);
    bool IsValid() const; bool IsValidIndex(int i) const;
    int Num() const; int Max() const; int Slack() const;
    T *GetData() const;
};
class FString : public TArray<wchar_t> { std::string ToString() const; };  // :88
template<typename K, typename V> class TPair { K &Key(); V &Value(); };    // :105
class UE_FName { int GetNumber() const; std::string GetName() const; };    // :121
```

---

## L4 分析服务层 —— AutoFix（**本项目最硬的一块**）

### 主入口

| 接口 | 签名 | 作用 | 现状 |
|---|---|---|---|
| 一键修补全表 | `bool AutoFix::RunFixup(IGameProfile *profile)` | 基于真实内存布局校验并修补 `UE_Offsets`。**必须在 `InitUEVars()` 成功之后调用**。任一锚点找不到则保留版本预设值并继续（安全降级） | ✅（`AutoFixOffsets.hpp:13`） |

**能自动推断的偏移全表**（编排在 `AutoFixOffsets.cpp:3131-3167`）：

| 类别 | 字段 |
|---|---|
| UObject | `NamePrivate` / `ObjectFlags` / `InternalIndex` / `ClassPrivate` / `OuterPrivate` |
| FName | `Size` / `ComparisonIndex` / `Number` / `CasePreserving` / `OutlineNumber` |
| UStruct | `SuperStruct` / `PropertiesSize` / `MinAlignment` / `Children` / `ChildProperties` |
| UField | `Next` |
| UClass | `ClassDefaultObject` / `ImplementedInterfaces` / `CastFlags` / `ClassFlags` |
| FField | `Name` / `ClassPrivate` / `Next` / `Owner` |
| FFieldClass | `Name` / `SuperClass` / `CastFlags` |
| FProperty | `ArrayDim` / `ElementSize` / `PropertyFlags` / `Offset_Internal` / `Size` |
| 其他 | `ULevel.Actors` / `UDataTable.RowMap` / `UEnum.Names` / `UFunction.Func` |

### 可独立调用的子能力

#### `AutoFixStructLayout`（`StructLayout.hpp`）

```cpp
struct StructLayoutInfo {
    int32_t LastMemberEnd; int32_t Size; int32_t AlignedSize; int32_t Alignment;
    bool UseExplicitAlignment; bool HasReusedTrailingPadding; bool IsFinal;
};
struct FieldLayoutInfo {
    UE_FProperty Field; int32_t Offset; int32_t ElementSize;
    int32_t ArrayDim; int32_t TotalSize;
    bool IsBitfield; uint8_t BitIndex; uint8_t FieldMask;
};

const StructLayoutInfo &        GetStructLayoutInfo(const UE_UStruct &strct);
std::vector<FieldLayoutInfo>    GetSortedFields(const UE_UStruct &strct);
int32_t                         GetOwnMemberStart(const UE_UStruct &strct);
int32_t                         GetDisplayStructSize(const UE_UStruct &strct);
void                            Warmup();
void                            Invalidate();
```

**作用**：推断结构体 Size / Alignment、父类对齐修正、**位域 bool 的 bit index**、字段排序输出（含 ArrayDim / 元素大小 / 总大小）。

#### `AutoFixPropertyOffsets`（`PropertyOffsetFinder.cpp:318-463`）

```cpp
void EnsureResolved();
void Invalidate();
```

**作用**：锚点比对推断 **13 项子属性偏移**：`ObjectProperty.PropertyClass`、`StructProperty.Struct`、`ByteProperty.Enum`、`BoolProperty.Base`、`EnumProperty.UnderlayingProp/Enum`、`DelegateProperty.SignatureFunction`、`ArrayProperty.Inner`、`SetProperty.ElementProp`、`MapProperty.KeyProp/ValueProp`、`ClassProperty.MetaClass`、`InterfaceProperty.InterfaceClass`

#### `AutoFixVTable`（`VirtualFunctionResolver.hpp`）

```cpp
uintptr_t FindVTableCallOffset(uintptr_t functionAddress);      // exec 桩内间接调用的槽位偏移，0=未找到
uintptr_t FindDirectBranchCallTarget(uintptr_t functionAddress); // 第一个不自环的 B/BL 目标
int       OffsetToIndex(uintptr_t off);                          // inline，slot/8
uintptr_t ResolveVTableFunction(uintptr_t objectAddress, uintptr_t vtableOffset);
```

#### `AutoFixNativeFunctions`（`NativeFunctionResolver.hpp`）

```cpp
struct NativeFunctionExportInfo {
    uintptr_t FuncOffset = 0;
    uintptr_t VTableOffset = 0;
    uintptr_t RealOffset = 0;
};
NativeFunctionExportInfo ResolveNativeFunctionInfo(uintptr_t ownerClassAddr, uintptr_t funcPtr, uint32_t funcFlags);
```

**作用**：解析原生函数三地址（含 CDO 扫描与接口类 vtable 解析）。

#### `AutoFixArm64`（`Arm64Decode.hpp`）

```cpp
uint64_t DecodeADRP(uint64_t pc, uint32_t insn);
uint64_t DecodeADD(uint32_t insn);
bool IsADRP / IsADD / IsLDR_Imm / IsBL / IsB / IsBLR / IsBR / IsRET / IsMov_Reg / IsAddImm / IsMovz (uint32_t insn);
bool DecodeLDR_Imm(uint32_t insn, int *outRn, int *outRt, uint32_t *outImm12, uint32_t *outScale);
bool DecodeMovReg(uint32_t insn, int *outRd, int *outRm);
bool DecodeAddImm(uint32_t insn, int *outRd, int *outRn, uint32_t *outImm);
int  GetBLR_Reg(uint32_t insn);
int  GetBR_Reg(uint32_t insn);
int64_t DecodeBranchOffset(uint32_t insn);
```

#### 诊断

| 接口 | 位置 | 作用 |
|---|---|---|
| `DumpAutoFoundOffsets` | `AutoFixOffsets.cpp:504` | 打印 **AutoFix 前后偏移对比**——诊断利器 |

---

## L5 批处理层 —— 探测 / 转储 / 产出

### 可独立调用的底座

```cpp
// src/Dumper.hpp:31/33 —— public，无 UI 依赖
bool UEDumper::Init(IGameProfile *profile, bool reuseInitializedState = false);
void UEDumper::Dump(std::unordered_map<std::string, BufferFmt> *outBuffersMap);
// ⚠️ 审查修正：两个函数实际都返回 bool（Dumper.hpp:31/33），不是 void
```

### 需外迁的编排（`executable.cpp` 匿名 namespace）

| 接口 | 位置 | 作用 | 现状 |
|---|---|---|---|
| `ExecuteProbe(const AutoProcessCandidate)` | `:644-752` | 探测：初始化 KittyMemoryMgr → 专用 profile → 失败回退 AutoFix → 填偏移与结构 | 🔧 |
| `ExecuteDump(const AutoProcessCandidate)` | `:754-857` | 转储：6 阶段写 SDK | 🔧（依赖全局 `gProbeResult` `:141`，需换状态对象） |
| `ExecuteDumpUnrealLib(const AutoProcessCandidate)` | `:859-943` | 转储 `libUE4.so` / `libUnreal.so` | 🔧 |
| `SaveDumpBuffers(...)` | `:388` | 统一写盘 | 🔧 |
| `CollectProbeOffsets()` | `:415` | 汇总 9 项核心指针 | 🔧 |
| `CollectStructGroups()` | `:449` | 汇总 11 组结构字段表（含红/绿识别状态） | 🔧 |

### 产物（`/sdcard/UnrealMemoryTools/<包名>/`，转储前会 `delete_directory`）

| 文件 | 位置 | 内容 |
|---|---|---|
| `Logs.txt` | `Dumper.cpp:142` | 完整运行日志 |
| `Offsets.hpp` | `:165` | 引擎指针 + `UE_Offsets::ToString()` |
| `Objects.txt` | `:171` | 对象清单 |
| `AIOHeader.hpp` | `:184` | 聚合头 |
| `SDK_Enums.hpp` | `:184-193` | 枚举 |
| `SDK_Structs.hpp` | `:184-193` | 结构体 |
| `SDK_Classes.hpp` | `:184-193` | 类（**可达几十 MB**，含虚表注释：VTableIndex / 槽位 / 函数 RVA，Dumper-7 风格成员偏移） |
| `SDK_Offset.hpp` | `:184-193` | 偏移定义 |
| `script.json` | `:200-213` | `Functions:[{Address,Name}]` + 作者声明（可直接喂 IDA/Ghidra 脚本） |
| `libUE4.so` / `libUnreal.so` | `executable.cpp:931`（`kMgr.dumpMemELF`） | 转储的引擎库（可选） |

> 除 `.json` 外，写盘时统一加作者 banner（`BuildDumpFileBanner` @ `executable.cpp:218`，在 `SaveDumpBuffers` `:397` 处逐文件调用）。

> `scripts/ida.py` / `ghidra.py` / `ida_py3.py` 是仓库自带离线脚本（读 `script.json` 批量命名函数），**非运行时产物**。

---

## L6 查询服务层 —— `SDKExplorer`（`src/SDKExplorer.cpp`）

> ⚠️ 全部逻辑在 `:16-19` 的**内层匿名 namespace**，250 处 `ImGui::` 调用，状态全在文件级全局。
> **不可直接复用**，需另起无头 `SDKQuery` 层（约 200–400 行）。
>
> 📌 **行号语义（二次审查澄清）**：下表行号均为**函数定义行号**（`Render*Panel()` 等），
> 已逐条验证。UI 字符串在 `Render()` 主函数的调用点（`:1409-1446` 附近），两者不要混淆。

| 能力 | 函数 | 位置 | 作用 |
|---|---|---|---|
| 对象浏览器 | `RenderObjectBrowser()` | `:316-455` | 分页（PageSize 10–1000）、异步搜索带命中计数与进度、搜索/全部切换 |
| 元数据 tab | `RenderMetadataPanel()` | `:922` | 对象 / 包 / 类 / 父类 / Size / 对齐 / ChildProperties 计数 / CDO / CastFlags / ClassFlags / StructFlags / 函数签名 |
| **容器视图** | `RenderContainerView()` | `:1246` | **Array / Set / Map 的 Num / Max / Data 并逐元素读值**（MaxRows 1–4096）← AI 分析数组结构最需要 |
| **运行时视图** | `RenderRuntimePanel()` | `:1007` | **UWorld 的 PersistentLevel / OwningGameInstance / NetDriver + Actor 预览（`RenderActorPreview()` `:875`）+ ULevel Actor 列表 + UDataTable RowStruct/RowMap** |
| 属性 tab | `RenderPropertiesPanel()` | `:458` | Address / VFTable / ClassPrivate / ObjectFlags / OuterPrivate / NamePrivate / InternalIndex / FullName / CppName |
| 成员视图 | `RenderInspectorPanel()` | `:1106-1243` | 按地址 Inspect、Add Tag 收藏、Back 返回栈、字段表（Type/Name/Offset/Size/Value）、点 ObjectProperty 值**跳转下级对象** |
| 值解析 | `ReadFieldValue()` | `:598` | 按属性类型读出实际值 |
| 函数列表 | `RenderFunctionsList()` | `:523` | Signature / Flags / Num / Size / Func / RVA / Owner，点击跳转 |
| 声明构造 | `:795` `BuildPropertyDecl` / `BuildFunctionSignature`（声明 `:520`、定义 `:807`） | 生成属性/函数的 C++ 声明文本（⚠️ 二次审查修正：`:520` 是声明、`:807` 是定义，**两者都真实存在**，首次审查误判为"实际在 :520"） |
| 字段收集 | `:720` `CollectFields`（**会递归父类**：`:724` `if (super) CollectFields(super, out, depth+1)`）/ `:770` `CountChildProperties` / `:757` `FindFieldRow` / `:120` `MatchNeedle` | 纯逻辑，可复用；`CollectFields` 递归性正好补上 `FindChildProp` 只查当前层的缺口 |
| 虚拟键盘 | `:220` | 自绘输入法（MCP 不需要） |

---

## L7 远程调用层 —— ptrace

| 接口 | 位置 | 作用 | 现状 |
|---|---|---|---|
| `KittyTraceMgr::callFunctionFrom(caller, addr, nargs, ...)` | `KittyTrace.cpp:140` | 在目标进程内调用函数（含 x0–x7 参数、>8 参数压远端栈、LR 陷阱、寄存器恢复） | 🔧 **src/ 零引用** |
| `KittyTraceMgr::callFunction(addr, nargs, args...)` | `KittyTrace.hpp:114-122` | 简化版 | 🔧 |
| 所属成员 | `KittyMemoryMgr.hpp:47` `KittyTraceMgr trace;` | （注意：不叫 `kTraceMgr`） | ✅ |

**风险**：与现有 `/proc/pid/mem` 无注入设计冲突，也更容易被反作弊识别。单机游戏场景可接受。

---

## 已完成的能力（源码里已有）

| 能力 | 说明 |
|---|---|
| ✅ socket 命令服务 | `src/mcp/CommandServer.cpp`：`bind(127.0.0.1, 35515)` + `listen(1)` + `accept`，NDJSON 分帧，Token 鉴权，心跳 2s/超时 120s |
| ✅ 任务取消 | `executable.cpp:156` `gCancelRequested` + `CANCEL_JOB`(:2696)，ExecuteProbe/Dump 有检查点 |
| ✅ 43 条 MCP 命令 | A~I 组全覆盖，内联注册（未外迁） |

## 设计上选择不做（非 bug）

| 能力 | 说明 |
|---|---|
| 参数化扫描 | `GetNamesPtr()` 无参；`GetGUObjectArrayPtr` 双向扫描已修，签名仍无参（约定设计） |
| 批量采样 | 采样点写死 5（`Dumper.cpp:248`/`:267`），不影响 MCP 命令调用 |
| 偏移编辑 UI | 不存在（UI 层，非服务端范围） |
| profile 管理 UI | 不存在（UI 层，非服务端范围） |
| 配置持久化 | `AndroidImgui.cpp:20` `io.IniFilename = nullptr` |
| 命令行参数 | `src/Utils/KittyCmdln.cpp` 框架存在但 `main()` 无参不解析 argv —— **死代码** |

---

## 实现顺序建议

按依赖从下往上，**每层可独立验证**：

```
L0 会话层     外迁 FindAutoProcessCandidates / InvalidateProbeReuse     ~30 行
L1 原语层     大部分已有，只需封装；findSymbol 需接线                    ~150 行
L2 配置层     参数化 GetNamesPtr（`GetGUObjectArrayPtr` 双向扫描已修，仅保留无参现状） ~60 行
L3 对象层     现有 API 直接包装成命令                                   ~200 行
L4 分析层     AutoFix 五个子能力直接包装（现成的，最划算）               ~200 行
L5 批处理层   外迁 3 个 Execute + 换掉 gProbeResult 全局                 ~250 行
L6 查询层     另起无头 SDKQuery 层（容器视图 + 运行时视图优先）          ~400 行
L7 远程调用   KittyTraceMgr 接线 + 四重兜底                             ~200 行
────────────────────────────────────────────────────────
路由          命令服务 + 队列 + 分发（**所有层的硬前置**）               ~700 行
```
