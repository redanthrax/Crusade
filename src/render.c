#include <tonc.h>
#include "render.h"
#include "actor.h"
#include "oam_pool.h"
#include "sprites.h"
#include "camera.h"
#include "fixed.h"

#define WALK_FRAME_SHIFT 3 /* 8 frames per walk-cycle step */

static u8 sprite_for(const Actor *a) {
    if (a->sprite != SPR_DEFAULT) return a->sprite;
    switch (a->kind) {
        case AKIND_PLAYER:    return SPR_PH_PLAYER;
        case AKIND_COMPANION: return SPR_PH_NPC;
        default:              return SPR_PH_RIVAL; /* enemy + projectile placeholder */
    }
}

static void write_obj(OBJ_ATTR *obj, Actor *a) {
    const SpriteDesc *d = &g_spriteDesc[sprite_for(a)];
    int sx = FX8_TO_INT(a->x) - FX8_TO_INT(g_camera.x);
    int sy = FX8_TO_INT(a->y) - FX8_TO_INT(g_camera.y);
    u16 attr2 = ATTR2_PALBANK(d->palbank + a->palVariant) | ATTR2_PRIO(1);

    /* Off-screen: hide rather than let coordinates wrap around. */
    if (sx < -24 || sx > SCREEN_WIDTH + 8 || sy < -16 || sy > SCREEN_HEIGHT + 16) {
        obj_hide(obj);
        return;
    }

    if (d->size == SPRSZ_16x16) {
        obj_set_attr(obj, ATTR0_SQUARE, ATTR1_SIZE_16x16, attr2 | d->tileBase);
        obj_set_pos(obj, sx, sy);
        return;
    }

    int row, frame = 0;
    u16 flip = 0;
    switch (a->facing) {
        case FACE_UP:    row = 1; break;
        case FACE_LEFT:  row = 2; flip = ATTR1_HFLIP; break;
        case FACE_RIGHT: row = 2; break;
        default:         row = 0; break;
    }
    if (a->state == AST_WALK) {
        a->animTick++;
        frame = (a->animTick >> WALK_FRAME_SHIFT) & 3;
    } else {
        a->animTick = 0;
    }
    if (d->size == SPRSZ_32x32) {
        obj_set_attr(obj, ATTR0_SQUARE, ATTR1_SIZE_32x32 | flip,
                     attr2 | (d->tileBase + (row * 4 + frame) * 16));
        obj_set_pos(obj, sx - 8, sy - 16);
    } else {
        obj_set_attr(obj, ATTR0_TALL, ATTR1_SIZE_16x32 | flip,
                     attr2 | (d->tileBase + (row * 4 + frame) * 8));
        obj_set_pos(obj, sx, sy - 16);
    }
}

void render_actors_to_oam(void) {
    Actor *order[MAX_ACTORS];
    s8 slots[MAX_ACTORS];
    int n = 0, i, j;

    for (i = 0; i < MAX_ACTORS; i++) {
        Actor *a = &g_actors[i];
        if (a->kind == AKIND_NONE) continue;
        if (a->oamId < 0) {
            a->oamId = oam_pool_alloc();
            if (a->oamId < 0) continue; /* pool full, skip this actor's sprite */
        }
        /* Insertion sort: actors by y descending, slots ascending, so the
         * actor nearest the bottom of the screen gets the lowest OAM index
         * and draws in front. */
        for (j = n; j > 0 && order[j - 1]->y < a->y; j--) order[j] = order[j - 1];
        order[j] = a;
        for (j = n; j > 0 && slots[j - 1] > a->oamId; j--) slots[j] = slots[j - 1];
        slots[j] = a->oamId;
        n++;
    }

    for (i = 0; i < n; i++) {
        order[i]->oamId = slots[i];
        write_obj(&g_oamShadow[(int)slots[i]], order[i]);
    }
}
