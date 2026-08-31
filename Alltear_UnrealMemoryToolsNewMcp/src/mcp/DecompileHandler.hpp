#pragma once

#include <string>
#include <cstdint>
#include "Protocol.hpp"
#include "CommandDispatcher.hpp"
#include "../../third_party/ghidra_decomp/GhidraDecompiler.hpp"

namespace UmtMcp {

// 反编译器模块状态
struct DecompilerStatus {
    bool ready = false;
    std::string version;
    std::string error;
};

// DECOMPILE 命令处理器
class DecompileHandler {
public:
    // 处理 DECOMPILE 命令
    static json Handle(const json& args);
    
    // 处理 DECOMPILER_STATUS 命令
    static json Status();
    
    // 初始化反编译器（静态，只调用一次）
    static bool Initialize(const std::string& spec_dir);
    
    // 检查是否已初始化
    static bool IsInitialized();

private:
    static bool s_initialized;
    static DecompilerStatus s_status;
    static GhidraDecompiler s_engine;
};

}  // namespace UmtMcp
