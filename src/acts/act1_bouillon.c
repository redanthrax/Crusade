/* act1_bouillon.c -- Bouillon training yard: mother's lesson + rival duel. */
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
#include "scene_act1.h"
#include "ui.h"

#define WALK_SPEED FX8(1.0)
#define RUN_SPEED  FX8(2.0)
#define RIVAL_HP   3

#define HB_INSET 2
#define HB_SIZE  12

static void setup_hitbox(Actor *a) {
    a->hbX = HB_INSET;
    a->hbY = HB_INSET;
    a->hbW = HB_SIZE;
    a->hbH = HB_SIZE;
}

static void rival_ai_step(Actor *rival, Actor *player) {
    if (rival->kind == AKIND_NONE) return;
    if (rival->state == AST_HITSTUN || rival->state == AST_ATTACK) return;

    fx8 dx = player->x - rival->x;
    fx8 dy = player->y - rival->y;
    int adx = FX8_TO_INT(dx); if (adx < 0) adx = -adx;
    int ady = FX8_TO_INT(dy); if (ady < 0) ady = -ady;

    /* Scripted-rules duel AI: close to striking range, then swing. Never
     * chases past melee range so the fight reads as a fair, bounded duel
     * rather than a chase. */
    if (adx < 14 && ady < 14) {
        combat_attack(rival);
    } else {
        fx8 vx = (dx > 0) ? WALK_SPEED : (dx < 0) ? -WALK_SPEED : 0;
        fx8 vy = (dy > 0) ? WALK_SPEED : (dy < 0) ? -WALK_SPEED : 0;
        rival->state = AST_WALK;
        collision_move_actor(rival, vx, vy);
    }
}

void act1_bouillon_run(void) {
    map_load(&g_mapAct1TrainingYard);
    actor_pool_reset();
    oam_pool_reset();

    Actor *player = actor_spawn(AKIND_PLAYER, int2fx(120), int2fx(120));
    setup_hitbox(player);
    player->hp = 10;

    scene_run(g_sceneAct1, SCN_A1_INTRO); /* intro -> mother -> duel challenge */

    Actor *rival = actor_spawn(AKIND_ENEMY, int2fx(120), int2fx(50));
    setup_hitbox(rival);
    rival->hp = RIVAL_HP;

    while (rival->kind != AKIND_NONE) {
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
            combat_attack(player);
        }

        rival_ai_step(rival, player);

        actor_update_all();
        combat_update();
        camera_follow(player->x, player->y, 0);
        render_actors_to_oam();
        oam_pool_flush();
    }

    scene_run(g_sceneAct1, SCN_A1_DUEL_WIN); /* duel win -> blessing */
}
