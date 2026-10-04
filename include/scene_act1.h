/* scene_act1.h -- scene script table for the Bouillon training yard (Act 1 opening) */
#ifndef CRUSADE_SCENE_ACT1_H
#define CRUSADE_SCENE_ACT1_H

#include "scene.h"

/* Each multi-line exchange is a chain: SCN_A1_X -> SCN_A1_X_2 -> ... */
enum {
    SCN_A1_INTRO = 0,
    SCN_A1_INTRO_2,
    SCN_A1_INTRO_3,
    SCN_A1_WICHER,
    SCN_A1_WICHER_2,
    SCN_A1_WICHER_3,
    SCN_A1_PELLS_DONE,
    SCN_A1_BALDWIN,
    SCN_A1_BALDWIN_2,
    SCN_A1_BALDWIN_3,
    SCN_A1_BALDWIN_4,
    SCN_A1_BALDWIN_5,
    SCN_A1_BALDWIN_6,
    SCN_A1_DUEL_LOSE,
    SCN_A1_DUEL_LOSE_2,
    SCN_A1_DUEL_WIN,
    SCN_A1_DUEL_WIN_2,
    SCN_A1_IDA_CALL,
    SCN_A1_IDA_CALL_2,
    SCN_A1_BLESS,
    SCN_A1_BLESS_2,
    SCN_A1_BLESS_3,
    SCN_A1_BLESS_4,
    SCN_A1_BLESS_5,
    SCN_A1_BLESS_6,
    SCN_A1_OUTRO,
    SCN_A1_OUTRO_2,
    SCN_A1_COUNT
};

extern const SceneScriptEntry g_sceneAct1[SCN_A1_COUNT];

#endif /* CRUSADE_SCENE_ACT1_H */
