#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "defs/types_systems.h"
#include <stddef.h>

Player Get_Default_Player();
void Close_Player(Player* player);
void GiveItem(Player* player,int id);
void RecalculateStats(StatBlock* stats, EquipmentSet* gear);
void RemoveItemAt(Player* player, int index);
int CanAfford(Player* player, CostSlot slot);
void ProcessRecipe(Player* player, Exchange e);
#endif
