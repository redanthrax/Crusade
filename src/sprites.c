#include <tonc.h>
#include "sprites.h"
#include "placeholder_sprites.h"
#include "placeholder_gfx.h"
#include "spr_guide.h"
#include "spr_pilgrim.h"
#include "spr_looter.h"

#define TILE_GUIDE   32
#define TILE_PILGRIM 224
#define TILE_LOOTER  320
#define PAL_GUIDE    2
#define PAL_PILGRIM  3
#define PAL_LOOTER   9 /* after the 6 pilgrim banks (3-8) */

const SpriteDesc g_spriteDesc[SPR_COUNT] = {
    [SPR_DEFAULT]   = { SPRITE_TILE_PLAYER, 0, SPRSZ_16x16 },
    [SPR_PH_PLAYER] = { SPRITE_TILE_PLAYER, 0, SPRSZ_16x16 },
    [SPR_PH_NPC]    = { SPRITE_TILE_NPC,    0, SPRSZ_16x16 },
    [SPR_PH_RIVAL]  = { SPRITE_TILE_RIVAL,  0, SPRSZ_16x16 },
    [SPR_GUIDE]     = { TILE_GUIDE,   PAL_GUIDE,   SPRSZ_32x32 },
    [SPR_PILGRIM]   = { TILE_PILGRIM, PAL_PILGRIM, SPRSZ_16x32 },
    [SPR_LOOTER]    = { TILE_LOOTER,  PAL_LOOTER,  SPRSZ_16x32 },
};

/* Latin cross menu cursor; '1' = gold, shadow is derived at +1,+1. */
static const char *const s_cursorMask[8] = {
    "...11...", "...11...", ".111111.", ".111111.",
    "...11...", "...11...", "...11...", "...11...",
};

/* Icons: digits are UI-bank palette indices (see load_icons). */
static const char *const s_alertArt[8] = {
    ".222222.", "23344332", "23344332", "23344332",
    "23333332", "23344332", ".222222.", "..22....",
};
static const char *const s_purseArt[8] = {
    "...11...", "..2112..", ".266662.", "26777762",
    "26677662", "26666662", ".266662.", "..2222..",
};

static void load_art(int tile, const char *const art[8]) {
    int r, i;
    for (r = 0; r < 8; r++) {
        u32 w = 0;
        for (i = 7; i >= 0; i--) {
            char ch = art[r][i];
            w = (w << 4) | (ch == '.' ? 0 : (u32)(ch - '0'));
        }
        tile_mem_obj[0][tile].data[r] = w;
    }
}

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

static void load_icons(void) {
    load_art(SPRITE_TILE_EMOTE_ALERT, s_alertArt);
    load_art(SPRITE_TILE_PURSE, s_purseArt);
    pal_obj_bank[SPRITE_PALBANK_UI][3] = BGR15(30, 29, 26);
    pal_obj_bank[SPRITE_PALBANK_UI][4] = BGR15(24, 4, 3);
    pal_obj_bank[SPRITE_PALBANK_UI][6] = BGR15(14, 8, 4);
    pal_obj_bank[SPRITE_PALBANK_UI][7] = BGR15(21, 14, 8);
}

void sprites_load_placeholder(void) {
    memcpy16(&tile_mem_obj[0][SPRITE_TILE_PLAYER], g_spritePlayerGfx, 4 * 16);
    memcpy16(&tile_mem_obj[0][SPRITE_TILE_NPC], g_spriteNpcGfx, 4 * 16);
    memcpy16(&tile_mem_obj[0][SPRITE_TILE_RIVAL], g_spriteRivalGfx, 4 * 16);
    memcpy16(pal_obj_mem, g_spritePalette, 16);
    load_cursor();
    load_icons();
}

void sprites_load_prelude1(void) {
    memcpy32(&tile_mem_obj[0][TILE_GUIDE], spr_guideTiles, spr_guideTilesLen / 4);
    memcpy32(&tile_mem_obj[0][TILE_PILGRIM], spr_pilgrimTiles, spr_pilgrimTilesLen / 4);
    memcpy32(&tile_mem_obj[0][TILE_LOOTER], spr_looterTiles, spr_looterTilesLen / 4);
    memcpy16(pal_obj_bank[PAL_GUIDE], spr_guidePal, 16);
    memcpy16(pal_obj_bank[PAL_PILGRIM], spr_pilgrimPal, 16 * PILGRIM_VARIANTS);
    memcpy16(pal_obj_bank[PAL_LOOTER], spr_looterPal, 16);
}
