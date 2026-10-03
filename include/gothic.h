/* gothic.h -- variable-width blackletter text (UnifrakturCook raster, see
 * graphics/gen/gothic_font.h), drawn into the title backdrop canvas for the
 * logo and subtitle. Menus and dialogue stay on the plain TTE system font.
 */
#ifndef CRUSADE_GOTHIC_H
#define CRUSADE_GOTHIC_H

#include <tonc.h>
#include "gothic_font.h"

int gothic_text_width(const char *s);

/* Writes `ink` into an 8bpp buffer wherever a glyph pixel is set. (x,y) is
 * the top-left of the glyph band; pixels outside [0,w)x[0,h) are clipped. */
void gothic_draw(u8 *buf, int pitch, int w, int h, int x, int y,
                 const char *s, u8 ink);

/* Draws the 1bpp logo bitmap with a per-row colour from `rowInk(row)`. */
void gothic_draw_logo(u8 *buf, int pitch, int w, int h, int x, int y,
                      u8 (*rowInk)(int row));

#endif /* CRUSADE_GOTHIC_H */
