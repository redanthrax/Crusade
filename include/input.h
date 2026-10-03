/* input.h -- thin wrapper over libtonc key_* polling. */
#ifndef CRUSADE_INPUT_H
#define CRUSADE_INPUT_H

#include <tonc.h>

/* Call exactly once per frame, after VBlankIntrWait(), before any
 * key_is_down/key_hit calls that frame. Thin wrapper kept so the act files
 * never call key_poll() directly -- keeps one call site if input needs
 * remapping (e.g. a rebindable scheme) later. */
INLINE void input_poll(void) { key_poll(); }

#endif /* CRUSADE_INPUT_H */
