#include <tonc.h>
#include "sprites.h"
#include "placeholder_sprites.h"
#include "placeholder_gfx.h"
#include "spr_guide.h"
#include "spr_pilgrim.h"
#include "spr_looter.h"

#define TILE_GUIDE   32
#define TILE_PILGRIM 128
#define TILE_LOOTER  224
#define PAL_GUIDE    2
#define PAL_PILGRIM  3
#define PAL_LOOTER   6

const SpriteDesc g_spriteDesc[SPR_COUNT] = {
    [SPR_DEFAULT]   = { SPRITE_TILE_PLAYER, 0, 0 },
    [SPR_PH_PLAYER] = { SPRITE_TILE_PLAYER, 0, 0 },
    [SPR_PH_NPC]    = { SPRITE_TILE_NPC,    0, 0 },
    [SPR_PH_RIVAL]  = { SPRITE_TILE_RIVAL,  0, 0 },
    [SPR_GUIDE]     = { TILE_GUIDE,   PAL_GUIDE,   1 },
    [SPR_PILGRIM]   = { TILE_PILGRIM, PAL_PILGRIM, 1 },
    [SPR_LOOTER]    = { TILE_LOOTER,  PAL_LOOTER,  1 },
};

/* Latin cross menu cursor; '1' = gold, shadow is derived at +1,+1. */
static const char *const s_cursorMask[8] = {
    "...11...", "...11...", ".111111.", ".111111.",
    "...11...", "...11...", "...11...", "...11...",
};

static void load_cursor(void) {
    int r, i;
    for (r = 0; r < 8; r++) {
        u32 w = 0;
        for (i = 7; i >= 0; i--) {
            u32 c = 0;
            if (s_cursorMask[r][i] == '1') c = 1;
            else if (r > 0 && i > 0 && s_cursorMask[r - 1][i - 1] == '1') c = 2;
            w = (w << 4) | c;
        }
        tile_mem_obj[0][SPRITE_TILE_CURSOR].data[r] = w;
    }
    pal_obj_bank[SPRITE_PALBANK_UI][1] = BGR15(31, 25, 9);
    pal_obj_bank[SPRITE_PALBANK_UI][2] = BGR15(4, 2, 4);
}

void sprites_load_placeholder(void) {
    memcpy16(&tile_mem_obj[0][SPRITE_TILE_PLAYER], g_spritePlayerGfx, 4 * 16);
    memcpy16(&tile_mem_obj[0][SPRITE_TILE_NPC], g_spriteNpcGfx, 4 * 16);
    memcpy16(&tile_mem_obj[0][SPRITE_TILE_RIVAL], g_spriteRivalGfx, 4 * 16);
    memcpy16(pal_obj_mem, g_spritePalette, 16);
    load_cursor();
}

void sprites_load_prelude1(void) {
    memcpy32(&tile_mem_obj[0][TILE_GUIDE], spr_guideTiles, spr_guideTilesLen / 4);
    memcpy32(&tile_mem_obj[0][TILE_PILGRIM], spr_pilgrimTiles, spr_pilgrimTilesLen / 4);
    memcpy32(&tile_mem_obj[0][TILE_LOOTER], spr_looterTiles, spr_looterTilesLen / 4);
    memcpy16(pal_obj_bank[PAL_GUIDE], spr_guidePal, 16);
    memcpy16(pal_obj_bank[PAL_PILGRIM], spr_pilgrimPal, 16 * PILGRIM_VARIANTS);
    memcpy16(pal_obj_bank[PAL_LOOTER], spr_looterPal, 16);
}
