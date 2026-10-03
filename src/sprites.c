#include <tonc.h>
#include "sprites.h"
#include "placeholder_sprites.h"
#include "placeholder_gfx.h"

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
