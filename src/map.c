#include <string.h>
#include <tonc.h>
#include "map.h"
#include "camera.h"
#include "fixed.h"

const MapHeader *g_currentMap = NULL;

void map_load(const MapHeader *map) {
    g_currentMap = map;

    /* BG0 control: charblock 0 for tile gfx, screenblock 16 for the
     * tilemap, 4bpp, default 32x32-tile (256x256px) size -- see
     * docs/DESIGN.md "BG VRAM Memory Map". Set on every map load since
     * this is the only place that owns BG0's VRAM layout decisions. */
    /* Priority 1 so the text layer (BG3, priority 0) draws above the world;
     * on equal priority the lower-numbered BG would win and hide text. */
    REG_BG0CNT = BG_CBB(0) | BG_SBB(16) | BG_4BPP | BG_PRIO(1);
    REG_DISPCNT |= DCNT_BG0;

    /* Load tileset gfx + palette into BG charblock 0 / BG palette bank 0.
     * First-playable slice uses a single shared 4bpp tileset per map. */
    memcpy16(&tile_mem[0][0], map->tilesetGfx, map->tilesetGfxLen);
    memcpy16(pal_bg_mem, map->tilesetPal, 16);

    /* Expand the metatile layer (16x16 px cells) into the BG0 screen-block
     * tile map (8x8 px cells): each metatile covers a 2x2 tile block using
     * four consecutive tile ids starting at metatileId*4, by convention of
     * the hand-authored/grit-exported tileset layout.
     *
     * VRAM layout (see docs/DESIGN.md "BG memory map"): world tile gfx in
     * charblock 0 (screenblocks 0-7), world tilemap in screenblock 16
     * (charblock 2's range, so it never overlaps the tile gfx); the text
     * layer (ui.c) separately owns charblock 1 + screenblock 28. */
    int mw = map->widthMeta, mh = map->heightMeta;
    SCR_ENTRY *sb = se_mem[16];
    int my, mx;
    for (my = 0; my < mh; my++) {
        for (mx = 0; mx < mw; mx++) {
            u16 metaId = map->metatileLayer[my * mw + mx];
            u16 base = metaId * 4;
            int tx = mx * 2, ty = my * 2;
            /* Screenblocks are always a fixed 32x32 SCR_ENTRY grid in VRAM
             * regardless of the BG's logical size, so the row stride here
             * must be 32, not the map's own (narrower) tile width. */
            sb[(ty + 0) * 32 + (tx + 0)] = base + 0;
            sb[(ty + 0) * 32 + (tx + 1)] = base + 1;
            sb[(ty + 1) * 32 + (tx + 0)] = base + 2;
            sb[(ty + 1) * 32 + (tx + 1)] = base + 3;
        }
    }

    camera_reset(mw * METATILE_PX, mh * METATILE_PX);
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
