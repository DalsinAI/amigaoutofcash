#!/bin/sh
# Out of Cash for GFOS: host tests.
#   tests/host/run.sh [GFOPENALLEGRO/allegro4]
# Upstream's .DAT files read back through gfopenallegro's allegro4 core on
# its fake acgame.library; and the game's sources compile on the host with
# amiga/tools/dos_includes.py's header names, as the 68040 build does.
# Copyright (c) 2026 AmiCradle Team. GPL-3 or later, as Out of Cash.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
A4=${1:-"$ROOT/../gfopenallegro/allegro4"}
CC=${HOSTCC:-cc}
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT HUP INT TERM
"$CC" -std=gnu99 -D_DEFAULT_SOURCE -w -I"$A4/include" -I"$A4/src" -I"$A4/tests" \
    "$A4"/src/a4_*.c "$A4"/tests/fake_acgame.c "$ROOT/tests/host/test_data.c" -lm -o "$T/test_data"
"$T/test_data" "$ROOT"
python3 "$ROOT/amiga/tools/dos_includes.py" "$ROOT" "$T/inc"
n=0
for f in "$ROOT"/*.C; do
    "$CC" -x c -std=gnu17 -fsyntax-only -w -fgnu89-inline -D__amigaos__ -I"$A4/include" -I"$T/inc" -I"$ROOT" "$f"
    n=$((n + 1))
done
echo "sources: $n compile with the GFOS defines"
