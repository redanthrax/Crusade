#include <tonc.h>
#include "ui.h"

#define TEXTBOX_TOP_PX    (16 * 8)  /* row 16 of 20, in pixels */
#define TEXTBOX_BOTTOM_PX (20 * 8)  /* bottom of screen */

void ui_init(void) {
    tte_init_se_default(3, BG_CBB(1) | BG_SBB(28));
    REG_DISPCNT |= DCNT_BG3;
    ui_clear_all();
}

void ui_clear_all(void) {
    tte_erase_screen();
}

void ui_textbox_clear(void) {
    tte_erase_rect(0, TEXTBOX_TOP_PX, SCREEN_WIDTH, TEXTBOX_BOTTOM_PX);
}

void ui_textbox_draw(const char *text) {
    ui_textbox_clear();
    tte_set_pos(4, TEXTBOX_TOP_PX + 2);
    tte_write(text);
}

void ui_print_at(int col, int row, const char *text) {
    tte_set_pos(col * 8, row * 8);
    tte_write(text);
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
