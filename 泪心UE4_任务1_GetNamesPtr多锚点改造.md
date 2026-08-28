# 任务 1 · `GetNamesPtr()` 多锚点改造

> **状态（2026-08-29 更新）：代码改动已完成 ✅，整项目编译通过 ✅，真机验收未执行。**
> 编译环境：NDK r27d（`D:\ProgramerDevelop\windowsNDK27`）+ VS2026 自带 cmake/ninja，
> 51/51 目标编译链接通过，产物 `outputs/arm64-v8a/UnrealMemoryTools`（3.2 MB）。

> 属于《开发必读架构》§5.6.4 实施顺序的第 ① 项。
> **预估改动量：约 85 行。风险：低。收益：直接。**
> 这是全部七个任务里**投入产出比最高**的一个——改动最小，但直接命中
> AutoFix 失败率最高的那个硬编码卡点。

***

## 1. 任务目标

把 `IGameProfile::GetNamesPtr()` 里**单锚点 + 单偏移**的判定，
改成**多锚点 + 偏移候选表**的判定。

**一句话**：现在只认 `"ByteProperty"` 一个名字、只在一个固定偏移上找；
改成认 12 个 UE 引擎内建名、在多个候选偏移上找。

***

## 2. 现状（精确到行）

文件：`src/UE/UEGameProfile.cpp`，函数 `IGameProfile::GetNamesPtr()`（**971–1084 行**）

### 2.1 四个硬编码卡点

| 卡点             | 行号                     | 现状                                                           |
| -------------- | ---------------------- | ------------------------------------------------------------ |
| 指针值范围          | `995-996`              | `kMinPtr=0x4FFFFFFFFF` / `kMaxPtr=0x7FFFFFFFFF` —— **本任务不动** |
| FNamePool 锚点偏移 | `1030`                 | 只读 `val + 0x8` 一处                                            |
| GNames 锚点偏移    | `1046`（PUBG 变体 `1058`） | 只读 `p1 + 0x24` 一处                                            |
| 锚点字符串          | `999-1001`             | `isByteProperty()` 只比对 `"ByteProperty"`                      |

### 2.2 当前判定逻辑

```cpp
// 999-1002
auto isByteProperty = [](const char *s) -> bool
{
    return std::strncmp(s, "ByteProperty", 12) == 0;
};

// 1028-1040  tryFNamePool
if (!vm_rpm_ptr((void *)(val + 0x8), strBuf0, 12) || !isByteProperty(strBuf0))
    return 0;
uintptr_t poolBase = candidate;
if (off->FNamePool.BlocksOff && candidate >= off->FNamePool.BlocksOff)
    poolBase -= off->FNamePool.BlocksOff;

// 1042-1050  tryGNames
uintptr_t p1 = ...;
if (p1 && vm_rpm_ptr((void *)(p1 + 0x24), strBuf1, 12) && isByteProperty(strBuf1))
    return candidate;
```

**问题**：`+0x8` / `+0x24` 是 FNameEntry 头之后第一个名字的偏移。
条目头长度随 UE 版本变化（2 字节 / 4 字节 / 带 alignment 填充），
**偏移一变，读到的就是垃圾，立即判定失败**——哪怕 GNames 真的就在那里。

### 2.3 那份没用上的名单

```cpp
// 80-84 行，位于 BruteForceDeltaForceNames()（函数起始于 64 行）内部
static const char *kKnownNames[] = {
    "None", "ByteProperty", "IntProperty", "BoolProperty",
    "FloatProperty", "ObjectProperty", "NameProperty",
    "StructProperty", "ArrayProperty", "Object", "Class", "Function"
};
```

⚠️ **它是** **`BruteForceDeltaForceNames()`** **的函数内局部 static，
`GetNamesPtr()`** **看不到。** 而且只有三角洲一条路径在用。

**这份名单是现成的**——12 个名字全是 UE 引擎内建名，
所有 UE 游戏的名字表开头都是这一批，与具体游戏无关。

***

## 3. 改动方案

### 改动 A：把名单提到文件作用域（约 15 行）

在 `UEGameProfile.cpp` 顶部的匿名 namespace 里（或其他文件级常量处）新增：

```cpp
namespace
{
    // UE 引擎内建名：所有 UE 游戏的名字表开头都是这一批，与具体游戏无关。
    // 来源：UEGameProfile.cpp:80-84（原为 BruteForceDeltaForceNames 的局部 static）。
    const char *const kEngineNames[] = {
        "None", "ByteProperty", "IntProperty", "BoolProperty",
        "FloatProperty", "ObjectProperty", "NameProperty",
        "StructProperty", "ArrayProperty", "Object", "Class", "Function"
    };
    constexpr size_t kEngineNamesCount =
        sizeof(kEngineNames) / sizeof(kEngineNames[0]);

    // 通过阈值：命中几个引擎名才算确认（可调，先定 2）
    constexpr int kMinAnchorHits = 2;

    // FNamePool 内首个名字条目的候选偏移（条目头长度随版本变化）
    const uintptr_t kPoolEntryOffsets[] = { 0x8, 0x6, 0xA, 0x4, 0xC, 0x10 };
    // GNames 二级解引用后条目头的候选偏移
    const uintptr_t kGNamesEntryOffsets[] = { 0x24, 0x2C, 0x1C, 0x14, 0x34 };
}
```

然后把 `BruteForceDeltaForceNames()` 里 80-84 的局部定义**改为引用这份**，
避免两份名单漂移。

### 改动 B：匹配辅助函数（约 12 行）

```cpp
// 返回命中的引擎名索引，未命中返回 -1
int MatchEngineName(const char *s)
{
    if (!s || !*s) return -1;
    for (size_t i = 0; i < kEngineNamesCount; ++i)
    {
        if (std::strncmp(s, kEngineNames[i],
                         std::strlen(kEngineNames[i])) == 0)
            return static_cast<int>(i);
    }
    return -1;
}
```

### 改动 C：两级判定（约 25 行）

**Tier 1 —— 精确匹配（快，首选）**
遍历偏移候选表，在每个偏移上读字符串，匹配任意一个引擎名：

```cpp
// 命中返回 true；matchedOffset 回传实际命中的偏移
bool TryExactAnchors(uintptr_t base, char *buf, size_t bufCap,
                     const uintptr_t *offsets, size_t count,
                     uintptr_t *matchedOffset)
{
    for (size_t k = 0; k < count; ++k)
    {
        std::memset(buf, 0, bufCap);
        if (!vm_rpm_ptr((void *)(base + offsets[k]), buf, 16))
            continue;
        if (MatchEngineName(buf) >= 0)
        {
            if (matchedOffset) *matchedOffset = offsets[k];
            return true;
        }
    }
    return false;
}
```

**Tier 2 —— 窗口扫描（宽松兜底，抗条目头长度变化）**
在候选位置后的一小段窗口内统计出现的引擎名数量：

```cpp
// 在 [base, base+window) 内统计出现的不同引擎名数量
int CountEngineNamesInWindow(uintptr_t base, size_t window)
{
    char buf[128] = {};
    const size_t toRead = window < sizeof(buf) ? window : sizeof(buf) - 1;
    if (!vm_rpm_ptr((void *)base, buf, toRead))
        return 0;

    int hits = 0;
    bool seen[kEngineNamesCount] = {};
    for (size_t i = 0; i + 1 < toRead; ++i)
    {
        int idx = MatchEngineName(buf + i);
        if (idx >= 0 && !seen[idx])
        {
            seen[idx] = true;
            if (++hits >= kMinAnchorHits)
                return hits;
        }
    }
    return hits;
}
```

### 改动 D：改接 `tryFNamePool` / `tryGNames`（约 25 行）

**`tryFNamePool`**（原 1028-1040）：

```cpp
auto tryFNamePool = [&]() -> uintptr_t
{
    uintptr_t hitOff = 0;
    bool ok = TryExactAnchors(val, strBuf0, sizeof(strBuf0),
                              kPoolEntryOffsets,
                              sizeof(kPoolEntryOffsets) / sizeof(uintptr_t),
                              &hitOff);

    // 精确匹配失败 → 窗口扫描兜底
    if (!ok)
        ok = CountEngineNamesInWindow(val, 96) >= kMinAnchorHits;

    if (!ok) return 0;

    uintptr_t poolBase = candidate;
    if (off->FNamePool.BlocksOff && candidate >= off->FNamePool.BlocksOff)
        poolBase -= off->FNamePool.BlocksOff;

    LOGI("[Bootstrap] FNamePool @ 0x%lx (slot @ 0x%lx, anchorOff=0x%lx)",
         static_cast<unsigned long>(poolBase),
         static_cast<unsigned long>(candidate),
         static_cast<unsigned long>(hitOff));
    return poolBase;
};
```

**`tryGNames`**（原 1042-1066）：同理，把 `p1 + 0x24` 的
`isByteProperty` 判定换成 `TryExactAnchors(p1, ...)` + 窗口兜底；
PUBG 变体（`q3 + 0x24`）同样处理。

### 改动 E：其他（约 8 行）

- `isByteProperty` 若无处引用则删除；若仍被别处使用则保留
- 日志补上实际命中的偏移量，便于事后分析哪个偏移在当前游戏上有效

### 实际落地与方案的差异（2026-08-29 核对，均为等价或改进）

| 项 | 方案 | 实际实现 |
| --- | --- | --- |
| 名单命名 | `kEngineNames` | `kKnownNames`（复用原名，内容相同，`BruteForceDeltaForceNames` 已共用） |
| 两级判定 | `TryExactAnchors` + `CountEngineNamesInWindow` 两个函数 | 合并为 `ScanEngineNameAnchors()`，返回 `AnchorScanResult{hit, exact, offset, hits}` |
| 远程读次数 | 每个候选偏移读一次（最多 6 次） | **单次读 128 字节窗口后本地跑两级判定**（更省，`kAnchorWindowSize=128`） |
| Tier2 命中处理 | 命中即用 | **宽松命中不立即返回**：记入 `weakPool`/`weakGNames`，整轮扫完无精确命中才回退使用——避免靠前的弱结果盖掉后面的精确命中 |
| 日志 | 含 `anchorOff` | 含 `anchorOff` + `hits`（命中引擎名数量），Tier2 回退时 `LOGW` 提示用 probe 复核 |

**结论**：实际实现覆盖方案全部要点，且在远程读次数与候选竞争处理上优于方案。
备份文件已改名 `UEGameProfile未改动代码.cpp.bak`，不会被 `file(GLOB src/UE/*.cpp)` 误编译。

***

## 4. 明确不做（避免范围蔓延）

| 项                                            | 留给                            |
| -------------------------------------------- | ----------------------------- |
| 指针值范围放宽（`995-996`）                           | **任务 2** —— 会显著改变候选数量与耗时，需单独测 |
| 符号定位接线                                       | **任务 2**                      |
| 扫描区域改为全内存（现仅扫 ELF 段）                         | 任务 5（参数化扫描）                   |
| 解耦 `GetGUObjectArrayPtr` 对 `GetNamesPtr` 的依赖 | 任务 5                          |

***

## 5. 前置条件（已完成 ✅，2026-08-29 实测）

`CMakeLists.txt` 第 2 行的硬编码已改为可覆盖形式：

```cmake
# 允许构建时覆盖：-DNDK_PATH=D:/ProgramerDevelop/ndk23
if(NOT DEFINED NDK_PATH)
    set(NDK_PATH E:/ndk/android-ndk-r29)
endif()
```

实测配置命令（VS2026 自带 cmake/ninja，本机 PATH 无）：

```
cmake -S . -B build -G Ninja -DNDK_PATH=D:/ProgramerDevelop/windowsNDK27
cmake --build build
```

### NDK 版本踩坑与解决（已解决 ✅）

第一次实测用的 `D:\ProgramerDevelop\ndk23` 实为 **NDK r23.2**（source.properties: 23.2.8568313），
**低于项目要求的 r25+**。配置可以通过，但整项目构建在 `src/Dumper.cpp` 失败，
两个错误均与本任务无关（本任务只改了 `UEGameProfile.cpp`，且该文件单独编译通过）：

| 错误 | 根因 |
|---|---|
| `json.hpp:6094` `u8string()` 类型不匹配 | r23 的 libc++ 中 `filesystem::u8string()` 返回 `std::string`，而 `nlohmann/json`（C++20 写法）期望 `std::u8string`；r25+ 的 libc++ 已修正 |
| `Dumper.cpp:427` `operator==` 二义性 | 同为工具链版本相关的重载决议差异 |

**解决**：改用 **NDK r27d**（`D:\ProgramerDevelop\windowsNDK27`，Pkg.Revision=27.3.13750724），
重新配置后 **51/51 目标编译链接全部通过**，产物
`outputs/arm64-v8a/UnrealMemoryTools`（约 3.2 MB）。
两个报错未改任何代码，纯工具链升级即消失，反向确认与任务 1 改动无关。

注：本机 PATH 无 cmake/ninja，用的是 VS2026 自带版本：
`D:\ProgramerDevelop\VS2026\SDK\Common7\IDE\CommonExtensions\Microsoft\CMake\`。

（README 也提示过：Windows 上 Ninja 在中文路径下会偶发 `GetOverlappedResult`，
建议用英文路径或在 CLion 内构建。）

***

## 6. 验收标准

| # | 标准                | 判定方式                                                          | 状态（2026-08-29） |
| - | ----------------- | ------------------------------------------------------------- | ----------- |
| 1 | **编译通过**          | ninja 无 error                                                 | ✅ 51/51 通过，产物已生成（NDK r27d） |
| 2 | **自有游戏定位成功且地址正确** | 拿你自己的 UE4 手游（有 ground truth）测，日志里的 `FNamePool @ 0x...` 与已知值一致 | ⬜ 待真机测试 |
| 3 | **回归不破**          | 至少 2 款原本 AutoFix 能成功的游戏，改造后仍成功                                | ⬜ 待真机测试 |
| 4 | **无新增误报**         | 指向一个非 UE 进程，不得返回非零地址                                          | ⬜ 待真机测试 |
| 5 | **日志可读**          | 日志含实际命中的 `anchorOff`，便于后续统计哪个偏移最常见                            | ✅ 代码已实现（含 `anchorOff` 与 `hits`） |

**第 2 条最关键**——你有 ground truth，这是别人没有的优势。
改造前后各跑一次，直接对比。

***

## 7. 风险与回退

| 风险        | 说明                  | 应对                                 |
| --------- | ------------------- | ---------------------------------- |
| 误报率上升     | 窗口扫描较宽松，可能匹配到非名字表区域 | 阈值 `kMinAnchorHits` 先定 2；若误报多，提到 3 |
| 耗时增加      | 每个候选要多试 6 个偏移       | 偏移表按可能性排序，命中即停；实测若明显变慢再收紧          |
| 改动被后续任务覆盖 | 任务 5 会重构扫描逻辑        | 本任务只改判定，不改结构；重构时保留判定函数             |

**回退**：改动集中在 `GetNamesPtr()` 与文件级常量，
git 上单独一个提交，随时可 revert。

***

## 8. 提交要求

- Commit message：`fix(ue): GetNamesPtr 改用多锚点与偏移候选表`
  （沿用项目 Conventional Commits 约定）
- 单个 PR 只含本任务改动（遵循"一 PR 一逻辑变更"）
- PR 描述里写明：改造前后在你自有游戏上的定位结果对比

***

## 9. 任务完成后

更新两份文档：

- 《功能说明》§2.2 的四个硬编码卡点表——标注前三项已修复（第四项留给任务 2）
- 本任务的实测数据（命中率、耗时变化）回填到《功能说明》§9 验收标准表

***

## 附：改动位置速查（改造后实际行号，2026-08-29）

| 内容                      | 行号                                                      |
| ----------------------- | ------------------------------------------------------- |
| `GetNamesPtr()` 函数体     | `src/UE/UEGameProfile.cpp:1061-1239`                    |
| `isByteProperty` lambda | **已删除**（被 `ScanEngineNameAnchors` 取代）                   |
| 文件级常量与判定函数（匿名 namespace）| `:65-156`（名单 `:68`、`kMinAnchorHits` `:76`、`MatchEngineName` `:92`、`ScanEngineNameAnchors` `:115`） |
| `tryFNamePool` lambda   | `:1122-1147`                                            |
| `tryGNames` lambda      | `:1149-1209`（PUBG 变体 `:1177-1206`）                     |
| 偏好顺序分支                  | `:1211-1220`                                            |
| 宽松命中回退（`weakPool`/`weakGNames`） | `:1095-1096` 定义、`:1226-1235` 使用                |
| CMake NDK 路径            | `CMakeLists.txt:2-5`（已改为可覆盖形式）                          |

