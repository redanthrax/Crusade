/* map_act1_trainingyard.c -- Bouillon training yard (Act 1 opening map) */
#include <tonc.h>
#include "map.h"
#include "scene.h"
#include "audio.h"
#include "placeholder_tileset.h"

#define MAP_W 15
#define MAP_H 10

static const u16 s_act1MetaLayer[MAP_W * MAP_H] = {
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
    2,0,0,0,0,0,1,1,1,0,0,0,0,0,2,
    2,0,0,0,0,0,1,1,1,0,0,0,0,0,2,
    2,0,0,0,0,0,1,1,1,0,0,0,0,0,2,
    2,0,0,0,0,0,1,1,1,0,0,0,0,0,2,
    2,0,0,0,0,0,1,1,1,0,0,0,0,0,2,
    2,0,0,0,0,0,1,1,1,0,0,0,0,0,2,
    2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
};

const MapHeader g_mapAct1TrainingYard = {
    .widthMeta = MAP_W,
    .heightMeta = MAP_H,
    .metatileLayer = s_act1MetaLayer,
    .collisionFlags = g_placeholderCollisionFlags,
    .tilesetGfx = g_placeholderTilesetGfx,
    .tilesetGfxLen = 256,
    .tilesetPal = g_placeholderTilesetPal,
    .encounterTableId = 0,
    .scriptEntryId = 0,
    .bgmId = AUDIO_BGM_NONE
};
