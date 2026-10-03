/* text_bank.c -- all dialogue/narration strings, indexed by text_ids.h.
 *
 * Plain ASCII, one string per TEXT_* id. Rendering (8x8 font, bottom text
 * box) is wired up in scene.c; this file only owns the content.
 */
#include "scene.h"
#include "text_ids.h"

const char *const g_textBank[TEXT_COUNT] = {
    [TEXT_P1_INTRO] =
        "Jerusalem, in the years after Constantine. "
        "Pilgrims gather in the courtyard of the Holy Sepulchre.",
    [TEXT_P1_TUTORIAL_MOVE] =
        "D-pad to walk. Hold B to hurry.",
    [TEXT_P1_TUTORIAL_HERD] =
        "Lead the procession up to the Anastasis door. "
        "Keep them close. Stragglers draw trouble.",
    [TEXT_P1_SCRAP_WARN] =
        "A cutpurse slips out of the colonnade! "
        "Get near and press A to shove him off. No need for steel here.",
    [TEXT_P1_WAIT] =
        "Not yet. The procession must enter together.",
    [TEXT_P1_LOOTER_FLEES] =
        "The cutpurse flees into the crowd.",
    [TEXT_P1_OUTRO] =
        "The procession enters the Anastasis, where the tomb of Christ "
        "is kept. Safe, this time.",

    [TEXT_A1_INTRO] =
        "Bouillon. Years later. "
        "Godfrey trains in the yard at dawn.",
    [TEXT_A1_MOTHER] =
        "Strength without devotion is only violence, my son.",
    [TEXT_A1_DUEL_CHALLENGE] =
        "Let's see what the tutors taught you, Godfrey.",
    [TEXT_A1_DUEL_WIN] =
        "Godfrey holds his footing.\n"
        "The yard falls quiet.",
    [TEXT_A1_BLESSING] =
        "His mother watches from the gate,\n"
        "and says nothing -- which is approval.",
};
