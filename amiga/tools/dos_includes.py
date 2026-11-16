#!/usr/bin/env python3
"""Out of Cash for GFOS: the DOS sources name their headers in any case
(engine.h, IDATA.h, PDATA.H for ENGINE.H, IDATA.H, PDATA.H), which DOS and
AmigaDOS take but a cross compiler on Linux does not. This makes OUT_DIR
with one link per spelling the sources use, to the header that has it.

  dos_includes.py SRC_DIR OUT_DIR

Copyright (c) 2026 AmiCradle Team. GPL-3 or later, as Out of Cash.
"""
import os
import re
import sys


def main(src, out):
    src = os.path.abspath(src)
    files = {f.lower(): f for f in os.listdir(src) if os.path.isfile(os.path.join(src, f))}
    wanted = set()
    for f in files.values():
        if f.upper().endswith((".C", ".H")):
            with open(os.path.join(src, f), encoding="latin-1") as fh:
                wanted.update(re.findall(r'^\s*#\s*include\s*"([^"/]+)"', fh.read(), re.M))
    os.makedirs(out, exist_ok=True)
    made = 0
    for name in sorted(wanted):
        real = files.get(name.lower())
        if real is None:
            continue  # a library's header (allegro.h), found on the include path
        link = os.path.join(out, name)
        if name == real:
            continue
        if os.path.lexists(link):
            os.remove(link)
        os.symlink(os.path.join(src, real), link)
        made += 1
    print("%d header names linked" % made)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1], sys.argv[2]) if len(sys.argv) == 3 else 2)
