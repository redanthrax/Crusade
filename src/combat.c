#include "combat.h"
#include "fixed.h"

static BOOL is_hostile(const Actor *a, const Actor *b) {
    /* NOTE: projectiles are always treated as player/companion-owned for
     * the first-playable slice (only the player throws spears so far). If
     * enemy-thrown projectiles are added later, Actor needs an "owner
     * faction" field instead of inferring it from AKIND_PROJECTILE. */
    BOOL aFriendly = (a->kind == AKIND_PLAYER || a->kind == AKIND_COMPANION || a->kind == AKIND_PROJECTILE);
    BOOL bFriendly = (b->kind == AKIND_PLAYER || b->kind == AKIND_COMPANION);
    if (a->kind == AKIND_ENEMY && bFriendly) return TRUE;
    if (aFriendly && b->kind == AKIND_ENEMY) return TRUE;
    return FALSE;
}

static BOOL hitbox_overlap(const Actor *a, const Actor *b) {
    int ax0 = FX8_TO_INT(a->x) + a->hbX, ax1 = ax0 + a->hbW;
    int ay0 = FX8_TO_INT(a->y) + a->hbY, ay1 = ay0 + a->hbH;
    int bx0 = FX8_TO_INT(b->x) + b->hbX, bx1 = bx0 + b->hbW;
    int by0 = FX8_TO_INT(b->y) + b->hbY, by1 = by0 + b->hbH;
    return (ax0 < bx1 && ax1 > bx0 && ay0 < by1 && ay1 > by0);
}

void combat_attack(Actor *a) {
    if (a->state != AST_IDLE && a->state != AST_WALK) return;
    if (a->stamina < STAMINA_ATTACK_COST) return;
    a->stamina -= STAMINA_ATTACK_COST;
    a->state = AST_ATTACK;
    a->timer = 14; /* swing duration, frames */
}

void combat_bash(Actor *a) {
    if (a->state != AST_IDLE && a->state != AST_WALK) return;
    if (a->stamina < STAMINA_BASH_COST) return;
    a->stamina -= STAMINA_BASH_COST;
    a->state = AST_BASH;
    a->timer = 10;
}

Actor *combat_throw_spear(Actor *a) {
    if (a->state != AST_IDLE && a->state != AST_WALK) return NULL;
    if (a->stamina < STAMINA_THROW_COST) return NULL;

    fx8 vx = 0, vy = 0;
    const fx8 speed = FX8(2.5);
    switch (a->facing) {
        case FACE_UP:    vy = -speed; break;
        case FACE_DOWN:  vy =  speed; break;
        case FACE_LEFT:  vx = -speed; break;
        case FACE_RIGHT: vx =  speed; break;
    }

    Actor *proj = actor_spawn(AKIND_PROJECTILE, a->x, a->y);
    if (proj == NULL) return NULL;

    a->stamina -= STAMINA_THROW_COST;
    a->state = AST_THROW;
    a->timer = 12;

    proj->vx = vx;
    proj->vy = vy;
    proj->facing = a->facing;
    proj->hbX = -2; proj->hbY = -2; proj->hbW = 4; proj->hbH = 4;
    proj->hp = 1;
    proj->timer = 90; /* despawn after ~1.5s if nothing is hit */
    return proj;
}

void combat_apply_hit(Actor *target, u16 damage, u16 stunFrames) {
    if (target->hp == 0) return;
    if (damage >= target->hp) {
        target->hp = 0;
        actor_kill(target);
        return;
    }
    target->hp -= damage;
    target->state = AST_HITSTUN;
    target->timer = stunFrames;
}

void combat_update(void) {
    int i, j;
    for (i = 0; i < MAX_ACTORS; i++) {
        Actor *a = &g_actors[i];
        if (a->kind == AKIND_NONE) continue;

        BOOL aIsActiveAttack =
            (a->kind == AKIND_PROJECTILE) ||
            (a->state == AST_ATTACK) || (a->state == AST_BASH);
        if (!aIsActiveAttack) continue;

        for (j = 0; j < MAX_ACTORS; j++) {
            if (i == j) continue;
            Actor *b = &g_actors[j];
            if (b->kind == AKIND_NONE) continue;
            if (b->state == AST_HITSTUN) continue; /* no multi-hit while stunned this frame */

            BOOL targetable = is_hostile(a, b);
            if (!targetable) continue;

            if (hitbox_overlap(a, b)) {
                if (a->kind == AKIND_PROJECTILE) {
                    combat_apply_hit(b, 1, HITSTUN_FRAMES_LIGHT);
                    actor_kill(a);
                } else if (a->state == AST_BASH) {
                    combat_apply_hit(b, 0, HITSTUN_FRAMES_HEAVY); /* stagger, no damage */
                } else {
                    combat_apply_hit(b, 1, HITSTUN_FRAMES_LIGHT);
                }
            }
        }
    }
}
