#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "defs/types_systems.h"
#include <stddef.h>

Player Get_Default_Player();
void ClosePlayer(Player* player);
void GiveItem(Player* player,int id);
void RecalculateStats(StatBlock* stats, Gear* gear);
int CanAfford(Player* player, CostSlot slot);
void ProcessRecipe(Player* player, Exchange e);
void SetDefaultStat(Player* player);
void DamagePlayer(int damage);
void UseItem(Player* player, ItemDefinition* item);
void UpdateBuffs(Player* player, float dt) ;
void RemoveOneFromInventory(Player* player, int id);
// systems/player.h

void PlayerEquipWeapon(Player* player, int new_item_id);
void PlayerUnequipWeapon(Player* player);

void PlayerEquipAccessory(Player* player, int slot_index, int new_item_id);
void PlayerUnequipAccessory(Player* player, int slot_index);

void PlayerEquipTarot(Player* player, int slot_index, int new_item_id);
void PlayerUnequipTarot(Player* player, int slot_index);

void PlayerCycleActiveTarot(Player* player);
#endif
