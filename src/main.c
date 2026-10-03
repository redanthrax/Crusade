#include <tonc.h>
#include "ui.h"
#include "menu.h"
#include "save.h"
#include "scene.h"
#include "acts.h"
#include "sprites.h"
#include "oam_pool.h"
#include "actor.h"
#include "audio.h"
#include "input.h"

int main(void) {
    irq_init(NULL);
    irq_enable(II_VBLANK);

    /* Hide all 128 hardware OAM entries: the pool only manages 96, and
     * power-on OAM is zeroed (= visible 8x8 sprites at 0,0). BG0 is
     * enabled by map_load() once a world map exists. */
    oam_init(oam_mem, 128);
    REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;

    ui_init();
    sprites_load_placeholder();
    oam_pool_reset();
    actor_pool_reset();
    audio_init();

    BOOL hasSave = save_read();
    if (!hasSave) {
        save_init_stub(&g_save);
    }

    while (1) {
        TitleChoice choice = title_screen_run(hasSave);

        if (choice == TITLE_SETTINGS) {
            settings_screen_run();
            continue;
        }

        if (choice == TITLE_NEW_GAME || !hasSave) {
            u32 preservedSettings = g_save.flags & SFLAG_SETTINGS_MASK;
            save_init_stub(&g_save);
            g_save.flags |= preservedSettings;
            save_write();
            hasSave = TRUE;
        }

        ui_clear_all();

        if (g_save.chapterId <= CHAPTER_PRELUDE_1) {
            prelude1_run();
            g_save.chapterId = CHAPTER_ACT1_BOUILLON;
            save_write();
        }
        if (g_save.chapterId <= CHAPTER_ACT1_BOUILLON) {
            act1_bouillon_run();
        }

        /* First-playable slice ends here -- Acts 2+ aren't implemented yet. */
        ui_clear_all();
        ui_print_at(2, 8, "End of first-playable slice.");
        ui_print_at(2, 9, "Press START for the title screen.");
        while (1) {
            VBlankIntrWait();
            input_poll();
            if (key_hit(KEY_START)) break;
        }

        /* Clear the leftover world (map + sprites) before the title. */
        REG_DISPCNT &= ~DCNT_BG0;
        actor_pool_reset();
        oam_pool_reset();
        oam_pool_flush();
        ui_clear_all();
    }

    return 0;
}
