/* menu.h -- title screen and settings screen. */
#ifndef CRUSADE_MENU_H
#define CRUSADE_MENU_H

#include <tonc.h>

typedef enum {
    TITLE_NEW_GAME = 0,
    TITLE_CONTINUE,
    TITLE_SETTINGS
} TitleChoice;

/* Draws the title screen on BG3 (text layer) and blocks until the player
 * confirms a choice with A. `hasSave` disables/skips highlighting Continue
 * as a no-op if there's no valid save (still shown, just re-offers the
 * menu instead of proceeding if chosen). */
TitleChoice title_screen_run(BOOL hasSave);

/* Draws the settings screen and blocks until the player backs out with B.
 * Mutates g_save.flags's SFLAG_SETTING_* bits live and calls save_write()
 * on exit so settings persist immediately. */
void settings_screen_run(void);

#endif /* CRUSADE_MENU_H */
