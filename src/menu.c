#include <string.h>
#include <tonc.h>
#include "menu.h"
#include "ui.h"
#include "input.h"
#include "save.h"
#include "title_bg.h"
#include "oam_pool.h"
#include "sprites.h"
#include "placeholder_gfx.h"

/* Text palette banks (ui_set_text_color); bank 15 stays TTE's default. */
#define PB_SELECTED 14
#define PB_NORMAL   13
#define PB_DISABLED 12
#define PB_DEFAULT  15

#define ROW_FIRST 9
#define ROW_STEP  2

static s8  s_cursorSlot = -1;
static u32 s_frame;

static int item_row(int i) { return ROW_FIRST + i * ROW_STEP; }

static void menu_begin(void) {
    title_bg_show();
    ui_clear_all();
    ui_set_text_color(PB_SELECTED, BGR15(31, 30, 24));
    ui_set_text_color(PB_NORMAL,   BGR15(20, 17, 14));
    ui_set_text_color(PB_DISABLED, BGR15(10, 8, 9));
    if (s_cursorSlot < 0) s_cursorSlot = oam_pool_alloc();
}

/* keepBg: leave the backdrop up when moving to another menu screen. */
static void menu_end(BOOL keepBg) {
    oam_pool_free(s_cursorSlot);
    s_cursorSlot = -1;
    oam_pool_flush();
    ui_clear_all();
    ui_set_text_palbank(PB_DEFAULT);
    if (!keepBg) title_bg_hide();
}

static void print_item(int col, int row, const char *text, int bank) {
    ui_set_text_palbank(bank);
    ui_print_at(col, row, text);
}

/* Waits a frame, animates the backdrop and the bobbing cross cursor placed
 * just left of (col,row), then polls input. */
static void menu_frame(int col, int row) {
    static const s8 bob[4] = { 0, 1, 2, 1 };
    VBlankIntrWait();
    title_bg_vblank(s_frame);
    if (s_cursorSlot >= 0) {
        OBJ_ATTR *o = &g_oamShadow[(int)s_cursorSlot];
        obj_set_attr(o, ATTR0_SQUARE, ATTR1_SIZE_8x8,
                     ATTR2_PALBANK(SPRITE_PALBANK_UI) | ATTR2_PRIO(0) | SPRITE_TILE_CURSOR);
        obj_set_pos(o, col * 8 - 13 + bob[(s_frame >> 3) & 3], row * 8);
        oam_pool_flush();
    }
    s_frame++;
    input_poll();
}

/* ---- title ------------------------------------------------------------- */

#define TITLE_OPTION_COUNT 3
#define TITLE_COL 11 /* 8-char labels centred on 30 columns */
static const char *const s_titleLabels[TITLE_OPTION_COUNT] = {
    "New Game", "Continue", "Settings"
};

static void draw_title_items(int selected, const BOOL *enabled) {
    int i;
    for (i = 0; i < TITLE_OPTION_COUNT; i++) {
        int bank = !enabled[i] ? PB_DISABLED : (i == selected) ? PB_SELECTED : PB_NORMAL;
        print_item(TITLE_COL, item_row(i), s_titleLabels[i], bank);
    }
}

static int step_enabled(int from, int dir, const BOOL *enabled) {
    int i = from;
    do {
        i = (i + dir + TITLE_OPTION_COUNT) % TITLE_OPTION_COUNT;
    } while (!enabled[i] && i != from);
    return i;
}

TitleChoice title_screen_run(BOOL hasSave) {
    const BOOL enabled[TITLE_OPTION_COUNT] = { TRUE, hasSave, TRUE };
    int selected = hasSave ? TITLE_CONTINUE : TITLE_NEW_GAME;

    menu_begin();
    draw_title_items(selected, enabled);

    while (1) {
        menu_frame(TITLE_COL, item_row(selected));

        if (key_hit(KEY_UP)) {
            selected = step_enabled(selected, -1, enabled);
            draw_title_items(selected, enabled);
        } else if (key_hit(KEY_DOWN)) {
            selected = step_enabled(selected, 1, enabled);
            draw_title_items(selected, enabled);
        } else if (key_hit(KEY_A) || key_hit(KEY_START)) {
            menu_end(selected == TITLE_SETTINGS);
            return (TitleChoice)selected;
        }
    }
}

/* ---- settings ---------------------------------------------------------- */

#define SET_COUNT 3
#define SET_COL   5
#define SET_WIDTH 20 /* label left, value right-aligned: cols 5..24 */

enum { SET_SOUND = 0, SET_TEXT, SET_BACK };

static void format_row(char *out, const char *label, const char *value) {
    int lv = (int)strlen(value);
    memset(out, ' ', SET_WIDTH);
    memcpy(out, label, strlen(label));
    memcpy(out + SET_WIDTH - lv, value, lv);
    out[SET_WIDTH] = '\0';
}

static void draw_settings(int selected) {
    char line[SET_WIDTH + 1];
    int i;
    for (i = 0; i < SET_COUNT; i++) {
        if (i == SET_SOUND)
            format_row(line, "Sound", (g_save.flags & SFLAG_SETTING_SOUND_OFF) ? "Off" : "On");
        else if (i == SET_TEXT)
            format_row(line, "Text Speed", (g_save.flags & SFLAG_SETTING_TEXT_FAST) ? "Fast" : "Normal");
        else
            format_row(line, "        Back", "");
        print_item(SET_COL, item_row(i), line, i == selected ? PB_SELECTED : PB_NORMAL);
    }
}

void settings_screen_run(void) {
    int selected = SET_SOUND;

    menu_begin();
    draw_settings(selected);

    while (1) {
        menu_frame(SET_COL, item_row(selected));

        if (key_hit(KEY_UP)) {
            selected = (selected + SET_COUNT - 1) % SET_COUNT;
            draw_settings(selected);
        } else if (key_hit(KEY_DOWN)) {
            selected = (selected + 1) % SET_COUNT;
            draw_settings(selected);
        } else if (key_hit(KEY_LEFT) || key_hit(KEY_RIGHT) ||
                   (key_hit(KEY_A) && selected != SET_BACK)) {
            if (selected == SET_SOUND) g_save.flags ^= SFLAG_SETTING_SOUND_OFF;
            if (selected == SET_TEXT)  g_save.flags ^= SFLAG_SETTING_TEXT_FAST;
            draw_settings(selected);
        } else if (key_hit(KEY_B) || (key_hit(KEY_A) && selected == SET_BACK)) {
            save_write();
            menu_end(TRUE); /* back to the title, which reuses the backdrop */
            return;
        }
    }
}
