/* collision.h -- metatile-based movement resolution. */
#ifndef CRUSADE_COLLISION_H
#define CRUSADE_COLLISION_H

#include "actor.h"
#include "fixed.h"

/* Moves `a` by (dx, dy), axis-separated so it slides along walls instead of
 * stopping dead on diagonal contact. Each axis is rejected independently if
 * the resulting hitbox would overlap an MTF_SOLID metatile. */
void collision_move_actor(Actor *a, fx8 dx, fx8 dy);

#endif /* CRUSADE_COLLISION_H */
