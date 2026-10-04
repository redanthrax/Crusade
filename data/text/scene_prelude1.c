#include "scene_prelude1.h"
#include "text_ids.h"
#include "save.h"

const SceneScriptEntry g_scenePrelude1[SCN_P1_COUNT] = {
    [SCN_P1_INTRO] = {
        .textBankId = TEXT_P1_INTRO, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_INTRO_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_INTRO_2] = {
        .textBankId = TEXT_P1_INTRO_CROWD, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_MEET] = {
        .textBankId = TEXT_P1_MEET_GUIDE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GUIDE,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_MEET_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_MEET_2] = {
        .textBankId = TEXT_P1_MEET_THEO, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_THEO,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_MEET_3, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_MEET_3] = {
        .textBankId = TEXT_P1_MEET_GUIDE2, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GUIDE,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_MEET_4, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_MEET_4] = {
        .textBankId = TEXT_P1_MEET_SILVANUS, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_SILVANUS,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_MEET_5, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_MEET_5] = {
        .textBankId = TEXT_P1_MEET_ANNA, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_ANNA,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_MEET_6, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_MEET_6] = {
        .textBankId = TEXT_P1_MEET_GUIDE3, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GUIDE,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_MEET_7, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_MEET_7] = {
        .textBankId = TEXT_P1_TUTORIAL_MOVE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_HINT,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_MEET_8, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_MEET_8] = {
        .textBankId = TEXT_P1_TUTORIAL_HERD, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_HINT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_THEO_WELL] = {
        .textBankId = TEXT_P1_THEO_WELL, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_THEO,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_THEO_WELL_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_THEO_WELL_2] = {
        .textBankId = TEXT_P1_HINT_RECALL, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_HINT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_RECALL1] = {
        .textBankId = TEXT_P1_RECALL1_GUIDE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GUIDE,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_RECALL1_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_RECALL1_2] = {
        .textBankId = TEXT_P1_RECALL1_THEO, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_THEO,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_SNATCH] = {
        .textBankId = TEXT_P1_SNATCH_SILVANUS, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_SILVANUS,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_SNATCH_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_SNATCH_2] = {
        .textBankId = TEXT_P1_HINT_CHASE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_HINT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_CAUGHT] = {
        .textBankId = TEXT_P1_CAUGHT, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_LOOTER_FLEES] = {
        .textBankId = TEXT_P1_LOOTER_FLEES, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_LOOTER_FLEES_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_LOOTER_FLEES_2] = {
        .textBankId = TEXT_P1_HINT_PURSE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_HINT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_RETURN] = {
        .textBankId = TEXT_P1_RETURN_GUIDE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GUIDE,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_RETURN_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_RETURN_2] = {
        .textBankId = TEXT_P1_RETURN_SILVANUS, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_SILVANUS,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_RETURN_3, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_RETURN_3] = {
        .textBankId = TEXT_P1_RETURN_ANNA, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_ANNA,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_THEO_FIGS] = {
        .textBankId = TEXT_P1_THEO_FIGS, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_THEO,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_RECALL2] = {
        .textBankId = TEXT_P1_RECALL2_GUIDE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GUIDE,
        .flagsRequired = 0, .nextOnDefault = SCN_P1_RECALL2_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_RECALL2_2] = {
        .textBankId = TEXT_P1_RECALL2_THEO, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_THEO,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_WAIT] = {
        .textBankId = TEXT_P1_WAIT, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GUIDE,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_WAIT_PURSE] = {
        .textBankId = TEXT_P1_WAIT_PURSE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GUIDE,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_FINALE] = {
        .textBankId = TEXT_P1_FINALE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GUIDE,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_P1_OUTRO] = {
        .textBankId = TEXT_P1_OUTRO, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
};
