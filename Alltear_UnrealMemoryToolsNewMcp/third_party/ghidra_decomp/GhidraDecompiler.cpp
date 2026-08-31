#include "GhidraDecompiler.hpp"
#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>
#include <unistd.h>
#if UMT_GHIDRA
#include "libdecomp.hh"
#include "raw_arch.hh"
#endif
namespace UmtMcp {
bool GhidraDecompiler::initialize(const std::string& spec, std::string& error) {
#if UMT_GHIDRA
    if (ready_) return true;
    try { ghidra::startDecompilerLibrary(std::vector<std::string>{spec}); ready_ = true; return true; }
    catch (const ghidra::LowlevelError& e) { error = e.explain; }
    catch (const std::exception& e) { error = e.what(); }
    return false;
#else
    (void)spec;
    error = "ghidra-native backend is not included in this build";
    return false;
#endif
}
std::string GhidraDecompiler::decompile(uint64_t address, const uint8_t* bytes, uint32_t size,
                                        uint32_t maxInstructions, bool, bool, std::string& error) {
#if UMT_GHIDRA
    if (!ready_) { error = "decompiler is not initialized"; return {}; }
    if (!bytes || size < 4 || size % 4) { error = "invalid ARM64 byte range"; return {}; }
    std::ostringstream path; path << "/data/local/tmp/umt_decompile_" << getpid() << ".bin";
    const std::string image = path.str();
    { std::ofstream out(image, std::ios::binary | std::ios::trunc); if (!out) { error = "unable to create temporary image"; return {}; } out.write(reinterpret_cast<const char*>(bytes), size); }
    std::ostringstream diagnostics;
    try {
        std::unique_ptr<ghidra::Architecture> arch(new ghidra::RawBinaryArchitecture(image, "AARCH64:LE:64:v8A", &diagnostics));
        ghidra::DocumentStorage store; arch->init(store); arch->loader->adjustVma(static_cast<long>(address));
        ghidra::AddrSpace* ram = arch->translate->getSpaceByName("ram");
        if (!ram) { error = "AARCH64 ram space unavailable"; unlink(image.c_str()); return {}; }
        ghidra::Address entry(ram, address);
        ghidra::Funcdata* fn = arch->symboltab->getGlobalScope()->addFunction(entry, "decompiled_function")->getFunction();
        const uint32_t bound = std::min<uint32_t>(size, std::max<uint32_t>(1, maxInstructions) * 4u);
        fn->followFlow(entry, entry + bound); arch->allacts.getCurrent()->reset(*fn);
        if (arch->allacts.getCurrent()->perform(*fn) < 0) { error = "decompilation interrupted"; unlink(image.c_str()); return {}; }
        std::ostringstream result; arch->print->setOutputStream(&result); arch->print->docFunction(fn); unlink(image.c_str()); return result.str();
    } catch (const ghidra::LowlevelError& e) { error = e.explain; }
      catch (const ghidra::DecoderError& e) { error = e.explain; }
      catch (const std::exception& e) { error = e.what(); }
    unlink(image.c_str()); return {};
#else
    (void)address; (void)bytes; (void)size; (void)maxInstructions;
    error = "ghidra-native backend is not included in this build";
    return {};
#endif
}
void GhidraDecompiler::shutdown() {
#if UMT_GHIDRA
    if (ready_) { ghidra::shutdownDecompilerLibrary(); ready_ = false; }
#else
    ready_ = false;
#endif
}
const char* GhidraDecompiler::version() { return "ghidra-native-2.1.0"; }
}
