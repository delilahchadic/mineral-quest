#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "defs/types_systems.h"
#include "systems/inventory.h"
#include <stddef.h>

Player Get_Default_Player();
void Close_Player(Player* player);
void GiveItem(Player* player,int id);
void RecalculateStats(StatBlock* stats, EquipmentSet* gear);
void RemoveItemAt(Player* player, int index);
#endif
