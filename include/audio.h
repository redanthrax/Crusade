/* audio.h -- thin Maxmod wrapper.
 *
 * Tracked modules only (Maxmod .mod/.s3m via mmutil), no streaming audio.
 */
#ifndef CRUSADE_AUDIO_H
#define CRUSADE_AUDIO_H

#include <tonc.h>

#define AUDIO_BGM_NONE 0xFFFF

/* Sets up Maxmod against the mmutil-generated soundbank, if present. */
void audio_init(void);

/* Starts a looping BGM module by its mmutil-assigned id. No-ops if audio
 * isn't initialized or moduleId == AUDIO_BGM_NONE. */
void audio_play_bgm(u16 moduleId);

/* Call once per frame (inside or right after VBlankIntrWait()) to let
 * Maxmod mix the next audio buffer. */
void audio_frame_update(void);

#endif /* CRUSADE_AUDIO_H */
