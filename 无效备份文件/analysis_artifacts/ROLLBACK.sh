#!/usr/bin/env bash
set -eu
script_dir="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
printf '%s\n' 'UE4 analysis fixture' > "$script_dir/ROLLBACK_COPY"
printf '%s\n' 'restored baseline copy'
