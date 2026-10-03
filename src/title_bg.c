/* title_bg.c -- procedural title/menu backdrop (see title_bg.h).
 *
 * Everything is drawn once into an 8bpp-per-pixel canvas in EWRAM (.sbss,
 * uninitialised, not stored in ROM), packed into 4bpp tiles with empty
 * tiles collapsed onto tile 0, and cached for re-upload. Not hot-path code:
 * it runs once, the first time a menu opens.
 */
#include <string.h>
#include <tonc.h>
#include "title_bg.h"
#include "placeholder_gfx.h"
#include "gothic.h"

#define CW 240
#define CH 160
#define MAX_TILES 512 /* one 16KB charblock of 4bpp tiles */
#define LOGO_Y 7
#define SUB_Y  (LOGO_Y + GOTHIC_LOGO_H + 2)

enum {
    C_CLEAR = 0,
    C_SIL,       /* city silhouette */
    C_WINDOW,    /* lamp-lit window */
    C_STAR,      /* bright star (twinkles) */
    C_HILL_FAR,
    C_GOLD_HI,   /* logo gradient, top */
    C_GOLD,
    C_GOLD_LO,
    C_OUTLINE,   /* logo outline + drop shadow */
    C_STAR_DIM,  /* dim star (twinkles opposite phase) */
    C_PARCH,     /* subtitle */
    C_HILL_NEAR
};

static const u16 s_pal[16] = {
    BGR15(0, 0, 0),    BGR15(2, 1, 5),    BGR15(31, 22, 8),  BGR15(30, 30, 31),
    BGR15(9, 5, 13),   BGR15(31, 28, 14), BGR15(30, 21, 6),  BGR15(22, 13, 3),
    BGR15(3, 1, 4),    BGR15(16, 16, 24), BGR15(28, 25, 19), BGR15(5, 3, 9),
    0, 0, 0, 0
};

static u8  s_canvas[CH][CW]         __attribute__((section(".sbss")));
static u32 s_tiles[MAX_TILES][8]    __attribute__((section(".sbss")));
static u16 s_map[CH / 8][CW / 8]    __attribute__((section(".sbss")));
static u16 s_sky[CH + 1]; /* +1: the HBlank after line 159 reads one past */
static int s_tileCount;
static BOOL s_built = FALSE;
static BOOL s_visible = FALSE;

/* ---- drawing primitives ------------------------------------------------ */

static void rect(int x0, int y0, int x1, int y1, u8 c) {
    int x, y;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > CW) x1 = CW;
    if (y1 > CH) y1 = CH;
    for (y = y0; y < y1; y++)
        for (x = x0; x < x1; x++)
            s_canvas[y][x] = c;
}

static int isqrt(int n) {
    int r = 0;
    while ((r + 1) * (r + 1) <= n) r++;
    return r;
}

static void dome(int cx, int baseY, int r, u8 c) {
    int dx;
    for (dx = -r; dx <= r; dx++) {
        int h = isqrt(r * r - dx * dx);
        rect(cx + dx, baseY - h, cx + dx + 1, baseY, c);
    }
}

/* Battlements: 2px merlons every 4px sitting on top of y=top. */
static void crenels(int x0, int x1, int top, u8 c) {
    int x;
    for (x = x0; x < x1; x++)
        if ((((x - x0) >> 1) & 1) == 0)
            rect(x, top - 2, x + 1, top, c);
}

static void cypress(int cx, int top, int bottom) {
    int y;
    for (y = top; y < bottom; y++) {
        int t = y - top;
        int half = (t < 2) ? 0 : (t < 6) ? 1 : 2;
        rect(cx - half, y, cx + half + 1, y + 1, C_SIL);
    }
}

/* ---- text ------------------------------------------------------------- */

/* Logo gold gradient: thirds of the bitmap height. */
static u8 logo_ink(int row) {
    return row < GOTHIC_LOGO_H / 3 ? C_GOLD_HI :
           row < (GOTHIC_LOGO_H * 2) / 3 ? C_GOLD : C_GOLD_LO;
}

static BOOL is_ink(u8 c) {
    return c == C_GOLD_HI || c == C_GOLD || c == C_GOLD_LO || c == C_PARCH;
}

/* Drop shadow (+1,+1) then a 4-neighbour outline around all ink pixels in
 * rows [y0,y1). Iterating bottom-right to top-left keeps the shadow pass
 * from feeding on itself; outline pixels never count as ink. */
static void outline_band(int y0, int y1) {
    int x, y;
    for (y = y1 - 1; y >= y0; y--)
        for (x = CW - 2; x >= 0; x--)
            if (is_ink(s_canvas[y][x]) && s_canvas[y + 1][x + 1] == C_CLEAR)
                s_canvas[y + 1][x + 1] = C_OUTLINE;
    for (y = y0; y < y1; y++)
        for (x = 1; x < CW - 1; x++)
            if (s_canvas[y][x] == C_CLEAR &&
                (is_ink(s_canvas[y][x - 1]) || is_ink(s_canvas[y][x + 1]) ||
                 is_ink(s_canvas[y - 1][x]) || is_ink(s_canvas[y + 1][x])))
                s_canvas[y][x] = C_OUTLINE;
}

/* ---- scene ------------------------------------------------------------- */

static void draw_stars(void) {
    u32 seed = 0x1099; /* deterministic: same sky every boot */
    int i;
    for (i = 0; i < 90; i++) {
        seed = seed * 1664525u + 1013904223u;
        int x = (seed >> 8) % CW;
        int y = 2 + (seed >> 20) % 104;
        /* keep the logo and menu areas clean */
        if (x >= 24 && x < 216 && y >= 6 && y < 68) continue;
        if (x >= 40 && x < 200 && y >= 68 && y < 116) continue;
        if ((i & 7) == 0) { /* a few larger four-point stars */
            rect(x - 1, y, x + 2, y + 1, C_STAR_DIM);
            rect(x, y - 1, x + 1, y + 2, C_STAR_DIM);
            rect(x, y, x + 1, y + 1, C_STAR);
        } else {
            rect(x, y, x + 1, y + 1, (i & 1) ? C_STAR : C_STAR_DIM);
        }
    }
}

static void draw_hills(void) {
    int x;
    for (x = 0; x < CW; x++) {
        int far = 118 + ((lu_sin(x * 180) * 5) >> 12) +
                  ((lu_sin(x * 530 + 0x3000) * 3) >> 12);
        int near = 130 + ((lu_sin(x * 400 + 0x8000) * 4) >> 12);
        rect(x, far, x + 1, CH, C_HILL_FAR);
        rect(x, near, x + 1, CH, C_HILL_NEAR);
    }
}

typedef struct { u8 x, w, top; } House;

static const House s_houses[] = {
    {0, 14, 128},  {36, 10, 126}, {46, 12, 130}, {64, 14, 124},
    {78, 10, 128}, {88, 16, 122}, {104, 12, 126}, {116, 14, 120},
    {130, 10, 127}, {140, 10, 124}, {198, 10, 122}, {216, 12, 118},
    {228, 12, 126},
};

static const u8 s_windows[][2] = {
    {23, 106}, {23, 118}, {19, 128}, {27, 128},           /* Tower of David */
    {4, 132}, {40, 130}, {68, 130}, {92, 128}, {108, 130},
    {120, 126}, {134, 132}, {202, 128}, {220, 124}, {232, 132},
    {156, 128}, {166, 128}, {180, 124}, {190, 124}, {200, 134}, /* Sepulchre */
    {211, 106}, {211, 118},                                  /* bell tower */
    {57, 120},
};

static void draw_city(void) {
    unsigned i;
    int r;

    /* city wall */
    rect(0, 140, CW, CH, C_SIL);
    crenels(0, CW, 140, C_SIL);

    for (i = 0; i < sizeof(s_houses) / sizeof(s_houses[0]); i++)
        rect(s_houses[i].x, s_houses[i].top,
             s_houses[i].x + s_houses[i].w, 140, C_SIL);
    dome(96, 122, 4, C_SIL);
    dome(123, 120, 5, C_SIL);
    dome(234, 126, 4, C_SIL);

    /* Tower of David (citadel, left) and a small wall tower */
    rect(14, 98, 34, 140, C_SIL);
    crenels(14, 34, 98, C_SIL);
    rect(52, 112, 62, 140, C_SIL);
    crenels(52, 62, 112, C_SIL);

    /* Church of the Holy Sepulchre: nave, rotunda drum, great dome,
     * lantern and cross; small dome over the side chapel. */
    rect(150, 118, 206, 140, C_SIL);
    rect(173, 110, 198, 118, C_SIL);
    dome(185, 110, 12, C_SIL);
    rect(184, 94, 187, 99, C_SIL);
    rect(185, 85, 186, 94, C_SIL);
    rect(183, 88, 188, 89, C_SIL);
    dome(160, 118, 7, C_SIL);

    /* bell tower with a pyramid roof */
    rect(208, 100, 216, 140, C_SIL);
    for (r = 0; r < 8; r++) {
        int w = 1 + r;
        rect(212 - (w >> 1), 92 + r, 212 - (w >> 1) + w, 93 + r, C_SIL);
    }

    cypress(44, 116, 130);
    cypress(72, 112, 126);
    cypress(144, 114, 126);
    cypress(224, 108, 120);

    for (i = 0; i < sizeof(s_windows) / sizeof(s_windows[0]); i++)
        rect(s_windows[i][0], s_windows[i][1],
             s_windows[i][0] + 1, s_windows[i][1] + 2, C_WINDOW);
}

static void draw_logo(void) {
    const char *sub = "Godfrey of Bouillon";
    gothic_draw_logo(&s_canvas[0][0], CW, CW, CH,
                     (CW - GOTHIC_LOGO_W) / 2, LOGO_Y, logo_ink);
    gothic_draw(&s_canvas[0][0], CW, CW, CH,
                (CW - gothic_text_width(sub)) / 2, SUB_Y, sub, C_PARCH);

    /* ornament: rule with a centre lozenge */
    rect(84, 61, 112, 62, C_GOLD_LO);
    rect(129, 61, 157, 62, C_GOLD_LO);
    rect(120, 58, 121, 65, C_GOLD);
    rect(118, 60, 123, 63, C_GOLD);
    rect(119, 59, 122, 64, C_GOLD);
    rect(120, 60, 121, 63, C_GOLD_HI);

    outline_band(LOGO_Y - 2, 68);
}

static void build_sky(void) {
    static const struct { u8 y, r, g, b; } k[] = {
        {0, 1, 1, 6}, {50, 4, 3, 12}, {90, 11, 6, 15},
        {115, 22, 11, 12}, {135, 30, 19, 9}, {160, 31, 24, 12},
    };
    int y, i = 0;
    for (y = 0; y <= CH; y++) {
        int yy = (y < CH) ? y : CH - 1;
        while (yy > k[i + 1].y) i++;
        int span = k[i + 1].y - k[i].y, t = yy - k[i].y;
        int r = k[i].r + (k[i + 1].r - k[i].r) * t / span;
        int g = k[i].g + (k[i + 1].g - k[i].g) * t / span;
        int b = k[i].b + (k[i + 1].b - k[i].b) * t / span;
        s_sky[y] = BGR15(r, g, b);
    }
}

static void pack_tiles(void) {
    int tx, ty, r, i;
    memset(s_tiles[0], 0, sizeof(s_tiles[0]));
    s_tileCount = 1;
    for (ty = 0; ty < CH / 8; ty++) {
        for (tx = 0; tx < CW / 8; tx++) {
            u32 t[8], any = 0;
            for (r = 0; r < 8; r++) {
                const u8 *row = &s_canvas[ty * 8 + r][tx * 8];
                u32 w = 0;
                for (i = 7; i >= 0; i--) w = (w << 4) | row[i];
                t[r] = w;
                any |= w;
            }
            if (any == 0 || s_tileCount >= MAX_TILES) {
                s_map[ty][tx] = 0;
            } else {
                memcpy(s_tiles[s_tileCount], t, sizeof(t));
                s_map[ty][tx] = (u16)s_tileCount++;
            }
        }
    }
}

static void build(void) {
    memset(s_canvas, 0, sizeof(s_canvas));
    draw_stars();
    draw_hills();
    draw_city();
    draw_logo();
    pack_tiles();
    build_sky();
    s_built = TRUE;
}

/* ---- public API -------------------------------------------------------- */

void title_bg_show(void) {
    int ty;
    if (s_visible) return;
    if (!s_built) build();

    REG_DMA[0].cnt = 0;
    memcpy32(tile_mem[0], s_tiles, s_tileCount * 8);
    for (ty = 0; ty < CH / 8; ty++)
        memcpy16(&se_mem[16][ty * 32], s_map[ty], CW / 8);
    memcpy16(pal_bg_mem, s_pal, 16);
    pal_bg_mem[0] = s_sky[0];

    REG_BG0CNT = BG_CBB(0) | BG_SBB(16) | BG_4BPP | BG_PRIO(1);
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_DISPCNT |= DCNT_BG0;
    s_visible = TRUE;
}

void title_bg_vblank(u32 frame) {
    if (!s_visible) return;
    pal_bg_mem[0] = s_sky[0];
    DMA_TRANSFER(&pal_bg_mem[0], &s_sky[1], 1, 0, DMA_HDMA | DMA_16);

    /* triangle wave 0..31..0 over 64 frames */
    int p = frame & 63, tri = (p < 32) ? p : 63 - p;
    int hi = 22 + (tri >> 2), lo = 22 + ((31 - tri) >> 2);
    pal_bg_mem[C_STAR] = RGB15(hi, hi, 31);
    pal_bg_mem[C_STAR_DIM] = RGB15(lo - 6, lo - 6, lo);
}

void title_bg_hide(void) {
    REG_DMA[0].cnt = 0;
    pal_bg_mem[0] = 0;
    REG_DISPCNT &= ~DCNT_BG0;
    s_visible = FALSE;
}
