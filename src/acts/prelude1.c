/* prelude1.c -- Jerusalem courtyard vignette (movement + herd + shove-scrap). */
#include <tonc.h>
#include "actor.h"
#include "collision.h"
#include "combat.h"
#include "render.h"
#include "oam_pool.h"
#include "camera.h"
#include "input.h"
#include "fixed.h"
#include "maps.h"
#include "map.h"
#include "scene.h"
#include "scene_prelude1.h"
#include "ui.h"

#define WALK_SPEED FX8(1.0)
#define RUN_SPEED  FX8(2.0)
#define HERD_FOLLOW_RADIUS_PX 40
#define HERD_CATCHUP_SPEED FX8(1.2)

#define PLAYER_HB_INSET 2
#define PLAYER_HB_SIZE  12

static void setup_hitbox(Actor *a) {
    a->hbX = PLAYER_HB_INSET;
    a->hbY = PLAYER_HB_INSET;
    a->hbW = PLAYER_HB_SIZE;
    a->hbH = PLAYER_HB_SIZE;
}

static int dist_px(Actor *a, Actor *b) {
    int dx = FX8_TO_INT(a->x) - FX8_TO_INT(b->x);
    int dy = FX8_TO_INT(a->y) - FX8_TO_INT(b->y);
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return dx > dy ? dx : dy; /* Chebyshev distance, cheap on GBA */
}

static void pilgrim_ai_step(Actor *pilgrim, Actor *player) {
    if (dist_px(pilgrim, player) <= HERD_FOLLOW_RADIUS_PX) {
        pilgrim->state = AST_IDLE;
        return;
    }
    fx8 dx = player->x - pilgrim->x;
    fx8 dy = player->y - pilgrim->y;
    fx8 vx = (dx > 0) ? HERD_CATCHUP_SPEED : (dx < 0) ? -HERD_CATCHUP_SPEED : 0;
    fx8 vy = (dy > 0) ? HERD_CATCHUP_SPEED : (dy < 0) ? -HERD_CATCHUP_SPEED : 0;
    pilgrim->state = AST_WALK;
    collision_move_actor(pilgrim, vx, vy);
}

static void looter_ai_step(Actor *looter, Actor *target) {
    if (looter->kind == AKIND_NONE) return;
    if (looter->state == AST_HITSTUN) return; /* let combat.c's countdown run */
    fx8 dx = target->x - looter->x;
    fx8 dy = target->y - looter->y;
    fx8 vx = (dx > 0) ? WALK_SPEED : (dx < 0) ? -WALK_SPEED : 0;
    fx8 vy = (dy > 0) ? WALK_SPEED : (dy < 0) ? -WALK_SPEED : 0;
    looter->state = AST_WALK;
    collision_move_actor(looter, vx, vy);
}

void prelude1_run(void) {
    map_load(&g_mapPrelude1Courtyard);
    actor_pool_reset();
    oam_pool_reset();

    Actor *player = actor_spawn(AKIND_PLAYER, int2fx(120), int2fx(100));
    setup_hitbox(player);
    player->hp = 10;

    Actor *pilgrimA = actor_spawn(AKIND_COMPANION, int2fx(60), int2fx(60));
    setup_hitbox(pilgrimA);
    Actor *pilgrimB = actor_spawn(AKIND_COMPANION, int2fx(180), int2fx(60));
    setup_hitbox(pilgrimB);

    Actor *looter = actor_spawn(AKIND_ENEMY, int2fx(120), int2fx(40));
    setup_hitbox(looter);
    looter->hp = 1;

    scene_run(g_scenePrelude1, SCN_P1_INTRO);

    /* Gameplay: herd the pilgrims and shove the looter off (A = bash, no
     * sword in this vignette). combat_bash never deals damage (stagger
     * only, by design), so the looter can't be "killed" by hp; instead we
     * count stagger hits here and remove it once it's been shoved off
     * twice -- the non-lethal equivalent of driving it away. */
    int looterHits = 0;
    while (looter->kind != AKIND_NONE) {
        VBlankIntrWait();
        input_poll();

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

        if (key_hit(KEY_A)) {
            combat_bash(player);
        }

        pilgrim_ai_step(pilgrimA, player);
        pilgrim_ai_step(pilgrimB, player);
        looter_ai_step(looter, pilgrimA);

        actor_update_all();
        combat_update();

        if (looter->kind != AKIND_NONE &&
            looter->state == AST_HITSTUN && looter->timer == HITSTUN_FRAMES_HEAVY) {
            looterHits++;
            if (looterHits >= 2) {
                actor_kill(looter);
            }
        }

        camera_follow(player->x, player->y, 0);
        render_actors_to_oam();
        oam_pool_flush();
    }

    scene_run(g_scenePrelude1, SCN_P1_OUTRO);
}
