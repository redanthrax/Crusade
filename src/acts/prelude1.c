/* prelude1.c -- Holy Sepulchre courtyard vignette: movement tutorial,
 * keep-the-procession-together, a non-lethal shove-scrap with a cutpurse,
 * ends when the whole procession reaches the Anastasis steps. */
#include <tonc.h>
#include "actor.h"
#include "collision.h"
#include "combat.h"
#include "render.h"
#include "oam_pool.h"
#include "camera.h"
#include "input.h"
#include "debug.h"
#include "fixed.h"
#include "maps.h"
#include "map.h"
#include "scene.h"
#include "scene_prelude1.h"
#include "sprites.h"
#include "ui.h"

#define MT(n) int2fx((n) * METATILE_PX)

#define WALK_SPEED          FX8(1.0)
#define RUN_SPEED           FX8(2.0)
#define PILGRIM_COUNT       3
#define FOLLOW_GAP_PX       18   /* chain spacing behind the actor ahead */
#define FOLLOW_FAR_PX       40   /* beyond this a pilgrim hurries */
#define TOGETHER_PX         56   /* every pilgrim within this of the guide */
#define AXIS_DEADZONE_PX    2

#define LOOTER_SPAWN_ROW    11   /* guide passes this metatile row */
#define LOOTER_GRAB_PX      12   /* close enough to hold a pilgrim back */
#define SHOVE_REACH_PX      24
#define SHOVE_KNOCK         FX8(2.5)
#define SHOVES_TO_ROUT      2
#define LOOTER_FLEE_FRAMES  60

enum { LOOTER_ABSENT, LOOTER_STALK, LOOTER_FLEE, LOOTER_GONE };

/* 16x32 characters collide with a feet footprint; (x,y) is the top-left of
 * the lower 16x16 of the sprite. */
static void setup_feet(Actor *a) {
    a->hbX = 3; a->hbY = 8; a->hbW = 10; a->hbH = 8;
}

static int absi(int v) { return v < 0 ? -v : v; }

static int dist_px(const Actor *a, const Actor *b) {
    int dx = absi(FX8_TO_INT(a->x) - FX8_TO_INT(b->x));
    int dy = absi(FX8_TO_INT(a->y) - FX8_TO_INT(b->y));
    return dx > dy ? dx : dy; /* Chebyshev distance, cheap on GBA */
}

static void face_toward(Actor *a, int dx, int dy) {
    if (absi(dx) > absi(dy)) a->facing = dx < 0 ? FACE_LEFT : FACE_RIGHT;
    else                     a->facing = dy < 0 ? FACE_UP : FACE_DOWN;
}

/* Step `a` toward `target` at `speed`, per-axis with a small dead zone so
 * followers don't jitter once aligned. */
static void step_toward(Actor *a, const Actor *target, fx8 speed) {
    int dx = FX8_TO_INT(target->x) - FX8_TO_INT(a->x);
    int dy = FX8_TO_INT(target->y) - FX8_TO_INT(a->y);
    fx8 vx = dx > AXIS_DEADZONE_PX ? speed : dx < -AXIS_DEADZONE_PX ? -speed : 0;
    fx8 vy = dy > AXIS_DEADZONE_PX ? speed : dy < -AXIS_DEADZONE_PX ? -speed : 0;
    if (vx == 0 && vy == 0) { a->state = AST_IDLE; return; }
    face_toward(a, dx, dy);
    a->state = AST_WALK;
    collision_move_actor(a, vx, vy);
}

static void pilgrim_step(Actor *p, const Actor *ahead, BOOL held) {
    if (p->state == AST_HITSTUN) return;
    int d = dist_px(p, ahead);
    if (held || d <= FOLLOW_GAP_PX) {
        p->state = AST_IDLE;
        if (!held) face_toward(p, FX8_TO_INT(ahead->x - p->x), FX8_TO_INT(ahead->y - p->y));
        return;
    }
    step_toward(p, ahead, d > FOLLOW_FAR_PX ? RUN_SPEED : WALK_SPEED);
}

static Actor *spawn_char(ActorKind kind, u8 sprite, fx8 x, fx8 y) {
    Actor *a = actor_spawn(kind, x, y);
    a->sprite = sprite;
    setup_feet(a);
    return a;
}

/* Feet-centre metatile flags under an actor. */
static u8 flags_under(const Actor *a) {
    return map_collision_at(a->x + int2fx(8), a->y + int2fx(12));
}

static void frame_end(Actor *player) {
    camera_follow(player->x + int2fx(8), player->y, 0);
    render_actors_to_oam();
}

void prelude1_run(void) {
    int i;
    map_load(&g_mapPrelude1Courtyard);
    actor_pool_reset();
    oam_pool_reset();
    sprites_load_prelude1();

    /* Procession enters through the south gate onto the processional path
     * (metatile columns 13-16). */
    Actor *player = spawn_char(AKIND_PLAYER, SPR_GUIDE, MT(14) + int2fx(8), MT(16));
    player->hp = 10;
    player->facing = FACE_UP;

    Actor *pilgrims[PILGRIM_COUNT];
    for (i = 0; i < PILGRIM_COUNT; i++) {
        pilgrims[i] = spawn_char(AKIND_COMPANION, SPR_PILGRIM,
                                 MT(14) + int2fx(8 + ((i & 1) ? -6 : 6)),
                                 MT(16) + int2fx(FOLLOW_GAP_PX * (i + 1)));
        pilgrims[i]->palVariant = i;
        pilgrims[i]->facing = FACE_UP;
    }

    Actor *looter = NULL;
    int looterMode = LOOTER_ABSENT, shoves = 0;
    Actor *victim = pilgrims[PILGRIM_COUNT - 1];
    BOOL onSteps = FALSE;

    frame_end(player);
    scene_run(g_scenePrelude1, SCN_P1_INTRO);

    for (;;) {
        vsync_wait();
        /* Present last frame's scroll + sprites inside VBlank. */
        camera_commit();
        oam_pool_flush();
        input_poll();

        /* Guide movement. */
        fx8 speed = key_is_down(KEY_B) ? RUN_SPEED : WALK_SPEED;
        fx8 dx = 0, dy = 0;
        if (key_is_down(KEY_LEFT))  { dx = -speed; player->facing = FACE_LEFT; }
        if (key_is_down(KEY_RIGHT)) { dx = speed;  player->facing = FACE_RIGHT; }
        if (key_is_down(KEY_UP))    { dy = -speed; player->facing = FACE_UP; }
        if (key_is_down(KEY_DOWN))  { dy = speed;  player->facing = FACE_DOWN; }
        if (player->state == AST_IDLE || player->state == AST_WALK) {
            player->state = (dx != 0 || dy != 0) ? AST_WALK : AST_IDLE;
            collision_move_actor(player, dx, dy);
        }

        /* Shove: the vignette resolves the hit itself (feet hitboxes are
         * too small for combat_update's overlap test to feel fair). */
        if (key_hit(KEY_A)) {
            combat_bash(player);
            if (player->state == AST_BASH && looterMode == LOOTER_STALK &&
                looter->state != AST_HITSTUN && dist_px(player, looter) <= SHOVE_REACH_PX) {
                combat_apply_hit(looter, 0, HITSTUN_FRAMES_HEAVY);
                looter->vx = looter->x > player->x ? SHOVE_KNOCK : -SHOVE_KNOCK;
                looter->vy = looter->y > player->y ? SHOVE_KNOCK : -SHOVE_KNOCK;
                if (++shoves >= SHOVES_TO_ROUT) {
                    looterMode = LOOTER_FLEE;
                    looter->timer = LOOTER_FLEE_FRAMES;
                }
            }
        }

        /* Procession: each pilgrim follows the one ahead; the cutpurse
         * holds back whoever he reaches. */
        BOOL held = looterMode == LOOTER_STALK && looter->state != AST_HITSTUN &&
                    dist_px(looter, victim) <= LOOTER_GRAB_PX;
        for (i = 0; i < PILGRIM_COUNT; i++) {
            pilgrim_step(pilgrims[i], i == 0 ? player : pilgrims[i - 1],
                         held && pilgrims[i] == victim);
        }

        /* Cutpurse appears from the east colonnade once the guide is
         * mid-courtyard. */
        if (looterMode == LOOTER_ABSENT && player->y < MT(LOOTER_SPAWN_ROW)) {
            looter = spawn_char(AKIND_ENEMY, SPR_LOOTER, MT(27), MT(9));
            looter->facing = FACE_LEFT;
            looterMode = LOOTER_STALK;
            frame_end(player);
            scene_run(g_scenePrelude1, SCN_P1_SCRAP_WARN);
            continue;
        }

        if (looterMode == LOOTER_STALK) {
            if (looter->state == AST_HITSTUN) {
                collision_move_actor(looter, looter->vx, looter->vy);
                looter->vx = looter->vx * 7 / 8;
                looter->vy = looter->vy * 7 / 8;
            } else {
                looter->vx = looter->vy = 0; /* actor_update integrates v while walking */
                step_toward(looter, victim, FX8(0.9));
            }
        } else if (looterMode == LOOTER_FLEE) {
            /* Bolts east through the colonnade and out of sight. */
            looter->state = AST_WALK;
            looter->vx = looter->vy = 0;
            looter->facing = FACE_RIGHT;
            looter->x += RUN_SPEED;
            if (looter->timer == 0 || --looter->timer == 0) {
                actor_kill(looter);
                looter = NULL;
                looterMode = LOOTER_GONE;
                frame_end(player);
                scene_run(g_scenePrelude1, SCN_P1_LOOTER_FLEES);
                continue;
            }
        }

        actor_update_all();

        /* Goal: the Anastasis steps, with the cutpurse dealt with and the
         * procession gathered. */
        BOOL nowOnSteps = (flags_under(player) & MTF_TRIGGER) != 0;
        if (nowOnSteps && !onSteps) {
            BOOL together = looterMode == LOOTER_GONE;
            for (i = 0; i < PILGRIM_COUNT; i++) {
                if (dist_px(pilgrims[i], player) > TOGETHER_PX) together = FALSE;
            }
            if (together) {
                player->state = AST_IDLE;
                player->facing = FACE_UP;
                frame_end(player);
                break;
            }
            player->state = AST_IDLE;
            frame_end(player);
            scene_run(g_scenePrelude1, SCN_P1_WAIT);
        }
        onSteps = nowOnSteps;

        frame_end(player);
    }

    scene_run(g_scenePrelude1, SCN_P1_OUTRO);
}
