/* map.h -- metatile map headers and collision flags.
 *
 * Maps are stored as const data in ROM: a flat array of metatile ids (16x16
 * px cells) plus a small per-metatile-id flag table used for collision.
 * There is no per-pixel collision anywhere in this codebase.
 */
#ifndef CRUSADE_MAP_H
#define CRUSADE_MAP_H

#include <tonc.h>
#include "fixed.h"

#define METATILE_PX 16 /* each metatile is 16x16 px = 2x2 8px tiles */

/* Collision flag bits, OR'd together in MapHeader.collisionFlags[metatileId] */
#define MTF_SOLID    (1 << 0) /* blocks movement */
#define MTF_WATER    (1 << 1) /* slows movement, flavor only for now */
#define MTF_LEDGE    (1 << 2) /* one-way drop, not used by first slice */
#define MTF_TRIGGER  (1 << 3) /* stepping onto this metatile fires a scene script */

typedef struct MapHeader {
    u16 widthMeta, heightMeta;    /* map size in metatiles */
    const u16 *metatileLayer;      /* metatile id per cell, widthMeta*heightMeta */
    const u8  *collisionFlags;      /* MTF_* flags, indexed by metatile id */
    const u16 *tilesetGfx;          /* 4bpp tile gfx (grit-exported or hand-authored) */
    u32 tilesetGfxLen;               /* length of tilesetGfx in u16 units */
    const u16 *tilesetPal;           /* 16-color palette (BGR555) */
    u16 encounterTableId;            /* index into enemy spawn table, 0 = none */
    u16 scriptEntryId;                /* first scene-script entry run on load, 0 = none */
    u16 bgmId;                        /* Maxmod module id for this map, 0xFFFF = none */
} MapHeader;

/* Loads a map's BG tiles/palette/metatile layer into VRAM and sets up the
 * collision lookup used by collision.c. Resets the actor pool's non-player
 * actors; does not touch SaveData. */
void map_load(const MapHeader *map);

/* Returns the MTF_* flags for the metatile under the given pixel-space
 * world position. */
u8 map_collision_at(fx8 worldX, fx8 worldY);

extern const MapHeader *g_currentMap;

#endif /* CRUSADE_MAP_H */
