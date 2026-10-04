/* scene_prelude1.h -- scene script table for the Jerusalem courtyard vignette */
#ifndef CRUSADE_SCENE_PRELUDE1_H
#define CRUSADE_SCENE_PRELUDE1_H

#include "scene.h"

/* Each multi-line exchange is a chain: SCN_P1_X -> SCN_P1_X_2 -> ... */
enum {
    SCN_P1_INTRO = 0,
    SCN_P1_INTRO_2,
    SCN_P1_MEET,
    SCN_P1_MEET_2,
    SCN_P1_MEET_3,
    SCN_P1_MEET_4,
    SCN_P1_MEET_5,
    SCN_P1_MEET_6,
    SCN_P1_MEET_7,
    SCN_P1_MEET_8,
    SCN_P1_THEO_WELL,
    SCN_P1_THEO_WELL_2,
    SCN_P1_RECALL1,
    SCN_P1_RECALL1_2,
    SCN_P1_SNATCH,
    SCN_P1_SNATCH_2,
    SCN_P1_CAUGHT,
    SCN_P1_LOOTER_FLEES,
    SCN_P1_LOOTER_FLEES_2,
    SCN_P1_RETURN,
    SCN_P1_RETURN_2,
    SCN_P1_RETURN_3,
    SCN_P1_THEO_FIGS,
    SCN_P1_RECALL2,
    SCN_P1_RECALL2_2,
    SCN_P1_WAIT,
    SCN_P1_WAIT_PURSE,
    SCN_P1_FINALE,
    SCN_P1_OUTRO,
    SCN_P1_COUNT
};

extern const SceneScriptEntry g_scenePrelude1[SCN_P1_COUNT];

#endif /* CRUSADE_SCENE_PRELUDE1_H */
