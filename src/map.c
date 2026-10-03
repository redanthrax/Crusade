#include <string.h>
#include <tonc.h>
#include "map.h"
#include "camera.h"
#include "fixed.h"

const MapHeader *g_currentMap = NULL;

/* Screen entry for 8x8 cell (dx,dy) of metatile `id`. */
static u16 meta_se(const MapHeader *map, u16 id, int dx, int dy) {
    if (map->metaSheetMap == NULL)
        return id * 4 + dy * 2 + dx;
    int cols = map->metaSheetCols;
    int sx = (id % cols) * 2 + dx;
    int sy = (id / cols) * 2 + dy;
    return map->metaSheetMap[sy * cols * 2 + sx];
}

void map_load(const MapHeader *map) {
    int mw = map->widthMeta, mh = map->heightMeta;
    int tw = mw * 2, th = mh * 2;
    BOOL wide = tw > 32, tall = th > 32;
    int my, mx, dx, dy;

    g_currentMap = map;

    /* BG0: charblock 0 tile gfx, tilemap from screenblock 16 (up to 4
     * screenblocks, 16-19, for a 64x64-tile BG), 4bpp. Priority 1 so the
     * text layer (BG3, priority 0) draws above the world -- see
     * docs/DESIGN.md "BG VRAM Memory Map". */
    REG_BG0CNT = BG_CBB(0) | BG_SBB(16) | BG_4BPP | BG_PRIO(1) |
                 BG_SIZE((wide ? 1 : 0) | (tall ? 2 : 0));
    REG_DISPCNT |= DCNT_BG0;

    memcpy16(&tile_mem[0][0], map->tilesetGfx, map->tilesetGfxLen);
    memcpy16(pal_bg_mem, map->tilesetPal, 16);
    memset32(se_mem[16], 0, (4 * sizeof(SCREENBLOCK)) / 4);

    /* Expand each 16x16 metatile into 2x2 screen entries. Screenblocks are
     * fixed 32x32 grids; a 64-wide BG puts the right half in the next
     * screenblock, a 64-tall BG puts the bottom half 1 (or 2 if also wide)
     * screenblocks later. */
    for (my = 0; my < mh; my++) {
        for (mx = 0; mx < mw; mx++) {
            u16 id = map->metatileLayer[my * mw + mx];
            for (dy = 0; dy < 2; dy++) {
                for (dx = 0; dx < 2; dx++) {
                    int tx = mx * 2 + dx, ty = my * 2 + dy;
                    int sbb = 16 + (tx >> 5) + (ty >> 5) * (wide ? 2 : 1);
                    se_mem[sbb][(ty & 31) * 32 + (tx & 31)] = meta_se(map, id, dx, dy);
                }
            }
        }
    }

    camera_reset(mw * METATILE_PX, mh * METATILE_PX);
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
}

u8 map_collision_at(fx8 worldX, fx8 worldY) {
    if (g_currentMap == NULL) {
        return 0;
    }
    int px = FX8_TO_INT(worldX);
    int py = FX8_TO_INT(worldY);
    int mx = px / METATILE_PX;
    int my = py / METATILE_PX;
    if (mx < 0 || my < 0 || mx >= g_currentMap->widthMeta || my >= g_currentMap->heightMeta) {
        return MTF_SOLID; /* treat out-of-bounds as solid */
    }
    u16 metaId = g_currentMap->metatileLayer[my * g_currentMap->widthMeta + mx];
    return g_currentMap->collisionFlags[metaId];
}
