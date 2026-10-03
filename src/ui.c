#include <tonc.h>
#include "ui.h"

/* Text box layout (tile rows of the 30x20 screen):
 *   row 13      speaker name tab (dialogue only)
 *   rows 14-19  opaque panel: border on rows 14/19, text on rows 15-18
 * The panel lives on BG2 (CBB1, SBB29, priority 1) beneath the BG3 text.
 * WIN0 (box) and WIN1 (name tab) show only BG2+BG3, so the world and
 * sprites never bleed through behind the letters. */
#define BOX_ROW        14
#define BOX_TOP_PX     (BOX_ROW * 8)
#define TAB_ROW        13
#define TEXT_ROW       15
#define TEXT_COL       1
#define TEXT_COLS      28
#define TEXT_LINES     4
#define MAX_LINES      24

#define PANEL_SBB      29
#define SHADOW_FONT    96     /* CBB1 tile of the drop-shadow glyph copy */
#define PANEL_TILE     192    /* CBB1 tile of the panel/frame tiles */
#define FONT_GLYPHS    96     /* sys8: ASCII 32-127 */

/* Palette banks (the world tileset only uses bank 0). */
#define PB_TAB_TEXT    7
#define PB_PANEL_NARR  8
#define PB_PANEL_DLG   9
#define PB_TEXT_NARR   10
#define PB_TEXT_DLG    11

/* Panel tile ids, offsets from PANEL_TILE. */
enum {
    PT_FILL, PT_TL, PT_T, PT_TR, PT_L, PT_R, PT_BL, PT_B, PT_BR, PT_B_ARROW,
    PT_TAB_L, PT_TAB, PT_TAB_R,
    PT_N_TOP, PT_N_BOT, PT_N_ORN_L, PT_N_ORN_R, PT_N_BOT_ARROW,
    PT_COUNT
};

/* Panel palette indices: 1 fill, 2 outline, 3 gold, 4 light gold, 5 dark gold. */
enum { C_CLEAR, C_FILL, C_LINE, C_GOLD, C_GOLD_HI, C_GOLD_LO };

#define SIDE_T 1
#define SIDE_B 2
#define SIDE_L 4
#define SIDE_R 8

static u8 s_tile[8][8];

static void tile_commit(int id) {
    u32 *dst = (u32 *)&tile_mem[1][PANEL_TILE + id];
    int x, y;
    for (y = 0; y < 8; y++) {
        u32 row = 0;
        for (x = 0; x < 8; x++) row |= (u32)s_tile[y][x] << (x * 4);
        dst[y] = row;
    }
}

/* 4px bevelled frame edge on the given sides; corners rounded by a pixel. */
static void tile_frame(int sides) {
    int x, y;
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            int m = 8, lit = 0;
            if ((sides & SIDE_T) && y < m)     { m = y;     lit = 1; }
            if ((sides & SIDE_L) && x < m)     { m = x;     lit = 1; }
            if ((sides & SIDE_B) && 7 - y < m) { m = 7 - y; lit = 0; }
            if ((sides & SIDE_R) && 7 - x < m) { m = 7 - x; lit = 0; }
            u8 c = C_FILL;
            if (m == 0)      c = C_LINE;
            else if (m == 1) c = lit ? C_GOLD_HI : C_GOLD_LO;
            else if (m == 2) c = C_GOLD;
            else if (m == 3) c = C_LINE;
            if (((sides & (SIDE_T | SIDE_L)) == (SIDE_T | SIDE_L) && x == 0 && y == 0) ||
                ((sides & (SIDE_T | SIDE_R)) == (SIDE_T | SIDE_R) && x == 7 && y == 0) ||
                ((sides & (SIDE_B | SIDE_L)) == (SIDE_B | SIDE_L) && x == 0 && y == 7) ||
                ((sides & (SIDE_B | SIDE_R)) == (SIDE_B | SIDE_R) && x == 7 && y == 7))
                c = C_CLEAR;
            s_tile[y][x] = c;
        }
    }
}

/* Small "press A" triangle in the fill area above a bottom edge. */
static void tile_arrow(void) {
    int x;
    for (x = 1; x <= 6; x++) s_tile[0][x] = C_GOLD_HI;
    for (x = 2; x <= 5; x++) s_tile[1][x] = C_GOLD_HI;
    for (x = 3; x <= 4; x++) s_tile[2][x] = C_GOLD;
}

/* Narration band rule: outline, gold double line, from the outer edge. */
static void tile_rule(int top) {
    static const u8 prof[8] = { C_LINE, C_GOLD_HI, C_GOLD, C_LINE, C_FILL, C_GOLD_LO, C_FILL, C_FILL };
    int x, y;
    for (y = 0; y < 8; y++)
        for (x = 0; x < 8; x++)
            s_tile[y][x] = prof[top ? y : 7 - y];
}

/* Diamond ornament centred on the seam between two top-rule tiles. */
static void tile_ornament(int right) {
    int x, y;
    tile_rule(1);
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            int px = x + (right ? 8 : 0);
            int dx = 2 * px - 15, dy = 2 * y - 7;
            int d = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
            if (d <= 4)      s_tile[y][x] = C_GOLD_HI;
            else if (d <= 7) s_tile[y][x] = C_GOLD;
            else if (d <= 9) s_tile[y][x] = C_LINE;
        }
    }
}

static void tile_tab(int cap) {   /* cap: 0 middle, -1 left, 1 right */
    int x, y;
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            u8 c = y == 0 ? C_LINE : y == 1 ? C_GOLD_HI : C_GOLD;
            if (cap < 0 && x == 0) c = y == 0 ? C_CLEAR : C_LINE;
            if (cap < 0 && x == 1 && y > 0) c = C_GOLD_HI;
            if (cap > 0 && x == 7) c = y == 0 ? C_CLEAR : C_LINE;
            if (cap > 0 && x == 6 && y > 1) c = C_GOLD_LO;
            s_tile[y][x] = c;
        }
    }
}

static void build_panel_tiles(void) {
    tile_frame(0);                       tile_commit(PT_FILL);
    tile_frame(SIDE_T | SIDE_L);         tile_commit(PT_TL);
    tile_frame(SIDE_T);                  tile_commit(PT_T);
    tile_frame(SIDE_T | SIDE_R);         tile_commit(PT_TR);
    tile_frame(SIDE_L);                  tile_commit(PT_L);
    tile_frame(SIDE_R);                  tile_commit(PT_R);
    tile_frame(SIDE_B | SIDE_L);         tile_commit(PT_BL);
    tile_frame(SIDE_B);                  tile_commit(PT_B);
    tile_frame(SIDE_B | SIDE_R);         tile_commit(PT_BR);
    tile_frame(SIDE_B); tile_arrow();    tile_commit(PT_B_ARROW);
    tile_tab(-1);                        tile_commit(PT_TAB_L);
    tile_tab(0);                         tile_commit(PT_TAB);
    tile_tab(1);                         tile_commit(PT_TAB_R);
    tile_rule(1);                        tile_commit(PT_N_TOP);
    tile_rule(0);                        tile_commit(PT_N_BOT);
    tile_ornament(0);                    tile_commit(PT_N_ORN_L);
    tile_ornament(1);                    tile_commit(PT_N_ORN_R);
    tile_rule(0); tile_arrow();          tile_commit(PT_N_BOT_ARROW);
}

/* Copy of the 1bpp system font as 4bpp tiles: ink = 1, and a drop shadow
 * (down/right) = 2, so text reads over any panel colour. */
static void build_shadow_font(void) {
    const u8 *src = (const u8 *)sys8Glyphs;
    int g, x, y;
    for (g = 0; g < FONT_GLYPHS; g++) {
        const u8 *b = src + g * 8;
        u32 *dst = (u32 *)&tile_mem[1][SHADOW_FONT + g];
        for (y = 0; y < 8; y++) {
            u32 row = 0;
            for (x = 0; x < 8; x++) {
                int ink = (b[y] >> x) & 1;
                int sh = (x > 0 && ((b[y] >> (x - 1)) & 1)) ||
                         (y > 0 && ((b[y - 1] >> x) & 1)) ||
                         (x > 0 && y > 0 && ((b[y - 1] >> (x - 1)) & 1));
                row |= (u32)(ink ? 1 : sh ? 2 : 0) << (x * 4);
            }
            dst[y] = row;
        }
    }
}

static void set_panel_pal(int bank, COLOR fill) {
    pal_bg_bank[bank][C_FILL]    = fill;
    pal_bg_bank[bank][C_LINE]    = RGB15(1, 1, 2);
    pal_bg_bank[bank][C_GOLD]    = RGB15(24, 18, 6);
    pal_bg_bank[bank][C_GOLD_HI] = RGB15(31, 28, 14);
    pal_bg_bank[bank][C_GOLD_LO] = RGB15(13, 9, 3);
}

static void load_box_palettes(void) {
    set_panel_pal(PB_PANEL_NARR, RGB15(3, 2, 2));
    set_panel_pal(PB_PANEL_DLG, RGB15(3, 4, 11));
    pal_bg_bank[PB_TEXT_NARR][1] = RGB15(30, 26, 18);
    pal_bg_bank[PB_TEXT_NARR][2] = RGB15(0, 0, 0);
    pal_bg_bank[PB_TEXT_DLG][1]  = RGB15(31, 31, 29);
    pal_bg_bank[PB_TEXT_DLG][2]  = RGB15(1, 1, 4);
    pal_bg_bank[PB_TAB_TEXT][1]  = RGB15(5, 3, 1);
    pal_bg_bank[PB_TAB_TEXT][2]  = RGB15(29, 24, 10);
}

static inline void panel_put(int col, int row, int tile, int pb) {
    se_mem[PANEL_SBB][row * 32 + col] = SE_PALBANK(pb) | (PANEL_TILE + tile);
}

static void panel_clear(void) {
    int i;
    for (i = TAB_ROW * 32; i < 20 * 32; i++) se_mem[PANEL_SBB][i] = 0;
}

/* Box state for pagination and the blinking advance arrow. */
static char s_lines[MAX_LINES][TEXT_COLS + 1];
static int s_lineCount;
static UiTextStyle s_style;
static u16 s_blink;

static int new_line(void) {
    if (s_lineCount >= MAX_LINES) return 0;
    s_lines[s_lineCount++][0] = '\0';
    return 1;
}

/* Greedy word wrap into s_lines; explicit '\n' forces a break and words
 * longer than a line are hard-split. */
static void wrap_text(const char *in) {
    int col = 0;
    s_lineCount = 1;
    s_lines[0][0] = '\0';
    while (*in) {
        int len = 0;
        char *l;
        if (*in == '\n') {
            if (!new_line()) return;
            col = 0; in++;
            continue;
        }
        if (*in == ' ') { in++; continue; }
        while (in[len] && in[len] != ' ' && in[len] != '\n') len++;
        if (col > 0 && col + 1 + len > TEXT_COLS) {
            if (!new_line()) return;
            col = 0;
        }
        l = s_lines[s_lineCount - 1];
        if (col > 0) l[col++] = ' ';
        while (len-- > 0) {
            if (col >= TEXT_COLS) {
                l[col] = '\0';
                if (!new_line()) return;
                l = s_lines[s_lineCount - 1];
                col = 0;
            }
            l[col++] = *in++;
        }
        l[col] = '\0';
    }
}

void ui_init(void) {
    tte_init_se_default(3, BG_CBB(1) | BG_SBB(28));
    build_shadow_font();
    build_panel_tiles();
    load_box_palettes();
    panel_clear();
    REG_BG2CNT = BG_CBB(1) | BG_SBB(PANEL_SBB) | BG_4BPP | BG_PRIO(1);
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_DISPCNT |= DCNT_BG2 | DCNT_BG3;
    ui_clear_all();
}

void ui_clear_all(void) {
    tte_erase_screen();
    panel_clear();
}

void ui_textbox_clear(void) {
    tte_erase_rect(0, TAB_ROW * 8, SCREEN_WIDTH, SCREEN_HEIGHT);
    panel_clear();
    REG_DISPCNT &= ~(DCNT_WIN0 | DCNT_WIN1);
}

static void draw_frame(const char *speaker) {
    int c, r, pb = s_style == UI_TEXT_NARRATION ? PB_PANEL_NARR : PB_PANEL_DLG;
    panel_clear();
    REG_DISPCNT &= ~DCNT_WIN1;
    if (s_style == UI_TEXT_NARRATION) {
        for (c = 0; c < 30; c++) {
            panel_put(c, BOX_ROW, PT_N_TOP, pb);
            panel_put(c, 19, PT_N_BOT, pb);
            for (r = BOX_ROW + 1; r < 19; r++) panel_put(c, r, PT_FILL, pb);
        }
        panel_put(14, BOX_ROW, PT_N_ORN_L, pb);
        panel_put(15, BOX_ROW, PT_N_ORN_R, pb);
    } else {
        for (r = BOX_ROW; r < 20; r++) {
            for (c = 0; c < 30; c++) {
                int t = PT_FILL;
                if (r == BOX_ROW) t = c == 0 ? PT_TL : c == 29 ? PT_TR : PT_T;
                else if (r == 19) t = c == 0 ? PT_BL : c == 29 ? PT_BR : PT_B;
                else if (c == 0) t = PT_L;
                else if (c == 29) t = PT_R;
                panel_put(c, r, t, pb);
            }
        }
        if (speaker && *speaker) {
            int len = 0, x0 = 1, x1;
            while (speaker[len] && len < 24) len++;
            x1 = x0 + len + 1;                 /* inclusive right cap */
            panel_put(x0, TAB_ROW, PT_TAB_L, pb);
            for (c = x0 + 1; c < x1; c++) panel_put(c, TAB_ROW, PT_TAB, pb);
            panel_put(x1, TAB_ROW, PT_TAB_R, pb);
            REG_WIN1H = ((x0 * 8) << 8) | ((x1 + 1) * 8);
            REG_WIN1V = ((TAB_ROW * 8) << 8) | BOX_TOP_PX;
            REG_DISPCNT |= DCNT_WIN1;
            tte_get_context()->cattr[TTE_SPECIAL] = SE_PALBANK(PB_TAB_TEXT) | SHADOW_FONT;
            tte_set_pos((x0 + 1) * 8, TAB_ROW * 8);
            tte_write(speaker);
        }
    }
    REG_WIN0H = (0 << 8) | SCREEN_WIDTH;
    REG_WIN0V = (BOX_TOP_PX << 8) | SCREEN_HEIGHT;
    REG_WININ = (WIN_BG2 | WIN_BG3) | ((WIN_BG2 | WIN_BG3) << 8);
    REG_WINOUT = WIN_BG0 | WIN_BG1 | WIN_BG3 | WIN_OBJ;
    REG_BLDCNT = 0;
    REG_DISPCNT |= DCNT_WIN0;
}

int ui_textbox_open(UiTextStyle style, const char *speaker, const char *text) {
    TTC *tc = tte_get_context();
    u16 saved = tc->cattr[TTE_SPECIAL];
    s_style = style;
    load_box_palettes();
    tte_erase_rect(0, TAB_ROW * 8, SCREEN_WIDTH, SCREEN_HEIGHT);
    draw_frame(style == UI_TEXT_DIALOGUE ? speaker : NULL);
    tc->cattr[TTE_SPECIAL] = saved;
    wrap_text(text);
    return (s_lineCount + TEXT_LINES - 1) / TEXT_LINES;
}

void ui_textbox_page(int page) {
    TTC *tc = tte_get_context();
    u16 saved = tc->cattr[TTE_SPECIAL];
    int i, pb = s_style == UI_TEXT_NARRATION ? PB_TEXT_NARR : PB_TEXT_DLG;
    tte_erase_rect(8, TEXT_ROW * 8, SCREEN_WIDTH - 8, (TEXT_ROW + TEXT_LINES) * 8);
    tc->cattr[TTE_SPECIAL] = SE_PALBANK(pb) | SHADOW_FONT;
    for (i = 0; i < TEXT_LINES; i++) {
        int n = page * TEXT_LINES + i, col = TEXT_COL;
        if (n >= s_lineCount) break;
        if (s_style == UI_TEXT_NARRATION) {
            int len = 0;
            while (s_lines[n][len]) len++;
            col = TEXT_COL + (TEXT_COLS - len) / 2;
        }
        tte_set_pos(col * 8, (TEXT_ROW + i) * 8);
        tte_write(s_lines[n]);
    }
    tc->cattr[TTE_SPECIAL] = saved;
    s_blink = 0;
}

void ui_textbox_tick(void) {
    int on = (s_blink++ >> 4) & 1;
    int pb = s_style == UI_TEXT_NARRATION ? PB_PANEL_NARR : PB_PANEL_DLG;
    int base = s_style == UI_TEXT_NARRATION ? PT_N_BOT : PT_B;
    int arrow = s_style == UI_TEXT_NARRATION ? PT_N_BOT_ARROW : PT_B_ARROW;
    panel_put(27, 19, on ? base : arrow, pb);
}

void ui_textbox_draw(const char *text) {
    ui_textbox_open(UI_TEXT_HINT, NULL, text);
    ui_textbox_page(0);
}

void ui_print_at(int col, int row, const char *text) {
    tte_set_pos(col * 8, row * 8);
    tte_write(text);
}

void ui_print_hud(int col, int row, const char *text) {
    TTC *tc = tte_get_context();
    u16 saved = tc->cattr[TTE_SPECIAL];
    tc->cattr[TTE_SPECIAL] = SE_PALBANK(PB_TEXT_DLG) | SHADOW_FONT;
    tte_set_pos(col * 8, row * 8);
    tte_write(text);
    tc->cattr[TTE_SPECIAL] = saved;
}

void ui_set_text_palbank(int bank) {
    TTC *tc = tte_get_context();
    tc->cattr[TTE_SPECIAL] = (tc->cattr[TTE_SPECIAL] & 0x0FFF) | (u16)(bank << 12);
}

void ui_set_text_color(int bank, COLOR color) {
    int i;
    for (i = 1; i < 16; i++) {
        pal_bg_bank[bank][i] = color;
    }
}
