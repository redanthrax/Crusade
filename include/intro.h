/* intro.h -- opening cinematic shown on New Game before Prelude 1: sets the
 * scene for players who don't know Godfrey's story (Manzikert, Clermont,
 * Godfrey taking the cross, the road east). A advances narration, START
 * skips the whole thing.
 *
 * Borrows BG0 (CBB0/SBB16, palette bank 0) and the blend registers; leaves
 * BG0 disabled and blending off for map_load() to take over.
 */
#ifndef CRUSADE_INTRO_H
#define CRUSADE_INTRO_H

void intro_run(void);

#endif /* CRUSADE_INTRO_H */
