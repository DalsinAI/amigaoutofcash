/* Out of Cash for GFOS: host test of the game's data.
 * Every .DAT upstream ships loads through gfopenallegro's allegro4 reader
 * (load_datafile, and create_datafile_index for MDATA.DAT as the game
 * does); each object has the index, name and type its grabber header
 * gives; and the fonts load with load_dat_font as GAME.C asks for them.
 * Copyright (c) 2026 AmiCradle Team. GPL-3 or later, as Out of Cash. */
#include <stdio.h>
#include <string.h>
#include "allegro.h"

static int fails;
#define CHECK(c, ...) do { if (!(c)) { fails++; printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static int type_of(const char *t)
{
    if (!strcmp(t, "BMP")) return DAT_BITMAP;
    if (!strcmp(t, "SAMP")) return DAT_SAMPLE;
    if (!strcmp(t, "MIDI")) return DAT_MIDI;
    if (!strcmp(t, "PAL")) return DAT_PALETTE;
    if (!strcmp(t, "FONT")) return DAT_FONT;
    if (!strcmp(t, "DATA")) return DAT_DATA;
    if (!strcmp(t, "RLE")) return DAT_RLE_SPRITE;
    return -1;
}

/* grabber header lines: #define <prefix>_<name>  <index>  / * TYPE * / */
static int check_dat(const char *dir, const char *base)
{
    char path[512], line[256], sym[96], type[16];
    int idx, n = 0, count;
    FILE *h;
    DATAFILE *df;
    snprintf(path, sizeof path, "%s/%s.H", dir, base);
    h = fopen(path, "r");
    snprintf(path, sizeof path, "%s/%s.DAT", dir, base);
    df = load_datafile(path);
    CHECK(h != NULL, "%s.H missing", base);
    CHECK(df != NULL, "%s.DAT did not load", base);
    if (!h || !df) { if (h) fclose(h); if (df) unload_datafile(df); return 0; }
    /* the grabber's own settings object ('info') is last and never in its header */
    for (count = 0; df[count].type != DAT_END; count++) ;
    if (count > 0 && df[count - 1].type == DAT_ID('i','n','f','o')) count--;
    else CHECK(0, "%s: last object %08x, not the grabber's info", base, count ? df[count - 1].type : 0);
    while (fgets(line, sizeof line, h)) {
        if (sscanf(line, " #define %95s %d /* %15s */", sym, &idx, type) != 3) continue;
        const char *name = strchr(sym, '_') ? strchr(sym, '_') + 1 : sym;
        const char *got;
        CHECK(idx < count, "%s %s: index %d past %d objects", base, sym, idx, count);
        if (idx >= count) continue;
        CHECK(df[idx].type == type_of(type), "%s #%d %s: type %08x, header %s", base, idx, sym, df[idx].type, type);
        got = get_datafile_property(&df[idx], DAT_NAME);
        CHECK(got && !strcmp(got, name), "%s #%d: name %s, header %s", base, idx, got ? got : "(none)", name);
        if (df[idx].type == DAT_BITMAP) {
            BITMAP *b = df[idx].dat;
            CHECK(b && b->w > 0 && b->h > 0 && bitmap_color_depth(b) == 8, "%s %s: bad bitmap", base, sym);
        }
        n++;
    }
    fclose(h);
    CHECK(n == count, "%s: header lists %d, file has %d", base, n, count);
    unload_datafile(df);
    printf("%s.DAT: %d objects\n", base, n);
    return n;
}

int main(int argc, char **argv)
{
    const char *dir = argc > 1 ? argv[1] : ".";
    static const char *dats[] = { "GDATA", "PDATA", "IDATA", "MDATA", "SDATA",
        "R00DATA", "R01DATA", "R02DATA", "R03DATA", "R04DATA", "R05DATA", "R06DATA",
        "R07DATA", "R08DATA", "R09DATA", "R10DATA", "R11DATA", "R12DATA", "R13DATA" };
    static char *f3[] = { "verdana8Bold", NULL }, *f5[] = { "verdana9Bold", NULL };
    char path[512];
    int i, total = 0;
    CHECK(allegro_init() == 0, "allegro_init");
    for (i = 0; i < (int)(sizeof dats / sizeof dats[0]); i++) total += check_dat(dir, dats[i]);
    printf("%d objects in %d files\n", total, (int)(sizeof dats / sizeof dats[0]));

    /* MDATA.DAT through an index, as room_load plays the room music */
    snprintf(path, sizeof path, "%s/MDATA.DAT", dir);
    {
        DATAFILE_INDEX *ix = create_datafile_index(path);
        DATAFILE *d = ix ? load_datafile_object_indexed(ix, 0) : NULL;
        CHECK(d && d->type == DAT_MIDI, "MDATA.DAT through its index");
        if (d) unload_datafile_object(d);
        if (ix) destroy_datafile_index(ix);
    }
    /* the fonts, as GAME.C loads them */
    snprintf(path, sizeof path, "%s/GDATA.DAT", dir);
    for (i = 0; i < 2; i++) {
        FONT *f = load_dat_font(path, NULL, i ? f5 : f3);
        CHECK(f != NULL, "font %s did not load", i ? f5[0] : f3[0]);
        if (f) {
            CHECK(text_height(f) > 0 && text_length(f, "Sin blanca 1995") > 0, "font %d: no glyphs", i);
            destroy_font(f);
        }
    }
    allegro_exit();
    printf(fails ? "test_data: %d FAILED\n" : "test_data: PASS\n", fails);
    return fails != 0;
}
