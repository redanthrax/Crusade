#include <string.h>
#include <tonc.h>
#include "oam_pool.h"

OBJ_ATTR g_oamShadow[MAX_OAM_SLOTS];

static BOOL s_used[MAX_OAM_SLOTS];

void oam_pool_reset(void) {
    int i;
    memset(s_used, 0, sizeof(s_used));
    for (i = 0; i < MAX_OAM_SLOTS; i++) {
        obj_hide(&g_oamShadow[i]);
    }
}

s8 oam_pool_alloc(void) {
    int i;
    for (i = 0; i < MAX_OAM_SLOTS; i++) {
        if (!s_used[i]) {
            s_used[i] = TRUE;
            return (s8)i;
        }
    }
    return -1;
}

void oam_pool_free(s8 slot) {
    if (slot < 0 || slot >= MAX_OAM_SLOTS) {
        return;
    }
    s_used[slot] = FALSE;
    obj_hide(&g_oamShadow[slot]);
}

void oam_pool_flush(void) {
    oam_copy(oam_mem, g_oamShadow, MAX_OAM_SLOTS);
}
