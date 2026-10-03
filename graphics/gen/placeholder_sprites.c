/* placeholder_sprites.c -- flat-color 16x16 OBJ sprites (player/NPC/rival)
 * for the first-playable slice. Each sprite is a 2x2 block of identical
 * solid-color 8x8 4bpp tiles, laid out contiguously for OBJ_SHAPE_SQUARE +
 * OBJ_SIZE_16 (tile ids base+0..base+3, GBA's standard 2x2 sprite tile
 * order matches BG's: top-left, top-right, bottom-left, bottom-right).
 */
#include <tonc.h>
#include "placeholder_gfx.h"

const u16 g_spritePalette[16] = {
    BGR15(0, 0, 0),    /* 0: transparent */
    BGR15(28, 20, 8),  /* 1: player */
    BGR15(10, 14, 24), /* 2: npc */
    BGR15(24, 8, 8),   /* 3: rival */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Palette-local indices used by SOLID_TILE16 below (re-using 1/2/3 since
 * this is a separate 16-color OBJ palette bank, not the BG one). */
#define SPAL_PLAYER 1
#define SPAL_NPC    2
#define SPAL_RIVAL  3

const u16 g_spritePlayerGfx[4 * 16] = {
    SOLID_TILE16(SPAL_PLAYER), SOLID_TILE16(SPAL_PLAYER),
    SOLID_TILE16(SPAL_PLAYER), SOLID_TILE16(SPAL_PLAYER)
};

const u16 g_spriteNpcGfx[4 * 16] = {
    SOLID_TILE16(SPAL_NPC), SOLID_TILE16(SPAL_NPC),
    SOLID_TILE16(SPAL_NPC), SOLID_TILE16(SPAL_NPC)
};

const u16 g_spriteRivalGfx[4 * 16] = {
    SOLID_TILE16(SPAL_RIVAL), SOLID_TILE16(SPAL_RIVAL),
    SOLID_TILE16(SPAL_RIVAL), SOLID_TILE16(SPAL_RIVAL)
};
