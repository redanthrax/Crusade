/* title_bg.h -- procedural title/menu backdrop on BG0.
 *
 * A dawn-sky gradient (HDMA on the backdrop colour), stars, the big
 * "CRUSADE" logo, subtitle, and a Jerusalem skyline silhouette with the
 * Church of the Holy Sepulchre. Rendered once into a pixel canvas at first
 * use, converted to tiles, then uploaded to BG0 (CBB0/SBB16, the same slots
 * the world map uses -- map_load() overwrites them when gameplay starts).
 */
#ifndef CRUSADE_TITLE_BG_H
#define CRUSADE_TITLE_BG_H

#include <tonc.h>

/* Uploads the backdrop and enables BG0 + the sky HDMA. */
void title_bg_show(void);

/* Call once per frame right after VBlankIntrWait(): restarts the sky
 * HDMA for the new frame and animates star twinkle. */
void title_bg_vblank(u32 frame);

/* Stops the HDMA, clears the backdrop colour and disables BG0. */
void title_bg_hide(void);

#endif /* CRUSADE_TITLE_BG_H */
