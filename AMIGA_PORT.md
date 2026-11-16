# Out of Cash for GFOS

Out of Cash (Sin blanca) 1.4 by Warcom Soft. (GPL-3 or later), from
https://github.com/warrior-rockk/out-of-cash at commit
8dd5c620197f4b2f40666a35693f2ab34215d865, built for AmigaOS 3.2 / GFOS on
a 68040, 68060 or 68080. No FPU is needed.

Your favourite rock band is coming to town, but you haven't the money for
a ticket, and your school marks won't help you get it. A hand-drawn point
and click comedy adventure set in a Spanish high school in the 90s, full
of the period's culture. It was written for MS-DOS on a 386, in 320 x 240
with 256 colours, which is what it uses here too. The game is in Spanish;
upstream keeps an older English version on its eng-lang branch.

Status: see the pull request (stove build and lab).

## What is in the drawer

- `OutOfCash`: the game.
- `GDATA.DAT`, `PDATA.DAT`, `IDATA.DAT`, `MDATA.DAT`, `SDATA.DAT` and
  `R00DATA.DAT` to `R13DATA.DAT`: the pictures, sounds, music and rooms,
  upstream's own files as they are.
- `Libs/allegro4.library` and `Libs/acgame.library`: Allegro 4 for GFOS
  (gfopenallegro) on ACGame. The game opens them from its own `Libs/`.
- `User/`: your saved games (`savegame.001` to `savegame.005`). Updates
  and `GFOpenUpTool REMOVE` leave it alone.
- `Controls`, `Manual.txt` (the author's manual, in Spanish),
  `Licences.txt`, `COPYING.txt`.

## Playing

Run Install-OutOfCash (or copy the OutOfCash drawer where you want it),
then double-click OutOfCash, or start it from a Shell from anywhere: the
game makes its own drawer its current directory while it runs. It is
played with the mouse; `Controls` has the buttons.

## How the port is made

- The code is upstream's, built against gfopenallegro's Allegro 4 with
  68040 soft float (`amiga/Makefile.amigaos`), as C17: the DOS code
  declares functions as `f()` and calls them with arguments.
- The DOS sources name their headers in any case (`engine.h`, `IDATA.h`,
  `PDATA.H`). DOS and AmigaDOS take that, the cross compiler does not, so
  `amiga/tools/dos_includes.py` makes a link for each name used.
- The saved games go in `User/` (`SAVEGAME_NAME` in `GAME.H`), and the
  Allegro trace file is not asked for. Both are marked `__amigaos__`.
- The `.DAT` files are upstream's, made with Allegro's grabber; the host
  test reads every object back through gfopenallegro.
- Sound and music are silent for now: gfopenallegro plays samples once
  ACGame has audio, and MIDI is not played yet.

## Building

    make -f amiga/Makefile.amigaos PREFIX=<stove>/prefix A4=<gfopenallegro>/allegro4 A4OUT=... ACGOUT=...

or, in Cradle, stove_build with the os32-gcc16-open6 stove and target
package (`package/build-amiga.sh`), which also builds allegro4.library
and acgame.library at the commits `package/package.env` pins, and packs
OutOfCash-VERSION-68040.lha and the GFOpenUp part.

    tests/host/run.sh <gfopenallegro>/allegro4

runs the host tests: every object of the 19 `.DAT` files loads through
gfopenallegro's reader with the index, name and type its grabber header
gives, MDATA.DAT through an index and the two fonts with `load_dat_font`
as the game does, and every source compiles with the GFOS defines.

Designed, tested and delivered by The AmiCradle Team.
