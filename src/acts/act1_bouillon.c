/* act1_bouillon.c -- Act 1 opening: the Bouillon training yard, spring 1096.
 *
 * Beats: fade in on the yard -> Wicher sets Godfrey at the pells (sword
 * tutorial) -> Baldwin rides in and challenges him (first to three touches)
 * -> Countess Ida calls from the chapel door and blesses him -> narration
 * over black: the march east. Hits are resolved here with a facing reach
 * box rather than combat_update's body overlap, which is too small for
 * 32x32 sword swings to feel fair.
 */
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
#include "save.h"
#include "scene.h"
#include "scene_act1.h"
#include "sprites.h"
#include "ui.h"

#define MT(n) int2fx((n) * METATILE_PX)

#define WALK_SPEED       FX8(1.0)
#define RUN_SPEED        FX8(2.0)
#define BALDWIN_SPEED    FX8(0.9)
#define LUNGE_SPEED      FX8(2.0)
#define AXIS_DEADZONE_PX 2
#define TALK_PX          24

#define PELL_COUNT       3
#define PELL_HITS        2
#define PELL_LEAN_FRAMES 14
enum { PELL_REST, PELL_LEAN_R, PELL_LEAN_L, PELL_BATTERED };

#define DUEL_TOUCHES     3
#define HURT_FRAMES      30
#define WINDUP_FRAMES    24   /* Baldwin's telegraph before his strike */
#define RIPOSTE_FRAMES   18   /* shorter telegraph after a block */
#define RECOVER_FRAMES   36   /* open after his blow */
#define CIRCLE_PX        34
#define STRIKE_PX        20
#define KNOCK            FX8(3.0)
#define SPARK_FRAMES     8

/* Ring interior (actor origin = top-left of the lower 16x16). */
#define RING_X0 (10 * 16 + 6)
#define RING_X1 (16 * 16 - 6)
#define RING_Y0 (6 * 16 + 4)
#define RING_Y1 (10 * 16)

typedef enum { B_IDLE, B_CIRCLE, B_APPROACH, B_WINDUP, B_RECOVER } BaldwinState;

typedef struct { s16 x, y; } Pt;

static const Pt PELL_CELLS[PELL_COUNT] = { { 4, 7 }, { 7, 5 }, { 5, 10 } };

static Actor *s_god, *s_bald, *s_wicher, *s_ida;
static Actor *s_pell[PELL_COUNT];
static u8 s_pellHits[PELL_COUNT], s_pellLean[PELL_COUNT];
static Actor *s_emoteOn;
static s8 s_emoteSlot, s_sparkSlot;
static Pt s_spark;
static u8 s_sparkT;
static fx8 s_kbx[2], s_kby[2];   /* knockback: [0] Godfrey, [1] Baldwin */
static BOOL s_inRing;
static BaldwinState s_bState;
static u16 s_bTimer;
static s8 s_bStrafe;
static u8 s_score[2];
static u16 s_tick;
static u32 s_rng = 0x1096u;

static int rnd(int n) {
    s_rng = s_rng * 1664525u + 1013904223u;
    return (int)((s_rng >> 16) % (u32)n);
}

static int absi(int v) { return v < 0 ? -v : v; }
static int px(fx8 v) { return FX8_TO_INT(v); }

static int dist_pt(const Actor *a, int x, int y) {
    int dx = absi(px(a->x) - x), dy = absi(px(a->y) - y);
    return dx > dy ? dx : dy;
}

static int dist_px(const Actor *a, const Actor *b) {
    return dist_pt(a, px(b->x), px(b->y));
}

static void face_toward(Actor *a, int dx, int dy) {
    if (absi(dx) > absi(dy)) a->facing = dx < 0 ? FACE_LEFT : FACE_RIGHT;
    else                     a->facing = dy < 0 ? FACE_UP : FACE_DOWN;
}

static void face_actor(Actor *a, const Actor *b) {
    face_toward(a, px(b->x - a->x), px(b->y - a->y));
}

/* Steps `a` toward a world point; with `ghost` it ignores scenery. Returns
 * TRUE on arrival. */
static BOOL walk_ex(Actor *a, int tx, int ty, fx8 speed, BOOL ghost) {
    int dx = tx - px(a->x), dy = ty - px(a->y);
    fx8 vx = dx > AXIS_DEADZONE_PX ? speed : dx < -AXIS_DEADZONE_PX ? -speed : 0;
    fx8 vy = dy > AXIS_DEADZONE_PX ? speed : dy < -AXIS_DEADZONE_PX ? -speed : 0;
    a->vx = a->vy = 0; /* actor_update_all integrates v while walking */
    if (vx == 0 && vy == 0) {
        a->state = AST_IDLE;
        return TRUE;
    }
    face_toward(a, dx, dy);
    a->state = AST_WALK;
    if (ghost) {
        a->x += vx;
        a->y += vy;
    } else {
        collision_move_actor(a, vx, vy);
    }
    return FALSE;
}

static BOOL walk_ghost(Actor *a, int tx, int ty, fx8 speed) {
    return walk_ex(a, tx, ty, speed, TRUE);
}

static Actor *spawn_char(ActorKind kind, u8 sprite, int x, int y) {
    Actor *a = actor_spawn(kind, int2fx(x), int2fx(y));
    a->sprite = sprite;
    a->hbX = 3; a->hbY = 8; a->hbW = 10; a->hbH = 8;
    return a;
}

/* ---- Icons + ambient ------------------------------------------------- */

static void icon_put(s8 slot, u16 tile, int wx, int wy) {
    OBJ_ATTR *o = &g_oamShadow[(int)slot];
    int sx = wx - px(g_camera.x), sy = wy - px(g_camera.y);
    if (sx < -8 || sx > SCREEN_WIDTH || sy < -8 || sy > SCREEN_HEIGHT) {
        obj_hide(o);
        return;
    }
    obj_set_attr(o, ATTR0_SQUARE, ATTR1_SIZE_8x8,
                 ATTR2_PALBANK(SPRITE_PALBANK_UI) | ATTR2_PRIO(0) | tile);
    obj_set_pos(o, sx, sy);
}

static void spark_at(int wx, int wy) {
    s_spark.x = wx;
    s_spark.y = wy;
    s_sparkT = SPARK_FRAMES;
}

static void icons_render(void) {
    if (s_emoteOn && s_emoteOn->kind != AKIND_NONE) {
        int bob = (s_tick >> 3) & 1;
        icon_put(s_emoteSlot, SPRITE_TILE_EMOTE_ALERT,
                 px(s_emoteOn->x) + 4, px(s_emoteOn->y) - 26 - bob);
    } else {
        obj_hide(&g_oamShadow[(int)s_emoteSlot]);
    }
    if (s_sparkT) {
        s_sparkT--;
        icon_put(s_sparkSlot, SPRITE_TILE_SPARK, s_spark.x - 4, s_spark.y - 4);
    } else {
        obj_hide(&g_oamShadow[(int)s_sparkSlot]);
    }
}

static void pells_update(void) {
    int i;
    for (i = 0; i < PELL_COUNT; i++) {
        if (s_pellLean[i] && --s_pellLean[i] == 0)
            s_pell[i]->scriptId = s_pellHits[i] >= PELL_HITS ? PELL_BATTERED : PELL_REST;
    }
}

static void clamp_ring(Actor *a) {
    int x = px(a->x), y = px(a->y);
    if (x < RING_X0) a->x = int2fx(RING_X0);
    if (x > RING_X1) a->x = int2fx(RING_X1);
    if (y < RING_Y0) a->y = int2fx(RING_Y0);
    if (y > RING_Y1) a->y = int2fx(RING_Y1);
}

static void knockback_update(void) {
    Actor *who[2] = { s_god, s_bald };
    int i;
    for (i = 0; i < 2; i++) {
        if (!who[i] || who[i]->kind == AKIND_NONE) continue;
        if (s_kbx[i] || s_kby[i]) {
            collision_move_actor(who[i], s_kbx[i], s_kby[i]);
            s_kbx[i] = s_kbx[i] * 3 / 4;
            s_kby[i] = s_kby[i] * 3 / 4;
            if (absi(s_kbx[i]) < FX8(0.1)) s_kbx[i] = 0;
            if (absi(s_kby[i]) < FX8(0.1)) s_kby[i] = 0;
        }
        if (s_inRing) clamp_ring(who[i]);
    }
}

/* Runs under dialogue too (scene frame hook). */
static void ambient_update(void) {
    s_tick++;
    pells_update();
    knockback_update();
    actor_update_all();
    icons_render();
}

/* ---- Camera + frame helpers ------------------------------------------ */

static void camera_ease(fx8 cx, fx8 cy, int maxStep) {
    fx8 ox = g_camera.x, oy = g_camera.y;
    camera_follow(cx, cy, 0);
    fx8 tx = g_camera.x, ty = g_camera.y;
    fx8 mx = int2fx(maxStep);
    fx8 sx = (tx - ox) / 4, sy = (ty - oy) / 4;
    if (sx > mx) sx = mx;
    if (sx < -mx) sx = -mx;
    if (sy > mx) sy = mx;
    if (sy < -mx) sy = -mx;
    g_camera.x = absi(tx - ox) <= int2fx(1) ? tx : ox + sx;
    g_camera.y = absi(ty - oy) <= int2fx(1) ? ty : oy + sy;
}

static void camera_on(const Actor *a, int maxStep) {
    camera_ease(a->x + int2fx(8), a->y, maxStep);
}

static void frame_begin(void) {
    vsync_wait();
    camera_commit();
    oam_pool_flush();
    input_poll();
}

static void frame_end(void) {
    ambient_update();
    render_actors_to_oam();
}

static void talk(u16 entry) {
    frame_end();
    scene_run(g_sceneAct1, entry);
}

static void set_black(int level) {   /* 0 = clear, 16 = black */
    REG_BLDCNT = BLD_BG0 | BLD_OBJ | BLD_BACKDROP | BLD_BLACK;
    REG_BLDY = level;
}

/* Walks the listed actors to their marks (ghosting through scenery). */
static void walk_all(Actor *const *who, const Pt *to, int n, fx8 speed, const Actor *cam) {
    int f, i;
    for (f = 0; f < 300; f++) {
        BOOL done = TRUE;
        frame_begin();
        for (i = 0; i < n; i++)
            if (!walk_ghost(who[i], to[i].x, to[i].y, speed)) done = FALSE;
        if (cam) camera_on(cam, 3);
        frame_end();
        if (done) break;
    }
}

/* ---- Player ---------------------------------------------------------- */

static void player_move(void) {
    Actor *g = s_god;
    fx8 speed = key_is_down(KEY_B) ? RUN_SPEED : WALK_SPEED;
    fx8 dx = 0, dy = 0;
    if (g->state != AST_IDLE && g->state != AST_WALK) return;
    if (key_is_down(KEY_LEFT))  { dx = -speed; g->facing = FACE_LEFT; }
    if (key_is_down(KEY_RIGHT)) { dx = speed;  g->facing = FACE_RIGHT; }
    if (key_is_down(KEY_UP))    { dy = -speed; g->facing = FACE_UP; }
    if (key_is_down(KEY_DOWN))  { dy = speed;  g->facing = FACE_DOWN; }
    g->vx = g->vy = 0;
    g->state = (dx != 0 || dy != 0) ? AST_WALK : AST_IDLE;
    collision_move_actor(g, dx, dy);
}

/* TRUE on the single frame a swing's blade comes down. */
static BOOL striking(const Actor *a) {
    return a->state == AST_ATTACK && a->timer == ATTACK_STRIKE_FRAMES;
}

/* Does `a`'s sword, swung toward its facing, reach `t`? */
static BOOL in_reach(const Actor *a, const Actor *t) {
    int dx = px(t->x - a->x), dy = px(t->y - a->y);
    switch (a->facing) {
        case FACE_RIGHT: return dx >= 4 && dx <= 28 && absi(dy) <= 14;
        case FACE_LEFT:  return dx <= -4 && dx >= -28 && absi(dy) <= 14;
        case FACE_DOWN:  return dy >= 4 && dy <= 26 && absi(dx) <= 16;
        default:         return dy <= -4 && dy >= -26 && absi(dx) <= 16;
    }
}

static void spark_between(const Actor *a, const Actor *b) {
    spark_at((px(a->x) + px(b->x)) / 2 + 8, (px(a->y) + px(b->y)) / 2 - 4);
}

/* ---- Beats ----------------------------------------------------------- */

static void opening(void) {
    int f;
    camera_follow(s_god->x + int2fx(8), s_god->y, 0);
    for (f = 0; f < 64; f++) {
        frame_begin();
        set_black(16 - (f > 16 ? (f - 16) / 3 : 0));
        frame_end();
    }
    set_black(0);
    REG_BLDCNT = 0;
    talk(SCN_A1_INTRO);

    /* Wicher comes over from the weapon rack. */
    {
        Actor *who[1] = { s_wicher };
        Pt to[1] = { { 9 * 16, 6 * 16 + 4 } };
        walk_all(who, to, 1, WALK_SPEED, s_god);
    }
    face_actor(s_wicher, s_god);
    face_actor(s_god, s_wicher);
    talk(SCN_A1_WICHER);
}

static BOOL pells_done(void) {
    int i;
    for (i = 0; i < PELL_COUNT; i++)
        if (s_pellHits[i] < PELL_HITS) return FALSE;
    return TRUE;
}

static void pells_phase(void) {
    int i;
    for (;;) {
        frame_begin();
        player_move();
        if (key_hit(KEY_A)) combat_attack(s_god);
        if (striking(s_god)) {
            for (i = 0; i < PELL_COUNT; i++) {
                Actor *p = s_pell[i];
                if (!in_reach(s_god, p)) continue;
                spark_between(s_god, p);
                if (s_pellHits[i] < PELL_HITS) s_pellHits[i]++;
                p->scriptId = px(p->x) >= px(s_god->x) ? PELL_LEAN_R : PELL_LEAN_L;
                s_pellLean[i] = PELL_LEAN_FRAMES;
            }
        }
        camera_on(s_god, 8);
        frame_end();
        if (pells_done() && s_god->state != AST_ATTACK) break;
    }
    /* Let the last pell settle. */
    for (i = 0; i < PELL_LEAN_FRAMES + 4; i++) {
        frame_begin();
        camera_on(s_god, 8);
        frame_end();
    }
    s_god->state = AST_IDLE;
    face_actor(s_wicher, s_god);
    face_actor(s_god, s_wicher);
    talk(SCN_A1_PELLS_DONE);
}

static void baldwin_arrives(void) {
    int f;
    s_bald = spawn_char(AKIND_ENEMY, SPR_KNIGHT, 9 * 16 + 8, 16 * 16);
    s_bald->palVariant = 1;
    s_bald->facing = FACE_UP;
    s_bald->hp = 255;
    /* In through the gate; the camera drifts to meet him. */
    for (f = 0; f < 240; f++) {
        frame_begin();
        BOOL there = walk_ghost(s_bald, 9 * 16 + 8, 11 * 16, WALK_SPEED);
        camera_ease((s_god->x + s_bald->x) / 2 + int2fx(8), (s_god->y + s_bald->y) / 2, 3);
        if (f > 20) face_actor(s_god, s_bald);
        frame_end();
        if (there) break;
    }
    face_actor(s_bald, s_god);
    face_actor(s_god, s_bald);
    talk(SCN_A1_BALDWIN);
}

static const Pt DUEL_MARK[2] = { { 11 * 16 + 4, 8 * 16 }, { 15 * 16 - 4, 8 * 16 } };

static void duel_score_print(void) {
    char buf[32];
    int n = 0;
    const char *a = "Godfrey ", *b = "  Baldwin ";
    while (*a) buf[n++] = *a++;
    buf[n++] = '0' + s_score[0];
    while (*b) buf[n++] = *b++;
    buf[n++] = '0' + s_score[1];
    buf[n] = 0;
    ui_print_hud(1, 1, buf);
}

static void duel_take_marks(void) {
    Actor *who[3] = { s_god, s_bald, s_wicher };
    Pt to[3] = { DUEL_MARK[0], DUEL_MARK[1], { 8 * 16 + 4, 9 * 16 } };
    s_inRing = FALSE;
    s_kbx[0] = s_kby[0] = s_kbx[1] = s_kby[1] = 0;
    walk_all(who, to, 3, WALK_SPEED, s_god);
    s_god->facing = FACE_RIGHT;
    s_bald->facing = FACE_LEFT;
    s_wicher->facing = FACE_RIGHT;
    s_inRing = TRUE;
    s_score[0] = s_score[1] = 0;
    s_bState = B_CIRCLE;
    s_bTimer = 60 + rnd(40);
    s_bStrafe = 1;
}

static void knock(int who, const Actor *from, const Actor *to) {
    int dx = px(to->x - from->x), dy = px(to->y - from->y);
    s_kbx[who] = dx > 2 ? KNOCK : dx < -2 ? -KNOCK : 0;
    s_kby[who] = dy > 2 ? KNOCK : dy < -2 ? -KNOCK : 0;
}

static void baldwin_windup(int frames) {
    Actor *b = s_bald;
    face_actor(b, s_god);
    b->vx = b->vy = 0;
    b->state = AST_ATTACK;
    b->timer = ATTACK_STRIKE_FRAMES + frames;
    s_bState = B_WINDUP;
}

/* Is `from` in front of `b` (inside his guard)? */
static BOOL guarded_from(const Actor *b, const Actor *from) {
    int dx = px(from->x - b->x), dy = px(from->y - b->y);
    switch (b->facing) {
        case FACE_RIGHT: return dx > 0 && absi(dy) <= dx;
        case FACE_LEFT:  return dx < 0 && absi(dy) <= -dx;
        case FACE_DOWN:  return dy > 0 && absi(dx) <= dy;
        default:         return dy < 0 && absi(dx) <= -dy;
    }
}

static void baldwin_ai(void) {
    Actor *b = s_bald, *g = s_god;
    int dx = px(g->x - b->x), dy = px(g->y - b->y);
    int d = absi(dx) > absi(dy) ? absi(dx) : absi(dy);

    if (b->state == AST_HITSTUN) {
        s_bState = B_RECOVER;
        s_bTimer = 1; /* back to circling once on his feet */
        return;
    }
    switch (s_bState) {
    case B_IDLE:
        break;
    case B_CIRCLE: {
        fx8 vx = 0, vy = 0;
        face_actor(b, g);
        if (d > CIRCLE_PX + 6) {
            vx = dx > 0 ? BALDWIN_SPEED : dx < 0 ? -BALDWIN_SPEED : 0;
            vy = dy > 0 ? BALDWIN_SPEED : dy < 0 ? -BALDWIN_SPEED : 0;
        } else if (d < CIRCLE_PX - 6) {
            vx = dx > 0 ? -BALDWIN_SPEED : BALDWIN_SPEED;
            vy = dy > 0 ? -BALDWIN_SPEED : BALDWIN_SPEED;
        } else if (absi(dx) > absi(dy)) {
            vy = s_bStrafe * BALDWIN_SPEED / 2;
        } else {
            vx = s_bStrafe * BALDWIN_SPEED / 2;
        }
        b->vx = b->vy = 0;
        b->state = (vx || vy) ? AST_WALK : AST_IDLE;
        {
            fx8 ox = b->x, oy = b->y;
            collision_move_actor(b, vx, vy);
            clamp_ring(b);
            if ((vx || vy) && b->x == ox && b->y == oy) s_bStrafe = -s_bStrafe;
        }
        face_actor(b, g);
        if ((s_tick & 63) == 0 && rnd(3) == 0) s_bStrafe = -s_bStrafe;
        if (--s_bTimer == 0) {
            s_bState = B_APPROACH;
            s_bTimer = 90;
        }
        break;
    }
    case B_APPROACH: {
        BOOL aligned = absi(dx) > absi(dy) ? absi(dy) <= 10 : absi(dx) <= 12;
        if ((d <= STRIKE_PX && aligned) || --s_bTimer == 0) {
            baldwin_windup(WINDUP_FRAMES);
            break;
        }
        walk_ex(b, px(g->x) - (dx > 0 ? 14 : dx < 0 ? -14 : 0), px(g->y), BALDWIN_SPEED, FALSE);
        clamp_ring(b);
        break;
    }
    case B_WINDUP:
        /* A short lunge as the blade comes down. */
        if (b->state == AST_ATTACK && b->timer > ATTACK_STRIKE_FRAMES &&
            b->timer <= ATTACK_STRIKE_FRAMES + 6 && d > 14) {
            fx8 vx = 0, vy = 0;
            switch (b->facing) {
                case FACE_RIGHT: vx = LUNGE_SPEED; break;
                case FACE_LEFT:  vx = -LUNGE_SPEED; break;
                case FACE_DOWN:  vy = LUNGE_SPEED; break;
                default:         vy = -LUNGE_SPEED; break;
            }
            collision_move_actor(b, vx, vy);
            clamp_ring(b);
        }
        if (b->state != AST_ATTACK) {
            s_bState = B_RECOVER;
            s_bTimer = RECOVER_FRAMES;
        }
        break;
    case B_RECOVER:
        b->state = AST_IDLE;
        if (--s_bTimer == 0) {
            s_bState = B_CIRCLE;
            s_bTimer = 50 + rnd(50);
            s_bStrafe = rnd(2) ? 1 : -1;
        }
        break;
    }
}

/* Returns 0 while fighting, 1 when Godfrey wins, 2 when Baldwin does. */
static int duel_step(void) {
    Actor *g = s_god, *b = s_bald;
    player_move();
    clamp_ring(g);
    if (key_hit(KEY_A)) combat_attack(g);
    baldwin_ai();

    if (striking(g) && b->state != AST_HITSTUN && in_reach(g, b)) {
        spark_between(g, b);
        if ((s_bState == B_CIRCLE || s_bState == B_APPROACH) && guarded_from(b, g)) {
            /* Turned on his shield; he answers with a quick riposte. */
            knock(0, b, g);
            s_kbx[0] /= 2;
            s_kby[0] /= 2;
            baldwin_windup(RIPOSTE_FRAMES);
        } else {
            s_score[0]++;
            combat_apply_hit(b, 0, HURT_FRAMES);
            knock(1, g, b);
            s_bState = B_RECOVER;
            duel_score_print();
        }
    }
    if (striking(b) && g->state != AST_HITSTUN && in_reach(b, g)) {
        spark_between(b, g);
        s_score[1]++;
        combat_apply_hit(g, 0, HURT_FRAMES);
        knock(0, b, g);
        duel_score_print();
    }
    if (s_score[0] >= DUEL_TOUCHES) return 1;
    if (s_score[1] >= DUEL_TOUCHES) return 2;
    return 0;
}

static void duel_settle(void) {
    int f;
    s_bState = B_IDLE;
    for (f = 0; f < HURT_FRAMES + 10; f++) {
        frame_begin();
        if (s_bald->state == AST_WALK) s_bald->state = AST_IDLE;
        if (s_god->state == AST_WALK) s_god->state = AST_IDLE;
        camera_ease((s_god->x + s_bald->x) / 2 + int2fx(8), (s_god->y + s_bald->y) / 2, 4);
        frame_end();
    }
    s_god->state = AST_IDLE;
    s_bald->state = AST_IDLE;
    face_actor(s_god, s_bald);
    face_actor(s_bald, s_god);
}

static void duel(void) {
    for (;;) {
        int result = 0;
        duel_take_marks();
        duel_score_print();
        while (!result) {
            frame_begin();
            result = duel_step();
            camera_ease((s_god->x + s_bald->x) / 2 + int2fx(8), (s_god->y + s_bald->y) / 2, 4);
            frame_end();
        }
        duel_settle();
        s_inRing = FALSE;
        ui_print_hud(1, 1, "                    ");
        if (result == 1) break;
        talk(SCN_A1_DUEL_LOSE);
    }
    talk(SCN_A1_DUEL_WIN);
    if (g_save.arms < 255) g_save.arms++;
}

static void ida_beat(void) {
    int f;
    s_ida = spawn_char(AKIND_COMPANION, SPR_IDA, 3 * 16 + 8, 2 * 16);
    s_ida->facing = FACE_DOWN;
    for (f = 0; f < 90; f++) {
        frame_begin();
        walk_ghost(s_ida, 3 * 16 + 8, 3 * 16 + 6, FX8(0.5));
        camera_ease((s_god->x + s_ida->x) / 2 + int2fx(8), (s_god->y + s_ida->y) / 2, 3);
        if (f == 30) {
            face_actor(s_god, s_ida);
            s_emoteOn = s_ida;
        }
        frame_end();
    }
    s_ida->state = AST_IDLE;
    s_ida->facing = FACE_DOWN;
    s_emoteOn = s_ida;
    face_actor(s_bald, s_ida);
    face_actor(s_wicher, s_ida);
    talk(SCN_A1_IDA_CALL);

    /* Free walk to the chapel door. */
    for (;;) {
        frame_begin();
        player_move();
        if (key_hit(KEY_A) && dist_px(s_god, s_ida) <= TALK_PX) {
            s_god->state = AST_IDLE;
            face_actor(s_god, s_ida);
            face_actor(s_ida, s_god);
            s_emoteOn = NULL;
            break;
        }
        camera_on(s_god, 8);
        frame_end();
    }
    talk(SCN_A1_BLESS);
    if (g_save.piety < 255) g_save.piety++;
}

static void outro(void) {
    int f;
    scene_set_frame_hook(NULL);
    for (f = 0; f < 72; f++) {
        frame_begin();
        set_black(f / 4 > 16 ? 16 : f / 4);
        frame_end();
    }
    set_black(16);
    actor_pool_reset();
    oam_pool_reset();
    vsync_wait();
    oam_pool_flush();
    REG_DISPCNT &= ~DCNT_BG0;
    pal_bg_mem[0] = CLR_BLACK;
    REG_BLDCNT = 0;
    REG_BLDY = 0;
    scene_run(g_sceneAct1, SCN_A1_OUTRO);
}

/* ---- Main ------------------------------------------------------------ */

void act1_bouillon_run(void) {
    int i;
    set_black(16);
    map_load(&g_mapAct1TrainingYard);
    actor_pool_reset();
    oam_pool_reset();
    sprites_load_act1();
    s_emoteSlot = oam_pool_alloc();
    s_sparkSlot = oam_pool_alloc();
    s_emoteOn = NULL;
    s_sparkT = 0;
    s_tick = 0;
    s_inRing = FALSE;
    s_bald = s_ida = NULL;
    s_kbx[0] = s_kby[0] = s_kbx[1] = s_kby[1] = 0;

    for (i = 0; i < PELL_COUNT; i++) {
        s_pell[i] = spawn_char(AKIND_ENEMY, SPR_PELL,
                               PELL_CELLS[i].x * 16, PELL_CELLS[i].y * 16 - 4);
        s_pell[i]->scriptId = PELL_REST;
        s_pell[i]->facing = FACE_DOWN;
        s_pellHits[i] = 0;
        s_pellLean[i] = 0;
    }
    s_god = spawn_char(AKIND_PLAYER, SPR_KNIGHT, 9 * 16, 8 * 16 + 4);
    s_god->hp = 10;
    s_god->facing = FACE_LEFT;
    s_wicher = spawn_char(AKIND_COMPANION, SPR_WICHER, 8 * 16, 4 * 16);
    s_wicher->facing = FACE_DOWN;

    scene_set_frame_hook(ambient_update);
    opening();
    pells_phase();
    baldwin_arrives();
    duel();
    ida_beat();
    outro();
    scene_set_frame_hook(NULL);
}
