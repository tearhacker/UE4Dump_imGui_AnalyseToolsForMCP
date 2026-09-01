# UE Memory Tools MCP 分层探测架构设计

## 核心设计原则

```
通用搜索（快速路径） → 失败 → MCP Fallback（暴力搜索/解密/模式匹配） → Dump
```

## 架构层次

### L1: 内存原语层（KittyMemory）
- `vm_rpm_ptr` - 读取指针
- `vm_rpm_str` - 读取字符串
- `kMgr.readMem/writeMem` - 内存读写

### L2: 引擎定位层（UEGameProfile）
- `GetNamesPtr()` - 定位 GNames/FNamePool
- `GetGUObjectArrayPtr()` - 定位 GUObjectArray
- `InitUEVars()` - 初始化所有引擎变量

### L3: AutoFix 层（自动偏移修正）
- `BootstrapCoreObjectArrayOffsets()` - 自动探测对象数组布局
- `AutoFixOffsets()` - 运行时验证和修正偏移
- `BruteForceDeltaForceNames()` - Delta Force 专用暴力搜索

### L4: MCP 工具层（AI 可调用能力）
- `SCAN_PATTERN` - 特征码扫描
- `SEARCH_MEMORY` - 内存搜索（支持正则/模式）
- `FIND_REFERENCES` - 引用查找
- `DECODE_ADRL` - ARM64 ADRP 解码
- `DISASSEMBLE` - 反汇编
- `DECOMPILE` - 反编译

### L5: Dump 层
- `DumpObjects()` - 导出 UObject 列表
- `DumpClasses()` - 导出 UClass 信息
- `DumpProperties()` - 导出属性信息

## 关键问题：失败时的处理

### 问题现状
```cpp
// 当前 InitUEVars() 失败时直接返回错误码
if (!kPtrValidator.isPtrReadable(_UEVars.GUObjectsArrayPtr))
    return UEVarsInitStatus::ERROR_INIT_GUOBJECTARRAY;  // ← 直接失败
```

### 期望行为
```cpp
// 通用搜索失败后，应该：
// 1. 打印详细诊断日志
// 2. 返回特殊状态码（而非直接失败）
// 3. 通知 MCP 层启动 fallback 探测
```

## 设计方案：三层探测策略

### 策略 1: 通用搜索（当前实现）
```cpp
uintptr_t IGameProfile::GetGUObjectArrayPtr() const
{
    // 基于 GNames 附近搜索
    // 验证 "/Script/CoreUObject" 锚点
    // 范围: ±0x300000
}
```

### 策略 2: 扩展搜索（新增）
```cpp
// 当通用搜索失败时，MCP 工具可以：
// 1. 使用 SEARCH_MEMORY 搜索所有可能的 TUObjectArray 地址
// 2. 验证候选地址的有效性（是否包含有效的 UObject 列表）
// 3. 返回最佳匹配
```

### 策略 3: 暴力搜索（已有部分实现）
```cpp
// AutoFixOffsets.cpp 中已有：
// - BruteForceDeltaForceNames() - Delta Force 专用
// - BootstrapCoreObjectArrayOffsets() - 自动偏移探测
```

## 建议的改进

### 1. 添加详细的失败诊断
```cpp
if (!kPtrValidator.isPtrReadable(_UEVars.GUObjectsArrayPtr))
{
    LOGE("[Bootstrap] ERROR_INIT_GUOBJECTARRAY");
    LOGI("[Bootstrap] 诊断信息:");
    LOGI("[Bootstrap]   NamesPtr = 0x%lx", (unsigned long)_UEVars.NamesPtr);
    LOGI("[Bootstrap]   GUObjectsArrayPtr = 0x%lx", (unsigned long)_UEVars.GUObjectsArrayPtr);
    LOGI("[Bootstrap]   建议: 使用 SEARCH_MEMORY 命令搜索 TUObjectArray");
    return UEVarsInitStatus::ERROR_INIT_GUOBJECTARRAY;
}
```

### 2. 添加 MCP fallback 触发机制
```cpp
// 在 executable.cpp 中添加：
void TriggerFallbackSearch(UEVarsInitStatus status)
{
    if (status == UEVarsInitStatus::ERROR_INIT_GUOBJECTARRAY)
    {
        LOGW("[Fallback] 通用搜索失败，启动 MCP fallback 探测...");
        // 这里可以调用 MCP 工具或启动异步搜索任务
    }
}
```

### 3. 添加新的 MCP 命令
```cpp
// 新增命令：PROBE_GAME_ENGINE
// 功能：自动探测 UE 引擎变量（GNames, GUObjectArray, GWorld）
// 使用：当通用搜索失败时，由 AI 调用此命令
CommandDispatcher::Register("PROBE_GAME_ENGINE", [](const json &args) -> json
{
    // 1. 尝试通用搜索
    // 2. 如果失败，使用 SEARCH_MEMORY 搜索
    // 3. 使用 SCAN_PATTERN 匹配已知模式
    // 4. 返回最佳匹配结果
});
```

## 实施计划

### 阶段 1: 完善诊断日志（当前）
- ✓ 移除多余限制
- ✓ 保留通用搜索逻辑
- → 添加详细失败诊断信息

### 阶段 2: 添加 MCP fallback 触发
- 修改 `InitUEVars()` 返回特殊状态码
- 添加 `TriggerFallbackSearch()` 函数
- 在 `executable.cpp` 中集成

### 阶段 3: 实现 PROBE_GAME_ENGINE 命令
- 整合通用搜索 + 暴力搜索 + MCP 工具
- 支持多策略并行探测
- 自动选择最佳匹配

## 文件修改清单

1. `src/UE/UEGameProfile.cpp`
   - ✓ 已恢复通用搜索逻辑
   - → 添加详细诊断日志

2. `src/executable.cpp`
   - → 添加 PROBE_GAME_ENGINE 命令
   - → 集成 fallback 触发机制

3. `src/mcp/Protocol.hpp`
   - → 添加新的错误码（如 E_PROBE_FALLINGBACK）

## 与 AI 协作流程

```
1. AI 调用 START_PROBE
2. 系统执行通用搜索
3. 如果失败，返回 ERROR_INIT_GUOBJECTARRAY
4. AI 收到错误，调用 PROBE_GAME_ENGINE
5. 系统执行扩展搜索（SEARCH_MEMORY + SCAN_PATTERN）
6. 找到目标，返回成功
7. 继续 Dump
```
