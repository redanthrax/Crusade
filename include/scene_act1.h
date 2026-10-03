/* scene_act1.h -- scene script table for the Bouillon training yard (Act 1 opening) */
#ifndef CRUSADE_SCENE_ACT1_H
#define CRUSADE_SCENE_ACT1_H

#include "scene.h"

enum {
    SCN_A1_INTRO = 0,
    SCN_A1_MOTHER,
    SCN_A1_DUEL_CHALLENGE,
    SCN_A1_DUEL_WIN,
    SCN_A1_BLESSING,
    SCN_A1_COUNT
};

extern const SceneScriptEntry g_sceneAct1[SCN_A1_COUNT];

#endif /* CRUSADE_SCENE_ACT1_H */
