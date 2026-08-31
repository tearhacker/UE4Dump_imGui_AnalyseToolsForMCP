# Ghidra-Native 集成与 MCP 工具设计方案

> 状态：**设计草案**
> 日期：2026-08-31
> 范围：设备端集成 + MCP工具设计

---

## 1. 目标与价值

### 1.1 解决什么问题

| 现状 | 问题 |
|---|---|
| `DISASSEMBLE` 命令 | 输出ARM64汇编，LLM理解成本高 |
| `AutoFix` 候选验证 | 只能对照汇编判断对错，容易翻车 |
| native逻辑定位 | 看不出变量语义，只能猜寄存器用途 |

### 1.2 集成后能力

```
输入：函数RVA + 大小
输出：C伪代码（可读的变量名、控制流、函数调用）
```

**核心收益：LLM从"读汇编猜逻辑"升级为"读C代码确认语义"**

---

## 2. 架构决策

### 2.1 编译路线

| 路线 | 方案 | 评估 |
|---|---|---|
| A | Docker + NDK交叉编译 → .a静态库 | ✅ **推荐** |
| B | Windows本地编译（WSL2） | 备选 |
| C | dlopen预编译.so | ❌ 不可行（ABI未知） |

### 2.2 WSL2构建流程

```bash
# 1. 准备环境
wsl Ubuntu-22.04
apt update && apt install -y bison flex g++-aarch64-linux-gnu make cmake

# 2. 复制源码
cp -r /mnt/d/.../ilbil2cppMCP源项目/ghidra-native/src/decompiler/ /tmp/ghidra-decompiler/
cp -r /mnt/d/.../ilbil2cppMCP源项目/ghidra-native/src/Processors/AARCH64/ /tmp/ghidra-processors/

# 3. 打补丁
cd /tmp/ghidra-native
for patch in patches/*.patch; do patch -p1 < $patch; done

# 4. 生成语法解析器
cd src/decompiler
bison -d grammar.y pcodeparse.y ruleparse.y slghparse.y xml.y
flex slghscan.l

# 5. 交叉编译
cmake -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=30 \
      -DCMAKE_BUILD_TYPE=Release \
      -S . -B build
cmake --build build -j$(nproc)

# 6. 产出
# build/src/decompiler/libdecomp.a (~5-10MB)
```

---

## 3. 设备端集成设计

### 3.1 目录结构

```
Alltear_UnrealMemoryToolsNewMcp/
├── third_party/ghidra_decomp/              # 新增
│   ├── CMakeLists.txt                      # libghidra_decomp target
│   ├── libdecomp.a                         # 预编译静态库（WSL2产出）
│   ├── spec/AARCH64/                       # AARCH64 spec（4文件）
│   │   ├── AARCH64.cspec
│   │   ├── AARCH64.ldefs
│   │   ├── AARCH64.pspec
│   │   └── AARCH64.sla
│   └── GlidraDecompiler.hpp/.cpp           # 胶水层（~100行）
├── src/mcp/
│   ├── DecompileHandler.hpp/.cpp           # 新增：DECOMPILE命令handler
│   └── ...（其余不变）
├── src/executable.cpp                      # +5行：注册命令
├── src/mcp/Protocol.hpp                    # +2行：错误码
└── CMakeLists.txt                          # +10行：链接库
```

### 3.2 GlidraDecompiler 接口设计

```cpp
// GlidraDecompiler.hpp
#pragma once
#include <string>
#include <memory>

class KittyMemoryMgr;  // 前向声明，避免循环依赖

class GlidraDecompiler {
public:
    // 初始化：加载spec + dlopen静态库
    bool init(const std::string& spec_dir);
    
    // 反编译单个函数
    // address: 函数起始RVA
    // size: 函数大小（字节）
    // max_instructions: 最大指令数（默认256）
    // max_output_bytes: 最大输出字节（默认262144）
    // optimize: 是否优化（默认true）
    // stop_at_return: 遇到return是否停止（默认true）
    std::string decompile(
        uint64_t address,
        uint32_t size,
        uint32_t max_instructions = 256,
        uint32_t max_output_bytes = 262144,
        bool optimize = true,
        bool stop_at_return = true
    );
    
    void shutdown();
    bool is_ready() const;
    
private:
    struct Impl;  // PIMPL隐藏实现细节
    std::unique_ptr<Impl> m_impl;
};
```

### 3.3 内存读取桥接

```cpp
// MemLoadImage：继承ghidra::LoadImage抽象类
class MemLoadImage : public ghidra::LoadImage {
public:
    MemLoadImage(uint64_t base_addr, KittyMemoryMgr& mgr);
    
    // 核心：从目标进程内存读取代码段
    void loadFill(uint1* ptr, int4 size, const Address& addr) override;
    
private:
    uint64_t m_base;
    KittyMemoryMgr& m_mgr;
};
```

### 3.4 命令注册（executable.cpp）

```cpp
// 在SetupMcpCommands()中，靠近DISASSEMBLE注册处添加：
UmtMcp::CommandDispatcher::Register("DECOMPILE",
    [](const nlohmann::json& args) {
        return UmtMcp::DecompileHandler::Handle(args);
    },
    false);  // 重活
```

---

## 4. MCP工具设计

### 4.1 工具签名

```python
# mcp_server/src/umt_mcp/tools.py

@tool
def decompile(
    address: str = Field(..., description="函数起始地址（0x前缀）"),
    size: int = Field(256, ge=4, le=65536, description="函数大小（字节，必须是4的倍数）"),
    max_instructions: int = Field(256, ge=1, le=4096, description="最大反编译指令数"),
    max_output_bytes: int = Field(262144, ge=256, le=1048576, description="最大输出字节数"),
    optimize: bool = Field(True, description="是否启用优化"),
    stop_at_return: bool = Field(True, description="遇到return是否停止")
) -> str:
    """使用 Ghidra-native ARM64 反编译器将指定函数反编译为 C 伪代码。
    
    <use_case>
    - AutoFix候选验证：拿到RVA后调用此工具确认函数语义
    - native逻辑定位：理解伤害计算、血量写入等游戏逻辑
    - SDK逆向辅助：结合DISASSEMBLE，先汇编后反编译
    </use_case>
    
    <important_notes>
    - 地址必须在有效代码段内，否则返回E_READ_FAILED
    - size过大可能导致反编译超时（默认60s）
    - 首次调用需要加载spec（约1-3秒延迟）
    </important_notes>
    
    **中文触发词**：反编译、decompile、看函数、还原代码
    """
    return _dev("decompile", 
                address=address,
                size=size,
                maxInstructions=max_instructions,
                maxOutputBytes=max_output_bytes,
                optimize=optimize,
                stopAtReturn=stop_at_return)
```

### 4.2 协议映射

| MCP工具参数 | 设备端命令参数 | 类型 |
|---|---|---|
| `address` | `address` | string "0x..." |
| `size` | `size` | int |
| `max_instructions` | `maximum` | int |
| `max_output_bytes` | `max_output` | int |
| `optimize` | `optimize` | bool→int(0/1) |
| `stop_at_return` | `stop_at_return` | bool→int(0/1) |

### 4.3 响应格式

```json
{
  "ok": true,
  "data": {
    "address": "0x7a8b9c0000",
    "size": 256,
    "c_code": "void* UWorld::GetGameInstance() {\n    return this->OwningGameInstance;\n}",
    "instructions_count": 42,
    "output_bytes": 1024
  }
}
```

### 4.4 错误处理

| 错误码 | 含义 | PC侧行为 |
|---|---|---|
| `E_DECOMPILE_FAILED` | 反编译失败 | `isError: true` + 错误详情 |
| `E_READ_FAILED` | 内存读取失败 | `isError: true` + 地址 |
| `E_TIMEOUT` | 反编译超时 | `isError: true` + progress |

---

## 5. 实施路线

### Phase 0：质量验证（1天）

```bash
# WSL2中验证反编译效果
cd /mnt/d/.../ilbil2cppMCP源项目/ghidra-native
make patch
cd src/decompiler
bison -d grammar.y pcodeparse.y ruleparse.y slghparse.y xml.y
flex slghscan.l
# 编译Linux版本测试
make decomp_opt
./decomp_opt -l AARCH64 -f /path/to/libUE4.so -s <RVA> -e <RVA+size>
```

**验收标准**：对已知函数（如`UWorld::GetGameInstance`）输出可读C代码

### Phase 1：交叉编译（1天）

```bash
# WSL2中NDK交叉编译
export NDK=/path/to/ndk27
cmake -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=30 \
      -DCMAKE_BUILD_TYPE=Release \
      -S . -B build
cmake --build build -j$(nproc)
# 产出：build/src/decompiler/libdecomp.a
```

### Phase 2：胶水代码（0.5天）

- 创建 `third_party/ghidra_decomp/GlidraDecompiler.{hpp,cpp}`
- 实现 `MemLoadImage` 继承
- 实现线程安全封装

### Phase 3：命令集成（0.5天）

- 创建 `src/mcp/DecompileHandler.{hpp,cpp}`
- 注册到 `executable.cpp`
- 编译验证

### Phase 4：Python工具（0.5天）

- `mcp_server/src/umt_mcp/tools.py` 添加 `decompile()`
- 添加契约测试
- 更新文档

---

## 6. Token预算影响

| 指标 | 变化 |
|---|---|
| 工具数 | 43 → 44 (+1) |
| 单次响应 | ≤262KB输出（可配置） |
| 常驻工具 | 仍≤25（decompile属G组动态挂载） |

**建议**：默认 `max_output_bytes=262144`（256KB），AI可根据需要调整

---

## 7. 风险与缓解

| 风险 | 等级 | 缓解 |
|---|---|---|
| 编译失败（ghidra-native复杂） | 🟡 | Phase 0先用Linux验证，确认能编译再交叉 |
| 反编译质量不达标 | 🟡 | Phase 0实测，若差则放弃 |
| 内存读取性能 | 🟢 | 函数代码段通常<4KB，kMgr.readMem够用 |
| 首次加载延迟 | 🟢 | spec加载1-3秒，可接受 |
| 二进制体积增加 | 🟢 | 静态库~5-10MB，可接受 |

---

## 8. 验收标准

### DoD（Definition of Done）

- [ ] WSL2编译成功，产出 `libdecomp.a`
- [ ] 设备端 `DECOMPILE` 命令注册成功
- [ ] Python `decompile()` 工具可调用
- [ ] 对已知函数反编译输出可读C代码
- [ ] 契约测试通过（请求/响应格式正确）
- [ ] 错误处理正确（无效地址、超时等）
- [ ] 文档更新（协议、可用工具文档）

---

## 附录：参考实现

ilbil2cppMCP源项目的命令格式：
```
DECOMP_DECOMPILE {address} {size} {max_instr} {max_output} {optimize} {stop_at_return}
```

响应格式（参考）：
```json
{
  "address": "0x...",
  "size": 256,
  "c_code": "...",
  "instructions": 42
}
```

---

## 9. 架构澄清（重要）

### 9.1 项目类型

| 项目 | 类型 | 运行方式 |
|---|---|---|
| **Alltear_UnrealMemoryToolsNewMcp** | **独立Android应用**（NDK CMake可执行文件） | `adb push` → 设备运行 |
| ilbil2cppMCP源项目 | Magisk Zygisk模块 | 注入到游戏进程 |

### 9.2 内存访问方式

```cpp
// executable.cpp:887
kMgr.initialize(candidate.pid, EK_MEM_OP_SYSCALL, false);
// 或
kMgr.initialize(candidate.pid, EK_MEM_OP_IO, false);

// 读取目标进程内存
size_t bytesRead = UEMemory::kMgr.readMem(addr, buf.data(), size);
```

**关键**：Alltear通过 `KittyMemoryMgr` 以**外部调试器**方式读取目标进程内存，
不需要注入到目标进程内部。

### 9.3 Ghidra集成位置

```
┌─────────────────────────────────────────────────────────────┐
│  Alltear_UnrealMemoryToolsNewMcp (独立App)                    │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  src/mcp/DecompileHandler.cpp                       │   │
│  │    ↓                                                │   │
│  │  third_party/ghidra_decomp/                          │   │
│  │    ├── libdecomp.a (ARM64静态库)                     │   │
│  │    ├── GlidraDecompiler.cpp                         │   │
│  │    │    ↓                                            │   │
│  │    │  MemLoadImage::loadFill()                      │   │
│  │    │    ↓                                            │   │
│  │    │  kMgr.readMem(addr, buf, size) ← 已有能力      │   │
│  │    └── spec/AARCH64/                                │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

**不需要注入到游戏进程**，ghidra在Alltear进程内运行，通过kMgr读取目标进程内存。

### 9.4 与Zygisk方案的本质区别

| 维度 | Zygisk方案 | Alltear方案 |
|---|---|---|
| 运行位置 | 注入游戏进程内部 | 独立调试器App |
| 内存访问 | 直接指针操作 | ptrace/readMem |
| 稳定性 | 依赖游戏进程存活 | 独立运行，更稳定 |
| 检测风险 | 高（可能触发AC） | 低（外部调试器） |
| 编译复杂度 | 需要NDK .so | 需要NDK .a + 链接 |

**Alltear方案更适合逆向分析场景**——不会被游戏AC检测为注入行为。
