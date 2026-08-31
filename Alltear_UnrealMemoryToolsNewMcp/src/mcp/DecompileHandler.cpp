#include "DecompileHandler.hpp"
#include "Arm64Disasm.hpp"
#include "MemoryHelpers.hpp"
#include "../UE/UEMemory.hpp"
#include <algorithm>
#include <sstream>
#include <mutex>

namespace UmtMcp {

// 静态成员初始化
bool DecompileHandler::s_initialized = false;
DecompilerStatus DecompileHandler::s_status;
GhidraDecompiler DecompileHandler::s_engine;
static std::mutex g_decompileMutex;

bool DecompileHandler::Initialize(const std::string& spec_dir) {
    std::lock_guard<std::mutex> lock(g_decompileMutex);
    std::string error;
    if (!s_engine.initialize(spec_dir, error)) { s_status = {false, GhidraDecompiler::version(), error}; return false; }
    s_initialized = true; s_status = {true, GhidraDecompiler::version(), {}}; return true;
}

bool DecompileHandler::IsInitialized() {
    return s_initialized;
}

json DecompileHandler::Status() {
    json result;
    result["ready"] = s_status.ready;
    result["version"] = s_status.version;
    if (!s_status.error.empty()) {
        result["error"] = s_status.error;
    }
    return result;
}

json DecompileHandler::Handle(const json& args) {
    // 检查是否 attach
    if (!UEMemory::kMgr.isMemValid()) {
        throw HandlerError(Err::kNotAttached, "未 attach 到目标进程");
    }
    
    // 解析参数
    std::string addrStr = args.value("address", "");
    uintptr_t addr = 0;
    if (!ParseAddress(addrStr, addr)) {
        throw HandlerError(Err::kBadArgs, "address 格式无效: " + addrStr);
    }
    
    uint32_t size = args.value("size", 256u);
    if (size % 4 != 0) {
        throw HandlerError(Err::kBadArgs, "size 必须是 4 的倍数");
    }
    if (size < 4 || size > 65536) {
        throw HandlerError(Err::kBadArgs, "size 须在 [4, 65536] 范围内");
    }
    
    uint32_t max_instr = args.value("maxInstructions", 256u);
    uint32_t max_output = args.value("maxOutputBytes", 262144u);
    bool optimize = args.value("optimize", true);
    bool stop_at_return = args.value("stopAtReturn", true);
    
    std::vector<uint8_t> code(size);
    size_t read = UEMemory::kMgr.readMem(addr, code.data(), size);
    if (read < 4) {
        throw HandlerError(Err::kReadFailed, "无法读取代码段");
    }

    if (!s_initialized && !Initialize("/data/local/tmp/UnrealMemoryTools/ghidra_decomp/spec"))
        throw HandlerError(Err::kDecompileFailed, s_status.error);
    std::lock_guard<std::mutex> lock(g_decompileMutex);
    std::string error;
    const std::string cCode = s_engine.decompile(addr, code.data(), static_cast<uint32_t>(read), max_instr, optimize, stop_at_return, error);
    if (cCode.empty()) throw HandlerError(Err::kDecompileFailed, error.empty() ? "反编译失败" : error);
    return {{"address", FormatAddress(addr)}, {"size", (int)read}, {"mode", "ghidra-native"},
            {"c_code", cCode}, {"instructions_count", (int)std::min<size_t>(read / 4, max_instr)}, {"output_bytes", (int)cCode.size()}};
    
    // 旧汇编降级路径保留在历史实现中，不会执行。
    json instructions = json::array();
    uintptr_t pc = addr;
    size_t off = 0;
    while (off + 4 <= read && instructions.size() < max_instr) {
        uint32_t insn = 0;
        memcpy(&insn, code.data() + off, 4);
        std::string text = DisassembleArm64(pc, insn);
        instructions.push_back({
            {"address", FormatAddress(pc)},
            {"bytes", BytesToHex(code.data() + off, 4)},
            {"text", text}
        });
        pc += 4;
        off += 4;
    }
    
    return {
        {"address", FormatAddress(addr)},
        {"size", (int)size},
        {"mode", "assembly_fallback"},  // 标记为降级方案
        {"c_code", ""},  // 暂为空
        {"instructions", instructions},
        {"instructions_count", (int)instructions.size()},
        {"note", "Ghidra 反编译器将在 Phase 2 集成后启用"}
    };
}

}  // namespace UmtMcp
