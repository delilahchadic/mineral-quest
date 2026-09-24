#include "systems/gear.h"
#include "systems/player.h"
void InitGear(Gear* gear) {
    gear->weapon_id = 12; // equip the classical guitar
    for (int i = 0; i < MAX_ACCESSORY_SLOTS; i++) {
        gear->accessory_ids[i] = -1;
    }
    for (int i = 0; i < 3; i++) {
        gear->tarot_ids[i] = -1;
    }
}

void PlayerEquipTarot(Player* player, int slot_index, int new_item_id) {
    if (slot_index < 0 || slot_index >= 3) return;

    // If there's already a tarot card here, return it to inventory
    if (player->gear.tarot_ids[slot_index] != -1) {
        GiveItem(player, player->gear.tarot_ids[slot_index]);
    }

    // Equip the new one and remove from inventory
    player->gear.tarot_ids[slot_index] = new_item_id;
    if (new_item_id != -1) {
        player->item_inventory[new_item_id]--;
    }

    RecalculateStats(&player->stats, &player->gear);
}

void PlayerUnequipTarot(Player* player, int slot_index) {
    if (slot_index < 0 || slot_index >= 3) return;

    if (player->gear.tarot_ids[slot_index] != -1) {
        GiveItem(player, player->gear.tarot_ids[slot_index]);
        player->gear.tarot_ids[slot_index] = -1;
    }

    RecalculateStats(&player->stats, &player->gear);
}

void PlayerEquipWeapon(Player* player, int new_item_id) {
    // If a weapon is already equipped, return it to inventory
    if (player->gear.weapon_id != -1) {
        GiveItem(player, player->gear.weapon_id);
    }

    // Equip the new weapon and deduct from inventory
    player->gear.weapon_id = new_item_id;
    if (new_item_id != -1) {
        player->item_inventory[new_item_id]--;
    }

    RecalculateStats(&player->stats, &player->gear);
}

void PlayerUnequipWeapon(Player* player) {
    if (player->gear.weapon_id != -1) {
        GiveItem(player, player->gear.weapon_id);
        player->gear.weapon_id = -1;
    }

    RecalculateStats(&player->stats, &player->gear);
}

void PlayerEquipAccessory(Player* player, int slot_index, int new_item_id) {
    int max_slots = player->stats.current[STAT_ACCESORY_COUNT];
    if (slot_index < 0 || slot_index >= max_slots) return;

    // If an accessory is already in this slot, return it to inventory
    if (player->gear.accessory_ids[slot_index] != -1) {
        GiveItem(player, player->gear.accessory_ids[slot_index]);
    }

    // Equip the new accessory and deduct from inventory
    player->gear.accessory_ids[slot_index] = new_item_id;
    if (new_item_id != -1) {
        player->item_inventory[new_item_id]--;
    }

    RecalculateStats(&player->stats, &player->gear);
}

void PlayerUnequipAccessory(Player* player, int slot_index) {
    int max_slots = player->stats.current[STAT_ACCESORY_COUNT];
    if (slot_index < 0 || slot_index >= max_slots) return;

    if (player->gear.accessory_ids[slot_index] != -1) {
        GiveItem(player, player->gear.accessory_ids[slot_index]);
        player->gear.accessory_ids[slot_index] = -1;
    }

    RecalculateStats(&player->stats, &player->gear);
}
