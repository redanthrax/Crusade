/* ui.h -- text rendering (libtonc TTE) for dialogue boxes and menus.
 *
 * Owns BG3 exclusively: charblock 1 for the font glyph cache, screenblock
 * 28 for its tilemap (see docs/DESIGN.md "BG VRAM Memory Map"). BG0 (world
 * tiles) never shares either region.
 */
#ifndef CRUSADE_UI_H
#define CRUSADE_UI_H

#include <tonc.h>

/* Initializes TTE on BG3 with the default system font and enables BG3.
 * Call once at boot, after irqInit(). */
void ui_init(void);

/* Clears the bottom 4-row dialogue box region of BG3. */
void ui_textbox_clear(void);

/* Draws `text` word-wrapped into the bottom dialogue box (rows 16-19 of a
 * 30x20 tile screen) and leaves it on screen (caller/scene.c owns the
 * "wait for A" pacing). */
void ui_textbox_draw(const char *text);

/* Clears the whole BG3 text layer (used when switching between full-screen
 * UI like the title menu and in-map dialogue boxes). */
void ui_clear_all(void);

/* Prints `text` at the given tile coordinate on BG3, no wrapping/clearing. */
void ui_print_at(int col, int row, const char *text);

/* Selects which BG palette bank subsequent TTE text uses (default 15). */
void ui_set_text_palbank(int bank);

/* Sets the ink colour of a text palette bank (banks 11-15 are reserved for
 * text; the world tileset only uses bank 0). */
void ui_set_text_color(int bank, COLOR color);

#endif /* CRUSADE_UI_H */
