/* combat.h -- shared combat verbs: sword, bash, thrown spear, charge.
 *
 * All combat is hitbox-rect overlap against Actor::hbX/Y/W/H, resolved once
 * per frame from combat_update(). Stamina gates attacks (not mana); hitstun
 * is just AST_HITSTUN + Actor::timer counting down.
 */
#ifndef CRUSADE_COMBAT_H
#define CRUSADE_COMBAT_H

#include <tonc.h>
#include "actor.h"

#define STAMINA_MAX        255
#define STAMINA_ATTACK_COST  24
#define STAMINA_BASH_COST    32
#define STAMINA_THROW_COST   40
#define STAMINA_REGEN_PER_FRAME 1

#define HITSTUN_FRAMES_LIGHT 20
#define HITSTUN_FRAMES_HEAVY 36

/* Attempts a sword swing for `a` (usually the player or a companion).
 * No-ops if stamina is insufficient or `a` is already mid-action. */
void combat_attack(Actor *a);

/* Shield bash: short-range, staggers on hit instead of damaging. */
void combat_bash(Actor *a);

/* Thrown spear: spawns an AKIND_PROJECTILE actor traveling in `a`'s facing
 * direction. Returns the projectile actor, or NULL if the pool is full or
 * stamina is insufficient. */
Actor *combat_throw_spear(Actor *a);

/* Applies `damage` to `target`, puts it in AST_HITSTUN for `stunFrames`,
 * and kills it (actor_kill) if hp reaches 0. */
void combat_apply_hit(Actor *target, u16 damage, u16 stunFrames);

/* Runs hitbox overlap checks between all live actors whose kinds are
 * hostile to each other (player/companion vs enemy, projectiles vs any
 * opposing kind) and resolves hits via combat_apply_hit. Call once per
 * frame after actor_update_all(). */
void combat_update(void);

#endif /* CRUSADE_COMBAT_H */
