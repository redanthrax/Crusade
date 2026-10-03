#include <tonc.h>
#include "scene.h"
#include "save.h"
#include "input.h"
#include "debug.h"
#include "ui.h"
#include "render.h"
#include "oam_pool.h"
#include "camera.h"

const char *const g_speakerNames[SPK_COUNT] = {
    [SPK_NARRATOR] = NULL,
    [SPK_HINT]     = NULL,
    [SPK_GUIDE]    = "Guide",
    [SPK_IDA]      = "Countess Ida",
    [SPK_RETAINER] = "Retainer",
};

static BOOL flags_satisfied(u8 flagsRequired) {
    if (flagsRequired == 0) return TRUE;
    return (g_save.flags & flagsRequired) == flagsRequired;
}

void scene_run(const SceneScriptEntry *table, u16 entryId) {
    while (entryId != SCENE_END) {
        const SceneScriptEntry *entry = &table[entryId];
        /* Portraits are out of scope until portrait tile art exists;
         * portraitId is still threaded through SceneScriptEntry so acts
         * only need to supply real art later, not change call sites. */
        u8 spk = entry->speakerId < SPK_COUNT ? entry->speakerId : SPK_HINT;
        UiTextStyle style = spk == SPK_NARRATOR ? UI_TEXT_NARRATION
                          : spk == SPK_HINT     ? UI_TEXT_HINT
                                                : UI_TEXT_DIALOGUE;
        int pages = ui_textbox_open(style, g_speakerNames[spk],
                                    g_textBank[entry->textBankId]);
        int page;

        /* One A press per page, polling input once per frame via VBlank. */
        for (page = 0; page < pages; page++) {
            ui_textbox_page(page);
            while (1) {
                vsync_wait();
                /* Present scroll + sprites in VBlank, then prepare the next. */
                camera_commit();
                oam_pool_flush();
                render_actors_to_oam();
                ui_textbox_tick();
                input_poll();
                if (key_hit(KEY_A)) break;
            }
        }

        if (entry->nextOnFlagSet != SCENE_END && flags_satisfied(entry->flagsRequired)) {
            entryId = entry->nextOnFlagSet;
        } else {
            entryId = entry->nextOnDefault;
        }
    }
    ui_textbox_clear();
}
