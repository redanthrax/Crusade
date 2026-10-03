/* text_bank.c -- all dialogue/narration strings, indexed by text_ids.h.
 *
 * Plain ASCII, one string per TEXT_* id. Rendering (8x8 font, bottom text
 * box) is wired up in scene.c; this file only owns the content.
 */
#include "scene.h"
#include "text_ids.h"

const char *const g_textBank[TEXT_COUNT] = {
    [TEXT_P1_INTRO] =
        "Jerusalem, after Constantine.\n"
        "Pilgrims gather at the Holy Sepulchre.",
    [TEXT_P1_TUTORIAL_MOVE] =
        "Use the D-pad to walk, B to run.",
    [TEXT_P1_TUTORIAL_HERD] =
        "Keep the procession together.\n"
        "Stragglers draw trouble.",
    [TEXT_P1_SCRAP_WARN] =
        "Looters! Shove them off with A --\n"
        "no need for steel here.",
    [TEXT_P1_OUTRO] =
        "The procession reaches the Sepulchre\n"
        "safely, this time.",

    [TEXT_A1_INTRO] =
        "Bouillon. Years later.\n"
        "Godfrey trains in the yard at dawn.",
    [TEXT_A1_MOTHER] =
        "\"Strength without devotion is only\n"
        "violence,\" his mother had taught him.",
    [TEXT_A1_DUEL_CHALLENGE] =
        "A rival retainer steps forward.\n"
        "\"Let's see what the tutors taught you.\"",
    [TEXT_A1_DUEL_WIN] =
        "Godfrey holds his footing.\n"
        "The yard falls quiet.",
    [TEXT_A1_BLESSING] =
        "His mother watches from the gate,\n"
        "and says nothing -- which is approval.",
};
