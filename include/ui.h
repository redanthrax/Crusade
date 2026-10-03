/* ui.h -- text rendering (libtonc TTE) for dialogue boxes and menus.
 *
 * Owns BG3 (text: CBB1 font tiles 0-95, SBB28) and BG2 (text-box panel:
 * CBB1 tiles 96-191 drop-shadow font + 192+ frame tiles, SBB29), plus
 * WIN0/WIN1 while a box is open (see docs/DESIGN.md "BG VRAM Memory Map").
 */
#ifndef CRUSADE_UI_H
#define CRUSADE_UI_H

#include <tonc.h>

/* Initializes TTE on BG3 with the default system font and enables BG3.
 * Call once at boot, after irqInit(). */
void ui_init(void);

typedef enum {
    UI_TEXT_NARRATION,  /* dark band, gold rules, centred parchment text */
    UI_TEXT_DIALOGUE,   /* navy framed box + gold speaker name tab */
    UI_TEXT_HINT        /* framed box, no name tab (tutorial prompts) */
} UiTextStyle;

/* Opens the opaque bottom text box (rows 14-19, name tab on row 13) in the
 * given style, word-wraps `text` to 28 columns and returns the number of
 * 4-line pages. `speaker` is only shown for UI_TEXT_DIALOGUE (may be NULL).
 * Call ui_textbox_page() to show a page. */
int ui_textbox_open(UiTextStyle style, const char *speaker, const char *text);

/* Draws page `page` (0-based) of the text passed to ui_textbox_open(). */
void ui_textbox_page(int page);

/* Call once per frame while a box waits for input: blinks the advance arrow. */
void ui_textbox_tick(void);

/* Removes the text box, name tab and windows. */
void ui_textbox_clear(void);

/* Convenience: open as UI_TEXT_HINT and show page 0. */
void ui_textbox_draw(const char *text);

/* Clears the whole BG3 text layer (used when switching between full-screen
 * UI like the title menu and in-map dialogue boxes). */
void ui_clear_all(void);

/* Prints `text` at the given tile coordinate on BG3, no wrapping/clearing. */
void ui_print_at(int col, int row, const char *text);

/* Like ui_print_at() but with the drop-shadow font in white, readable over
 * the world (debug HUD). */
void ui_print_hud(int col, int row, const char *text);

/* Selects which BG palette bank subsequent TTE text uses (default 15). */
void ui_set_text_palbank(int bank);

/* Sets the ink colour of a text palette bank (banks 11-15 are reserved for
 * text; the world tileset only uses bank 0). */
void ui_set_text_color(int bank, COLOR color);

#endif /* CRUSADE_UI_H */
