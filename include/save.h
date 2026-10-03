/* save.h -- SRAM save layout.
 *
 * SRAM on GBA cartridges is wired to an 8-bit bus: always read/write one
 * byte at a time. Never DMA into/out of SRAM and never cast a struct
 * pointer directly onto the SRAM-mapped address range for a bulk copy.
 */
#ifndef CRUSADE_SAVE_H
#define CRUSADE_SAVE_H

#include <tonc.h>

#define SAVE_MAGIC 0x474F4446UL /* "GODF" */

/* Flags bitfield bit assignments (grow as Acts are implemented). */
#define SFLAG_SCOUTED_FOR_BALDWIN   (1 << 0) /* Act 2 side mission */
#define SFLAG_SHARED_FOOD_ANTIOCH   (1 << 1) /* Act 3 side mission */
#define SFLAG_COMPANION_LOST        (1 << 2) /* Act 4 death/desert branch fired */
#define SFLAG_SECURED_TEMPLE        (1 << 3) /* Act 5 side mission */
#define SFLAG_LIVESTOCK_RECOVERED   (1 << 4) /* Act 1 side mission */

/* Top two bits of the flags field are device settings, not story flags:
 * they persist across "New Game" resets (main.c preserves them explicitly
 * instead of calling save_init_stub over them). */
#define SFLAG_SETTING_SOUND_OFF     (1u << 30)
#define SFLAG_SETTING_TEXT_FAST     (1u << 31)
#define SFLAG_SETTINGS_MASK         (SFLAG_SETTING_SOUND_OFF | SFLAG_SETTING_TEXT_FAST)

typedef struct SaveData {
    u32 magic;         /* SAVE_MAGIC, validated on load */
    u8  chapterId;      /* current Act/vignette id, see scene.h ChapterId */
    u8  arms;            /* progression track: combat */
    u8  command;          /* progression track: leadership/set-pieces */
    u8  piety;              /* progression track: non-combat choices */
    u32 flags;               /* SFLAG_* bitfield */
    u8  checksum;              /* 8-bit sum over all preceding bytes */
    u8  pad[3];                 /* pad to 16 bytes for clean SRAM block writes */
} SaveData;

extern SaveData g_save;

/* Fills g_save with new-game defaults (chapter = prelude 1, tracks at 0,
 * flags clear) and recomputes the checksum. Does not touch SRAM. */
void save_init_stub(SaveData *s);

/* Writes g_save to SRAM, one byte at a time. */
void save_write(void);

/* Reads SRAM into g_save. Returns TRUE if the magic+checksum validated,
 * FALSE if SRAM was blank/corrupt (caller should fall back to
 * save_init_stub). */
BOOL save_read(void);

u8 save_calc_checksum(const SaveData *s);

#endif /* CRUSADE_SAVE_H */
