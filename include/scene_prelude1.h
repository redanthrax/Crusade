/* scene_prelude1.h -- scene script table for the Jerusalem courtyard vignette */
#ifndef CRUSADE_SCENE_PRELUDE1_H
#define CRUSADE_SCENE_PRELUDE1_H

#include "scene.h"

enum {
    SCN_P1_INTRO = 0,
    SCN_P1_TUTORIAL_MOVE,
    SCN_P1_TUTORIAL_HERD,
    SCN_P1_SCRAP_WARN,
    SCN_P1_WAIT,
    SCN_P1_LOOTER_FLEES,
    SCN_P1_OUTRO,
    SCN_P1_COUNT
};

extern const SceneScriptEntry g_scenePrelude1[SCN_P1_COUNT];

#endif /* CRUSADE_SCENE_PRELUDE1_H */
