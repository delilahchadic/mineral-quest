#include "systems/player.h"
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "registry/register.h"
#include <string.h>

void InitEquipmentSet(EquipmentSet* gear) {
    gear->weapon_id = 5;// equip the iron sword
    for (int i = 0; i < MAX_ACCESSORY_SLOTS; i++) {
        gear->accessory_ids[i] = -1;
    }
}

void SetDefaultStat(Player* player){
    player->stats.base[STAT_STR] = 8;
    player->stats.base[STAT_DEF] = 7;
    player->stats.base[STAT_MAG_OFF] = 5;
    player->stats.base[STAT_MAG_DEF] = 8;
    player->stats.base[STAT_SPEED] = 9;
    player->stats.base[STAT_GEOLOGY] = 10;
    player->stats.base[STAT_ALCHEMY] = 2;
    player->stats.base[STAT_BOTANY] = 0;
    player->stats.base[STAT_AEROBICS] = 11;
    player->stats.base[STAT_ACCESORY_COUNT] = 2;
}

void RecalculateStats(StatBlock* stats, EquipmentSet* gear) {
    // 1. Reset current stats to base stats
    for (int i = 0; i < STAT_COUNT; i++) {
        stats->current[i] = stats->base[i];
    }

    // 2. Add Weapon Stats
    if (gear->weapon_id != -1) {
        ItemDefinition* weapon = &ITEM_REGISTRY[gear->weapon_id];
        for (int s = 0; s < STAT_COUNT; s++) {
            stats->current[s] += weapon->stat_bonuses[s];
        }
    }

    // 3. Add Accessory Stats (Only up to currently unlocked slots)
    int allowed_slots = stats->current[STAT_ACCESORY_COUNT];
    if (allowed_slots > MAX_ACCESSORY_SLOTS) allowed_slots = MAX_ACCESSORY_SLOTS;

    for (int i = 0; i < allowed_slots; i++) {
        int acc_id = gear->accessory_ids[i];
        if (acc_id != -1) {
            ItemDefinition* acc = &ITEM_REGISTRY[acc_id];
            for (int s = 0; s < STAT_COUNT; s++) {
                stats->current[s] += acc->stat_bonuses[s];
            }
        }
    }
}

Player Get_Default_Player(){
  Player player = {0};
  InitInventory(&player.inventory);
  player.inventory.itemIds[0]=9;
  player.inventory.itemIds[1]=3;
  player.inventory.itemIds[2]=4;
  player.inventory.count = 3;
  player.speed = 250.0f;
  player.sprite = LoadTexture("data/sprites/sprite.png");
  SetDefaultStat(&player);
  InitEquipmentSet(&player.gear);
  RecalculateStats(&player.stats, &player.gear);
  return player;
}


void Close_Player(Player* player){
  UnloadTexture(player->sprite);
}

void GiveItem(Player* player,int id){
  if(id < 0){
    return;
  }

  int currItem = player->inventory.count;
  player->inventory.itemIds[currItem++] = id;
  player->inventory.count = currItem;
}

void RemoveItemAt(Player* player, int index) {
    if (index < 0 || index >= player->inventory.count) {
        return; // Out of bounds check
    }

    // Calculate how many elements need to be shifted down
    int elements_to_move = player->inventory.count - index - 1;

    if (elements_to_move > 0) {
        // memmove safely handles overlapping memory blocks in the same array
        memmove(
            &player->inventory.itemIds[index],     // Destination
            &player->inventory.itemIds[index + 1], // Source
            elements_to_move * sizeof(int)         // Number of bytes to copy
        );
    }

    player->inventory.count--;
}
