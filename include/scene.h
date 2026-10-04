/* scene.h -- dialogue/cutscene script VM.
 *
 * A scene is a small linked chain of SceneScriptEntry records in ROM. The
 * VM shows one text box (8x8 font, bottom of screen) + optional portrait,
 * waits for A, then branches to the next entry id, optionally gated by
 * SaveData.flags.
 */
#ifndef CRUSADE_SCENE_H
#define CRUSADE_SCENE_H

#include <tonc.h>

/* Chapter/vignette ids, stored in SaveData.chapterId. */
typedef enum {
    CHAPTER_PRELUDE_1 = 0,
    CHAPTER_PRELUDE_2,
    CHAPTER_PRELUDE_3,
    CHAPTER_ACT1_BOUILLON,
    CHAPTER_ACT2_MARCH,
    CHAPTER_ACT3_ANTIOCH,
    CHAPTER_ACT4_ROAD,
    CHAPTER_ACT5_JERUSALEM,
    CHAPTER_ACT6_EPILOGUE
} ChapterId;

#define SCENE_NO_PORTRAIT 0xFF
/* 0xFFFF terminates a chain. Entry id 0 is a valid, commonly-used first
 * entry, so the end sentinel must not collide with it. */
#define SCENE_END         0xFFFF

/* Who is talking. Selects the text-box style: SPK_NARRATOR gets the
 * narration band, SPK_HINT a plain framed box, others a framed box with a
 * name tab from g_speakerNames[]. */
typedef enum {
    SPK_NARRATOR = 0,
    SPK_HINT,
    SPK_GUIDE,
    SPK_IDA,
    SPK_RETAINER,
    SPK_THEO,          /* prelude 1 pilgrims */
    SPK_SILVANUS,
    SPK_ANNA,
    SPK_COUNT
} SpeakerId;

extern const char *const g_speakerNames[SPK_COUNT];

typedef struct SceneScriptEntry {
    u16 textBankId;      /* index into g_textBank[] */
    u8  portraitId;        /* index into portrait tile set, SCENE_NO_PORTRAIT = none */
    u8  speakerId;         /* SpeakerId; 0 = narration */
    u8  flagsRequired;      /* SFLAG_* bitmask; 0 = always eligible */
    u16 nextOnDefault;        /* next scriptEntryId to run after this one, or SCENE_END */
    u16 nextOnFlagSet;          /* branch target if all flagsRequired bits are set in
                                  * g_save.flags; SCENE_END = unused (falls through to
                                  * nextOnDefault instead) */
} SceneScriptEntry;

/* One null-terminated ASCII string per textBankId, in ROM. */
extern const char *const g_textBank[];

/* Runs the script chain starting at entryId to completion (blocking: pumps
 * its own input/VBlank loop). Draws the bottom text box via the shared
 * 8x8 font tileset. Returns when a chain reaches SCENE_END. */
void scene_run(const SceneScriptEntry *table, u16 entryId);

/* Optional callback run once per frame while scene_run() waits on a page
 * (after the sprites are presented, before they are rendered again), so a
 * map can keep ambient actors moving under dialogue. NULL to clear. */
void scene_set_frame_hook(void (*hook)(void));

#endif /* CRUSADE_SCENE_H */
