/* text_bank.c -- all dialogue/narration strings, indexed by text_ids.h.
 *
 * Plain ASCII, one string per TEXT_* id. Rendering (8x8 font, bottom text
 * box) is wired up in scene.c; this file only owns the content.
 */
#include "scene.h"
#include "text_ids.h"

const char *const g_textBank[TEXT_COUNT] = {
    [TEXT_INTRO_YEAR] =
        "In the year of Our Lord 1095, Jerusalem had been in Muslim hands "
        "for more than four hundred years. Still, Christian pilgrims walked "
        "the long roads east to pray at the tomb of Christ.",
    [TEXT_INTRO_SELJUKS] =
        "Then came the Seljuk Turks. In 1071 they broke the Byzantine army "
        "at Manzikert, and within a few years they held nearly all of "
        "Anatolia, and Jerusalem too.",
    [TEXT_INTRO_ALEXIOS] =
        "From Constantinople, the Emperor Alexios Komnenos sent envoys west "
        "to beg the Pope for fighting men.",
    [TEXT_INTRO_CLERMONT] =
        "In November 1095, in a field outside Clermont, Pope Urban II called "
        "on the knights of the West to take up the cross and march east.",
    [TEXT_INTRO_DEUS_VULT] =
        "The crowd answered him with one cry: \"Deus vult!\" God wills it!",
    [TEXT_INTRO_GODFREY] =
        "Among those who took the cross was Godfrey of Bouillon, Duke of "
        "Lower Lorraine, descended from Charlemagne on his mother's side.",
    [TEXT_INTRO_PLEDGE] =
        "To pay for the journey he pledged his castle of Bouillon to the "
        "Bishop of Liege, and sold much of what he owned.",
    [TEXT_INTRO_MARCH] =
        "In August 1096 he rode out with his brother Baldwin at the head of "
        "a great army: down the Danube, through Hungary to Constantinople, "
        "across Anatolia to Antioch.",
    [TEXT_INTRO_ROAD] =
        "Before them lay more than two thousand miles of road, and three "
        "years of war.",
    [TEXT_INTRO_BEGIN] =
        "But to understand why men would march so far, our story must "
        "begin long before... at an empty tomb in Jerusalem.",

    [TEXT_P1_INTRO] =
        "Jerusalem, in the years after Constantine. Over the empty tomb "
        "of Christ his builders raised the Anastasis, a great domed "
        "rotunda. Beside it, in an open court, stands the rock of "
        "Calvary.",
    [TEXT_P1_INTRO_CROWD] =
        "On feast days the courtyard fills with pilgrims from every "
        "corner of the empire.",
    [TEXT_P1_MEET_GUIDE] =
        "Here we are, friends. The Holy Sepulchre. Mind your feet on "
        "the old stones.",
    [TEXT_P1_MEET_THEO] =
        "Is that the rock where they crucified Him? Can I touch it?",
    [TEXT_P1_MEET_GUIDE2] =
        "Later, Theo. We go in together, or not at all.",
    [TEXT_P1_MEET_SILVANUS] =
        "Then let us not dawdle. I carry enough Antioch silver to buy "
        "half this courtyard, and a good share of it is for the church.",
    [TEXT_P1_MEET_ANNA] =
        "I walked here from Gaul, young man. I can wait a few minutes "
        "more.",
    [TEXT_P1_MEET_GUIDE3] =
        "Stay close, all of you. Feast days bring thieves as well as "
        "pilgrims.",
    [TEXT_P1_TUTORIAL_MOVE] =
        "D-pad to walk. Hold B to hurry.",
    [TEXT_P1_TUTORIAL_HERD] =
        "Lead the procession up to the Anastasis door. Keep them close.",
    [TEXT_P1_THEO_WELL] =
        "A well! I'm so thirsty. Just one sip!",
    [TEXT_P1_HINT_RECALL] =
        "Theo has wandered off. Walk up to him and press A to call him "
        "back.",
    [TEXT_P1_RECALL1_GUIDE] =
        "Theo! Back in line, please.",
    [TEXT_P1_RECALL1_THEO] =
        "Sorry! Everything here is just so big.",
    [TEXT_P1_SNATCH_SILVANUS] =
        "Hey! My purse! Stop, thief!",
    [TEXT_P1_HINT_CHASE] =
        "Catch the cutpurse! Hold B to run him down, then press A to "
        "shove him. No need for steel here.",
    [TEXT_P1_CAUGHT] =
        "The cutpurse sprawls on the flagstones, and the purse tumbles "
        "from his belt.",
    [TEXT_P1_LOOTER_FLEES] =
        "He scrambles up and vanishes into the crowd.",
    [TEXT_P1_HINT_PURSE] =
        "Pick up the purse and bring it back to Silvanus. Press A "
        "beside him.",
    [TEXT_P1_RETURN_GUIDE] =
        "Your silver, Silvanus.",
    [TEXT_P1_RETURN_SILVANUS] =
        "Bless you, friend! Perhaps... I should not have boasted quite "
        "so loudly.",
    [TEXT_P1_RETURN_ANNA] =
        "Perhaps not.",
    [TEXT_P1_THEO_FIGS] =
        "Figs! Look how ripe those ones are!",
    [TEXT_P1_RECALL2_GUIDE] =
        "Theo. The figs will still be here after.",
    [TEXT_P1_RECALL2_THEO] =
        "Coming, coming!",
    [TEXT_P1_WAIT] =
        "Not yet. The procession must enter together.",
    [TEXT_P1_WAIT_PURSE] =
        "Not without Silvanus's purse.",
    [TEXT_P1_FINALE] =
        "Here we are. Heads bowed, now. Quietly.",
    [TEXT_P1_OUTRO] =
        "The procession passes into the Anastasis, to the tomb of "
        "Christ. For now, the road to Jerusalem lies open to all who "
        "would walk it.",

    [TEXT_A1_INTRO] =
        "Bouillon, in the Ardennes. Spring, in the year of Our Lord 1096.",
    [TEXT_A1_INTRO_PLEDGE] =
        "Godfrey, Duke of Lower Lorraine, has taken the cross. To pay for the march he has pledged his castle to the Bishop of Liege.",
    [TEXT_A1_INTRO_YARD] =
        "The road east opens with the summer. Until then, there is the yard.",
    [TEXT_A1_WICHER_HELLO] =
        "Morning, my lord duke. The pells are set.",
    [TEXT_A1_WICHER_PELLS] =
        "They won't strike back. But they won't flatter you either.",
    [TEXT_A1_HINT_SWORD] =
        "Press A to swing your sword. Beat down all three pells.",
    [TEXT_A1_PELLS_DONE] =
        "Straw doesn't bleed, my lord. Men do. You want a partner who hits back.",
    [TEXT_A1_BALDWIN_HELLO] =
        "Then you're in luck! Good morning, brother.",
    [TEXT_A1_BALDWIN_GODFREY1] =
        "Baldwin. Shouldn't you be packing?",
    [TEXT_A1_BALDWIN_SOLD] =
        "Packed days ago. They say you sold Bouillon to the bishop to pay for all this.",
    [TEXT_A1_BALDWIN_GODFREY2] =
        "Pledged, not sold. I'll redeem it when I come home.",
    [TEXT_A1_BALDWIN_DARE] =
        "If you come home. Into the ring, then. First to three touches.",
    [TEXT_A1_HINT_DUEL] =
        "Baldwin guards against a swing from the front. Strike while his sword is raised, or just after his blow. Hold B to step clear.",
    [TEXT_A1_DUEL_LOSE] =
        "That's three to me! Again, brother.",
    [TEXT_A1_DUEL_LOSE_WICHER] =
        "Watch his sword arm, my lord. It rises before every blow.",
    [TEXT_A1_DUEL_WIN] =
        "Enough, enough! You've still got it.",
    [TEXT_A1_DUEL_WIN_WICHER] =
        "A duke's blow, that. Save the rest for the road.",
    [TEXT_A1_IDA_CALL] =
        "Godfrey.",
    [TEXT_A1_HINT_IDA] =
        "Your mother waits at the chapel door. Go to her.",
    [TEXT_A1_BLESS_GOING] =
        "You are going, then.",
    [TEXT_A1_BLESS_CROSS] =
        "I have taken the cross, Mother. I cannot set it down.",
    [TEXT_A1_BLESS_RIGHTLY] =
        "Then carry it rightly. Strength without devotion is only violence, my son.",
    [TEXT_A1_BLESS_ROAD] =
        "Not for gold, and not for land. The road to the Sepulchre was open once. Go and make it so again.",
    [TEXT_A1_BLESS_WILL] =
        "I will, Mother.",
    [TEXT_A1_BLESS_SIGN] =
        "Countess Ida makes the sign of the cross over her son, as she did when he was a boy.",
    [TEXT_A1_OUTRO] =
        "In August of 1096, Godfrey of Bouillon rode east with his brother Baldwin and the knights of Lorraine, bound for Constantinople.",
    [TEXT_A1_OUTRO_2] =
        "Jerusalem lay three years down the road.",
};
