/* gothic.c -- blackletter text rendering (see gothic.h). Menu-time only. */
#include <tonc.h>
#include "gothic.h"

static const u16 *glyph_rows(char ch, int *w) {
    int i = (u8)ch - GOTHIC_FIRST;
    if (i < 0 || i >= GOTHIC_COUNT) i = '?' - GOTHIC_FIRST;
    *w = gothic_width[i];
    return gothic_rows[i];
}

int gothic_text_width(const char *s) {
    int total = 0, w;
    for (; *s; s++) {
        glyph_rows(*s, &w);
        total += w + (s[1] ? GOTHIC_TRACKING : 0);
    }
    return total;
}

void gothic_draw(u8 *buf, int pitch, int w, int h, int x, int y,
                 const char *s, u8 ink) {
    for (; *s; s++) {
        int gw, gx, gy;
        const u16 *rows = glyph_rows(*s, &gw);
        for (gy = 0; gy < GOTHIC_HEIGHT; gy++) {
            int py = y + gy;
            if (py < 0 || py >= h) continue;
            for (gx = 0; gx < gw; gx++) {
                int px = x + gx;
                if (px >= 0 && px < w && ((rows[gy] >> gx) & 1))
                    buf[py * pitch + px] = ink;
            }
        }
        x += gw + GOTHIC_TRACKING;
    }
}

void gothic_draw_logo(u8 *buf, int pitch, int w, int h, int x, int y,
                      u8 (*rowInk)(int row)) {
    int lx, ly;
    for (ly = 0; ly < GOTHIC_LOGO_H; ly++) {
        const u8 *src = &gothic_logo[ly * GOTHIC_LOGO_STRIDE];
        u8 ink = rowInk(ly);
        int py = y + ly;
        if (py < 0 || py >= h) continue;
        for (lx = 0; lx < GOTHIC_LOGO_W; lx++) {
            int px = x + lx;
            if (px >= 0 && px < w && ((src[lx >> 3] >> (lx & 7)) & 1))
                buf[py * pitch + px] = ink;
        }
    }
}
