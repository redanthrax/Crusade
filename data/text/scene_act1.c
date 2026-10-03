#include "scene_act1.h"
#include "text_ids.h"
#include "save.h"

const SceneScriptEntry g_sceneAct1[SCN_A1_COUNT] = {
    [SCN_A1_INTRO] = {
        .textBankId = TEXT_A1_INTRO, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_MOTHER, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_MOTHER] = {
        .textBankId = TEXT_A1_MOTHER, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_DUEL_CHALLENGE, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_DUEL_CHALLENGE] = {
        .textBankId = TEXT_A1_DUEL_CHALLENGE, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_DUEL_WIN] = {
        .textBankId = TEXT_A1_DUEL_WIN, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_BLESSING, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BLESSING] = {
        .textBankId = TEXT_A1_BLESSING, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
};
