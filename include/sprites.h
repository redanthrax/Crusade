/* sprites.h -- OBJ (sprite) tile/palette loading and the sprite descriptor
 * table render.c draws actors with.
 *
 * OBJ VRAM (1D mapping, 4bpp tile ids):
 *   0-15    placeholder 16x16 squares (player/npc/rival), OBJ palbank 0
 *   16      menu cursor cross, OBJ palbank 1
 *   17-18   "!" bubble + purse icons, OBJ palbank 1
 *   32-223  prelude guide sheet     (32x32, 12 frames x 16 tiles), palbank 2
 *   224-319 prelude pilgrim sheet   (16x32, palbanks 3-8, six variants)
 *   320-415 prelude looter sheet    (16x32, palbank 9)
 */
#ifndef CRUSADE_SPRITES_H
#define CRUSADE_SPRITES_H

#include <tonc.h>

#define SPRITE_TILE_PLAYER 0
#define SPRITE_TILE_NPC    4
#define SPRITE_TILE_RIVAL  8
#define SPRITE_TILE_CURSOR 16 /* 8x8 menu cross, OBJ palette bank 1 */
#define SPRITE_PALBANK_UI  1
/* 8x8 icons in the UI palette bank, loaded with the cursor. */
#define SPRITE_TILE_EMOTE_ALERT 17 /* "!" speech bubble */
#define SPRITE_TILE_PURSE       18 /* leather coin purse */

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

#define PILGRIM_VARIANTS 6 /* 0-2 procession, 3-5 courtyard crowd */
#define PILGRIM_CROWD    3  /* first crowd palVariant */

typedef enum {
    SPRSZ_16x16 = 0,   /* single frame */
    SPRSZ_16x32,       /* character sheet, 8 tiles per frame */
    SPRSZ_32x32,       /* character sheet, 16 tiles per frame */
} SpriteSize;

/* Character sheets have 4 columns (frames: stand, stride, bob, stride) x
 * 3 rows (down, up, side-facing-right), each frame's tiles contiguous.
 * Left = side row + horizontal flip. The actor's (x,y) is the top-left of
 * its 16x16 footprint (the feet): frames are drawn 16px above it, and
 * 32-wide frames are centred on it (8px to the left). */
typedef struct {
    u16 tileBase;
    u8  palbank;
    u8  size;          /* SpriteSize */
} SpriteDesc;

extern const SpriteDesc g_spriteDesc[SPR_COUNT];

/* Loads the flat-color placeholder sprite sheet + palette into OBJ VRAM.
 * Call once at boot; OBJ VRAM isn't touched by map_load(). */
void sprites_load_placeholder(void);

/* Loads the prelude 1 character sheets (guide, pilgrims, looter). */
void sprites_load_prelude1(void);

#endif /* CRUSADE_SPRITES_H */
