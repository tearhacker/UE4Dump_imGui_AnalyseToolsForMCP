# GhidraNative 反编译器模块集成方案

> 状态：**最终设计**
> 日期：2026-08-31
> 定位：Alltear_UnrealMemoryToolsNewMcp 的内嵌反编译功能模块

---

## 1. 模块定位

### 1.1 现有能力（对比）

| 模块 | 功能 | 输出 | 局限 |
|---|---|---|---|
| `Arm64Disasm` | ARM64反汇编 | 汇编指令流 | LLM难以理解语义 |
| **`GhidraDecompiler`（新增）** | ARM64反编译 | C伪代码 | 需理解控制流/变量名 |

### 1.2 模块价值

```
DISASSEMBLE → 能看指令，但看不出变量语义
DECOMPILE   → 能看C代码，LLM直接理解函数意图
```

**核心场景**：
- AutoFix候选验证：确认函数签名/参数/返回值
- native逻辑定位：伤害计算、血量写入、AC检查等
- SDK逆向辅助：结合汇编+反编译双重验证

---

## 2. 模块架构

### 2.1 目录落位

```
Alltear_UnrealMemoryToolsNewMcp/
├── third_party/ghidra_decomp/              # 反编译器模块根目录
│   ├── CMakeLists.txt                      # libghidra_decomp target
│   ├── libdecomp.a                         # WSL2交叉编译产出（ARM64静态库）
│   ├── spec/AARCH64/                       # AARCH64处理器spec
│   │   ├── AARCH64.cspec
│   │   ├── AARCH64.ldefs
│   │   ├── AARCH64.pspec
│   │   └── AARCH64.sla
│   ├── GlidraDecompiler.hpp                # 模块对外接口
│   └── GlidraDecompiler.cpp                # 模块实现
│       ├── MemLoadImage（继承ghidra::LoadImage）
│       └── GhidraEngine（封装初始化/反编译/清理）
├── src/mcp/
│   ├── DecompileHandler.hpp                # DECOMPILE命令handler
│   └── DecompileHandler.cpp
└── src/executable.cpp                      # +5行注册命令
```

### 2.2 依赖关系

```
┌─────────────────────────────────────────────────────────┐
│  Alltear_UnrealMemoryToolsNewMcp (NDK CMake可执行文件)    │
│                                                         │
│  ┌─────────────────────────────────────────────────┐   │
│  │  MCP命令层                                       │   │
│  │  DecompileHandler → GlidraDecompiler::decompile │   │
│  └──────────────────────┬──────────────────────────┘   │
│                         │                               │
│  ┌──────────────────────▼──────────────────────────┐   │
│  │  反编译模块（third_party/ghidra_decomp/）        │   │
│  │                                                 │   │
│  │  GlidraDecompiler                               │   │
│  │    ├── init(spec_dir)                          │   │
│  │    ├── decompile(address, size, ...)           │   │
│  │    └── shutdown()                              │   │
│  │        │                                       │   │
│  │        └── MemLoadImage::loadFill()            │   │
│  │                │                                │   │
│  │                └── kMgr.readMem(addr, buf, sz) │   │
│  │                     ↑                           │   │
│  │               已有能力（KittyMemoryEx）         │   │
│  │                                                 │   │
│  │  libdecomp.a ← WSL2交叉编译产出                 │   │
│  └─────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

### 2.3 模块化设计原则

1. **独立目录**：`third_party/ghidra_decomp/` 删除即功能消失
2. **CMake option**：`-DUMT_GHIDRA=OFF` 时不编译不链接
3. **前向声明**：避免循环依赖（`class KittyMemoryMgr;`）
4. **线程安全**：引擎初始化一次，后续调用串行化

---

## 3. 接口设计

### 3.1 模块接口（GlidraDecompiler.hpp）

```cpp
#pragma once
#include <string>
#include <memory>

// 前向声明，避免循环依赖
class KittyMemoryMgr;

class GhidraDecompiler {
public:
    // 初始化：加载spec + 注册AARCH64语言
    bool init(const std::string& spec_dir);
    
    // 反编译单个函数
    // address: 函数起始RVA（目标进程虚拟地址）
    // size: 函数大小（字节，必须是4的倍数）
    // max_instructions: 最大指令数（默认256）
    // max_output_bytes: 最大输出字节（默认262144）
    // optimize: 是否启用优化
    // stop_at_return: 遇到return是否停止
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
    
    // 获取最新版本信息（用于decompiler_status命令）
    static std::string version();

private:
    struct Impl;  // PIMPL隐藏实现细节
    std::unique_ptr<Impl> m_impl;
};
```

### 3.2 内存桥接（MemLoadImage）

```cpp
// 继承ghidra::LoadImage抽象类
class MemLoadImage : public ghidra::LoadImage {
public:
    MemLoadImage(uint64_t base_addr, KittyMemoryMgr& mgr);
    
    // 核心：从目标进程内存读取代码段
    void loadFill(uint1* ptr, int4 size, const Address& addr) override;
    
private:
    uint64_t m_base;                    // 模块基址
    KittyMemoryMgr& m_mgr;              // 引用外部内存管理器
};
```

### 3.3 MCP命令接口（DecompileHandler）

```cpp
namespace UmtMcp {

class DecompileHandler {
public:
    // 处理DECOMPILE命令
    static nlohmann::json Handle(const nlohmann::json& args);
    
    // 检查反编译器状态（对应DECOMPILER_STATUS命令）
    static nlohmann::json Status();
    
private:
    // 参数解析
    static uint64_t ParseAddress(const nlohmann::json& args);
    static uint32_t ParseSize(const nlohmann::json& args);
    
    // 结果格式化
    static nlohmann::json FormatSuccess(uint64_t addr, uint32_t size, 
                                         const std::string& c_code);
    static nlohmann::json FormatError(Err code, const std::string& msg);
};

}  // namespace UmtMcp
```

---

## 4. 构建流程

### 4.1 WSL2编译（一次性）

```bash
# 进入WSL2
wsl Ubuntu-22.04

# 安装依赖
sudo apt update && sudo apt install -y bison flex \
    g++-aarch64-linux-gnu make cmake

# 设置NDK路径
export NDK=/mnt/d/ProgramerDevelop/windowsNDK27

# 准备源码
mkdir -p /tmp/ghidra-build
cp -r /mnt/d/.../ilbil2cppMCP源项目/ghidra-native/src/decompiler/ /tmp/ghidra-build/
cp -r /mnt/d/.../ilbil2cppMCP源项目/ghidra-native/src/Processors/AARCH64/ /tmp/ghidra-build/
cp -r /mnt/d/.../ilbil2cppMCP源项目/ghidra-native/patches/ /tmp/ghidra-build/

# 打补丁
cd /tmp/ghidra-build
for patch in patches/*.patch; do patch -p1 < "$patch"; done

# 生成语法解析器
cd src/decompiler
bison -d grammar.y pcodeparse.y ruleparse.y slghparse.y xml.y
flex slghscan.l

# 交叉编译
cd /tmp/ghidra-build
cmake -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=30 \
      -DCMAKE_BUILD_TYPE=Release \
      -S . -B build
cmake --build build -j$(nproc)

# 产出
# build/src/decompiler/libdecomp.a（~5-10MB）
```

### 4.2 集成到Alltear

```bash
# 1. 复制静态库
cp /tmp/ghidra-build/build/src/decompiler/libdecomp.a \
   Alltear_UnrealMemoryToolsNewMcp/third_party/ghidra_decomp/

# 2. 复制spec文件（已完成）
# third_party/ghidra_decomp/spec/AARCH64/{cspec,ldefs,pspec,sla}

# 3. 编写胶水代码
# third_party/ghidra_decomp/GlidraDecompiler.{hpp,cpp}

# 4. 修改CMakeLists.txt
# 添加third_party/ghidra_decomp目录
# 链接libdecomp.a
```

---

## 5. MCP工具设计

### 5.1 工具定义

```python
# mcp_server/src/umt_mcp/tools.py

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
    - AutoFix候选验证：拿到RVA后确认函数语义
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


def decompiler_status() -> str:
    """查询反编译器模块状态。"""
    return _dev("decompiler_status")
```

### 5.2 协议映射

| MCP工具 | 原生命令 | 参数 |
|---|---|---|
| `decompile` | `DECOMPILE` | address, size, maxInstructions, maxOutputBytes, optimize, stopAtReturn |
| `decompiler_status` | `DECOMPILER_STATUS` | 无 |

### 5.3 响应格式

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

---

## 6. 实施路线

### Phase 0：质量验证（1天）

**目标**：确认Ghidra对UE4 ARM64代码的反编译质量

```bash
# WSL2中编译Linux版本（快速验证）
cd /tmp/ghidra-native
make patch
cd src/decompiler
bison -d grammar.y pcodeparse.y ruleparse.y slghparse.y xml.y
flex slghscan.l
make decomp_opt

# 测试对libUE4.so的反编译
./decomp_opt -l AARCH64 \
  -f /path/to/libUE4.so \
  -s <已知函数RVA> \
  -e <RVA+size>
```

**验收标准**：对已知函数（如`UWorld::GetGameInstance`）输出可读C代码

### Phase 1：交叉编译（1天）

```bash
# WSL2中NDK交叉编译
cmake -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=30 \
      -DCMAKE_BUILD_TYPE=Release \
      -S . -B build
cmake --build build -j$(nproc)

# 产出：build/src/decompiler/libdecomp.a
```

### Phase 2：胶水代码（0.5天）

- 创建 `third_party/ghidra_decomp/GhidraDecompiler.{hpp,cpp}`
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

**总工时：约4天**

---

## 7. 验收标准（DoD）

- [ ] WSL2交叉编译成功，产出 `libdecomp.a`
- [ ] 设备端 `DECOMPILE` 命令注册成功
- [ ] Python `decompile()` 工具可调用
- [ ] 对已知函数反编译输出可读C代码
- [ ] 契约测试通过（请求/响应格式正确）
- [ ] 错误处理正确（无效地址、超时、内存读取失败）
- [ ] 文档更新（协议、可用工具文档）

---

## 8. 风险与缓解

| 风险 | 等级 | 缓解 |
|---|---|---|
| 编译失败（ghidra-native复杂） | 🟡 | Phase 0先用Linux验证，确认能编译再交叉 |
| 反编译质量不达标 | 🟡 | Phase 0实测，若差则放弃 |
| 内存读取性能 | 🟢 | 函数代码段通常<4KB，kMgr.readMem够用 |
| 首次加载延迟 | 🟢 | spec加载1-3秒，可接受 |
| 二进制体积增加 | 🟢 | 静态库~5-10MB，可接受 |

---

## 附录：参考实现

### A. ilbil2cppMCP源项目命令格式

```
DECOMP_DECOMPILE {address} {size} {max_instr} {max_output} {optimize} {stop_at_return}
```

### B. 响应示例

```json
{
  "address": "0x7a8b9c0000",
  "size": 256,
  "c_code": "void *UWorld::GetGameInstance(void)\n{\n  return this->OwningGameInstance;\n}",
  "instructions": 42
}
```

### C. 关键依赖

- bison 3.x（生成语法解析器）
- flex 2.x（生成词法解析器）
- g++-aarch64-linux-gnu（ARM64交叉编译器）
- NDK r27（Android工具链）
