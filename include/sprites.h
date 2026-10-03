/* sprites.h -- placeholder OBJ (sprite) tile/palette loading + per-kind
 * tile-id mapping used by render.c until real character art exists.
 */
#ifndef CRUSADE_SPRITES_H
#define CRUSADE_SPRITES_H

#include <tonc.h>

#define SPRITE_TILE_PLAYER 0
#define SPRITE_TILE_NPC    4
#define SPRITE_TILE_RIVAL  8
#define SPRITE_TILE_CURSOR 16 /* 8x8 menu cross, OBJ palette bank 1 */
#define SPRITE_PALBANK_UI  1

/* Loads the flat-color placeholder sprite sheet + palette into OBJ VRAM.
 * Call once at boot; OBJ VRAM isn't touched by map_load(). */
void sprites_load_placeholder(void);

#endif /* CRUSADE_SPRITES_H */
