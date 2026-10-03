#include <tonc.h>
#include "render.h"
#include "actor.h"
#include "oam_pool.h"
#include "sprites.h"
#include "camera.h"
#include "fixed.h"

static u16 tile_for_kind(u8 kind) {
    switch (kind) {
        case AKIND_PLAYER:    return SPRITE_TILE_PLAYER;
        case AKIND_COMPANION: return SPRITE_TILE_NPC;
        default:              return SPRITE_TILE_RIVAL; /* enemy + projectile placeholder */
    }
}

void render_actors_to_oam(void) {
    int i;
    for (i = 0; i < MAX_ACTORS; i++) {
        Actor *a = &g_actors[i];
        if (a->kind == AKIND_NONE) continue;

        if (a->oamId < 0) {
            a->oamId = oam_pool_alloc();
            if (a->oamId < 0) continue; /* pool full, skip this actor's sprite */
        }

        OBJ_ATTR *obj = &g_oamShadow[(int)a->oamId];
        obj_set_attr(obj, ATTR0_SQUARE, ATTR1_SIZE_16x16, ATTR2_PALBANK(0) | ATTR2_PRIO(1) | tile_for_kind(a->kind));
        int sx = FX8_TO_INT(a->x) - FX8_TO_INT(g_camera.x);
        int sy = FX8_TO_INT(a->y) - FX8_TO_INT(g_camera.y);
        obj_set_pos(obj, sx, sy);
    }
}
