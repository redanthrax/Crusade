/* prelude1.c -- Holy Sepulchre courtyard vignette.
 *
 * Scripted cold open (fade in on the church, narration, the procession walks
 * in from the south gate and introduces itself), then guided play: lead
 * Silvanus, Theo and Anna to the Anastasis door through a busy feast-day
 * courtyard. Beats: Theo wanders to the well (walk over, A to call him
 * back); a cutpurse snatches Silvanus's silver (chase him down, A to shove,
 * pick up and return the purse); Theo wanders to the fig baskets; the
 * procession enters the rotunda together. */
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
#define ANNA_WALK           FX8(0.85)
#define ANNA_HURRY          FX8(1.5)
#define CROWD_SPEED         FX8(0.5)
#define THIEF_SPEED         FX8(1.5)
#define THIEF_PANIC_SPEED   FX8(1.8)
#define THIEF_PANIC_PX      32
#define FOLLOW_GAP_PX       18   /* chain spacing behind the actor ahead */
#define FOLLOW_FAR_PX       40   /* beyond this a pilgrim hurries */
#define TOGETHER_PX         56   /* every pilgrim within this of the guide */
#define TALK_PX             24   /* A reaches someone this close */
#define AXIS_DEADZONE_PX    2
#define STUCK_FRAMES        20   /* blocked this long: slip past (no collision) */
#define SHOVE_KNOCK         FX8(2.5)

#define DOOR_Y_PX           (4 * 16 + 6) /* feet above this: inside the church */
#define THEO_WELL_ROW       12   /* guide passes these rows: story beats */
#define THIEF_ROW           9
#define THEO_FIGS_ROW       9

enum { P_SILVANUS, P_THEO, P_ANNA, PILGRIM_COUNT };

typedef enum {
    PH_WALK1,     /* towards the well */
    PH_THEO1,     /* Theo at the well */
    PH_WALK2,     /* towards the cutpurse */
    PH_CHASE,
    PH_PURSE,     /* purse dropped / carried back */
    PH_WALK3,     /* towards the figs */
    PH_THEO2,     /* Theo at the fig baskets */
    PH_FREE       /* everything done: on to the door */
} Phase;

enum { PURSE_NONE, PURSE_GROUND, PURSE_CARRIED, PURSE_DONE };

typedef struct { s16 x, y; } Pt; /* world pixels, feet top-left */

typedef struct {
    Actor *a;
    Pt home, target;
    u8 wander;     /* drifts around home; otherwise stays put */
    u8 facing;     /* rest facing for stationary crowd */
    u16 wait;
    u8 stuck;
} Crowd;

#define CROWD_COUNT 8

static Actor *s_guide;
static Actor *s_pil[PILGRIM_COUNT];
static u8 s_pilStuck[PILGRIM_COUNT];
static Actor *s_line[PILGRIM_COUNT]; /* follow order behind the guide */
static int s_lineN;
static Crowd s_crowd[CROWD_COUNT];
static Actor *s_thief;
static u8 s_thiefStuck;

/* Theo's scripted errands. */
static const Pt *s_theoPath;
static int s_theoPathN, s_theoWp;
static u8 s_theoStuck, s_theoRestFacing;
static BOOL s_theoAway;

static Actor *s_emoteOn;      /* "!" bubble above this actor, or NULL */
static int s_purseState;
static Pt s_purse;
static s8 s_emoteSlot, s_purseSlot;
static u16 s_tick;
static u32 s_rng = 0x1095u;

static const Pt THEO_TO_WELL[] = { { 9 * 16, 10 * 16 } };
static const Pt THEO_TO_FIGS[] = { { 18 * 16, 9 * 16 }, { 23 * 16, 9 * 16 }, { 23 * 16, 11 * 16 } };

/* The chase loop: corners of the open lanes around the courtyard. */
static const Pt CORNERS[4] = {
    { 4 * 16, 6 * 16 }, { 25 * 16, 6 * 16 }, { 25 * 16, 18 * 16 }, { 4 * 16, 18 * 16 },
};

static const struct { u8 col, row, facing, pal, wander; } CROWD_SPOTS[CROWD_COUNT] = {
    { 21,  9, FACE_UP,    3, 0 },  /* kneeling at Calvary */
    { 22,  9, FACE_UP,    5, 0 },
    { 25, 12, FACE_LEFT,  4, 0 },  /* fig seller */
    {  6, 10, FACE_RIGHT, 3, 0 },  /* drawing water */
    {  5,  7, FACE_DOWN,  4, 1 },
    {  9, 16, FACE_DOWN,  5, 1 },
    { 19, 17, FACE_DOWN,  3, 1 },
    { 24,  6, FACE_DOWN,  5, 1 },
};

static int rnd(int n) {
    s_rng = s_rng * 1664525u + 1013904223u;
    return (int)((s_rng >> 16) % (u32)n);
}

/* 16x32 characters collide with a feet footprint; (x,y) is the top-left of
 * the lower 16x16 of the sprite. */
static void setup_feet(Actor *a) {
    a->hbX = 3; a->hbY = 8; a->hbW = 10; a->hbH = 8;
}

static int absi(int v) { return v < 0 ? -v : v; }
static int px(fx8 v) { return FX8_TO_INT(v); }

static int dist_pt(const Actor *a, int x, int y) {
    int dx = absi(px(a->x) - x), dy = absi(px(a->y) - y);
    return dx > dy ? dx : dy; /* Chebyshev distance, cheap on GBA */
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

/* Steps `a` toward a world point, per-axis with a small dead zone so walkers
 * don't jitter once aligned. With `stuck` set, an actor blocked by scenery
 * for STUCK_FRAMES slips through without collision until it is free, so no
 * one is ever pinned behind a tree. Returns TRUE on arrival. */
static BOOL walk_ex(Actor *a, int tx, int ty, fx8 speed, u8 *stuck, BOOL ghost) {
    int dx = tx - px(a->x), dy = ty - px(a->y);
    fx8 vx = dx > AXIS_DEADZONE_PX ? speed : dx < -AXIS_DEADZONE_PX ? -speed : 0;
    fx8 vy = dy > AXIS_DEADZONE_PX ? speed : dy < -AXIS_DEADZONE_PX ? -speed : 0;
    a->vx = a->vy = 0; /* actor_update_all integrates v while walking */
    if (vx == 0 && vy == 0) {
        a->state = AST_IDLE;
        if (stuck) *stuck = 0;
        return TRUE;
    }
    face_toward(a, dx, dy);
    a->state = AST_WALK;
    if (ghost) {
        a->x += vx; a->y += vy;
        return FALSE;
    }
    if (stuck && *stuck >= STUCK_FRAMES) {
        a->x += vx; a->y += vy;
        (*stuck)++;
        if (*stuck > STUCK_FRAMES + 12) *stuck = 0; /* try colliding again */
        return FALSE;
    }
    fx8 ox = a->x, oy = a->y;
    collision_move_actor(a, vx, vy);
    if (stuck) {
        int moved = absi(a->x - ox) + absi(a->y - oy);
        if (moved * 2 < (vx ? absi(vx) : 0) + (vy ? absi(vy) : 0)) (*stuck)++;
        else *stuck = 0;
    }
    return FALSE;
}

static BOOL walk_to(Actor *a, int tx, int ty, fx8 speed, u8 *stuck) {
    return walk_ex(a, tx, ty, speed, stuck, FALSE);
}

/* Scripted walk ignoring scenery (off-map entrances, the church door). */
static BOOL walk_ghost(Actor *a, int tx, int ty, fx8 speed) {
    return walk_ex(a, tx, ty, speed, NULL, TRUE);
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

/* ---- Procession ------------------------------------------------------ */

static void line_remove(Actor *p) {
    int i, j = 0;
    for (i = 0; i < s_lineN; i++) if (s_line[i] != p) s_line[j++] = s_line[i];
    s_lineN = j;
}

static void line_append(Actor *p) {
    s_line[s_lineN++] = p;
}

static int pil_index(const Actor *p) {
    int i;
    for (i = 0; i < PILGRIM_COUNT; i++) if (s_pil[i] == p) return i;
    return 0;
}

static void follow_step(Actor *p, const Actor *ahead) {
    int i = pil_index(p);
    int d = dist_px(p, ahead);
    if (p->state == AST_HITSTUN) return;
    if (d <= FOLLOW_GAP_PX) {
        p->state = AST_IDLE;
        s_pilStuck[i] = 0;
        face_actor(p, ahead);
        return;
    }
    fx8 walk = p == s_pil[P_ANNA] ? ANNA_WALK : WALK_SPEED;
    fx8 hurry = p == s_pil[P_ANNA] ? ANNA_HURRY : RUN_SPEED;
    walk_to(p, px(ahead->x), px(ahead->y), d > FOLLOW_FAR_PX ? hurry : walk, &s_pilStuck[i]);
}

static void procession_update(Phase ph) {
    int i;
    if (ph == PH_CHASE || ph == PH_PURSE) {
        /* Huddled where Silvanus was robbed, watching the chase. */
        for (i = 0; i < s_lineN; i++) {
            Actor *p = s_line[i];
            p->state = AST_IDLE;
            face_actor(p, s_thief ? s_thief : s_guide);
        }
        return;
    }
    for (i = 0; i < s_lineN; i++) follow_step(s_line[i], i == 0 ? s_guide : s_line[i - 1]);
}

static BOOL procession_together(void) {
    int i;
    if (s_lineN != PILGRIM_COUNT) return FALSE;
    for (i = 0; i < PILGRIM_COUNT; i++) {
        if (dist_px(s_pil[i], s_guide) > TOGETHER_PX) return FALSE;
    }
    return TRUE;
}

/* ---- Ambient: crowd, Theo's errands, icons --------------------------- */

static void crowd_spawn(void) {
    int i;
    for (i = 0; i < CROWD_COUNT; i++) {
        Crowd *c = &s_crowd[i];
        c->home.x = CROWD_SPOTS[i].col * 16;
        c->home.y = CROWD_SPOTS[i].row * 16;
        c->target = c->home;
        c->wander = CROWD_SPOTS[i].wander;
        c->facing = CROWD_SPOTS[i].facing;
        c->wait = 30 + rnd(120);
        c->stuck = 0;
        c->a = spawn_char(AKIND_COMPANION, SPR_PILGRIM, int2fx(c->home.x), int2fx(c->home.y));
        c->a->palVariant = CROWD_SPOTS[i].pal;
        c->a->facing = c->facing;
    }
}

static void crowd_update(void) {
    int i;
    for (i = 0; i < CROWD_COUNT; i++) {
        Crowd *c = &s_crowd[i];
        if (!c->wander) {
            /* Stationary folk glance around now and then. */
            c->a->state = AST_IDLE;
            if (c->wait > 0) { c->wait--; continue; }
            c->a->facing = c->a->facing == c->facing ? (u8)rnd(4) : c->facing;
            c->wait = c->a->facing == c->facing ? 120 + rnd(240) : 40 + rnd(40);
            continue;
        }
        if (c->wait > 0) {
            c->wait--;
            c->a->state = AST_IDLE;
            continue;
        }
        if (walk_to(c->a, c->target.x, c->target.y, CROWD_SPEED, NULL)) {
            c->target.x = c->home.x + (rnd(5) - 2) * 16;
            c->target.y = c->home.y + (rnd(5) - 2) * 8;
            c->wait = 60 + rnd(150);
            c->stuck = 0;
        } else if (++c->stuck > 90) {
            /* Blocked by scenery: give up and pause at the current spot. */
            c->target.x = px(c->a->x);
            c->target.y = px(c->a->y);
        }
    }
}

static void theo_send(const Pt *path, int n, u8 restFacing) {
    Actor *t = s_pil[P_THEO];
    line_remove(t);
    s_theoPath = path;
    s_theoPathN = n;
    s_theoWp = 0;
    s_theoStuck = 0;
    s_theoRestFacing = restFacing;
    s_theoAway = TRUE;
    s_emoteOn = t;
}

static void theo_update(void) {
    Actor *t = s_pil[P_THEO];
    if (!s_theoAway) return;
    if (s_theoWp < s_theoPathN) {
        const Pt *w = &s_theoPath[s_theoWp];
        if (walk_to(t, w->x, w->y, WALK_SPEED, &s_theoStuck)) s_theoWp++;
    } else {
        t->state = AST_IDLE;
        t->facing = s_theoRestFacing;
    }
}

static void theo_recall(void) {
    s_theoAway = FALSE;
    s_emoteOn = NULL;
    s_pilStuck[P_THEO] = 0;
    line_append(s_pil[P_THEO]);
}

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

static void icons_render(void) {
    if (s_emoteOn && s_emoteOn->kind != AKIND_NONE) {
        int bob = (s_tick >> 3) & 1;
        icon_put(s_emoteSlot, SPRITE_TILE_EMOTE_ALERT,
                 px(s_emoteOn->x) + 4, px(s_emoteOn->y) - 26 - bob);
    } else {
        obj_hide(&g_oamShadow[(int)s_emoteSlot]);
    }
    if (s_purseState == PURSE_GROUND) {
        icon_put(s_purseSlot, SPRITE_TILE_PURSE, s_purse.x, s_purse.y);
    } else if (s_purseState == PURSE_CARRIED) {
        icon_put(s_purseSlot, SPRITE_TILE_PURSE, px(s_guide->x) + 4, px(s_guide->y) - 30);
    } else {
        obj_hide(&g_oamShadow[(int)s_purseSlot]);
    }
}

/* Runs under dialogue too (scene frame hook): the courtyard keeps living. */
static void ambient_update(void) {
    s_tick++;
    crowd_update();
    theo_update();
    actor_update_all();
    icons_render();
}

/* ---- Camera + frame helpers ------------------------------------------ */

/* Eases the camera toward centring (cx, cy), at most `maxStep` px a frame. */
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

static void camera_on_guide(void) {
    camera_ease(s_guide->x + int2fx(8), s_guide->y, 8);
}

static void frame_begin(void) {
    vsync_wait();
    /* Present last frame's scroll + sprites inside VBlank. */
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
    scene_run(g_scenePrelude1, entry);
}

static void set_black(int level) {   /* 0 = clear, 16 = black */
    REG_BLDCNT = BLD_BG0 | BLD_OBJ | BLD_BACKDROP | BLD_BLACK;
    REG_BLDY = level;
}

/* ---- Scripted sequences ---------------------------------------------- */

static void opening(void) {
    int i, f;
    /* Hold black on the church while the crowd settles, then fade in. */
    g_camera.x = int2fx(120);
    g_camera.y = 0;
    for (f = 0; f < 64; f++) {
        frame_begin();
        set_black(16 - (f > 16 ? (f - 16) / 3 : 0));
        frame_end();
    }
    set_black(0);
    REG_BLDCNT = 0;
    talk(SCN_P1_INTRO);

    /* Pan down to the south gate as the procession walks in. */
    for (f = 0; f < 240; f++) {
        BOOL done = TRUE;
        frame_begin();
        if (!walk_ghost(s_guide, 14 * 16 + 8, 14 * 16, WALK_SPEED)) done = FALSE;
        for (i = 0; i < PILGRIM_COUNT; i++) {
            Actor *p = s_line[i];
            int tx = 14 * 16 + 8 + ((i & 1) ? -6 : 6);
            if (!walk_ghost(p, tx, 14 * 16 + FOLLOW_GAP_PX * (i + 1), WALK_SPEED)) done = FALSE;
        }
        camera_ease(s_guide->x + int2fx(8), s_guide->y, 2);
        frame_end();
        if (done && g_camera.y == int2fx(14 * 16 - 80)) break;
    }

    /* Introductions: the guide turns to face the pilgrims. */
    s_guide->facing = FACE_DOWN;
    for (i = 0; i < PILGRIM_COUNT; i++) s_line[i]->facing = FACE_UP;
    talk(SCN_P1_MEET);
    s_guide->facing = FACE_UP;
}

static void snatch(void) {
    Actor *silv = s_pil[P_SILVANUS];
    int f;
    s_thief = spawn_char(AKIND_ENEMY, SPR_LOOTER, MT(27), MT(9));
    s_thief->facing = FACE_LEFT;
    s_thiefStuck = 0;
    s_guide->state = AST_IDLE;
    for (f = 0; f < 300; f++) {
        frame_begin();
        procession_update(PH_WALK2);
        if (walk_to(s_thief, px(silv->x) + 10, px(silv->y), RUN_SPEED, &s_thiefStuck)) break;
        camera_on_guide();
        frame_end();
    }
    /* The grab. */
    s_thief->state = AST_IDLE;
    face_actor(s_thief, silv);
    face_actor(silv, s_thief);
    s_emoteOn = silv;
    talk(SCN_P1_SNATCH);
    s_emoteOn = NULL;
}

static int s_thiefCorner = -1, s_thiefPrev = -1; /* edge prev -> corner */
static u16 s_thiefTurnCooldown;
static Pt s_thiefGoal;

static int dist_guide_pt(Pt p) { return dist_pt(s_guide, p.x, p.y); }

static void thief_set_corner(int prev, int corner) {
    s_thiefPrev = prev;
    s_thiefCorner = corner;
    s_thiefGoal = CORNERS[corner];
}

/* From mid-court he first runs straight to whichever lane (north or south)
 * is farther from the guide, then circles the courtyard lanes. */
static void thief_start(void) {
    int tx = px(s_thief->x);
    s_thiefCorner = s_thiefPrev = -1;
    s_thiefTurnCooldown = 0;
    s_thiefStuck = 0;
    s_thiefGoal.x = tx;
    s_thiefGoal.y = dist_pt(s_guide, tx, 6 * 16) >= dist_pt(s_guide, tx, 18 * 16) ? 6 * 16 : 18 * 16;
}

static void thief_chase_update(void) {
    Actor *t = s_thief;
    if (t->state == AST_HITSTUN) return;
    int dGuide = dist_px(t, s_guide);
    if (s_thiefTurnCooldown) s_thiefTurnCooldown--;

    /* Cut off ahead on his own edge: double back the way he came. */
    if (s_thiefPrev >= 0 && !s_thiefTurnCooldown && dGuide < 48 &&
        dist_guide_pt(s_thiefGoal) < dist_pt(t, s_thiefGoal.x, s_thiefGoal.y)) {
        thief_set_corner(s_thiefCorner, s_thiefPrev);
        s_thiefTurnCooldown = 45;
    }

    fx8 speed = dGuide <= THIEF_PANIC_PX ? THIEF_PANIC_SPEED : THIEF_SPEED;
    if (!walk_to(t, s_thiefGoal.x, s_thiefGoal.y, speed, &s_thiefStuck)) return;

    if (s_thiefCorner < 0) {
        /* On a lane: take the corner along it that leads away. */
        int k, best = 0, bestD = -9999;
        for (k = 0; k < 4; k++) {
            if (CORNERS[k].y != s_thiefGoal.y) continue;
            int d = dist_guide_pt(CORNERS[k]) - dist_pt(t, CORNERS[k].x, CORNERS[k].y);
            if (d > bestD) { bestD = d; best = k; }
        }
        thief_set_corner(-1, best);
    } else {
        int c = s_thiefCorner, n1 = (c + 1) & 3, n2 = (c + 3) & 3;
        thief_set_corner(c, dist_guide_pt(CORNERS[n1]) >= dist_guide_pt(CORNERS[n2]) ? n1 : n2);
    }
}

static void thief_caught(void) {
    Actor *t = s_thief;
    int f;
    /* Knockback slide while stunned; the purse spills. */
    s_purse.x = px(t->x) + 4;
    s_purse.y = px(t->y) + 6;
    s_purseState = PURSE_GROUND;
    for (f = 0; f < 60 && t->state == AST_HITSTUN; f++) {
        frame_begin();
        s_guide->vx = s_guide->vy = 0;
        collision_move_actor(t, t->vx, t->vy);
        t->vx = t->vx * 7 / 8;
        t->vy = t->vy * 7 / 8;
        procession_update(PH_CHASE);
        camera_on_guide();
        frame_end();
    }
    t->state = AST_IDLE;
    talk(SCN_P1_CAUGHT);

    /* He bolts for the nearest side of the courtyard and is gone. */
    fx8 dir = px(t->x) < 15 * 16 ? -FX8(2.5) : FX8(2.5);
    for (f = 0; f < 120; f++) {
        int sx;
        frame_begin();
        t->state = AST_WALK;
        t->vx = t->vy = 0;
        t->facing = dir < 0 ? FACE_LEFT : FACE_RIGHT;
        t->x += dir;
        procession_update(PH_PURSE);
        camera_on_guide();
        frame_end();
        sx = px(t->x) - px(g_camera.x);
        if (sx < -32 || sx > SCREEN_WIDTH + 16) break;
    }
    actor_kill(t);
    s_thief = NULL;
    talk(SCN_P1_LOOTER_FLEES);
}

static void finale(void) {
    int f, i;
    s_guide->state = AST_IDLE;
    s_guide->facing = FACE_UP;
    talk(SCN_P1_FINALE);
    scene_set_frame_hook(NULL);

    /* Up the steps and in through the door, fading as they go. */
    for (f = 0; f < 120; f++) {
        frame_begin();
        int doorX = 14 * 16 + 8;
        if (s_guide->kind == AKIND_NONE) {
            /* already through the door */
        } else if (dist_pt(s_guide, doorX, px(s_guide->y)) > 2) {
            walk_ghost(s_guide, doorX, px(s_guide->y), WALK_SPEED);
        } else {
            s_guide->state = AST_WALK;
            s_guide->facing = FACE_UP;
            s_guide->y -= FX8(0.75);
        }
        for (i = 0; i < s_lineN; i++) {
            Actor *ahead = i == 0 ? s_guide : s_line[i - 1];
            if (s_line[i]->kind == AKIND_NONE) continue;
            if (ahead->kind == AKIND_NONE) {
                walk_ghost(s_line[i], doorX, DOOR_Y_PX - 8, WALK_SPEED);
            } else if (dist_px(s_line[i], ahead) > 14) {
                walk_ghost(s_line[i], px(ahead->x), px(ahead->y), WALK_SPEED);
            } else {
                s_line[i]->state = AST_IDLE;
            }
        }
        /* Swallowed by the doorway once their feet cross the threshold. */
        if (s_guide->kind != AKIND_NONE && px(s_guide->y) < DOOR_Y_PX) actor_kill(s_guide);
        for (i = 0; i < s_lineN; i++)
            if (s_line[i]->kind != AKIND_NONE && px(s_line[i]->y) < DOOR_Y_PX) actor_kill(s_line[i]);
        if (f >= 56) set_black((f - 56) / 4 > 16 ? 16 : (f - 56) / 4);
        camera_on_guide();
        frame_end();
    }
    set_black(16);

    /* Narrate over black. */
    actor_pool_reset();
    oam_pool_reset();
    vsync_wait();
    oam_pool_flush();
    REG_DISPCNT &= ~DCNT_BG0;
    pal_bg_mem[0] = CLR_BLACK; /* backdrop; the next map reloads it */
    REG_BLDCNT = 0;
    REG_BLDY = 0;
    scene_run(g_scenePrelude1, SCN_P1_OUTRO);
}

/* ---- Main ------------------------------------------------------------ */

void prelude1_run(void) {
    int i;
    set_black(16);
    map_load(&g_mapPrelude1Courtyard);
    actor_pool_reset();
    oam_pool_reset();
    sprites_load_prelude1();
    s_emoteSlot = oam_pool_alloc();
    s_purseSlot = oam_pool_alloc();
    s_emoteOn = NULL;
    s_purseState = PURSE_NONE;
    s_theoAway = FALSE;
    s_thief = NULL;
    s_tick = 0;

    crowd_spawn();

    /* The procession waits below the south gate, off the map; the opening
     * walks it in along the processional path (metatile columns 13-16). */
    s_guide = spawn_char(AKIND_PLAYER, SPR_GUIDE, MT(14) + int2fx(8), MT(20) + int2fx(8));
    s_guide->hp = 10;
    s_guide->facing = FACE_UP;
    static const u8 order[PILGRIM_COUNT] = { P_SILVANUS, P_THEO, P_ANNA };
    static const u8 pal[PILGRIM_COUNT] = { 1, 0, 2 };
    s_lineN = 0;
    for (i = 0; i < PILGRIM_COUNT; i++) {
        Actor *p = spawn_char(AKIND_COMPANION, SPR_PILGRIM,
                              MT(14) + int2fx(8 + ((i & 1) ? -6 : 6)),
                              MT(20) + int2fx(8 + FOLLOW_GAP_PX * (i + 1)));
        p->palVariant = pal[i];
        p->facing = FACE_UP;
        s_pil[order[i]] = p;
        s_pilStuck[order[i]] = 0;
        line_append(p);
    }

    scene_set_frame_hook(ambient_update);
    opening();

    Phase ph = PH_WALK1;
    BOOL onSteps = FALSE;

    for (;;) {
        frame_begin();

        /* Guide movement. */
        fx8 speed = key_is_down(KEY_B) ? RUN_SPEED : WALK_SPEED;
        fx8 dx = 0, dy = 0;
        Actor *g = s_guide;
        if (key_is_down(KEY_LEFT))  { dx = -speed; g->facing = FACE_LEFT; }
        if (key_is_down(KEY_RIGHT)) { dx = speed;  g->facing = FACE_RIGHT; }
        if (key_is_down(KEY_UP))    { dy = -speed; g->facing = FACE_UP; }
        if (key_is_down(KEY_DOWN))  { dy = speed;  g->facing = FACE_DOWN; }
        if (g->state == AST_IDLE || g->state == AST_WALK) {
            g->vx = g->vy = 0;
            g->state = (dx != 0 || dy != 0) ? AST_WALK : AST_IDLE;
            collision_move_actor(g, dx, dy);
        }

        /* A: talk to whoever needs it, otherwise shove. */
        if (key_hit(KEY_A)) {
            Actor *theo = s_pil[P_THEO], *silv = s_pil[P_SILVANUS];
            if (s_theoAway && dist_px(g, theo) <= TALK_PX) {
                g->state = AST_IDLE;
                face_actor(g, theo);
                face_actor(theo, g);
                talk(ph == PH_THEO1 ? SCN_P1_RECALL1 : SCN_P1_RECALL2);
                theo_recall();
                ph = ph == PH_THEO1 ? PH_WALK2 : PH_FREE;
                continue;
            }
            if (s_purseState == PURSE_CARRIED && dist_px(g, silv) <= TALK_PX) {
                g->state = AST_IDLE;
                face_actor(g, silv);
                face_actor(silv, g);
                s_purseState = PURSE_DONE;
                talk(SCN_P1_RETURN);
                ph = PH_WALK3;
                continue;
            }
            /* The vignette resolves the shove itself (feet hitboxes are too
             * small for combat_update's overlap test to feel fair). */
            combat_bash(g);
            if (g->state == AST_BASH && ph == PH_CHASE && s_thief->state != AST_HITSTUN &&
                dist_px(g, s_thief) <= TALK_PX) {
                combat_apply_hit(s_thief, 0, HITSTUN_FRAMES_HEAVY);
                s_thief->vx = s_thief->x > g->x ? SHOVE_KNOCK : -SHOVE_KNOCK;
                s_thief->vy = s_thief->y > g->y ? SHOVE_KNOCK / 2 : -SHOVE_KNOCK / 2;
                ph = PH_PURSE;
                thief_caught();
                continue;
            }
        }

        procession_update(ph);
        if (ph == PH_CHASE) thief_chase_update();

        /* Purse pickup. */
        if (s_purseState == PURSE_GROUND &&
            dist_pt(g, s_purse.x - 4, s_purse.y - 4) <= 10) {
            s_purseState = PURSE_CARRIED;
        }

        /* Story beats, keyed to how far up the courtyard the guide is. */
        if (ph == PH_WALK1 && g->y < MT(THEO_WELL_ROW)) {
            theo_send(THEO_TO_WELL, 1, FACE_LEFT);
            ph = PH_THEO1;
            talk(SCN_P1_THEO_WELL);
            continue;
        }
        if (ph == PH_WALK2 && g->y < MT(THIEF_ROW)) {
            snatch();
            thief_start();
            ph = PH_CHASE;
            continue;
        }
        if (ph == PH_WALK3 && g->y < MT(THEO_FIGS_ROW)) {
            theo_send(THEO_TO_FIGS, 3, FACE_RIGHT);
            ph = PH_THEO2;
            talk(SCN_P1_THEO_FIGS);
            continue;
        }

        /* Goal: the Anastasis steps, with the procession gathered. */
        BOOL nowOnSteps = (flags_under(g) & MTF_TRIGGER) != 0;
        if (nowOnSteps && ph == PH_FREE && procession_together()) {
            g->state = AST_IDLE;
            camera_on_guide();
            frame_end();
            break;
        }
        if (nowOnSteps && !onSteps) {
            g->state = AST_IDLE;
            onSteps = TRUE;
            talk(ph == PH_CHASE || ph == PH_PURSE ? SCN_P1_WAIT_PURSE : SCN_P1_WAIT);
            continue;
        }
        onSteps = nowOnSteps;

        camera_on_guide();
        frame_end();
    }

    finale();
    scene_set_frame_hook(NULL);
}
