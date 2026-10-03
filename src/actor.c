#include <string.h>
#include "actor.h"
#include "oam_pool.h"

Actor g_actors[MAX_ACTORS];

void actor_pool_reset(void) {
    memset(g_actors, 0, sizeof(g_actors));
    int i;
    for (i = 0; i < MAX_ACTORS; i++) {
        g_actors[i].kind = AKIND_NONE;
        g_actors[i].oamId = -1;
    }
}

Actor *actor_spawn(ActorKind kind, fx8 x, fx8 y) {
    int i;
    for (i = 0; i < MAX_ACTORS; i++) {
        if (g_actors[i].kind == AKIND_NONE) {
            Actor *a = &g_actors[i];
            memset(a, 0, sizeof(Actor));
            a->kind = kind;
            a->state = AST_IDLE;
            a->facing = FACE_DOWN;
            a->x = x;
            a->y = y;
            a->hp = 1;
            a->stamina = 255;
            a->oamId = -1;
            return a;
        }
    }
    return NULL; /* pool full */
}

void actor_kill(Actor *a) {
    if (a == NULL || a->kind == AKIND_NONE) {
        return;
    }
    if (a->oamId >= 0) {
        oam_pool_free(a->oamId);
        a->oamId = -1;
    }
    a->state = AST_DEAD;
    a->kind = AKIND_NONE;
    a->hp = 0;
}

/* Mirrors combat.h's STAMINA_MAX / STAMINA_REGEN_PER_FRAME; kept local so
 * actor.c doesn't need to include combat.h for a header cycle. */
#define STAMINA_MAX_INTERNAL 255
#define STAMINA_REGEN_STEP   1

static void actor_update_one(Actor *a) {
    switch (a->state) {
        case AST_HITSTUN:
            if (a->timer > 0) {
                a->timer--;
            } else {
                a->state = AST_IDLE;
            }
            break;
        case AST_ATTACK:
        case AST_BASH:
        case AST_THROW:
            if (a->timer > 0) {
                a->timer--;
            } else {
                a->state = AST_IDLE;
            }
            break;
        default:
            break;
    }

    /* Integrate movement for anything not locked in an action/stun state. */
    if (a->state == AST_WALK || a->state == AST_CHARGE || a->kind == AKIND_PROJECTILE) {
        a->x += a->vx;
        a->y += a->vy;
    }

    /* Regenerate stamina for player/companion actors. */
    if (a->kind == AKIND_PLAYER || a->kind == AKIND_COMPANION) {
        if (a->stamina < STAMINA_MAX_INTERNAL) {
            a->stamina += STAMINA_REGEN_STEP;
        }
    }
}

void actor_update_all(void) {
    int i;
    for (i = 0; i < MAX_ACTORS; i++) {
        if (g_actors[i].kind != AKIND_NONE) {
            actor_update_one(&g_actors[i]);
        }
    }
}
