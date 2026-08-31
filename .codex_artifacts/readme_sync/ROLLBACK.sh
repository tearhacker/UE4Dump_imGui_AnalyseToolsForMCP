#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "${script_dir}/../.." && pwd)"
target_root="$(cd -- "${1:-${repo_root}}" && pwd)"
diff_file="${script_dir}/README_SYNC.diff"

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

readme_file="${target_root}/mcp_server/README.md"
readme_hash="$(sha256sum "${readme_file}" | awk '{print toupper($1)}')"
test "${readme_hash}" = "1872EA9D718C3CF9932C15E881D667980EBAD0BD574DD13FF5796EDEA479BDCB"

printf 'ROLLBACK_RESULT: restored\n'
printf 'RESTORED_README_SHA256: %s\n' "${readme_hash}"
printf 'RESTORED_BEHAVIOR: README returns to the pre-sync automatic-ADB documentation; source code is untouched.\n'
