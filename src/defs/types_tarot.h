#ifndef TYPE_TAROT_H
#define TYPE_TAROT_H
#include "stdint.h"

typedef enum TarotSuit{
    SUIT_WANDS,
    SUIT_PENTACLES,
    SUIT_SWORDS,
    SUIT_CUPS,
    SUIT_MAJOR_ARCANA
}TarotSuit;

typedef enum TarotBehavior{
    TAROT_BEHAVIOR_AIM,
    TAROT_BEHAVIOR_HEAL
}TarotBehavior;

typedef struct TarotCard{
    int id;
    char name[32];
    TarotSuit suit;
    int value;
    int equip_cost;
    int use_cost;
    uint32_t element_flags; // might not use since we can more easily set it at the fn level
    int potency;
    float duration;
    TarotBehavior behavior;
    int item_id;
}TarotCard;
#endif
