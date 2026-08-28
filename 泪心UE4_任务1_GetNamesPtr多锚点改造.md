# 任务 1 · `GetNamesPtr()` 多锚点改造

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

***

## 4. 明确不做（避免范围蔓延）

| 项                                            | 留给                            |
| -------------------------------------------- | ----------------------------- |
| 指针值范围放宽（`995-996`）                           | **任务 2** —— 会显著改变候选数量与耗时，需单独测 |
| 符号定位接线                                       | **任务 2**                      |
| 扫描区域改为全内存（现仅扫 ELF 段）                         | 任务 5（参数化扫描）                   |
| 解耦 `GetGUObjectArrayPtr` 对 `GetNamesPtr` 的依赖 | 任务 5                          |

***

## 5. 前置条件

⚠️ **CMakeLists.txt 第 2 行硬编码了 NDK 路径**：

```cmake
set(NDK_PATH E:/ndk/android-ndk-r29)
```

而你的 NDK 在 `D:/ProgramerDevelop/ndk23`，且项目要求 **NDK r25+**。
**先确认能正常构建，再动手改代码。**

建议改为可覆盖形式，避免改动被误提交：

```cmake
if(NOT DEFINED NDK_PATH)
    set(NDK_PATH E:/ndk/android-ndk-r29)
endif()
```

然后构建时传入：`cmake -S . -B build -G Ninja -DNDK_PATH=D:/ProgramerDevelop/ndk23`

（README 也提示过：Windows 上 Ninja 在中文路径下会偶发 `GetOverlappedResult`，
建议用英文路径或在 CLion 内构建。）

***

## 6. 验收标准

| # | 标准                | 判定方式                                                          |
| - | ----------------- | ------------------------------------------------------------- |
| 1 | **编译通过**          | ninja 无 error                                                 |
| 2 | **自有游戏定位成功且地址正确** | 拿你自己的 UE4 手游（有 ground truth）测，日志里的 `FNamePool @ 0x...` 与已知值一致 |
| 3 | **回归不破**          | 至少 2 款原本 AutoFix 能成功的游戏，改造后仍成功                                |
| 4 | **无新增误报**         | 指向一个非 UE 进程，不得返回非零地址                                          |
| 5 | **日志可读**          | 日志含实际命中的 `anchorOff`，便于后续统计哪个偏移最常见                            |

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

## 附：改动位置速查

| 内容                      | 行号                                                      |
| ----------------------- | ------------------------------------------------------- |
| `GetNamesPtr()` 函数体     | `src/UE/UEGameProfile.cpp:971-1084`                     |
| `isByteProperty` lambda | `:999-1002`                                             |
| `tryFNamePool` lambda   | `:1028-1040`                                            |
| `tryGNames` lambda      | `:1042-1066`（PUBG 变体 `:1052-1063`）                      |
| 偏好顺序分支                  | `:1068-1077`                                            |
| **可复用的名单（现为局部 static）** | **`:80-84`**（位于 `BruteForceDeltaForceNames()`，起始 `:64`） |
| CMake NDK 路径            | `CMakeLists.txt:2`                                      |

