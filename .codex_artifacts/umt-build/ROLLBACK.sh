#!/usr/bin/env bash
set -eu
copy="${1:?usage: ROLLBACK.sh <copy> [backup]>}"
backup="${2:-$copy.orig}"
test -f "$backup"
cp "$backup" "$copy"
cmp -s "$backup" "$copy"
