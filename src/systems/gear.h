#ifndef GEAR_H
#define GEAR_H
#include "defs/types_entities.h"

void InitGear(Gear* gear);
void PlayerEquipWeapon(Player* player, int new_item_id);
void PlayerUnequipWeapon(Player* player);
void PlayerEquipAccessory(Player* player, int slot_index, int new_item_id);
void PlayerUnequipAccessory(Player* player, int slot_index);
void PlayerEquipTarot(Player* player, int slot_index, int new_item_id);
void PlayerUnequipTarot(Player* player, int slot_index);
void PlayerCycleActiveTarot(Player* player);
#endif
