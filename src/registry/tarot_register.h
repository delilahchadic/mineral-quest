#ifndef TAROT_REGISTER_H
#define TAROT_REGISTER_H
#include "defs/types_entities.h"
extern TarotCard TAROT_REGISTRY[78];

TarotCard* GetTarotCardByItemId(int item_id);
void(*tarot_callbacks[100]) (void*);
void ExecuteTarotCommand(int id, void* context);
void ThreeOfWands(void* context);
void ThreeOfCups(void* context);
void TheTower(void *context);
#endif
