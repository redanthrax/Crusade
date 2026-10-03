#include "scene_prelude1.h"
#include "text_ids.h"
#include "save.h"

const SceneScriptEntry g_scenePrelude1[SCN_P1_COUNT] = {
    [SCN_P1_INTRO] = {
        .textBankId = TEXT_P1_INTRO, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_TUTORIAL_MOVE, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_TUTORIAL_MOVE] = {
        .textBankId = TEXT_P1_TUTORIAL_MOVE, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_TUTORIAL_HERD, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_TUTORIAL_HERD] = {
        .textBankId = TEXT_P1_TUTORIAL_HERD, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_SCRAP_WARN, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_SCRAP_WARN] = {
        .textBankId = TEXT_P1_SCRAP_WARN, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_OUTRO] = {
        .textBankId = TEXT_P1_OUTRO, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
};
