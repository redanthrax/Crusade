#include <tonc.h>
#include "camera.h"

Camera g_camera;

void camera_reset(s16 mapWidthPx, s16 mapHeightPx) {
    g_camera.x = 0;
    g_camera.y = 0;
    g_camera.bg = 0;
    g_camera.mapWidthPx = mapWidthPx;
    g_camera.mapHeightPx = mapHeightPx;
}

void camera_follow(fx8 targetX, fx8 targetY, int bg) {
    fx8 halfW = FX8(SCREEN_WIDTH / 2);
    fx8 halfH = FX8(SCREEN_HEIGHT / 2);
    fx8 cx = targetX - halfW;
    fx8 cy = targetY - halfH;

    fx8 maxX = int2fx(g_camera.mapWidthPx - SCREEN_WIDTH);
    fx8 maxY = int2fx(g_camera.mapHeightPx - SCREEN_HEIGHT);
    if (maxX < 0) maxX = 0;
    if (maxY < 0) maxY = 0;

    if (cx < 0) cx = 0;
    if (cy < 0) cy = 0;
    if (cx > maxX) cx = maxX;
    if (cy > maxY) cy = maxY;

    g_camera.x = cx;
    g_camera.y = cy;
    g_camera.bg = (u8)bg;
}

void camera_commit(void) {
    REG_BG_OFS[g_camera.bg].x = FX8_TO_INT(g_camera.x);
    REG_BG_OFS[g_camera.bg].y = FX8_TO_INT(g_camera.y);
}
