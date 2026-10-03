/* oam_pool.h -- shared hardware sprite slot allocator.
 *
 * OBJ_ATTR shadow buffer lives in IWRAM (libtonc's obj_mem / oam_mem), one
 * slot per active Actor::oamId. We cap active slots well under the
 * hardware's 128 to leave headroom, per the project's ~96-sprite budget.
 */
#ifndef CRUSADE_OAM_POOL_H
#define CRUSADE_OAM_POOL_H

#include <tonc.h>

#define MAX_OAM_SLOTS 96

/* Clears the shadow OAM buffer and the free-slot bookkeeping. Call once at
 * boot and on map load. */
void oam_pool_reset(void);

/* Reserves a slot. Returns its index, or -1 if the pool is full. */
s8 oam_pool_alloc(void);

/* Releases a slot back to the pool and hides its sprite (sets ATTR0 to
 * OBJ_DISABLE-style hidden via attribute set). */
void oam_pool_free(s8 slot);

/* Copies the shadow buffer to real OAM. Call once per frame inside VBlank. */
void oam_pool_flush(void);

/* Shadow buffer, indexed by slot. Game code writes OBJ_ATTR fields here and
 * oam_pool_flush() copies them out. */
extern OBJ_ATTR g_oamShadow[MAX_OAM_SLOTS];

#endif /* CRUSADE_OAM_POOL_H */
