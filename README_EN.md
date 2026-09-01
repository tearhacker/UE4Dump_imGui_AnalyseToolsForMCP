# UE4 ImGui MCP — Android UE4/UE5 Game Reverse-Engineering Platform

> **AI-driven UE4/UE5 memory analysis toolkit**: automatic engine global detection, memory scanning, SDK dump, ARM64 disassembly/decompilation, remote function calls.
>
> License: **GPL-3.0** (commercial use allowed; modifications must be open-sourced)

---

## 📌 In One Sentence

Transform a mobile UE4 game injector into an **MCP server**, enabling AI LLMs to autonomously complete the full reverse-engineering loop — locate offsets → read memory → parse SDK → disassemble — without manual UI interaction.

```
AI (Claude / Cursor / WorkBuddy)
   │ MCP stdio · tool calls
   ▼
mcp_server/        (Python, FastMCP, 43~47 tools)
   │ TCP + NDJSON · 127.0.0.1:35515 · adb forward
   ▼
Alltear_UnrealMemoryToolsNewMcp/   (Android ELF, C++ NDK27)
   │ Memory reads · UE probe · AutoFix · ptrace · Ghidra
   ▼
Target game process (com.tencent.letsgo / com.tencent.tmgp.pubgmhd / ...)
```

---

## 🎯 Core Capabilities

| Capability | Tool Examples | Description |
|------------|---------------|-------------|
| **Engine auto-probe** | `startProbe` / `locateEngineGlobals` | Auto-detect GNames / GUObjectArray / GWorld |
| **Memory read/write/scan** | `readMemory` / `writeMemory` / `scanPattern` | Byte-range + IDA-style pattern search |
| **SDK dump** | `startDump` / `dumpSDK` | Export Classes/Structs/Functions header files |
| **ARM64 disassembly** | `disassemble` / `decodeAdrl` | PC-side capstone |
| **ARM64 decompilation** | `decompile` / `decompileStatus` | Embedded Ghidra decompiler (optional) |
| **Remote function call** | `callRemoteFunction` / `callRemoteFunctionBatch` | ptrace injection; Batch variant recommended (stateless) |
| **Engine semantic search** | `searchClasses` / `describeClass` / `inspectObject` | Class lookup by name/type; object field expansion |
| **Pointer chain tracking** | `followPointerChain` | Supports `[k]` array indexing (`LocalPlayers[0]`) |
| **Candidate scanning** | `scanGNames` / `scanObjects` / `scanCandidates` | Inlined samples + scoring; 3 rounds compressed to 1 |

---

## 📁 Project Structure

```
ue4ImGuiAutoWorkingMcpBytear/
├── LICENSE                           # GPL-3.0 license (modifications must be open-sourced)
├── README.md                         # This file (English)
├── Alltear_UnrealMemoryToolsNewMcp/  # MCP version (Android C++ ELF)
│   ├── src/                          # Source code
│   │   ├── mcp/                    # Command service layer
│   │   │   ├── CommandServer.{hpp,cpp}
│   │   │   ├── CommandDispatcher.{hpp,cpp}
│   │   │   ├── CommandQueue.{hpp,cpp}
│   │   │   └── Protocol.hpp
│   │   ├── UE/                     # UE engine layer
│   │   │   ├── UEGameProfile.cpp   # Core probe logic
│   │   │   ├── UEOffsets.cpp       # Offset tables
│   │   │   └── UEWrappers.cpp      # Object wrappers
│   │   ├── AutoFix/                # Auto offset correction (3185 LOC)
│   │   ├── executable.cpp          # Main entry + MCP command registration (3589 LOC)
│   │   └── Dumper.cpp              # Dump pipeline
│   ├── build/                        # CMake build artifacts (NDK27 + VS2026 CMake/Ninja)
│   └── outputs/arm64-v8a/          # Compiled binary: UnrealMemoryTools
├── Andriod_UnrealMemoryTools/        # Original reference (probe-success baseline)
├── mcp_server/                       # Python MCP server
│   ├── src/umt_mcp/
│   │   ├── tools.py                # 43~47 tool registrations (camelCase)
│   │   ├── bridge.py               # Device-side socket bridge (background reader + reconnect)
│   │   ├── protocol.py             # NDJSON protocol
│   │   └── adb.py                  # ADB forward management
│   └── server.py                   # FastMCP entry point
├── docs/                             # Full documentation suite (27 files)
│   ├── api/                        # Device-side API deep-dive (15 docs)
│   ├── 先行开发修正文档手册.md         # Code diff & issue triage (2026-09-01)
│   ├── UMT_改动代码文档.md           # MCP vs original changes
│   ├── mcp-protocol.md             # Wire protocol spec
│   ├── MCP可用工具文档.md            # Tool specs
│   ├── 泪心UE4_imGui_AnalyseToolsForMCp开发必读架构.md  # Architecture v1.2
│   └── ...
├── ilbil2cppMCP源项目/               # il2cpp MCP reference (for comparison)
└── analysis_artifacts/               # Diff / patch / log comparison outputs
```

---

## 🔧 Build & Run

### Device Side (Android C++)

```bash
cd Alltear_UnrealMemoryToolsNewMcp

# Configure CMake (NDK27 + VS2026 CMake/Ninja)
CMAKE="D:/ProgramerDevelop/VS2026/SDK/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"
"$CMAKE" -S . -B build -DNDK_PATH=D:/ProgramerDevelop/windowsNDK27

# Incremental build (rm .o files before rebuild if source changed)
"$CMAKE" --build build

# Artifact: outputs/arm64-v8a/UnrealMemoryTools
# Verify new code: grep feature strings via NDK llvm-strings
llvm-strings.exe outputs/arm64-v8a/UnrealMemoryTools | grep "诊断"
```

**Known blocker**: `ANativeWindowCreator::ProcessMirrorDisplay()` is called at executable.cpp:4288 but the member was deleted from the header. Fixing this is out of scope for the MCP task.

### PC-Side MCP Server

```bash
cd mcp_server

# Start virtual environment (first time)
python -m venv .venv
.venv/Scripts/activate

# Install dependencies
pip install -r requirements.txt

# Start (requires adb forward)
python server.py
```

**Three network modes** (implemented):

| Mode | Scenario | Config |
|------|----------|--------|
| Mode 1 | LAN direct connect | Write `192.168.x.x` to `/sdcard/UnrealMemoryTools/mcp_bind.conf` |
| Mode 2 | Global expose (dangerous!) | Write `0.0.0.0` to config; use only on trusted networks |
| Mode 3 | Same-device (Operit) | No adb needed; `--no-adb --host 127.0.0.1` |

---

## 📚 Documentation Reading Order

> Read in this order when onboarding. Do not skip.

```
1. overview.md              → High-level project overview
2. 泪心UE4_imGui_AnalyseToolsForMCp功能说明.md   → What it does & why
3. 泪心UE4_imGui_AnalyseToolsForMCp开发必读架构.md → How to write & conventions
4. MCP可用工具文档.md       → 43~47 tool specs quick reference
5. mcp-protocol.md         → Wire protocol between sides
6. docs/api/00-索引.md     → Device-side 15-module API deep-dive index
7. UMT_改动代码文档.md      → MCP vs original change log
8. 先行开发修正文档手册.md   → Known issues & fix directions (2026-09-01)
```

---

## ⚠️ Known Issues (as of 2026-09-01)

> See `docs/先行开发修正文档手册.md` for details. Not yet fixed; pending triage.

| Priority | Issue | Symptom | Root Cause |
|----------|-------|---------|------------|
| **P0** | LetsGo probe failure | `ERROR_INIT_GUOBJECTARRAY`, `id=1392538734 name=''` | `AutoFix.cpp:186` forces `_outlineNumber=true`; verifyCandidate filters `id > 0x200000` |
| **P0** | pubgmhd (Peacekeeper Elite) probe failure | `ERROR_INIT_GUOBJECTARRAY` | `PUBGMHD.hpp` hardcodes `IsUsingFNamePool()=false`, but device scan finds FNamePool |
| **P1** | `NumElements` offset controversy | Original 0x14 vs MCP 0x10 (UE4_20/UE4_23_24) | Requires real-device verification |
| **P1** | LetsGo probe instability after restart | `ERROR_INIT_OBJOBJECTS`; needs manual `APPLY_PROBE_OVERRIDES` | Auto-probe robustness insufficient |
| — | Overall maturity score | **7/10** | Strong functional coverage; weak transport resilience |

**Confirmed consistent (excluded as suspect)**: `AutoFixOffsets.cpp` (3185 LOC), `Dumper.cpp`, probe/dump execution order, both versions' `GetNameEntry` logic.

---

## 🔐 Security Notes

- Device side binds to `127.0.0.1:35515` by default (loopback only) — **does not expose root-level memory access to LAN**.
- Non-loopback bind requires explicit write to `/sdcard/UnrealMemoryTools/mcp_bind.conf`; clear config and restart game when done.
- Dangerous operations (memory write, ptrace remote call) require explicit `confirm_dangerous=true`.
- No token authentication — intentional design for single-pair debugging; use only on trusted networks.

---

## 📄 License

**GPL-3.0** (GNU General Public License v3)

- ✅ Commercial use allowed
- ✅ Modifications allowed
- ❌ Modified distributions must open-source source code (strong copyleft)
- See root `LICENSE` file for full text

---

## 🤝 Contributing

1. Fork this repository
2. Develop on a `dev-*` branch
3. Rebuild device-side code after changes (NDK27 + CMake/Ninja)
4. Update `docs/UMT_改动代码文档.md` to record changes
5. Submit PR with description of purpose and verification method

---

*Project dates: 2026-08-29 ~ 2026-09-01*
*Maintained by: 泪心安卓领域基本盘技术*
