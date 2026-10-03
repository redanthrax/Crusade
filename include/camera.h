/* camera.h -- simple world-to-screen scroll tracking a target actor. */
#ifndef CRUSADE_CAMERA_H
#define CRUSADE_CAMERA_H

#include "fixed.h"

typedef struct Camera {
    fx8 x, y;              /* top-left world position shown on screen */
    s16 mapWidthPx, mapHeightPx; /* clamp bounds, set by map_load() */
    u8 bg;                 /* BG the camera scrolls (set by camera_follow) */
} Camera;

extern Camera g_camera;

/* Resets scroll to (0,0) and stores the clamp bounds for the loaded map. */
void camera_reset(s16 mapWidthPx, s16 mapHeightPx);

/* Centers the camera on (targetX, targetY), clamped to the map bounds, for
 * the given background. Only computes; call camera_commit() right after
 * VBlankIntrWait() so the scroll changes between frames (no tearing). */
void camera_follow(fx8 targetX, fx8 targetY, int bg);

/* Writes the camera position to the BG scroll registers. VBlank only. */
void camera_commit(void);

#endif /* CRUSADE_CAMERA_H */
