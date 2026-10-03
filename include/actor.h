/* actor.h -- shared actor pool for player, companions, enemies, projectiles.
 *
 * Actors live in a single static array (EWRAM .bss). There is no heap
 * allocation: "freeing" an actor just marks it AST_DEAD and clears its
 * kind/oam slot so the next spawn can reuse the index.
 */
#ifndef CRUSADE_ACTOR_H
#define CRUSADE_ACTOR_H

#include <tonc.h>
#include "fixed.h"

#define MAX_ACTORS 64

typedef enum {
    AST_IDLE,
    AST_WALK,
    AST_ATTACK,
    AST_BASH,
    AST_THROW,
    AST_CHARGE,
    AST_HITSTUN,
    AST_DEAD,
    AST_SCRIPTED
} ActorState;

typedef enum {
    AKIND_NONE = 0, /* empty slot */
    AKIND_PLAYER,
    AKIND_COMPANION,
    AKIND_ENEMY,
    AKIND_PROJECTILE
} ActorKind;

typedef enum {
    FACE_DOWN = 0,
    FACE_UP,
    FACE_LEFT,
    FACE_RIGHT
} ActorFacing;

typedef struct Actor {
    fx8 x, y;                 /* position, .8 fixed, world space */
    fx8 vx, vy;                /* velocity, .8 fixed */
    s16 hbX, hbY, hbW, hbH;    /* hitbox rect, pixel offsets from x,y + size */
    u16 hp;                    /* 0 => dead */
    u8  stamina;               /* player/companion only, 0-255 */
    u8  kind;                  /* ActorKind */
    u8  state;                 /* ActorState */
    u8  facing;                /* ActorFacing */
    u16 timer;                 /* hitstun frames / attack windup / cooldown */
    u16 scriptId;               /* enemy AI table index or companion assist id */
    s8  oamId;                  /* index into the OAM slot pool, -1 if unused */
    u8  sprite;                 /* SpriteId (sprites.h); 0 = placeholder by kind */
    u8  palVariant;             /* added to the sprite's base OBJ palette bank */
    u8  animTick;               /* walk-cycle frame counter */
} Actor; /* 32 bytes incl. padding */

extern Actor g_actors[MAX_ACTORS];

/* Zeroes the pool. Call once at boot and on map load. */
void actor_pool_reset(void);

/* Finds a free (AKIND_NONE) slot and initializes it as the given kind.
 * Returns the actor pointer, or NULL if the pool is full. */
Actor *actor_spawn(ActorKind kind, fx8 x, fx8 y);

/* Marks an actor dead and frees its OAM slot. Safe to call on an
 * already-dead actor. */
void actor_kill(Actor *a);

/* Advances every live actor's state-machine timer by one frame and runs
 * per-kind update hooks (movement integration, hitstun countdown). Does not
 * do collision resolution; see collision.c for that pass. */
void actor_update_all(void);

#endif /* CRUSADE_ACTOR_H */
