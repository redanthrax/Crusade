/* placeholder_gfx.h -- hand-authored stand-ins for the real art pipeline.
 *
 * No PNG/grit assets exist yet for the first-playable slice, so this
 * defines flat-color 4bpp tiles and a tiny palette directly as C data.
 * Once real art lands, these get replaced by grit-generated headers (see
 * the graphics/grit directory's .grit configs plus the Makefile's %.png
 * rule) without touching any calling code -- map.c and the act files only
 * depend on MapHeader's tilesetGfx/tilesetPal pointers and lengths.
 */
#ifndef CRUSADE_PLACEHOLDER_GFX_H
#define CRUSADE_PLACEHOLDER_GFX_H

#include <tonc.h>

/* Compile-time 15bit BGR, mirrors tonc's RGB15() for use in static
 * initializers (RGB15 is an inline function, not a constant expression). */
#define BGR15(r, g, b) ((u16)((r) + ((g) << 5) + ((b) << 10)))

/* A flat-color 4bpp tile is 16 halfwords, each equal to paletteIndex*0x1111
 * (every nibble -- i.e. every pixel in the byte -- set to the same index).
 * Deliberately has no enclosing braces: every call site uses it as a
 * sequence of 16 scalar elements inside a larger flat u16[] initializer
 * (e.g. "{ SOLID_TILE16(a), SOLID_TILE16(b) }"), not as its own sub-array. */
#define SOLID_TILE16(idx) \
    (u16)((idx) * 0x1111), (u16)((idx) * 0x1111), (u16)((idx) * 0x1111), (u16)((idx) * 0x1111), \
    (u16)((idx) * 0x1111), (u16)((idx) * 0x1111), (u16)((idx) * 0x1111), (u16)((idx) * 0x1111), \
    (u16)((idx) * 0x1111), (u16)((idx) * 0x1111), (u16)((idx) * 0x1111), (u16)((idx) * 0x1111), \
    (u16)((idx) * 0x1111), (u16)((idx) * 0x1111), (u16)((idx) * 0x1111), (u16)((idx) * 0x1111)

/* Palette indices shared by every placeholder tileset/sprite sheet. */
#define PIDX_TRANSPARENT 0
#define PIDX_DIRT        1
#define PIDX_GRASS       2
#define PIDX_WALL        3
#define PIDX_FLOOR       4
#define PIDX_PLAYER      5
#define PIDX_NPC         6
#define PIDX_RIVAL       7
#define PIDX_OUTLINE     8

#endif /* CRUSADE_PLACEHOLDER_GFX_H */
