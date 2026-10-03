/* placeholder_tileset.c -- shared flat-color BG tileset for the
 * first-playable slice (prelude 1 courtyard + Act 1 training yard).
 *
 * 4 metatiles, each backed by 4 identical solid-color 8x8 tiles (see
 * map.c's metatile-to-tile expansion convention: base = metaId*4).
 *   metaId 0 = dirt   (open)
 *   metaId 1 = grass  (open)
 *   metaId 2 = wall   (solid)
 *   metaId 3 = floor  (open)
 */
#include <tonc.h>
#include "map.h"
#include "placeholder_gfx.h"

const u16 g_placeholderTilesetPal[16] = {
    BGR15(0, 0, 0),    /* 0: transparent/backdrop */
    BGR15(18, 11, 4),  /* 1: dirt */
    BGR15(6, 20, 6),   /* 2: grass */
    BGR15(12, 12, 14), /* 3: wall */
    BGR15(22, 22, 20), /* 4: floor */
    BGR15(28, 20, 8),  /* 5: player */
    BGR15(10, 14, 24), /* 6: npc */
    BGR15(24, 8, 8),   /* 7: rival */
    BGR15(4, 4, 4),    /* 8: outline */
    BGR15(31, 31, 31), /* 9: text/UI */
    0, 0, 0, 0, 0, 0
};

const u16 g_placeholderTilesetGfx[4 * 4 * 16] = {
    /* metaId 0: dirt, tiles 0-3 */
    SOLID_TILE16(PIDX_DIRT), SOLID_TILE16(PIDX_DIRT),
    SOLID_TILE16(PIDX_DIRT), SOLID_TILE16(PIDX_DIRT),
    /* metaId 1: grass, tiles 4-7 */
    SOLID_TILE16(PIDX_GRASS), SOLID_TILE16(PIDX_GRASS),
    SOLID_TILE16(PIDX_GRASS), SOLID_TILE16(PIDX_GRASS),
    /* metaId 2: wall, tiles 8-11 */
    SOLID_TILE16(PIDX_WALL), SOLID_TILE16(PIDX_WALL),
    SOLID_TILE16(PIDX_WALL), SOLID_TILE16(PIDX_WALL),
    /* metaId 3: floor, tiles 12-15 */
    SOLID_TILE16(PIDX_FLOOR), SOLID_TILE16(PIDX_FLOOR),
    SOLID_TILE16(PIDX_FLOOR), SOLID_TILE16(PIDX_FLOOR),
};

const u32 g_placeholderTilesetGfxLen = sizeof(g_placeholderTilesetGfx) / 2; /* in u16 units */

const u8 g_placeholderCollisionFlags[4] = {
    0,          /* dirt: open */
    0,          /* grass: open */
    MTF_SOLID,  /* wall: solid */
    0           /* floor: open */
};
