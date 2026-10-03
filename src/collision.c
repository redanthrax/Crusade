#include "actor.h"
#include "map.h"
#include "fixed.h"
#include "collision.h"

/* Axis-separated move-and-check: tries X then Y independently so sliding
 * along a wall still works. Checks the four hitbox corners against the
 * metatile collision table. Metatile collision only -- no per-pixel work. */
static BOOL would_collide(Actor *a, fx8 newX, fx8 newY) {
    int left   = FX8_TO_INT(newX) + a->hbX;
    int right  = left + a->hbW - 1;
    int top    = FX8_TO_INT(newY) + a->hbY;
    int bottom = top + a->hbH - 1;

    if (map_collision_at(int2fx(left), int2fx(top)) & MTF_SOLID) return TRUE;
    if (map_collision_at(int2fx(right), int2fx(top)) & MTF_SOLID) return TRUE;
    if (map_collision_at(int2fx(left), int2fx(bottom)) & MTF_SOLID) return TRUE;
    if (map_collision_at(int2fx(right), int2fx(bottom)) & MTF_SOLID) return TRUE;
    return FALSE;
}

void collision_move_actor(Actor *a, fx8 dx, fx8 dy) {
    if (dx != 0) {
        fx8 newX = a->x + dx;
        if (!would_collide(a, newX, a->y)) {
            a->x = newX;
        }
    }
    if (dy != 0) {
        fx8 newY = a->y + dy;
        if (!would_collide(a, a->x, newY)) {
            a->y = newY;
        }
    }
}
