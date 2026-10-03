#include <string.h>
#include <stddef.h>
#include <tonc.h>
#include "save.h"
#include "scene.h"

SaveData g_save;

/* SRAM is only byte-addressable on real hardware (8-bit bus). libtonc maps
 * it at sram_mem; always read/write via volatile u8* one byte at a time. */
#define SRAM_BASE ((volatile u8 *)sram_mem)

u8 save_calc_checksum(const SaveData *s) {
    const u8 *bytes = (const u8 *)s;
    u8 sum = 0;
    unsigned int i;
    /* Sum every byte up to (not including) the checksum field itself. */
    unsigned int checksumOffset = offsetof(SaveData, checksum);
    for (i = 0; i < checksumOffset; i++) {
        sum += bytes[i];
    }
    return sum;
}

void save_init_stub(SaveData *s) {
    memset(s, 0, sizeof(SaveData));
    s->magic = SAVE_MAGIC;
    s->chapterId = CHAPTER_PRELUDE_1;
    s->arms = 0;
    s->command = 0;
    s->piety = 0;
    s->flags = 0;
    s->checksum = save_calc_checksum(s);
}

void save_write(void) {
    g_save.checksum = save_calc_checksum(&g_save);
    const u8 *src = (const u8 *)&g_save;
    unsigned int i;
    for (i = 0; i < sizeof(SaveData); i++) {
        SRAM_BASE[i] = src[i];
    }
}

BOOL save_read(void) {
    SaveData tmp;
    u8 *dst = (u8 *)&tmp;
    unsigned int i;
    for (i = 0; i < sizeof(SaveData); i++) {
        dst[i] = SRAM_BASE[i];
    }
    if (tmp.magic != SAVE_MAGIC) {
        return FALSE;
    }
    if (tmp.checksum != save_calc_checksum(&tmp)) {
        return FALSE;
    }
    g_save = tmp;
    return TRUE;
}
