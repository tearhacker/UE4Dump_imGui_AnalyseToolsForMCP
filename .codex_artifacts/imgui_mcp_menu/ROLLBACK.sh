#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "${script_dir}/../.." && pwd)"
target_root="$(cd -- "${1:-${repo_root}}" && pwd)"
diff_file="${script_dir}/UI_LAYOUT.diff"

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

executable_file="${target_root}/Alltear_UnrealMemoryToolsNewMcp/src/executable.cpp"
window_file="${target_root}/Alltear_UnrealMemoryToolsNewMcp/src/GUI/Android_draw/draw_Gui.cpp"
executable_hash="$(sha256sum "${executable_file}" | awk '{print toupper($1)}')"
window_hash="$(sha256sum "${window_file}" | awk '{print toupper($1)}')"

test "${executable_hash}" = "83A5C891D557EBB6F6C45D77BE8E25CF27506699A7815593952ACF93D47033B7"
test "${window_hash}" = "7D5C7D43BDEE12C8641DD66D770DBC8CF29B0FF8B02533FD336E3741C46B7C58"

printf 'ROLLBACK_RESULT: restored\n'
printf 'RESTORED_EXECUTABLE_SHA256: %s\n' "${executable_hash}"
printf 'RESTORED_WINDOW_SHA256: %s\n' "${window_hash}"
printf 'RESTORED_BEHAVIOR: MCP returns to Quick Actions; topbar returns to every page; initial canvas height returns to 840; contributor line is removed.\n'
