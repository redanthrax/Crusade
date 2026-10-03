/* fixed.h -- 24.8 fixed point helpers built on libtonc's FIXED type.
 *
 * libtonc already defines FIXED (s32, FIX_SHIFT=8) with int2fx/fx2int/
 * fxmul/fxdiv in tonc_math.h. We alias it as fx8 for naming clarity in our
 * own structs and add a couple of compile-time-only literal helpers.
 * No floating point is used at runtime anywhere in this codebase.
 */
#ifndef CRUSADE_FIXED_H
#define CRUSADE_FIXED_H

#include <tonc.h>

typedef FIXED fx8; /* s32, 24.8 fixed point */

#define FX8_ONE   (1 << FIX_SHIFT)
#define FX8_HALF  (FX8_ONE >> 1)

/* FX8(x): compile-time literal conversion, e.g. FX8(1.5). The multiply is
 * folded by the compiler for constant arguments; never call with a
 * runtime variable. */
#define FX8(x) ((fx8)((x) * FX8_ONE))

#define FX8_FROM_INT(i) int2fx(i)
#define FX8_TO_INT(f)   fx2int(f)

#endif /* CRUSADE_FIXED_H */
