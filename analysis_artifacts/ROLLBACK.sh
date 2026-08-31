#!/usr/bin/env bash
set -euo pipefail

target="${1:?target path required}"
backup="${2:?backup path required}"
cp -- "$backup" "$target"
printf 'RESTORED %s FROM %s\n' "$target" "$backup"
