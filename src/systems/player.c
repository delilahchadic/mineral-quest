#include "systems/player.h"
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "registry/register.h"
#include <string.h>

void DamagePlayer(int damage){
    int damageDealt = damage > GLOBAL_PLAYER.stats.current_hp ? GLOBAL_PLAYER.stats.current_hp : damage;
    GLOBAL_PLAYER.stats.current_hp -= damageDealt;
}
void InitEquipmentSet(EquipmentSet* gear) {
    gear->weapon_id = 12;// equip the classical guitar
    for (int i = 0; i < MAX_ACCESSORY_SLOTS; i++) {
        gear->accessory_ids[i] = -1;
    }
}

int CanAfford(Player* player, CostSlot slot){
    switch(slot.type){
        case ENTITY_ITEM:
            return player->item_inventory[slot.id] >= slot.amount;
        case ENTITY_PLANT:
            return player->plant_inventory[slot.id] >= slot.amount;
        case ENTITY_MINERAL:
            return player->mineral_inventory[slot.id] >= slot.amount;
        case ENTITY_NONE:
            return 1;
        default:
            return 0;
    }
}

// assumes each cost slot has been evaluated
void ProcessRecipe(Player* player, Exchange e){
    for(int i=0;i<3;i++){
        switch(e.cost_slots[i].type){
            case ENTITY_ITEM:
                player->item_inventory[e.cost_slots[i].id] -= e.cost_slots[i].amount;
                break;
            case ENTITY_PLANT:
                player->plant_inventory[e.cost_slots[i].id] -= e.cost_slots[i].amount;
                break;
            case ENTITY_MINERAL:
                player->mineral_inventory[e.cost_slots[i].id] -= e.cost_slots[i].amount;
                break;
            default:
                break;
        }
    }

    player->item_inventory[e.item_id]++;
}
void SetDefaultStat(Player* player){
    player->stats.max_hp =100;
    player->stats.current_hp = 100;
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
    // 1. Reset base stats and baseline max HP
    stats->max_hp = 100;
    for (int i = 0; i < STAT_COUNT; i++) {
        stats->current[i] = stats->base[i];
    }

    // 2. Add Weapon Stats (Applied ONCE, outside the stat loop)
    if (gear->weapon_id != -1) {
        ItemDefinition* weapon = &ITEM_REGISTRY[gear->weapon_id];
        stats->max_hp += weapon->hp_bonus;
        for (int s = 0; s < STAT_COUNT; s++) {
            stats->current[s] += weapon->stat_bonuses[s];
        }
    }

    // 3. Add Accessory Stats (Only up to unlocked slots)
    int allowed_slots = stats->current[STAT_ACCESORY_COUNT];
    if (allowed_slots > MAX_ACCESSORY_SLOTS) allowed_slots = MAX_ACCESSORY_SLOTS;

    for (int i = 0; i < allowed_slots; i++) {
        int acc_id = gear->accessory_ids[i];
        if (acc_id != -1) {
            ItemDefinition* acc = &ITEM_REGISTRY[acc_id];
            stats->max_hp += acc->hp_bonus; // Applied ONCE per accessory
            for (int s = 0; s < STAT_COUNT; s++) {
                stats->current[s] += acc->stat_bonuses[s];
            }
        }
    }

    // 4. Ensure current HP doesn't exceed the newly calculated max HP
    if (stats->current_hp > stats->max_hp) {
        stats->current_hp = stats->max_hp;
    }
}

Player Get_Default_Player(){
  Player player = {0};
  // InitInventory(&player.inventory);
  player.item_inventory[9]++;
  player.item_inventory[3]++;
  player.item_inventory[4]++;
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
  player->item_inventory[id]++;
}

void RemoveItem(Player* player, int id) {
    if (id < 0) {
        return; // Out of bounds check
    }
    if(player->item_inventory[id] > 0){
        player->item_inventory[id]--;
    }
}
