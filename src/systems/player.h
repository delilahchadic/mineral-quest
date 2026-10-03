#ifndef PLAYER_H
#define PLAYER_H
#include "defs/types_engine.h"
#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "raylib.h"
#include <stddef.h>

Player Get_Default_Player();
void ClosePlayer(Player *player);
void GiveItem(Player *player, int id);
void RecalculateStats(Player* player);
int CanAfford(Player *player, CostSlot slot);
void ProcessRecipe(Player *player, Exchange e);
void SetDefaultStat(Player *player);
void DamagePlayer(int damage);
void UseItem(Player *player, ItemDefinition *item);
void UpdateBuffs(Player *player, float dt);
void RemoveOneFromInventory(Player *player, int id);
void UsePlayerTarotSlot(Player *player, Gamestate *gamestate, int index);
void FullyRestPlayer(Player *player);
#endif
