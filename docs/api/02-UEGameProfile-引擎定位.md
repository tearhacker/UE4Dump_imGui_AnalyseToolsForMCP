# L2 配置服务层 API · UEGameProfile（引擎定位）

> **源码**：`src/UE/UEGameProfile.hpp`（77 行）/ `UEGameProfile.cpp`（1337 行）
> **配套**：`src/UE/UEGameProfiles/`（11 个 profile 文件，含 8 个内置 profile + AutoFix 2 个）
> **定位**：引擎定位层。`locateEngineGlobals` / `selectProcess` / `detectUEVersion` 的直接依赖。
> **已精读范围**：hpp 全文（1–77）；cpp 全文（1–1337），其中匿名 namespace 辅助函数逐行过，profile 子类仅列清单未逐行。
> **依赖的上游**：`UEMemory.hpp`（L1 原语）、`UEOffsets.hpp`（L4 偏移表，含 `UEVars`/`UEVarsInitStatus` 定义）。

---

## 0. 全局约定（读代码前必知）

### 0.1 类层次：`IGameProfile` 是抽象基类

| 类别 | 成员 | 说明 |
|---|---|---|
| **非虚** | `InitUEVars()` / `GetUEVars()` | 唯一入口 + 结果访问，**不可 override** |
| **纯虚（profile 必须实现）** | `ArchSupprted` / `GetAppName` / `GetAppIDs` / `isUsingCasePreservingName` / `IsUsingFNamePool` / `isUsingOutlineNumberName` / `GetOffsets` | 7 个 |
| **UMT 独有纯虚** | `GetMatrix` / `GetPhysx` / `GetFrameCount` | 上游 AndUEDumper 没有，UMT 扩展（hpp `:56-58`） |
| **带默认实现，可 override** | `GetUnrealELF` / `GetNamesPtr` / `GetGUObjectArrayPtr` / `GetNameEntry` / `GetNameEntryString` / `GetNameByID` / `findIdaPattern` / `isEmulator` / `GetStaticFindObject` / `GetNativeAndroidApp` / `GetProcessEvent` | 11 个 |

成员仅一个 `UEVars _UEVars`（protected）。

### 0.2 空值语义：地址类接口统一返回 `0`

`GetNamesPtr` / `GetGUObjectArrayPtr` / `GetStaticFindObject` / `GetNativeAndroidApp` / `GetProcessEvent` / `findIdaPattern` **失败一律返回 `0`**，不抛异常、不返回 -1。`GetNameEntry` 失败返回 `nullptr`，`GetNameEntryString` 失败返回 `""`（**空串，不是 `"None"`**，与 L3 `UE_FName::GetName()` 不同）。

### 0.3 两个进程级单例（非线程安全）

- `kMgr`（`KittyMemoryMgr`，全局单例）：`processID()`、`memScanner`、`findMemElf` 等
- `kPtrValidator`（`KittyPtrValidator`）：读内存前的可读性校验，`InitUEVars` 里 `setPID` 并 `setUseCache(true)`

→ MCP 命令服务必须**串行**调用本层（同 L3 约束）。

### 0.4 游戏专属硬编码（三处，MCP 必须警惕）

| AppId | 特判位置 | 行为 |
|---|---|---|
| `com.tencent.tmgp.dfm`（DeltaForce） | `GetNamesPtr` `:1074` | 走 `BruteForceDeltaForceNames` 解密暴力搜索 |
| `com.tencent.ig`（PUBG Classic） | `GetNamesPtr` `:1177` / `GetNameEntry` `:758` / `GetGUObjectArrayPtr` `:992` | 额外 `+0x110` 多级解引用 |
| （通用） | 无 | 走标准算法 |

---

## 1. `UEVars` 与 `UEVarsInitStatus`（定义在 `UEOffsets.hpp`）

### 1.1 `UEVarsInitStatus` 枚举（`UEOffsets.hpp:252-268`）

```
NONE = 0, SUCCESS,
ERROR_INVALID_ELF, ARCH_NOT_SUPPORTED, ERROR_ARCH_MISMATCH,
ERROR_LIB_INVALID_BASE, ERROR_LIB_NOT_FOUND, ERROR_IO_OPERATION,
ERROR_INIT_GNAMES, ERROR_INIT_NAMEPOOL, ERROR_INIT_GUOBJECTARRAY,
ERROR_INIT_OBJOBJECTS, ERROR_INIT_OFFSETS, ERROR_INIT_PTR_VALIDATOR
```

⚠️ **`ERROR_INVALID_ELF` / `ERROR_LIB_INVALID_BASE` / `ERROR_IO_OPERATION` 三个在 `InitUEVars` 里并未使用**（可能历史遗留或他处用），`InitUEVars` 实际返回的是其余 11 个。MCP 侧诊断时别假设所有枚举都会出现。

**关键诊断接口**：`static std::string UEVars::InitStatusToStr(UEVarsInitStatus s)`（`UEOffsets.hpp:314`）——把枚举转成可读字符串，`getProbeStatus.lastError` / 探测失败日志都靠它。真机日志里的 `ERROR_INIT_GUOBJECTARRAY` 就是这个枚举的字符串化。

### 1.2 `UEVars` 结构（`UEOffsets.hpp:270-315`）

`InitUEVars` 的产物，13 个字段 + 13 个 getter：

| 字段 | getter | 含义 |
|---|---|---|
| `BaseAddress` | `GetBaseAddress()` | UE ELF 基址 |
| `NamesPtr` | `GetNamesPtr()` | GNames / FNamePool 定位结果 |
| `GUObjectsArrayPtr` | `GetGUObjectsArrayPtr()` | GUObjectArray 定位结果 |
| `ObjObjectsPtr` | `GetObjObjectsPtr()` | `= GUObjectsArrayPtr + ObjObjects` |
| `ObjObjects_Objects` | `GetObjObjects_Objects()` | 解引用后的实际对象数组 |
| `Matrix` / `Physx` / `FrameCount` | 同名 getter | UMT 扩展，**profile 恒返回 0（不支持）** |
| `StaticFindObject` / `NativeAndroidApp` / `ProcessEvent` | 同名 getter | 函数定位（见 §7） |
| `Offsets` | `GetOffsets()` | 指向当前偏移表 |
| `pGetNameByID` | （无 getter，`GetNameByID(id)` 方法） | 名字解析回调 |

⚠️ **`Matrix/Physx/FrameCount` 恒为 0**：这三个纯虚在 AutoFix profile 里直接 `return 0`（上游 AndUEDumper 的功能，UMT 未实现）。MCP 侧若看到 0，**不是 bug，是"该功能不支持"**。

---

## 2. `InitUEVars()`（`cpp:638-739`）—— 主入口

**签名**：`UEVarsInitStatus InitUEVars()`（hpp `:34`，非虚）

**完整流程与每步失败返回**（这是 `startProbe` 的核心，MCP 的 `getProbeStatus.phase` 应该对齐这些阶段）：

| 步 | 动作 | 失败返回 | 行号 |
|---|---|---|---|
| 1 | `_UEVars = UEVars{}` 清零 | — | `:640` |
| 2 | 判 32/64 位：`getMapsEndWith(pid,"/linker64").empty()` | `ERROR_ARCH_MISMATCH` | `:642-658` |
| 3 | `GetUnrealELF()` 三级查找 | `ERROR_LIB_NOT_FOUND` | `:660-665` |
| 4 | `ArchSupprted()`；不支持且 header 可读 | `ARCH_NOT_SUPPORTED`（headerless 只 WARN 不返回） | `:667-678` |
| 5 | `kPtrValidator.setPID + setUseCache(true) + refreshRegionCache()` | `ERROR_INIT_PTR_VALIDATOR`（regions 空） | `:680-684` |
| 6 | `_UEVars.BaseAddress = ue_elf.base()` | — | `:686` |
| 7 | `GetOffsets()` | `ERROR_INIT_OFFSETS` | `:688-690` |
| 8 | `NamesPtr = GetNamesPtr()` + 可读性校验 | FNamePool 模式 `ERROR_INIT_NAMEPOOL`；否则 `ERROR_INIT_GNAMES` | `:694-704` |
| 9 | 设 `pGetNameByID` lambda | — | `:706-709` |
| 10 | `GUObjectsArrayPtr = GetGUObjectArrayPtr()` + 校验 | `ERROR_INIT_GUOBJECTARRAY` | `:711-713` |
| 11 | `BootstrapCoreObjectArrayOffsets(...)` 修正偏移表 | （不返回，失败降级保留预设） | `:715` |
| 12 | `ObjObjectsPtr = GUObjectsArrayPtr + ObjObjects` | — | `:717` |
| 13 | 读 `ObjObjects_Objects` + 校验 | `ERROR_INIT_OBJOBJECTS` | `:719-723` |
| 14 | `GetMatrix/GetPhysx/GetFrameCount/GetStaticFindObject/GetNativeAndroidApp` | （不校验） | `:730-734` |
| 15 | `UEWrappers::Init(GetUEVars())` | — | `:735` |
| 16 | `ProcessEvent = GetProcessEvent()` | （不校验） | `:736` |

**返回 `SUCCESS`**（`:738`）。

### 关键陷阱

1. **第 8 步的 NamesPtr 校验**：`IsUsingFNamePool()` 决定错误码是 `ERROR_INIT_NAMEPOOL` 还是 `ERROR_INIT_GNAMES`。**这个分支本身就暴露了当前 profile 用的是哪种名字表**——MCP 侧可据此推断，不必另查。
2. **第 11 步有副作用**：`BootstrapCoreObjectArrayOffsets` 会**改写 `GetOffsets()` 返回的偏移表**（修正 `ObjObjects`/`Objects`/`NumElements`/`NumElementsPerChunk`）。所以 `InitUEVars` 成功后偏移表 ≠ 版本预设值。MCP 的 `getProbeResults` 要如实报告"被 AutoFix 改过的值"。
3. **第 14/16 步不校验结果**：`StaticFindObject`/`NativeAndroidApp`/`ProcessEvent` 定位失败也**不影响 SUCCESS**——它们失败只是相应字段为 0。
4. **真机日志对照**：用户上次真机失败日志 `E: [Bootstrap] 通用方式搜索 GUObject 失败` → 对应第 10 步 `ERROR_INIT_GUOBJECTARRAY`，即 `GetGUObjectArrayPtr()` 返回 0（§5 的单向扫描 bug 是直接原因）。

---

## 3. `GetUnrealELF()`（`cpp:867-909`）—— 三级查找

**签名**：`virtual ElfScanner GetUnrealELF() const`（hpp `:37`）

**三级策略（按顺序，命中即返回）**：

| 级 | 方法 | 适用 | 行号 |
|---|---|---|---|
| 1 | `kMgr.findMemElf("libUE4.so"/"libUnreal.so")` | 常规 | `:873-881` |
| 2 | split config：扫 maps 找含进程名的 `.apk` → `findMemElfInZip` | 拆分包 / apk 内 so | `:883-897` |
| 3 | `kMgr.findMemElfFromLinker("libUE4.so"/"libUnreal.so")` | farlight/pubg 去 ELF header | `:899-906` |

**失败返回**：`ElfScanner{}`（空对象，`isValid()` 为 false）。

**陷阱**：三级查找都失败时**不打印错误**，`InitUEVars` 侧才统一报 `ERROR_LIB_NOT_FOUND`。MCP 侧调试时若 `selectProcess` 报 lib 未找到，要意识到是三级全失败，而不是"没这个 so"。

---

## 4. `GetNamesPtr()`（`cpp:1061-1239`）—— 多锚点扫描（任务1 改造点）

**签名**：`virtual uintptr_t GetNamesPtr() const`（hpp `:63`）

### 4.1 多锚点判定（匿名 namespace，`ScanEngineNameAnchors` `:115-156`）

**核心思想**：一次远程读 128 字节窗口，本地跑两级判定，替换原"单锚点 `ByteProperty` + 单偏移"。

- **锚点名表** `kKnownNames`（`:68-72`）：12 个引擎内建名
  `None / ByteProperty / IntProperty / BoolProperty / FloatProperty / ObjectProperty / NameProperty / StructProperty / ArrayProperty / Object / Class / Function`
- **FNamePool 条目头候选偏移** `kPoolEntryOffsets`（`:82`）：`{0x8, 0x6, 0xA, 0x4, 0xC, 0x10}`
- **GNames 二级解引用后条目头偏移** `kGNamesEntryOffsets`（`:87`）：`{0x24, 0x2C, 0x1C, 0x14, 0x34}`
- **两级判定**：
  - **Tier 1（精确）**：候选偏移处 `MatchEngineName` 命中 → 立即返回，`exact=true`
  - **Tier 2（窗口兜底）**：128 字节窗口内统计**不同**引擎名数量 `hits >= kMinAnchorHits(2)` → `hit=true` 但 `exact=false`

### 4.2 扫描主体（`preferFNamePool` 决定先试哪个）

```
遍历 UE ELF 可读段，按 kPtrSize 步进读指针
  val ∈ [0x4FFFFFFFFF, 0x7FFFFFFFFF] 才继续
    candidate = 段基址 + base + i   （指向 namepool 的「指针的地址」）
    tryFNamePool：ScanEngineNameAnchors(val, kPoolEntryOffsets)
      └ 命中且 BlocksOff 非零且 candidate >= BlocksOff → poolBase = candidate - BlocksOff
    tryGNames：val 二级解引用后 ScanEngineNameAnchors(p1, kGNamesEntryOffsets)
    PUBG 特判：val+0x110 四级解引用再判一次
  preferFNamePool ? 先 Pool 后 GNames : 先 GNames 后 Pool
```

### 4.3 宽松命中兜底（关键设计，避免"弱结果盖掉精确结果"）

```cpp
uintptr_t weakPool = 0, weakGNames = 0;   // :1095-1096
// Tier2 窗口命中（exact=false）不立即返回，记到 weak* 继续扫
// 整轮扫完都没有精确命中 → 才用 weakPool/weakGNames（:1226-1235）
// 仍无 → LOGE 后返回 0（:1237-1238）
```

### 4.4 DeltaForce 特判（`:1074-1080`）

`com.tencent.tmgp.dfm` → 先 `BruteForceDeltaForceNames`（§8.2 的解密暴力搜索），失败才回退通用算法。

### 陷阱

1. **返回值语义**：返回的是「指向 namepool 的指针的地址」（`candidate`），FNamePool 模式下若 `candidate >= BlocksOff` 会**回减 BlocksOff** 得到真正的 pool 基址。MCP 侧 `NamesPtr` 的语义要区分 FNamePool / GNames 两种布局。
2. **`kMinAnchorHits = 2`**：窗口兜底只要命中 2 个不同引擎名就算。单锚点误判率高的根因在此——宽松命中可能把数据段误判成名字表，所以文档里强制要求 `sampleGNames` 取样本复核。
3. **性能**：全可读段逐 8 字节扫描，段越大越慢，属于「慢 <5min」档。MCP 侧必须走 `waitMs` 短等 + 长轮询，否则 AI 会触发重试风暴。

---

## 5. `GetGUObjectArrayPtr()`（`cpp:987`）

**签名**：`virtual uintptr_t GetGUObjectArrayPtr() const`（hpp `:55`）

**算法**：以 `namesScanBase`（= NamesPtr，PUBG 额外 `+0x110` 解引用）为起点，**双向扫描**最多 `0x300000` 个 8 字节偏移：

```cpp
// 双向扫描：i=0 只扫高地址方向（避免重复），i>0 额外扫低地址方向
for (int i = 0; i < 0x300000; ++i) {
    // 高地址方向
    candObjAddr = namesScanBase + 8ULL * i;
    ...
    // 低地址方向（i > 0 时）
    candObjAddr = namesScanBase - 8ULL * i;
    if (candObjAddr < 0x10000) continue;
    ...
}
```

- `kNameOffs`（`:1008`）：22 个候选偏移 `{0x18,0x1c,...,0x6c}`
- 命中 `/Script/CoreUObject` 时，若 `no != namePrivateOff` 会**自动修正 `off->UObject.NamePrivate`**（`:1044-1049`）——又一个副作用
- **全部失败**：`LOGE("[Bootstrap] 通用方式搜索 GUObject 失败")` 返回 0（`:1057-1058`）

⚠️ **原 bug 已修复**（双向扫描）。之前 `candObjAddr = namesScanBase + 8*i` 只往高地址扫，低地址方向永远扫不到——这是真机 `ERROR_INIT_GUOBJECTARRAY` 的直接根因之一。

---

## 6. 名字解析（`cpp:741-865`）

### `GetNameEntry(int32_t id)`（`:741-796`）

**签名**：`virtual uint8_t *GetNameEntry(int32_t id) const`

- `id < 0` → `nullptr`（`:743-744`）
- `NamesPtr == 0` → `nullptr`（`:747-748`）
- **非 FNamePool**（`:750-781`）：static 缓存 `gNames`/`gNamesPtr`，`com.tencent.ig` 额外 `+0x110` 解引用；`ElementsPerChunk = 16384` 分块取条目
- **FNamePool**（`:783-795`）：`blockBit/BlocksOff/Stride` 计算 `block_offset + chunck_offset`，二级解引用取条目

### `GetNameEntryString(uint8_t *entry)`（`:798-860`）

**签名**：`virtual std::string GetNameEntryString(uint8_t *entry) const`（hpp `:67`，注释"can override if decryption is needed"）

- `!entry` → `""`
- 非 FNamePool：读 `FNameEntry.Index` + `Name`，`strLen = kMAX_UENAME_BUFFER`
- FNamePool：读 `uint16 header`；`isUsingOutlineNumberName()` 且长度为 0 时走**重定向条目**（`nextEntryId` 二次取条目 + `strNumber`）；否则 `strLen = min(GetLength(header), kMAX_UENAME_BUFFER)`
- `strNumber > 0` → 追加 `'_' + (strNumber - 1)`（`:856-857`）

### `GetNameByID(int32_t id)`（`:862-865`）

```cpp
return GetNameEntryString(GetNameEntry(id));
```
组合两步，**失败返回 `""`（空串）**。

### 陷阱

1. **static 缓存跨进程污染**（`:752-768`）：`gNames`/`gNamesPtr` 是函数内 static，只在 `gNamesPtr != namesPtr` 时刷新。**切换进程后若新进程 NamesPtr 恰好等于旧值（理论上可能），会拿到旧进程的 gNames**。MCP 侧切进程后应强制重新 `InitUEVars`（会重建缓存）。
2. **空串 vs `"None"`**：本层返回 `""`，但 L3 `UE_FName::GetName()` 失败返回 `"None"`。MCP 封装时两层语义要区分，不能混用。
3. **OutlineNumber 重定向**（`:829-844`）：UE5 的 outline number 模式下，条目可能是"转发"条目，要跳 `nextEntryId` 再取。这是 FNamePool 解析最容易踩的版本差异。

---

## 7. `findIdaPattern()`（`cpp:924-981`）

**签名**：`virtual uintptr_t findIdaPattern(PATTERN_MAP_TYPE, const std::string &pattern, int step, uint32_t skip_result = 0) const`（hpp `:72-74`）

- `PATTERN_MAP_TYPE` 枚举（hpp `:14-23`）：`ANY_R` / `ANY_X` / `ANY_W` / `BSS`
- 段筛选：BSS 用 `bssSegments()`（无则返回 0）；其余要求 `readable && is_private`，`ANY_X` 再要求 `executable`，`ANY_W` 再要求 `writeable`
- `skip_result > 0` → `findIdaPatternAll` 取第 `skip_result` 个；否则 `findIdaPatternFirst`
- **返回 `address + step`**（不是裸命中地址！`step` 是调用方要跳过的字节数）

### 陷阱

**返回值已经加了 `step`**（`:980`）。MCP 的 `scanPattern` 工具若直接包装此函数，要明确 `step` 语义——调用方传 `step` 是为了直接得到"指令地址"而非"pattern 起始地址"。

---

## 8. 其余定位接口

### `isEmulator()`（`:911`）

```cpp
if (!getMapsContain(pid,"/arm/nb/").empty() || !getMapsContain(pid,"/arm64/nb/").empty())
    return true;
auto ue_elf = GetUnrealELF();
if (!ue_elf.isValid()) return false; // 原代码 fallthrough 到 return true
for (auto &it : ue_elf.segments())
    if (it.executable) return false;
return true;
```

✅ **原 bug 已修复**：segments 为空（`GetUnrealELF()` 未找到 ELF）时不再 fallthrough 到 `return true`，而是保守返回 `false`（非模拟器）。

### `GetStaticFindObject()`（`:1241-1257`）

宽字符串 needle `"Illegal call to StaticFindObject() while serializing object data!"` → `FindWideDataInSegments` → `FindADRPForTarget` → `FilterADRPWithADD` → `FindFunctionStart`。失败返回 0。

### `GetNativeAndroidApp()`（`:1259-1303`）

扫可读段指针，`FixTaggedPtr` 去 tag 后找 `localeHolder`，比对 `"zhCN"` tag 定位。失败返回 0。

### `GetProcessEvent()`（`:1305-1336`）

取对象数组第 0 个对象的 vtable，遍历前 500 个槽位，找 `i∈(50,100)` 且连续三条 STP/STR 指令的函数地址。失败返回 0。

---

## 9. 匿名 namespace 辅助函数（`cpp:14-636`）

> 全部锁在匿名 namespace，**MCP 化时是「外迁」重点**（《设备端API清单》L2 标注）。

| 函数 | 行号 | 作用 |
|---|---|---|
| `IsLikelyReadablePtr` | `:19-22` | `>= 0x10000 && kPtrValidator.isPtrReadable` |
| `IsLikelyObjectName` | `:24-37` | 名字 `[0,96]` 且全 alnum/`_`/`/`/`.` |
| `HasAppId` | `:40-50` | 遍历 `GetAppIDs()` 匹配 |
| `GetModuleReadableEnd` | `:52-63` | 可读段最大 end |
| `MatchEngineName` | `:92-103` | 前缀匹配 `kKnownNames`，返回索引或 -1 |
| `ScanEngineNameAnchors` | `:115-156` | 两级多锚点判定（§4.1） |
| `BruteForceDeltaForceNames` | `:158-294` | DeltaForce 解密暴力搜索（§8.2） |
| `MakeCandidateList` | `:295-311` | primary + fallbacks 去重 |
| `ReadObjectByLayout` | `:313-335` | 按 flat/chunked 布局读对象 |
| `ScoreObjectArrayCandidate` | `:337-398` | 对象数组候选打分（采样 32 个 + 名字评分） |
| `BootstrapCoreObjectArrayOffsets` | `:400-500` | 修正对象数组偏移（§2 陷阱 2） |
| `DecodeADRP` / `DecodeADD` | `:502-520` | ADRP 解码 |
| `IsADD` / `IsSubSP` / `IsStpFpLr` / `IsStrInstruction` | `:522-546` | 指令识别 |
| `FixTaggedPtr` | `:548-553` | 去 `0xB4...` tag |
| `FindFunctionStart` | `:555-571` | 反推函数入口（STP FP LR / SUB SP 识别序言） |
| `FindWideDataInSegments` / `FindADRPForTarget` / `FilterADRPWithADD` | `:573-635` | 宽字符串→ADRP 反查（StaticFindObject 用） |

### 8.2 `BruteForceDeltaForceNames`（`:158-294`）

DeltaForce 专属 GNames 定位，包含**完整的 FName 解密**：

- `kFNameStride=0x2 / kFNameEntryToString=0x2 / kGNamesToFNamePool=0x38 / kFNamePoolToBlocks=0x0`
- 搜索范围 `base+0x18000000 ~ base+0x1E000000`，逐页扫，探针 id `{0,1,2,5,10,20,50,100}`
- **XOR key 推导**：`switch(strLength % 9)` 共 9 个 case（`:221-253`），每个 case 一个 key 公式
- 解密后比对 `kKnownNames`，命中即返回 candidate

⚠️ 这是**游戏专属加密的硬编码**。MCP 的 `scanGNames` 若要支持加密 FName，这里的解密参数是模板，但**每款加密游戏 key 推导不同**，不能通用。

---

## 10. Profile 子类清单（`UEGameProfiles/`）

| 文件 | profile | 备注 |
|---|---|---|
| `ArenaBreakout.hpp` | ArenaBreakout | 暗区突围 |
| `DeltaForce.hpp` | DeltaForce | 三角洲行动（`com.tencent.tmgp.dfm`，加密 FName） |
| `Farlight.hpp` | Farlight | 远光 84 |
| `NRC.hpp` | NRC | （疑 NightCrows，以类名为准） |
| `PUBG.hpp` | PUBG | `com.tencent.ig` |
| `PUBGMHD.hpp` | PUBGMHD | PUBG Mobile HD |
| `Valorant.hpp` | Valorant | 无畏契约手游 |
| `Sfps2.hpp` | **ShuishaProfile**（文件名与类名不一致） | 已接入；对应"Shuisha" |
| `Lineage2.hpp` | Lineage2Profile | ⚠️ **未 include、未注册（未接入）** |
| `AutoFix.hpp/.cpp` | AutoFixProfile | **通用兜底 profile**，`DetectVersion()` 在此 |

⚠️ **`Sfps2.hpp` 的类名是 `ShuishaProfile`**（`Sfps2.hpp:6`）——文件名与类名不一致的源码命名怪癖，它**已接入** `UE_Games`（`executable.cpp:42`）。
目录里真正未接入的是 **`Lineage2.hpp`**（`Lineage2Profile`，既未 include 也未注册）——MCP 化时以 `executable.cpp:38-47` 实际注册的 8 个为准。

### `AutoFixProfile::DetectVersion()`（`AutoFix.cpp:140`）

**签名**：`void AutoFixProfile::DetectVersion() const`

扫 rodata 的 `++UE4+Release-X.YY` 判版本 → 套 9 档 `UE_DefaultOffsets`（`UEOffsets.hpp:214-250`）。这是 `detectUEVersion` 工具的设备端底座，对应 MCP 的 `detectedFrom`（SO_NAME / RODATA_STRING / FALLBACK）。

---

## 11. MCP 封装陷阱清单

| # | 陷阱 | 后果 | 应对 |
|---|---|---|---|
| 1 | ~~`GetGUObjectArrayPtr` 只往高地址单向扫~~（**已修**） | ~~低地址方向永远扫不到~~ | ✅ 双向扫描已实现 |
| 2 | ~~`isEmulator` segments 空返回 true~~（**已修**） | ~~真机被误判模拟器~~ | ✅ ELF 无效时返回 false |
| 3 | `BootstrapCoreObjectArrayOffsets` 改写偏移表 | `InitUEVars` 后偏移 ≠ 预设值 | `getProbeResults` 如实报告修正后的值 |
| 4 | `GetNameEntry` 的 static 缓存 | 跨进程污染风险 | 切进程强制重新 `InitUEVars` |
| 5 | 本层返回 `""`，L3 返回 `"None"` | 空值语义混淆 | 两层分别显式处理 |
| 6 | `findIdaPattern` 返回 `address + step` | 语义误解 | `scanPattern` 封装时明确 step 含义 |
| 7 | 游戏专属硬编码（DeltaForce/PUBG） | 通用算法在这俩游戏行为不同 | 文档标注，MCP 侧按 package 分支 |
| 8 | `Matrix/Physx/FrameCount` 恒 0 | 误判"定位失败" | 0 = 不支持，不是 bug |
| 9 | `GetNamesPtr`/`GetGUObjectArrayPtr` 是慢操作 | AI 重试风暴 | `waitMs` + 长轮询 + 耗时档位 |
| 10 | OutlineNumber 重定向条目 | 名字读错 | `sampleGNames` 输出 `suspectConcatenation` 标记 |
| 11 | 全层依赖 `kMgr`/`kPtrValidator` 单例 | 并发数据竞争 | 命令服务串行化 |
| 12 | `InitUEVars` 返回枚举里 3 个未使用 | 诊断时误以为都会出现 | 只认 11 个实际用到的 |

---

## 12. 本次未精读范围

| 范围 | 内容 |
|---|---|
| `UEGameProfiles/*.hpp/.cpp` 各 profile 的纯虚实现 | 8 个 profile 的 `GetAppIDs`/`GetOffsets` 等具体返回值，仅列了清单未逐行 |
| `UEOffsets.hpp` 9 档偏移表的字段细节 | 属 04 文档范围（`UE_DefaultOffsets` `:214-250` 的 9 个函数） |
| `UEVars::InitStatusToStr` 实现体 | 在 `UEOffsets.cpp`，属 04 文档 |
| `AutoFix.cpp` 的 `DetectVersion` 完整 rodata 扫描逻辑 | 属 05 文档（AutoFix 主流程） |
