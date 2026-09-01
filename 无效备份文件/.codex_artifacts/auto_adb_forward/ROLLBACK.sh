#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "${script_dir}/../.." && pwd)"
target_root="$(cd -- "${1:-${repo_root}}" && pwd)"
diff_file="${script_dir}/AUTO_ADB_FORWARD.diff"

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

adb_hash="$(sha256sum "${target_root}/mcp_server/src/umt_mcp/adb.py" | awk '{print toupper($1)}')"
bridge_hash="$(sha256sum "${target_root}/mcp_server/src/umt_mcp/bridge.py" | awk '{print toupper($1)}')"
server_hash="$(sha256sum "${target_root}/mcp_server/server.py" | awk '{print toupper($1)}')"
readme_hash="$(sha256sum "${target_root}/mcp_server/README.md" | awk '{print toupper($1)}')"
tools_hash="$(sha256sum "${target_root}/mcp_server/src/umt_mcp/tools.py" | awk '{print toupper($1)}')"
resources_hash="$(sha256sum "${target_root}/mcp_server/src/umt_mcp/resources.py" | awk '{print toupper($1)}')"
instructions_hash="$(sha256sum "${target_root}/mcp_server/src/umt_mcp/instructions.py" | awk '{print toupper($1)}')"

test "${adb_hash}" = "7ED0F4B8D422C471576F14BC7186EE8A95A265D538D37178BADD577347032D6F"
test "${bridge_hash}" = "0F900AA36C6F9358619A40B1829EA647046434B0A4785F26454FAB46D4EEF91B"
test "${server_hash}" = "16018A513C7E50EB3A7D216664992550C5E5600D5614D1A31EECB8049DF3AA02"
test "${readme_hash}" = "4A2530AE92BD9B3996E085094980920C732C1B0B644B21B6A300A7B17677966B"
test "${tools_hash}" = "5B9A3CEA1431862346B3382F5A1F0D73F01B07610C24DC899B6889E89352DF84"
test "${resources_hash}" = "3D1DD064D253D5BEAD0EAD139932BAB6A7570802469A357669DAFC307F54EEA4"
test "${instructions_hash}" = "B547CDF753B5BBC73602126311EAC9690862A859B1AA9FBFF1537F58A4001163"
test ! -e "${target_root}/mcp_server/tests/test_adb.py"

if rg -q 'TOKEN_ENV_VAR|MSG_AUTH|auth_required' \
    "${target_root}/mcp_server/src/umt_mcp/bridge.py" \
    "${target_root}/mcp_server/src/umt_mcp/protocol.py"; then
    printf 'ROLLBACK_ERROR: token authentication unexpectedly returned\n' >&2
    exit 1
fi

printf 'ROLLBACK_RESULT: restored\n'
printf 'RESTORED_ADB_SHA256: %s\n' "${adb_hash}"
printf 'RESTORED_BRIDGE_SHA256: %s\n' "${bridge_hash}"
printf 'RESTORED_SERVER_SHA256: %s\n' "${server_hash}"
printf 'RESTORED_TOKEN_STATE: token authentication remains removed.\n'
printf 'RESTORED_BEHAVIOR: adb forward returns to startup-only best effort; bridge no longer refreshes it before connect/reconnect.\n'
