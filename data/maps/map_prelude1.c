/* map_prelude1.c -- Church of the Holy Sepulchre courtyard (prelude vignette 1) */
#include <tonc.h>
#include "map.h"
#include "scene.h"
#include "audio.h"
#include "placeholder_tileset.h"

#define MAP_W 15
#define MAP_H 10

static const u16 s_prelude1MetaLayer[MAP_W * MAP_H] = {
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,3,3,3,3,3,3,3,3,3,3,3,3,3,2,
    2,3,3,3,3,3,3,3,3,3,3,3,3,3,2,
    2,3,3,3,3,3,3,3,3,3,3,3,3,3,2,
    2,3,3,3,3,3,3,3,3,3,3,3,3,3,2,
    2,3,3,3,3,3,3,3,3,3,3,3,3,3,2,
    2,3,3,3,3,3,3,3,3,3,3,3,3,3,2,
    2,3,3,3,3,3,3,3,3,3,3,3,3,3,2,
    2,3,3,3,3,3,3,3,3,3,3,3,3,3,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
};

const MapHeader g_mapPrelude1Courtyard = {
    .widthMeta = MAP_W,
    .heightMeta = MAP_H,
    .metatileLayer = s_prelude1MetaLayer,
    .collisionFlags = g_placeholderCollisionFlags,
    .tilesetGfx = g_placeholderTilesetGfx,
    .tilesetGfxLen = 256, /* 4*4*16 */
    .tilesetPal = g_placeholderTilesetPal,
    .encounterTableId = 0,
    .scriptEntryId = 0,
    .bgmId = AUDIO_BGM_NONE
};
