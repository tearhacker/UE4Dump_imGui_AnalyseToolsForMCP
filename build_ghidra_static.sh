#!/usr/bin/env bash
set -euo pipefail
ROOT="/mnt/d/泪心安卓领域基本盘技术/ue4ImGuiAutoWorkingMcpBytear"
SRC="$ROOT/ilbil2cppMCP源项目/ghidra-native"
BUILD="$ROOT/.codex_artifacts/ghidra-build"
NDK=/mnt/d/ProgramerDevelop/windowsNDK27
rm -rf "$BUILD"
cp -r "$SRC" "$BUILD"
cd "$BUILD"
find src patches -type f ! -name .gitignore -exec perl -pi -e 's/\x0D//' {} +
for p in patches/*.patch; do patch --ignore-whitespace -p1 < "$p"; done
cd src/decompiler
bison -d grammar.y
bison -d pcodeparse.y
bison -p ruleparse -d -o ruleparse.cc ruleparse.y
bison -d slghparse.y
bison -d xml.y
flex -L -o slghscan.cc slghscan.l
export CXX="$NDK/toolchains/llvm/prebuilt/windows-x86_64/bin/aarch64-linux-android30-clang++"
make clean || true
make -j2 CXX="$CXX -std=c++11" ARCH_TYPE= EXTRA= ADDITIONAL_FLAGS='-fPIC -Wno-sign-compare' libdecomp.a
echo "BUILT=$BUILD/src/decompiler/libdecomp.a"
ls -lh "$BUILD/src/decompiler/libdecomp.a"
file "$BUILD/src/decompiler/libdecomp.a"
cp "$BUILD/src/decompiler/libdecomp.a" "$ROOT/Alltear_UnrealMemoryToolsNewMcp/third_party/ghidra_decomp/libdecomp.a"
echo "INSTALLED=$ROOT/Alltear_UnrealMemoryToolsNewMcp/third_party/ghidra_decomp/libdecomp.a"
