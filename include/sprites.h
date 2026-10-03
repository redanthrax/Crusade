/* sprites.h -- OBJ (sprite) tile/palette loading and the sprite descriptor
 * table render.c draws actors with.
 *
 * OBJ VRAM (1D mapping, 4bpp tile ids):
 *   0-15    placeholder 16x16 squares (player/npc/rival), OBJ palbank 0
 *   16      menu cursor cross, OBJ palbank 1
 *   32-127  prelude guide sheet     (16x32, 12 frames x 8 tiles), palbank 2
 *   128-223 prelude pilgrim sheet   (palbanks 3-5, three colour variants)
 *   224-319 prelude looter sheet    (palbank 6)
 */
#ifndef CRUSADE_SPRITES_H
#define CRUSADE_SPRITES_H

#include <tonc.h>

#define SPRITE_TILE_PLAYER 0
#define SPRITE_TILE_NPC    4
#define SPRITE_TILE_RIVAL  8
#define SPRITE_TILE_CURSOR 16 /* 8x8 menu cross, OBJ palette bank 1 */
#define SPRITE_PALBANK_UI  1

typedef enum {
    SPR_DEFAULT = 0,   /* placeholder square chosen from Actor::kind */
    SPR_PH_PLAYER,
    SPR_PH_NPC,
    SPR_PH_RIVAL,
    SPR_GUIDE,         /* prelude 1 pilgrim guide (player) */
    SPR_PILGRIM,       /* prelude 1 pilgrims, palVariant 0-2 */
    SPR_LOOTER,        /* prelude 1 looter */
    SPR_COUNT
} SpriteId;

#define PILGRIM_VARIANTS 3

/* Tall sprites are 16x32 character sheets: 4 columns (frames: stand,
 * stride, bob, stride) x 3 rows (down, up, side-facing-right), each frame
 * 8 contiguous tiles. Left = side row + horizontal flip. The actor's (x,y)
 * is the top-left of the lower 16x16 (the feet), so the sprite is drawn
 * 16px above it. Non-tall sprites are a single 16x16 frame. */
typedef struct {
    u16 tileBase;
    u8  palbank;
    u8  tall;
} SpriteDesc;

extern const SpriteDesc g_spriteDesc[SPR_COUNT];

/* Loads the flat-color placeholder sprite sheet + palette into OBJ VRAM.
 * Call once at boot; OBJ VRAM isn't touched by map_load(). */
void sprites_load_placeholder(void);

/* Loads the prelude 1 character sheets (guide, pilgrims, looter). */
void sprites_load_prelude1(void);

#endif /* CRUSADE_SPRITES_H */
