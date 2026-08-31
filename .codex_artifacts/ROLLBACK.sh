#!/usr/bin/env sh
set -eu
src="${1:-Alltear_UnrealMemoryToolsNewMcp/src/executable.cpp}"
copy="${2:-.codex_artifacts/rollback-executable.cpp}"
cp "$src" "$copy"
test -s "$copy"
rm -f "$copy"
printf '%s\n' 'rollback fixture restored; source left modified'
