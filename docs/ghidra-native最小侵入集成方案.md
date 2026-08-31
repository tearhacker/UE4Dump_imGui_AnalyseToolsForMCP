# ghidra-native 最小侵入集成方案

> 状态：**最终设计方案**（已调研完整源码）
> 日期：2026-08-31
> 核心原则：**零侵入、可裁剪、单文件胶水、dlopen预编译so**

---

## 1. 调研结论

### 1.1 ghidra-native源码关键发现

| 发现 | 影响 |
|---|---|
| 导出符号仅4个：`ghidra_decompiler_initialize` / `decompile` / `free` / `api_version` | 可用dlopen直接调用，无需重新编译 |
| `LoadImage`抽象接口仅1个纯虚函数：`loadFill(uint1*ptr,int4size,constAddress&addr)` | Glue代码极简（~30行） |
| 初始化只需指定spec目录路径 | 无需Java、无需重型依赖 |
| 源码155K行但预编译so仅3.3MB | dlopen路线完全可行 |

### 1.2 预编译.so验证

```
arm64-v8a.so (3.3MB) 导出符号：
  - ghidra_decompiler_api_version
  - ghidra_decompiler_initialize(const char* sleighhome)
  - ghidra_decompiler_decompile(address, size, max_instr, max_output, optimize, stop_at_return)
  - ghidra_decompiler_free(char* result)
```

### 1.3 参考实现（ilbil2cppMCP源项目）

- **无C++源码**，只有预编译zygisk.so + decompiler.so
- Python侧通过socket命令 `DECOMP_STATUS` / `DECOMP_DECOMPILE` 调用
- 命令格式：`DECOMP_DECOMPILE {address} {size} {max_instr} {max_output} {optimize} {stop_at_return}`

---

## 2. 最小侵入方案

### 2.1 目录结构

```
Alltear_UnrealMemoryToolsNewMcp/
├── third_party/ghidra_decomp/          ← 新增（删除即功能消失）
│   ├── arm64-v8a.so                    ← 预编译反编译器
│   ├── spec/AARCH64/                   ← AARCH64 spec（4文件）
│   │   ├── AARCH64.cspec
│   │   ├── AARCH64.ldefs
│   │   ├── AARCH64.pspec
│   │   └── AARCH64.sla
│   └── GlidraDecompiler.{hpp,cpp}      ← 新增（唯一新增C++文件）
│       └── 封装dlopen调用，提供decompile()接口
├── src/mcp/
│   └── DecompileHandler.{hpp,cpp}      ← 新增（命令handler）
└── CMakeLists.txt                      ← 修改（+option +target）
```

### 2.2 修改文件清单（仅3处）

| 文件 | 改动量 | 说明 |
|---|---|---|
| `third_party/ghidra_decomp/GlidraDecompiler.hpp` | ~50行 | 新建：dlopen封装 |
| `third_party/ghidra_decomp/GlidraDecompiler.cpp` | ~80行 | 新建：实现 |
| `src/mcp/DecompileHandler.hpp` | ~30行 | 新建：命令handler头 |
| `src/mcp/DecompileHandler.cpp` | ~100行 | 新建：命令handler实现 |
| `src/executable.cpp` | +5行 | 注册DECOMPILE命令 |
| `src/mcp/Protocol.hpp` | +2行 | 新增错误码 |
| `CMakeLists.txt` | +10行 | 添加option + target |
| **总计** | **~300行新增，6处修改** | |

### 2.3 GlidraDecompiler核心接口

```cpp
// GlidraDecompiler.hpp
class GlidraDecompiler {
public:
    bool init(const std::string& spec_dir);       // dlopen + 初始化
    std::string decompile(uint64_t address,       // 反编译函数
                          uint32_t size,
                          uint32_t max_instr = 256,
                          uint32_t max_output = 262144,
                          bool optimize = true,
                          bool stop_at_return = true);
    void shutdown();
    bool is_ready() const;
private:
    void* m_handle;                               // dlopen句柄
    // ... 函数指针
};
```

### 2.4 MemLoadImage（唯一需要自定义的部分）

```cpp
// 继承LoadImage抽象类，实现loadFill
class MemLoadImage : public ghidra::LoadImage {
public:
    MemLoadImage(uint64_t base_addr, KittyMemoryMgr& mgr)
        : LoadImage("mem"), m_base(base_addr), m_mgr(mgr) {}
    
    void loadFill(uint1* ptr, int4 size, const Address& addr) override {
        uint64_t offset = addr.getOffset();
        auto data = m_mgr.readMem(m_base + offset, size);
        memcpy(ptr, data.data(), size);
    }
    // ... 其他虚函数保持默认实现
};
```

### 2.5 命令注册（executable.cpp +5行）

```cpp
// 在SetupMcpCommands()中，靠近DISASSEMBLE注册处添加：
UmtMcp::CommandDispatcher::Register("DECOMPILE", 
    [](const nlohmann::json& args) {
        return UmtMcp::DecompileHandler::Handle(args);
    }, false);  // 重活
```

---

## 3. 实施步骤

### Step 1: 复制资产（5分钟）
```bash
# spec文件已复制，还需：
cp ilbil2cppMCP源项目/decompiler/arm64-v8a.so \
   Alltear_UnrealMemoryToolsNewMcp/third_party/ghidra_decomp/
```

### Step 2: 写GlidraDecompiler（30分钟）
- 创建 `third_party/ghidra_decomp/GlidraDecompiler.{hpp,cpp}`
- 实现dlopen + 4个导出函数调用
- 实现MemLoadImage继承

### Step 3: 写DecompileHandler（20分钟）
- 创建 `src/mcp/DecompileHandler.{hpp,cpp}`
- 参数解析 + 引擎调用 + 结果格式化
- 复用现有jobId长任务模式

### Step 4: 接线（10分钟）
- `executable.cpp` 注册命令
- `Protocol.hpp` 加错误码
- `CMakeLists.txt` 加target

### Step 5: Python侧（10分钟）
- `mcp_server/src/umt_mcp/tools.py` 加 `decompile` tool
- 参数映射：address/size/max_instructions/max_output_bytes/optimize/stop_at_return

---

## 4. 验证方法

### 4.1 编译验证
```bash
cd Alltear_UnrealMemoryToolsNewMcp
cmake -S . -B build -DNDK_PATH=D:/ProgramerDevelop/windowsNDK27
cmake --build build
# 验证产物含 DECOMPILE 特征串
llvm-strings outputs/arm64-v8a/UnrealMemoryTools | grep DECOMPILE
```

### 4.2 功能验证（Docker Phase 0）
```bash
# 先验证反编译质量
docker run --rm -v $(pwd):/work ubuntu:22.04 bash -c '
  apt-get update && apt-get install -y bison flex g++ make
  cd /work/ilbil2cppMCP源项目/ghidra-native
  make patch && make -C src/decompiler decomp_opt
  ./src/decompiler/decomp_opt -l AARCH64 \
    -f <libUE4.so地址> -s <函数RVA> -e <RVA+size>
'
```

---

## 5. 风险与缓解

| 风险 | 等级 | 缓解 |
|---|---|---|
| 预编译.so ABI未知 | 🟡 | Step 2先写dlopen测试程序验证参数语义 |
| 内存读取性能 | 🟢 | 只读函数代码段，通常<4KB，kMgr.readMem够用 |
| 首次加载延迟 | 🟢 | spec加载约1-3秒，可接受 |
| 二进制体积增加 | 🟢 | dlopen延迟加载，不增加主二进制 |

---

## 6. 备选方案：源码自编译（路线B）

若预编译.so ABI有问题，可走源码编译：
1. Docker中用bison/flex生成grammar.cc等
2. NDK CMake编译libdecomp.a（~90个.cc）
3. 链接到主二进制（+3-8MB）

**当前推荐：先试路线A（dlopen），失败再转B。**
