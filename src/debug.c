#include <tonc.h>
#include "debug.h"

#ifdef CRUSADE_DEBUG
#include "ui.h"

#define LINES_PER_FRAME 228
#define VBLANK_LINE     160
#define MAX_GAP         10   /* longer gaps are loads/menus, not drops */

static volatile u32 s_vblanks;
static u32 s_last, s_secStart, s_dropped;
static u16 s_framesSec, s_peakLines;

static void vblank_isr(void) { s_vblanks++; }

void debug_init(void) {
    irq_add(II_VBLANK, vblank_isr);
}

static char *put_uint(char *p, u32 v, int width) {
    char tmp[10];
    int n = 0;
    do { tmp[n++] = '0' + v % 10; v /= 10; } while (v && n < 10);
    while (width-- > n) *p++ = ' ';
    while (n) *p++ = tmp[--n];
    return p;
}

static void draw_hud(u32 fps, u32 cpuPct) {
    char buf[32], *p = buf;
    const char *s;
    for (s = "FPS "; *s; s++) *p++ = *s;
    p = put_uint(p, fps, 2);
    for (s = " DROP "; *s; s++) *p++ = *s;
    p = put_uint(p, s_dropped > 999 ? 999 : s_dropped, 3);
    for (s = " CPU "; *s; s++) *p++ = *s;
    p = put_uint(p, cpuPct, 3);
    *p++ = '%';
    *p = '\0';
    ui_print_hud(30 - (int)(p - buf), 0, buf);
}

void vsync_wait(void) {
    u32 vc = REG_VCOUNT, now, gap;
    u32 used = (vc + LINES_PER_FRAME - VBLANK_LINE) % LINES_PER_FRAME;
    if (used > s_peakLines) s_peakLines = (u16)used;

    VBlankIntrWait();

    now = s_vblanks;
    gap = now - s_last;
    if (s_last != 0 && gap > 1 && gap <= MAX_GAP) s_dropped += gap - 1;
    if (gap > MAX_GAP) {             /* resync after a load or menu */
        s_secStart = now;
        s_framesSec = 0;
        s_peakLines = 0;
    }
    s_last = now;
    s_framesSec++;

    if (now - s_secStart >= 60) {
        draw_hud(s_framesSec * 60 / (now - s_secStart),
                 s_peakLines * 100 / LINES_PER_FRAME);
        s_secStart = now;
        s_framesSec = 0;
        s_peakLines = 0;
    }
}
#endif /* CRUSADE_DEBUG */
