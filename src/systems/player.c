#include "systems/player.h"
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "raylib.h"
#include "registry/register.h"
#include <string.h>

void DamagePlayer(int damage) {
    int damageDealt = damage > GLOBAL_PLAYER.stats.current_hp ? GLOBAL_PLAYER.stats.current_hp : damage;
    GLOBAL_PLAYER.stats.current_hp -= damageDealt;
}

void InitEquipmentSet(EquipmentSet* gear) {
    gear->weapon_id = 12; // equip the classical guitar
    for (int i = 0; i < MAX_ACCESSORY_SLOTS; i++) {
        gear->accessory_ids[i] = -1;
    }
}

int CanAfford(Player* player, CostSlot slot) {
    switch (slot.type) {
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

void ProcessRecipe(Player* player, Exchange e) {
    for (int i = 0; i < 3; i++) {
        switch (e.cost_slots[i].type) {
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

void SetDefaultStat(Player* player) {
    player->stats.max_hp = 100;
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

    // 2. Add Weapon Stats
    if (gear->weapon_id != -1) {
        ItemDefinition* weapon = &ITEM_REGISTRY[gear->weapon_id];
        stats->max_hp += weapon->hp_bonus;
        for (int s = 0; s < STAT_COUNT; s++) {
            stats->current[s] += weapon->stat_bonuses[s];
        }
    }

    // 3. Add Accessory Stats dynamically
    for (int i = 0; i < MAX_ACCESSORY_SLOTS && i < stats->current[STAT_ACCESORY_COUNT]; i++) {
        int acc_id = gear->accessory_ids[i];
        if (acc_id != -1) {
            ItemDefinition* acc = &ITEM_REGISTRY[acc_id];
            stats->max_hp += acc->hp_bonus;
            for (int s = 0; s < STAT_COUNT; s++) {
                stats->current[s] += acc->stat_bonuses[s];
            }
        }
    }

    // 4. Aggregate all active multivariate buffs
    for (int i = 0; i < MAX_ACTIVE_BUFFS; i++) {
        if (!stats->buffs[i].active) continue;
        stats->max_hp += stats->buffs[i].hpBonus;
        for (int s = 0; s < STAT_COUNT; s++) {
            stats->current[s] += stats->buffs[i].modifiers[s];
        }
    }

    // 5. Cap HP to max_hp after all gear and buffs are aggregated
    if (stats->current_hp > stats->max_hp) {
        stats->current_hp = stats->max_hp;
    }
}

Player Get_Default_Player() {
    Player player = {0};
    player.item_inventory[9]++;
    player.item_inventory[3]++;
    player.item_inventory[4]++;
    player.item_inventory[13]++;
    player.speed = 250.0f;
    player.sprite = LoadTexture("data/sprites/sprite.png");
    SetDefaultStat(&player);
    InitEquipmentSet(&player.gear);
    RecalculateStats(&player.stats, &player.gear);
    return player;
}

void ClosePlayer(Player* player) {
    UnloadTexture(player->sprite);
}

void GiveItem(Player* player, int id) {
    if (id < 0) return;
    player->item_inventory[id]++;
}

void RemoveOneFromInventory(Player* player, int id) {
    if (id < 0) return;
    if (player->item_inventory[id] > 0) {
        player->item_inventory[id]--;
    }
}

void ApplyPermanentStat(Player* player, ItemDefinition* item) {
    player->stats.max_hp += item->hp_bonus;
    for (int s = 0; s < STAT_COUNT; s++) {
        player->stats.base[s] += item->stat_bonuses[s];
    }
}

bool AddActiveBuff(Player* player, ItemDefinition* item) {
    if (!player || !item) return false;

    StatBlock* stats = &player->stats;
    int empty_index = -1;

    // 1. Search for an existing buff from the same item source to refresh
    for (int i = 0; i < MAX_ACTIVE_BUFFS; i++) {
        if (stats->buffs[i].active && stats->buffs[i].id == item->id) {
            stats->buffs[i].duration = item->use_duration;
            stats->buffs[i].hpBonus = item->hp_bonus; // Replace, don't stack infinitely

            for (int s = 0; s < STAT_COUNT; s++) {
                stats->buffs[i].modifiers[s] = item->stat_bonuses[s];
            }

            RecalculateStats(stats, &player->gear);
            return true;
        }

        if (!stats->buffs[i].active && empty_index == -1) {
            empty_index = i;
        }
    }

    // 2. Populate a new slot
    if (empty_index != -1) {
        ActiveBuff* buff = &stats->buffs[empty_index];
        buff->id = item->id;
        buff->duration = item->use_duration;
        buff->hpBonus = item->hp_bonus;
        buff->active = true;

        for (int s = 0; s < STAT_COUNT; s++) {
            buff->modifiers[s] = item->stat_bonuses[s];
        }

        RecalculateStats(stats, &player->gear);
        return true;
    }

    return false;
}

void UseItem(Player* player, ItemDefinition* item) {
    if (item->type != ITEM_CONSUME || item->use_type == USE_NONE) return;
    bool used = false;

    switch (item->use_type) {
        case USE_RESTORE_HP:
            if (player->stats.current_hp < player->stats.max_hp) {
                player->stats.current_hp += item->hp_bonus;
                if (player->stats.current_hp > player->stats.max_hp) {
                    player->stats.current_hp = player->stats.max_hp;
                }
                used = true;
            }
            break;

        case USE_PERM_BOOST:
            ApplyPermanentStat(player, item);
            RecalculateStats(&player->stats, &player->gear);
            used = true;
            break;

        case USE_TEMP_BUFF:
            used = AddActiveBuff(player, item);
            break;

        default:
            return;
    }

    if (used) {
        RemoveOneFromInventory(player, item->id);
    }
}

void UpdateBuffs(Player* player, float dt) {
    if (!player) return;

    StatBlock* stats = &player->stats;
    bool needs_recalc = false;

    for (int i = 0; i < MAX_ACTIVE_BUFFS; i++) {
        if (!stats->buffs[i].active) continue;

        stats->buffs[i].duration -= dt;

        // Check if the buff has expired
        if (stats->buffs[i].duration <= 0.0f) {
            stats->buffs[i].duration = 0.0f;
            stats->buffs[i].hpBonus = 0;
            stats->buffs[i].active = false;

            // Clear modifier memory
            for (int s = 0; s < STAT_COUNT; s++) {
                stats->buffs[i].modifiers[s] = 0;
            }

            needs_recalc = true;
        }
    }

    // Only recalculate stats if an active buff actually ran out this frame
    if (needs_recalc) {
        RecalculateStats(stats, &player->gear);
    }
}
