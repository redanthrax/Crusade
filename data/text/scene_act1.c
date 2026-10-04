#include "scene_act1.h"
#include "text_ids.h"
#include "save.h"

const SceneScriptEntry g_sceneAct1[SCN_A1_COUNT] = {
    [SCN_A1_INTRO] = {
        .textBankId = TEXT_A1_INTRO, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_INTRO_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_INTRO_2] = {
        .textBankId = TEXT_A1_INTRO_PLEDGE, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_INTRO_3, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_INTRO_3] = {
        .textBankId = TEXT_A1_INTRO_YARD, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_WICHER] = {
        .textBankId = TEXT_A1_WICHER_HELLO, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_WICHER,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_WICHER_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_WICHER_2] = {
        .textBankId = TEXT_A1_WICHER_PELLS, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_WICHER,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_WICHER_3, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_WICHER_3] = {
        .textBankId = TEXT_A1_HINT_SWORD, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_HINT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_PELLS_DONE] = {
        .textBankId = TEXT_A1_PELLS_DONE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_WICHER,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BALDWIN] = {
        .textBankId = TEXT_A1_BALDWIN_HELLO, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_BALDWIN,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_BALDWIN_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BALDWIN_2] = {
        .textBankId = TEXT_A1_BALDWIN_GODFREY1, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GODFREY,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_BALDWIN_3, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BALDWIN_3] = {
        .textBankId = TEXT_A1_BALDWIN_SOLD, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_BALDWIN,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_BALDWIN_4, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BALDWIN_4] = {
        .textBankId = TEXT_A1_BALDWIN_GODFREY2, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GODFREY,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_BALDWIN_5, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BALDWIN_5] = {
        .textBankId = TEXT_A1_BALDWIN_DARE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_BALDWIN,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_BALDWIN_6, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BALDWIN_6] = {
        .textBankId = TEXT_A1_HINT_DUEL, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_HINT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_DUEL_LOSE] = {
        .textBankId = TEXT_A1_DUEL_LOSE, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_BALDWIN,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_DUEL_LOSE_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_DUEL_LOSE_2] = {
        .textBankId = TEXT_A1_DUEL_LOSE_WICHER, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_WICHER,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_DUEL_WIN] = {
        .textBankId = TEXT_A1_DUEL_WIN, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_BALDWIN,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_DUEL_WIN_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_DUEL_WIN_2] = {
        .textBankId = TEXT_A1_DUEL_WIN_WICHER, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_WICHER,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_IDA_CALL] = {
        .textBankId = TEXT_A1_IDA_CALL, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_IDA,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_IDA_CALL_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_IDA_CALL_2] = {
        .textBankId = TEXT_A1_HINT_IDA, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_HINT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BLESS] = {
        .textBankId = TEXT_A1_BLESS_GOING, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_IDA,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_BLESS_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BLESS_2] = {
        .textBankId = TEXT_A1_BLESS_CROSS, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GODFREY,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_BLESS_3, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BLESS_3] = {
        .textBankId = TEXT_A1_BLESS_RIGHTLY, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_IDA,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_BLESS_4, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BLESS_4] = {
        .textBankId = TEXT_A1_BLESS_ROAD, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_IDA,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_BLESS_5, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BLESS_5] = {
        .textBankId = TEXT_A1_BLESS_WILL, .portraitId = SCENE_NO_PORTRAIT, .speakerId = SPK_GODFREY,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_BLESS_6, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_BLESS_6] = {
        .textBankId = TEXT_A1_BLESS_SIGN, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_OUTRO] = {
        .textBankId = TEXT_A1_OUTRO, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCN_A1_OUTRO_2, .nextOnFlagSet = SCENE_END
    },
    [SCN_A1_OUTRO_2] = {
        .textBankId = TEXT_A1_OUTRO_2, .portraitId = SCENE_NO_PORTRAIT,
        .flagsRequired = 0, .nextOnDefault = SCENE_END, .nextOnFlagSet = SCENE_END
    },
};
