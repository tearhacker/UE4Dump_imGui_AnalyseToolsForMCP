#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "${script_dir}/../.." && pwd)"
target_root="$(cd -- "${1:-${repo_root}}" && pwd)"
diff_file="${script_dir}/TOKEN_AUTH_REMOVAL.diff"

if [[ "${target_root}" =~ ^/([A-Za-z])/(.*)$ ]]; then
    drive_root="/${BASH_REMATCH[1]}"
    target_prefix="${BASH_REMATCH[2]}"
else
    drive_root="/"
    target_prefix="${target_root#/}"
fi

git -c core.autocrlf=false -c core.eol=lf -C "${drive_root}" apply \
    --ignore-space-change --reverse --check --unsafe-paths \
    --directory="${target_prefix}" "${diff_file}"
git -c core.autocrlf=false -c core.eol=lf -C "${drive_root}" apply \
    --ignore-space-change --reverse --unsafe-paths \
    --directory="${target_prefix}" "${diff_file}"

# The task baseline used CRLF in this test file; git apply materializes LF.
perl -pi -e 's/\r?\n/\r\n/g' "${target_root}/mcp_server/tests/test_bridge.py"

executable_hash="$(sha256sum "${target_root}/Alltear_UnrealMemoryToolsNewMcp/src/executable.cpp" | awk '{print toupper($1)}')"
server_hash="$(sha256sum "${target_root}/Alltear_UnrealMemoryToolsNewMcp/src/mcp/CommandServer.cpp" | awk '{print toupper($1)}')"
header_hash="$(sha256sum "${target_root}/Alltear_UnrealMemoryToolsNewMcp/src/mcp/CommandServer.hpp" | awk '{print toupper($1)}')"
bridge_hash="$(sha256sum "${target_root}/mcp_server/src/umt_mcp/bridge.py" | awk '{print toupper($1)}')"
python_server_hash="$(sha256sum "${target_root}/mcp_server/server.py" | awk '{print toupper($1)}')"
readme_hash="$(sha256sum "${target_root}/mcp_server/README.md" | awk '{print toupper($1)}')"
tests_hash="$(sha256sum "${target_root}/mcp_server/tests/test_bridge.py" | awk '{print toupper($1)}')"

test "${executable_hash}" = "EF25E06609CECF678D85A9D89BBEA8770354204FC771C8AE7B21F8A917FE5A59"
test "${server_hash}" = "27DF5422C796CFD743C7A4D0140DF74DB21CB9DD08E53A0F0380DB12B6E23ABB"
test "${header_hash}" = "3529DB8CE4B2206DE98BD5CA04F30EF505FE25A60B9A05839D6325C82B6D9DF1"
test "${bridge_hash}" = "6B7DD047F6FA3BD7BA3E40E4FE7061BC63853867BA85EAF9A0C1BCF36D369C8A"
test "${python_server_hash}" = "0C37FA9E1BBB791A7970B6B456FA1166CAE28FFA4B0C22F56299D93FBE2457C4"
test "${readme_hash}" = "9AE1D8B9B0BA4AA9AE08A8AE81DE8BC79EDE7875535797F06600CA27D2FC2448"
test "${tests_hash}" = "5F4FE7AEF53484413878EA88A18B0B09D523D6C4C3AAD16CFA0CAE0B58DDD502"

printf 'ROLLBACK_RESULT: restored\n'
printf 'RESTORED_EXECUTABLE_SHA256: %s\n' "${executable_hash}"
printf 'RESTORED_COMMAND_SERVER_SHA256: %s\n' "${server_hash}"
printf 'RESTORED_BRIDGE_SHA256: %s\n' "${bridge_hash}"
printf 'RESTORED_TESTS_SHA256: %s\n' "${tests_hash}"
printf 'RESTORED_BEHAVIOR: random token generation and AUTH-before-command enforcement are restored; prior ImGui layout changes remain.\n'
