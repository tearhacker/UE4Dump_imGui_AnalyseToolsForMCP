#pragma once
#include <cstdint>
#include <string>
namespace UmtMcp {
class GhidraDecompiler {
public:
    bool initialize(const std::string&, std::string&);
    std::string decompile(uint64_t, const uint8_t*, uint32_t, uint32_t, bool, bool, std::string&);
    void shutdown();
    bool ready() const { return ready_; }
    static const char* version();
private:
    bool ready_ = false;
};
}
