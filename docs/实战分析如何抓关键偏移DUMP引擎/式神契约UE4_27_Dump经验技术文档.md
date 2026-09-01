# 《式神契约》UE4.27+ 游戏 Dump 经验技术文档

> 目标：记录一次通过 UMT MCP 服务分析 Android UE4.27+ 游戏进程、修正错误名称池候选、完成探针初始化并导出 SDK 的完整过程。
>
> 目标包名：com.huitgames.shikigami.summons  
> 设备序列号：3B15AM01NAE00000  
> 文档日期：2026-08-31  
> 最终状态：Probe 成功，Dump 成功，输出文件可读，名称和对象内容已在手机端人工确认。

---

## 1. 结论先行

本次成功的关键不是直接运行一次 Dump，而是把流程拆成：

连接进程 → 重新获取模块基址 → 计算相对偏移 → 读取并验证真实 FNamePool → 应用探针覆盖 → Probe → Dump → 拉取大文件 → 人工抽样确认。

最终结果：

- 进程：PID 14720，进程启动标记 2420。
- libUE4.so 基址：0x7965242000。
- FNamePool：模块相对偏移 0xBAE55C0，运行时地址 0x7970D275C0。
- GUObjectArray：模块相对偏移 0xBB29898，运行时地址 0x7970D6B898。
- GWorld：模块相对偏移 0xBCA0910，运行时地址 0x7970EE2910。
- Probe：finished=true、success=true，日志记录 RunFixup done (success=1)。
- Dump：dumpPercent=100、finished=true、success=true。
- 输出目录：/sdcard/UnrealMemoryTools/com.huitgames.shikigami.summons。
- 输出文件：AIOHeader.hpp、Logs.txt、Objects.txt、Offsets.hpp、SDK_Classes.hpp、SDK_Enums.hpp、SDK_Offset.hpp、SDK_Structs.hpp、script.json。
- Objects.txt 大小：3,786,398 bytes；通过 adb pull 拉取后内容可读。

最终验证样本：

~~~
[0000000000] [00000079404e9b60]: Package CoreUObject
[0000000001] [0000007977600b00]: Class CoreUObject.Object
[0000000002] [00000079404eb140]: Package SlateCore
[0000000006] [00000079404eb000]: Package UMG
[0000000008] [0000007977607900]: Class UMG.Widget
~~~

这组结果证明 Dump 使用了真实名称池和对象数组，不是把静态字符串表误当成 FNamePool。

---

## 2. 工具链与职责边界

~~~
Codex / MCP Client
        │ camelCase 工具
        ▼
mcp_server（Python FastMCP）
        │ TCP + NDJSON，经 adb forward
        ▼
手机 CommandServer（127.0.0.1:35515）
        │
        ▼
UMT 运行时：进程附加、内存读取、符号解析、UE Probe、SDK Dump
~~~

| 阶段 | 主要工具 | 目的 |
|---|---|---|
| 服务检查 | ping | 确认协议和设备连接 |
| 进程定位 | listProcesses、selectProcess | 锁定包名和 PID |
| 附加 | attach | 建立可读内存会话，取得 mapRevision |
| 模块定位 | listModules | 获取当前 ASLR 下的 libUE4.so 基址和段信息 |
| 静态定位 | resolveSymbol | 优先确认 GUObjectArray、GWorld、FName 相关符号 |
| 动态验证 | readMemory | 读取指针链和 FName 条目，判断候选是否真实 |
| 探针控制 | applyProbeOverrides、startProbe、getProbeStatus | 固定已验证地址并初始化结构偏移 |
| 结果读取 | getProbeResults、getLogs | 保存最终地址、偏移、警告和成功状态 |
| 导出 | startDump、getDumpStatus | 生成 SDK 文件 |
| 文件确认 | listOutputFiles、readOutputFile | 列出输出；大文件走 adb |

MCP 工具调用必须串行。上一条调用未完成时不要发下一条，尤其是内存扫描、Probe 和 Dump。

---

## 3. 第一次乱码的根因

### 3.1 现象

第一次错误 Dump 的 Objects.txt 中出现：

~~~
None
itialAcquireFinished
Keys
hileSeeking
~~~

手机编辑器中同时出现大量替换字符（�）。Dump 最终日志为 ERROR_EMPTY_PACKAGES，虽然文件生成了，但没有有效 Package/Class 集合。

### 3.2 误判点

当时使用的名称池候选为 0x79fc3c8588。对它执行标准 UE FNamePool 指针链后：

~~~
pool + 0x40 -> 0x79f6239a7e
~~~

该地址实际落在一段静态 ASCII 表中，附近内容是：

~~~
stem\0bInCanRenderWhileSeeking\0...
~~~

标准 FName 解析会把 ID 乘以 Stride=2，于是静态字符串中间位置被当成条目起点，刚好产生了 hileSeeking、Keys、None 等“看似合理但不连续”的假名称。这是典型的“候选地址能读到字符串，但不是 FNamePool”的误导。

### 3.3 纠正原则

只要 FName 条目解析出现以下任一情况，就不能继续 Dump：

1. ID=0 不能稳定解析为 None。
2. 连续条目的 Header、长度和 ASCII/UTF-16 内容不符合 UE FName 格式。
3. 名称集中出现半个单词、函数名后缀或随机二进制替换字符。
4. Probe 日志出现 NamePrivate ... not validated。
5. Dump 结束为 ERROR_EMPTY_PACKAGES。

正确做法是回到模块基址和相对偏移，重新建立候选，不要对乱码文件继续加工。

---

## 4. 成功流程详解

### 4.1 检查 MCP 服务和设备

先确认服务端和设备端都在工作：

~~~json
{"tool":"ping","arguments":{}}
~~~

关键响应：

~~~
build=1.0.0
protocol=1
connected=true
~~~

手机侧基础检查：

~~~powershell
adb devices
adb forward tcp:35515 tcp:35515
~~~

本次设备返回：

~~~
3B15AM01NAE00000    device
~~~

### 4.2 每次重启服务后重新发现进程

服务重启、游戏重启或进程重启都会使 PID、内存映射和绝对地址失效。重新调用：

~~~json
{"tool":"listProcesses","arguments":{}}
~~~

筛选结果：

~~~
package=com.huitgames.shikigami.summons
pid=14720
profileName=自动识别 (UE4/UE5 通用)
~~~

然后显式选择：

~~~json
{"tool":"selectProcess","arguments":{"package":"com.huitgames.shikigami.summons","pid":14720}}
~~~

### 4.3 附加并取得当前映射版本

~~~json
{"tool":"attach","arguments":{"package":"com.huitgames.shikigami.summons","pid":14720}}
~~~

关键响应：

~~~
attached=true
isMemValid=true
mapRevision=14720:52ffc723e22c2a75
processStartTime=2420
~~~

mapRevision 只用于识别当前映射快照，不应长期缓存。进程加载库、卸载库或服务重新读取 maps 后，它可能变化。

### 4.4 获取 libUE4.so 基址和段信息

~~~json
{"tool":"listModules","arguments":{"name_filter":"libUE4.so","include_segments":true}}
~~~

本次模块范围：

| 段 | 起始 | 结束 | fileOffset |
|---|---:|---:|---:|
| r-- | 0x7965242000 | 0x796B66F000 | 0x0 |
| r-x | 0x796B66F000 | 0x797039A000 | 0x642D000 |
| r-- | 0x797039A000 | 0x7970C27000 | 0xB158000 |
| rw- | 0x7970C27000 | 0x7970C3C000 | 0xB9E5000 |

模块基址：Base = 0x7965242000。

所有静态偏移都使用：

~~~
RuntimeAddress = Base + ModuleRelativeOffset
~~~

绝不直接复用旧进程绝对地址。

### 4.5 符号优先，静态偏移其次

本次通过 resolveSymbol 确认：

~~~
GUObjectArray -> moduleOffset 0xBB29898 -> 0x7970D6B898
GWorld        -> moduleOffset 0xBCA0910 -> 0x7970EE2910
~~~

同时能看到 FNamePool 方法和 _ZNK5FName18GetPlainNameStringEv 等符号。某些构建中 NamePoolData 精确全局符号不会直接暴露，因此最终仍以模块相对偏移加内存内容验证为准。

### 4.6 验证真实 FNamePool

最终名称池候选：

~~~
FNamePool = Base + 0xBAE55C0
          = 0x7965242000 + 0xBAE55C0
          = 0x7970D275C0
~~~

读取名称池的块指针：

~~~
*(0x7970D275C0 + 0x40) = 0x79774C0000
~~~

读取块 0 的前 128 字节，开头为：

~~~
1E014E6F6E6510034279746550726F7065727479C002496E7450726F7065727479...
~~~

按 UE4 FNamePool（Stride=2）解释后得到：

~~~
None
ByteProperty
IntProperty
BoolProperty
FloatProperty
~~~

这一步是排除乱码的决定性证据：指针链正确、Header 合法、条目连续且包含 UE 核心保留名称。

### 4.7 FNamePool 解析公式

UE4.25-4.27 常见配置：BlocksBit=16、BlocksOff=0x40、Stride=2、Header 位移为 6。

~~~cpp
blockIndex   = id >> 16;
withinBlock  = id & 0xFFFF;
blockBase    = *(uint8_t**)(FNamePool + 0x40 + blockIndex * 8);
entry        = blockBase + withinBlock * 2;
header       = *(uint16_t*)entry;
length       = header >> 6;
nameBytes    = entry + 2;
~~~

FNamePool 全局对象地址和块内存地址不是同一个区域。不能因为块地址附近存在可读字符串，就反推它是全局名称池。

### 4.8 应用探针覆盖

第一次带 map_revision 重试时遇到 E_MAP_STALE，因为调用之间映射版本已经改变。重新确认进程仍是 PID 14720 后，省略过期的 map_revision，直接应用覆盖：

~~~json
{
  "tool":"applyProbeOverrides",
  "arguments":{
    "pid":14720,
    "process_start_time":"2420",
    "validate_before_apply":true,
    "ttl_seconds":300,
    "overrides":{
      "names":"0x7970d275c0",
      "objects":"0x7970d6b898",
      "world":"0x7970ee2910"
    }
  }
}
~~~

响应确认 3 个覆盖已接受。覆盖是临时的，ttl_seconds=300 只覆盖本次探针和导出窗口；进程变化后必须重新计算。

### 4.9 运行 Probe 并等待完成

~~~json
{"tool":"startProbe","arguments":{}}
~~~

轮询：

~~~json
{"tool":"getProbeStatus","arguments":{}}
~~~

最终状态：

~~~
phase=探针完成
running=false
finished=true
success=true
~~~

日志中重要的自动修正结果：

~~~
ObjObjects=0x10
TU.Objects=0x0
TU.NumElements=0x10
FUItem.Object=0x0
FUItem.Size=0x18
chunk=65536
NamePrivate=0x18 validated
ClassPrivate=0x10 validated
FName Size=0x4 Number=0x0 DisplayIndex=0x0
ObjectFlags=0x8 InternalIndex=0xc OuterPrivate=0x20
UStruct SuperStruct=0x40 PropertiesSize=0x58 MinAlignment=0x5c
UClass ClassDefaultObject=0x118 ImplementedInterfaces=0x208
[AutoFix] === RunFixup done (success=1) ===
~~~

最终 Probe 日志提示 UE version string not found; fallback to UE4.25-4.27。表示本次运行没有从目标映射中抓到版本字符串，但结构布局、FNamePool 解析和静态符号均与 UE4.27 系列一致；此前一次运行曾检测到 ++UE4+Release-4.27。

### 4.10 读取探针结果并固化偏移

~~~json
{"tool":"getProbeResults","arguments":{}}
~~~

最终静态结果：

| 字段 | 相对偏移 | 运行时地址 |
|---|---:|---:|
| BaseAddress | 0x0 | 0x7965242000 |
| FNamePool | 0xBAE55C0 | 0x7970D275C0 |
| GUObjectArray | 0xBB29898 | 0x7970D6B898 |
| ObjObjects | 0xBB298A8 | 0x7970D6B8A8 |
| GWorld | 0xBCA0910 | 0x7970EE2910 |
| ProcessEvent | 0x7937104 | 0x796CB79104 |
| StaticFindObject | 0x79513D0 | 0x796CB933D0 |
| FrameCount | 0xBACE960 | 0x7970D10960 |
| Matrix | 0xBC94470 | 0x7970ED6470 |
| Physx | 0xBCFE8B0 | 0x7970F408B0 |
| NativeAndroidApp | 0xB9F9990 | 0x7970C3B990 |

对象布局和容器布局：

| 结构 | 字段 | 偏移/值 |
|---|---|---:|
| FUObjectArray | ObjObjects | 0x10 |
| TUObjectArray | Objects | 0x0 |
| TUObjectArray | NumElements | 0x10 |
| FUObjectItem | Object | 0x0 |
| FUObjectItem | Size | 0x18 |
| UObject | ObjectFlags | 0x8 |
| UObject | InternalIndex | 0xC |
| UObject | ClassPrivate | 0x10 |
| UObject | NamePrivate | 0x18 |
| UObject | OuterPrivate | 0x20 |
| UStruct | SuperStruct | 0x40 |
| UStruct | Children | 0x48 |
| UStruct | ChildProperties | 0x50 |
| UStruct | PropertiesSize | 0x58 |
| UStruct | MinAlignment | 0x5C |
| UClass | ClassDefaultObject | 0x118 |
| UClass | CastFlags | 0xD0 |
| UClass | ClassFlags | 0xD8 |
| UClass | ImplementedInterfaces | 0x208 |
| UField | Next | 0x28 |
| FField | NamePrivate | 0x28 |
| FField | Next | 0x20 |
| FField | ClassPrivate | 0x8 |
| FField | Owner | 0x10 |
| FProperty | ArrayDim | 0x34 |
| FProperty | ElementSize | 0x38 |
| FProperty | PropertyFlags | 0x3C |
| FProperty | Offset | 0x4C |
| FProperty | Size | 0x78 |
| UEnum | Names | 0x40 |
| UFunction | FunctionFlags | 0xB0 |
| UFunction | NumParams | 0xB7 |
| UFunction | ParmsSize | 0xBA |
| UFunction | Func | 0xD8 |
| ULevel | Actors | 0x98 |
| UDataTable | RowMap | 0x30 |

属性专用布局中，Object、Struct、Byte、Bool、Enum、Delegate、Array、Set、Map、Class、Interface 的核心属性偏移大多为 0x78；Map 的 ValueProp、Class 的 MetaClass、Enum 的 Enum 为 0x80。

### 4.11 启动 Dump，不使用一键重跑

~~~json
{"tool":"startDump","arguments":{}}
~~~

轮询：

~~~json
{"tool":"getDumpStatus","arguments":{}}
~~~

最终返回：

~~~
dumpPercent=100
phase=完成
running=false
finished=true
success=true
resultPath=/sdcard/UnrealMemoryTools/com.huitgames.shikigami.summons
~~~

这里特意没有再次使用会自动重跑 Probe 的一键 dumpSdk。一键调用可能清掉人工覆盖并重新进入 AUTO 探测，导致第一次那种“探针成功但名称为空”的回归。稳定顺序是：

~~~
applyProbeOverrides → startProbe → getProbeResults → startDump
~~~

### 4.12 列出并拉取输出

~~~json
{"tool":"listOutputFiles","arguments":{"package":"com.huitgames.shikigami.summons"}}
~~~

设备端目录：

~~~
/sdcard/UnrealMemoryTools/com.huitgames.shikigami.summons
~~~

大文件不能依赖 readOutputFile 返回正文。Objects.txt 超过 1 MB 时，MCP 正确返回空内容和 adbPull 提示，应直接执行：

~~~powershell
New-Item -ItemType Directory -Force -Path 'outputs\com.huitgames.shikigami.summons'
adb pull '/sdcard/UnrealMemoryTools/com.huitgames.shikigami.summons/Objects.txt' 'outputs\com.huitgames.shikigami.summons\'
~~~

本机最终文件：

~~~
D:\泪心安卓领域基本盘技术\ue4ImGuiAutoWorkingMcpBytear\outputs\com.huitgames.shikigami.summons\Objects.txt
~~~

---

## 5. 乱码排查决策树

~~~
Objects.txt 是否可读？
  ├─ 否：先停止继续导出
  │    ├─ 名称是半词/函数后缀？检查 FNamePool 候选是否落入静态 ASCII
  │    ├─ 出现 �？检查 Header、Stride、UTF-16/ASCII 解释
  │    └─ ERROR_EMPTY_PACKAGES？回到模块基址重新计算
  └─ 是：抽样 CoreUObject.Object、UObject、Package、UMG.Widget
       ├─ 连续且合理：继续检查 SDK_Classes.hpp 和 SDK_Structs.hpp
       └─ 只有零散名字：重新验证 GUObjectArray 和 FUObjectItem stride
~~~

### 常见错误与处理

| 错误/现象 | 原因 | 处理 |
|---|---|---|
| E_MAP_STALE | mapRevision 在两次调用间变化 | 重新 attach/listModules；不传旧 revision |
| NamePrivate ... not validated | 名称池候选不是真实 FNamePool，或 FName 偏移不对 | 读取 pool+0x40、块 0 和连续条目 |
| ERROR_EMPTY_PACKAGES | 对象遍历没有得到有效 Package/Class | 不使用该次文件；检查 names、objects、stride |
| 只有可读英文但名称错位 | 读到了静态字符串表 | 检查指针链，不以“能看到字符串”为成功标准 |
| readOutputFile 内容为空 | 文件超过 MCP 响应上限 | 使用返回的 adbPull 命令 |
| 一键 dumpSdk 又变乱码 | 一键流程重新运行 AUTO Probe，覆盖被重置 | 拆成 applyProbeOverrides → Probe → Dump |
| resolveSymbol 没结果 | ELF 被 strip 或符号名是 C++ mangled name | 用 match=CONTAINS 搜索 FName/GWorld，再回到内存验证 |

---

## 6. 可复跑操作清单

### 6.1 每次新进程都必须执行

- [ ] ping 确认 connected=true。
- [ ] listProcesses 按包名重新取 PID。
- [ ] selectProcess 显式锁定 PID。
- [ ] attach 取得新的 processStartTime 和 mapRevision。
- [ ] listModules("libUE4.so") 取得新的 Base。
- [ ] 用 Base + relativeOffset 计算绝对地址。
- [ ] 用 readMemory 验证 FNamePool 块指针和连续条目。
- [ ] 用 readMemory 或符号确认 GUObjectArray/GWorld。
- [ ] applyProbeOverrides(validate_before_apply=true)。
- [ ] startProbe 并等待 success=true。
- [ ] getProbeResults 保存偏移快照。
- [ ] startDump 并等待 dumpPercent=100。
- [ ] listOutputFiles，大文件执行 adb pull。
- [ ] 抽样检查 Objects.txt，确认 Package、Class、核心 UE 名称连续可读。

### 6.2 地址换算 PowerShell 模板

~~~powershell
$base = [uint64]0x7965242000
'FNamePool=0x{0:x}' -f ($base + [uint64]0xBAE55C0)
'GUObjectArray=0x{0:x}' -f ($base + [uint64]0xBB29898)
'GWorld=0x{0:x}' -f ($base + [uint64]0xBCA0910)
~~~

输出应为：

~~~
FNamePool=0x7970d275c0
GUObjectArray=0x7970d6b898
GWorld=0x7970ee2910
~~~

### 6.3 成功判定标准

必须同时满足：

1. getProbeStatus.success=true。
2. getProbeResults.fieldSources.names/objects/world 均为 OVERRIDE 或经过验证的正确来源。
3. 日志中 NamePrivate、ClassPrivate 等关键字段为 validated。
4. getDumpStatus.success=true 且 dumpPercent=100。
5. Objects.txt 含连续的 Package ... 和 Class ...，无大面积 �、半词和 ERROR_EMPTY_PACKAGES。

只满足“文件存在”不算成功。

---

## 7. 本次工作的思路与经验

### 7.1 先验证语义，再追求完整导出

Dump 是结果，不是诊断工具。先用几十到几百字节的 readMemory 验证一个结构化事实，例如 FName ID=0 是否为 None，比直接生成数 MB 文件更快、更容易定位问题。

### 7.2 静态信息和运行时信息分开管理

- 模块相对偏移可跨同一构建复用。
- 绝对地址受 ASLR、进程重启和库加载顺序影响，只在当前进程有效。
- Objects 运行时指针本次为 0x794A661920，它不是可长期硬编码的模块常量。

### 7.3 把候选地址当作假设，而不是事实

一个地址“可读”只证明页权限允许访问；只有指针链、结构 Header、连续条目、语义名称和 Probe 校验全部成立，才算定位成功。

### 7.4 让工具的校验参与决策

validate_before_apply=true、NamePrivate validated、ClassPrivate validated 和 RunFixup done (success=1) 都是流程闸门。任何一个关键闸门失败，都应停止 Dump 并回退到地址验证阶段。

### 7.5 失败结果也要保留为对照样本

第一次乱码说明错误候选会产生具有迷惑性的“半成功”输出。保留它可以帮助以后快速识别 static ASCII table masquerading as FNamePool，但不能把它当 SDK 使用。

---

## 8. 本次工作成果

### 8.1 设备端

~~~
/sdcard/UnrealMemoryTools/com.huitgames.shikigami.summons/
├── AIOHeader.hpp
├── Logs.txt
├── Objects.txt
├── Offsets.hpp
├── SDK_Classes.hpp
├── SDK_Enums.hpp
├── SDK_Offset.hpp
├── SDK_Structs.hpp
└── script.json
~~~

### 8.2 本机已拉取

~~~
D:\泪心安卓领域基本盘技术\ue4ImGuiAutoWorkingMcpBytear\outputs\com.huitgames.shikigami.summons\Objects.txt
~~~

### 8.3 可复用知识

- 一套针对 Android UE4.27+ 的 MCP 串行操作顺序。
- 一套基于 libUE4.so 基址的 ASLR 地址换算方法。
- 一套 FNamePool 指针链和条目 Header 验证方法。
- 一套 Probe 成功和 Dump 成功的硬判定标准。
- 一套针对 E_MAP_STALE、乱码名称、ERROR_EMPTY_PACKAGES 和大文件响应限制的排障表。

---

## 9. 参考实现位置

- MCP 工具总览：docs/MCP可用工具文档.md
- UE 游戏定位接口：docs/api/02-UEGameProfile-引擎定位.md
- UE 偏移说明：docs/api/04-UEOffsets-偏移表.md
- 探针与转储接口：docs/api/10-Dumper-探测与转储.md
- FNamePool 解析实现：Alltear_UnrealMemoryToolsNewMcp/src/UE/UEGameProfile.cpp
- 本次拉取的对象清单：outputs/com.huitgames.shikigami.summons/Objects.txt

文档的核心复跑口诀：

~~~
新进程先取 Base，偏移必须重算；
先验 FName，再验 Objects；
覆盖后跑 Probe，成功后再 Dump；
大文件用 adb pull，最后抽样看 Package/Class。
~~~

