/* render.h -- syncs live Actors to the shared OAM pool each frame. */
#ifndef CRUSADE_RENDER_H
#define CRUSADE_RENDER_H

/* Allocates an OAM slot for any live actor that doesn't have one yet,
 * positions every live actor's sprite relative to the camera, and hides
 * slots whose actor died. Call once per frame before oam_pool_flush(). */
void render_actors_to_oam(void);

#endif /* CRUSADE_RENDER_H */
