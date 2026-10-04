/* intro.c -- opening cinematic (see intro.h).
 *
 * Each scene is one grit image from tools/gen_intro_art.py: a vertical strip
 * of 240x160 frames (flat 30-wide maps) sharing one 16-colour palette. Scenes
 * are shown on BG0 with black brightness fades, palette animation (torches,
 * candles, the Seljuk advance) and map-entry swaps (Urban raising his arms,
 * the route drawing itself across the map). Fades only run while the
 * narration box is closed: the box's window disables blending outside it.
 */
#include <tonc.h>
#include "intro.h"
#include "ui.h"
#include "scene.h"
#include "text_ids.h"
#include "input.h"
#include "debug.h"

#include "intro_cards.h"
#include "intro_map.h"
#include "intro_clermont.h"
#include "intro_chapel.h"
#include "intro_route.h"

#define FRAME_ROWS 20
#define FRAME_COLS 30
#define SBB        16

typedef struct {
    const void *tiles;
    u32 tilesLen;
    const u16 *map;
    const u16 *pal;
} IntroImage;

#define IMAGE(n) { n##Tiles, n##TilesLen, (const u16 *)n##Map, (const u16 *)n##Pal }
static const IntroImage s_cards    = IMAGE(intro_cards);
static const IntroImage s_map      = IMAGE(intro_map);
static const IntroImage s_clermont = IMAGE(intro_clermont);
static const IntroImage s_chapel   = IMAGE(intro_chapel);

typedef enum { FX_NONE, FX_CARDS, FX_MAP, FX_CLERMONT, FX_CHAPEL } IntroFx;

static const IntroImage *s_img;
static u16 s_basePal[16];
static IntroFx s_fx;
static u32 s_t;
static BOOL s_skip;
static u32 s_rng = 0x1095u;

/* map state */
static int s_seljuk;       /* 0..64: Anatolia fading from parchment to red */
static BOOL s_seljukGrow;
static int s_routeShown;   /* cells of intro_routeCells[] revealed */
static BOOL s_routeGrow;

/* --- helpers ------------------------------------------------------------- */

static u32 rnd(void) {
    s_rng = s_rng * 1664525u + 1013904223u;
    return s_rng >> 16;
}

/* t in 0..64 */
static COLOR mix(COLOR a, COLOR b, int t) {
    int ra = a & 31, ga = (a >> 5) & 31, ba = (a >> 10) & 31;
    int rb = b & 31, gb = (b >> 5) & 31, bb = (b >> 10) & 31;
    return RGB15(ra + (rb - ra) * t / 64, ga + (gb - ga) * t / 64,
                 ba + (bb - ba) * t / 64);
}

static int tri(u32 t, int period, int amp) {
    int p = t % period, h = period / 2;
    return (p < h ? p : period - p) * amp / h;
}

static void show_frame(int f) {
    const u16 *src = s_img->map + f * FRAME_ROWS * FRAME_COLS;
    int r, c;
    for (r = 0; r < FRAME_ROWS; r++)
        for (c = 0; c < FRAME_COLS; c++)
            se_mem[SBB][r * 32 + c] = src[r * FRAME_COLS + c];
}

static void show_cell(int f, int cell) {
    int r = cell / FRAME_COLS, c = cell % FRAME_COLS;
    se_mem[SBB][r * 32 + c] = s_img->map[f * FRAME_ROWS * FRAME_COLS + cell];
}

/* Call only while the screen is black. */
static void load_image(const IntroImage *img, int frame, IntroFx fx) {
    s_img = img;
    memcpy32(tile_mem[0], img->tiles, img->tilesLen / 4);
    memcpy16(s_basePal, img->pal, 16);
    memcpy16(pal_bg_mem, img->pal, 16);
    show_frame(frame);
    s_fx = fx;
}

/* --- palette animation ----------------------------------------------------- */

static void fx_flame(int ink, int core) {
    if ((s_t & 3) == 0) {
        int k = rnd() % 24;
        pal_bg_mem[ink]  = mix(s_basePal[ink], RGB15(18, 6, 2), k);
        pal_bg_mem[core] = mix(s_basePal[core], s_basePal[ink], k);
    }
}

static void fx_tick(void) {
    switch (s_fx) {
    case FX_CARDS:
        pal_bg_mem[4] = mix(s_basePal[4], RGB15(31, 29, 20), tri(s_t, 120, 40));
        break;
    case FX_MAP: {
        int i;
        const COLOR red[3] = { RGB15(15, 4, 3), RGB15(9, 2, 1), RGB15(19, 7, 4) };
        if (s_seljukGrow && s_seljuk < 64) s_seljuk++;
        for (i = 0; i < 3; i++)
            pal_bg_mem[7 + i] = mix(s_basePal[7 + i], red[i], s_seljuk);
        pal_bg_mem[15] = mix(s_basePal[15], RGB15(29, 26, 20), s_seljuk);
        if (s_routeGrow && s_routeShown < INTRO_ROUTE_CELLS && (s_t & 1) == 0)
            show_cell(1, intro_routeCells[s_routeShown++]);
        /* Jerusalem's marker glints */
        pal_bg_mem[14] = mix(s_basePal[14], RGB15(31, 31, 24), tri(s_t, 60, 48));
        break;
    }
    case FX_CLERMONT:
        fx_flame(13, 14);
        break;
    case FX_CHAPEL: {
        int i, g = tri(s_t, 180, 14);
        fx_flame(13, 14);
        if ((s_t & 3) == 0)
            pal_bg_mem[15] = mix(s_basePal[15], s_basePal[2], rnd() % 20);
        for (i = 5; i <= 8; i++)
            pal_bg_mem[i] = mix(s_basePal[i], RGB15(31, 31, 28), g);
        pal_bg_mem[9] = mix(s_basePal[9], s_basePal[2], 14 - g);
        break;
    }
    default:
        break;
    }
}

/* --- timing ---------------------------------------------------------------- */

static void frame(void) {
    vsync_wait();
    fx_tick();
    input_poll();
    if (key_hit(KEY_START)) s_skip = TRUE;
    s_t++;
}

static void hold(int n) {
    while (n-- > 0 && !s_skip) frame();
}

static void set_black(int level) {   /* 0 = clear, 16 = black */
    REG_BLDCNT = BLD_BG0 | BLD_BACKDROP | BLD_BLACK;
    REG_BLDY = level;
}

static void fade_in(void) {
    int i;
    for (i = 16; i >= 0 && !s_skip; i--) {
        set_black(i);
        frame();
        frame();
    }
    set_black(0);
}

static void fade_out(void) {
    int i;
    for (i = 0; i <= 16; i++) {
        set_black(i);
        frame();
        if (!s_skip) frame();
    }
}

static void flash(void) {
    int i;
    REG_BLDCNT = BLD_BG0 | BLD_BACKDROP | BLD_WHITE;
    for (i = 12; i >= 0 && !s_skip; i--) {
        REG_BLDY = i;
        frame();
    }
    REG_BLDCNT = 0;
    REG_BLDY = 0;
}

/* Shows every page of a narration entry; the box stays open so consecutive
 * lines don't flicker. Caller closes it with ui_textbox_clear(). */
static void narrate(int textId) {
    int page, pages;
    if (s_skip) return;
    pages = ui_textbox_open(UI_TEXT_NARRATION, NULL, g_textBank[textId]);
    for (page = 0; page < pages && !s_skip; page++) {
        ui_textbox_page(page);
        while (1) {
            frame();
            ui_textbox_tick();
            if (s_skip || key_hit(KEY_A)) break;
        }
    }
}

static void close_box(void) {
    ui_textbox_clear();
}

/* --- the cinematic ------------------------------------------------------------ */

void intro_run(void) {
    s_skip = FALSE;
    s_t = 0;
    s_seljuk = 0;
    s_seljukGrow = FALSE;
    s_routeShown = 0;
    s_routeGrow = FALSE;

    set_black(16);
    REG_BG0CNT = BG_CBB(0) | BG_SBB(SBB) | BG_4BPP | BG_PRIO(1);
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_DISPCNT |= DCNT_BG0;

    /* 1095 */
    load_image(&s_cards, 0, FX_CARDS);
    fade_in();
    hold(60);
    narrate(TEXT_INTRO_YEAR);
    close_box();
    fade_out();

    /* The East falls to the Seljuks; Byzantium calls for help. */
    if (!s_skip) {
        load_image(&s_map, 0, FX_MAP);
        fade_in();
        hold(30);
        s_seljukGrow = TRUE;
        narrate(TEXT_INTRO_SELJUKS);
        narrate(TEXT_INTRO_ALEXIOS);
        close_box();
        fade_out();
    }

    /* Clermont */
    if (!s_skip) {
        load_image(&s_clermont, 0, FX_CLERMONT);
        fade_in();
        hold(40);
        narrate(TEXT_INTRO_CLERMONT);
        close_box();
        if (!s_skip) {
            show_frame(1);
            flash();
            hold(20);
        }
        narrate(TEXT_INTRO_DEUS_VULT);
        close_box();
        fade_out();
    }

    /* Godfrey takes the cross */
    if (!s_skip) {
        load_image(&s_chapel, 0, FX_CHAPEL);
        fade_in();
        hold(50);
        narrate(TEXT_INTRO_GODFREY);
        narrate(TEXT_INTRO_PLEDGE);
        close_box();
        fade_out();
    }

    /* The road east */
    if (!s_skip) {
        s_seljuk = 64;
        load_image(&s_map, 0, FX_MAP);
        fade_in();
        hold(20);
        s_routeGrow = TRUE;
        narrate(TEXT_INTRO_MARCH);
        narrate(TEXT_INTRO_ROAD);
        close_box();
        while (!s_skip && s_routeShown < INTRO_ROUTE_CELLS) frame();
        hold(40);
        fade_out();
    }

    /* ...but first, an empty tomb. */
    if (!s_skip) {
        load_image(&s_cards, 1, FX_CARDS);
        fade_in();
        hold(40);
        narrate(TEXT_INTRO_BEGIN);
        close_box();
        fade_out();
    }

    close_box();
    set_black(16);
    s_fx = FX_NONE;
    vsync_wait();
    REG_DISPCNT &= ~DCNT_BG0;
    REG_BLDCNT = 0;
    REG_BLDY = 0;
}
