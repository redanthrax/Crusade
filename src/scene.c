#include <tonc.h>
#include "scene.h"
#include "save.h"
#include "input.h"
#include "ui.h"
#include "render.h"
#include "oam_pool.h"

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
        ui_textbox_draw(g_textBank[entry->textBankId]);

        /* Wait for A to advance, polling input once per frame via VBlank. */
        BOOL advanced = FALSE;
        while (!advanced) {
            VBlankIntrWait();
            /* Keep the world's sprites on screen while dialogue is up. */
            render_actors_to_oam();
            oam_pool_flush();
            input_poll();
            if (key_hit(KEY_A)) {
                advanced = TRUE;
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
