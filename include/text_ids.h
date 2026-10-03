/* text_ids.h -- indices into g_textBank[], shared by every scene script
 * table so chapters can't collide on raw integers.
 */
#ifndef CRUSADE_TEXT_IDS_H
#define CRUSADE_TEXT_IDS_H

enum {
    /* Prelude 1: Jerusalem courtyard */
    TEXT_P1_INTRO = 0,
    TEXT_P1_TUTORIAL_MOVE,
    TEXT_P1_TUTORIAL_HERD,
    TEXT_P1_SCRAP_WARN,
    TEXT_P1_WAIT,
    TEXT_P1_LOOTER_FLEES,
    TEXT_P1_OUTRO,

    /* Act 1: Bouillon training yard */
    TEXT_A1_INTRO,
    TEXT_A1_MOTHER,
    TEXT_A1_DUEL_CHALLENGE,
    TEXT_A1_DUEL_WIN,
    TEXT_A1_BLESSING,

    TEXT_COUNT
};

#endif /* CRUSADE_TEXT_IDS_H */
