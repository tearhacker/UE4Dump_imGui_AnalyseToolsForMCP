#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
patch_file="$script_dir/DIFF_FILE.patch"
target_root="${1:-$(git -C "$script_dir/../.." rev-parse --show-toplevel)}"

if [[ ! -f "$patch_file" ]]; then
  printf 'ROLLBACK_ERROR: patch not found: %s\n' "$patch_file" >&2
  exit 2
fi

git -C "$target_root" apply -R --check "$patch_file"
git -C "$target_root" apply -R "$patch_file"
git -C "$target_root" apply --check "$patch_file"

printf 'ROLLBACK_OK: restored baseline in %s\n' "$target_root"
