/* debug.h -- debug-build frame pacing HUD.
 *
 * Gameplay loops call vsync_wait() instead of VBlankIntrWait(). In a
 * CRUSADE_DEBUG build (make DEBUG=1, the playtest default) it also:
 *   - counts VBlanks in an IRQ handler and flags frames the game missed
 *     (more than one VBlank between two vsync_wait() calls),
 *   - samples REG_VCOUNT on entry to measure how much of the 228-line
 *     frame the game logic used,
 * and prints "FPS nn DROP n CPU nn%" on the top text row once a second.
 * Release builds compile vsync_wait() down to VBlankIntrWait().
 */
#ifndef CRUSADE_DEBUG_H
#define CRUSADE_DEBUG_H

#include <tonc.h>

#ifdef CRUSADE_DEBUG
void debug_init(void);
void vsync_wait(void);
#else
static inline void debug_init(void) {}
static inline void vsync_wait(void) { VBlankIntrWait(); }
#endif

#endif /* CRUSADE_DEBUG_H */
