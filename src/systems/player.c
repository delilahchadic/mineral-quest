#include "systems/player.h"
#include "defs/types_engine.h"
#include "defs/types_minerals.h"
#include "defs/types_systems.h"
#include "defs/types_tarot.h"
#include "raylib.h"
#include "registry/register.h"
#include "registry/tarot_register.h"
#include "systems/gear.h"
#include "systems/weapon_grid.h"
#include <stdbool.h>
#include <string.h>

void DamagePlayer(int damage) {
    int damageDealt = damage > GLOBAL_PLAYER.stats.current_hp
                          ? GLOBAL_PLAYER.stats.current_hp
                          : damage;
    GLOBAL_PLAYER.stats.current_hp -= damageDealt;
}

int CanAfford(Player *player, CostSlot slot) {
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

void ProcessRecipe(Player *player, Exchange e) {
    for (int i = 0; i < 3; i++) {
        switch (e.cost_slots[i].type) {
        case ENTITY_ITEM:
            player->item_inventory[e.cost_slots[i].id] -=
                e.cost_slots[i].amount;
            break;
        case ENTITY_PLANT:
            player->plant_inventory[e.cost_slots[i].id] -=
                e.cost_slots[i].amount;
            break;
        case ENTITY_MINERAL:
            player->mineral_inventory[e.cost_slots[i].id] -=
                e.cost_slots[i].amount;
            break;
        default:
            break;
        }
    }

    player->item_inventory[e.item_id]++;
}

void SetDefaultStat(Player *player) {
    player->stats.max_hp = 100;
    player->stats.max_base_hp = 100;
    player->stats.current_hp = 100;

    player->stats.max_mp = 75;
    player->stats.max_base_mp = 75;
    player->stats.current_mp = 75;

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

void RecalculateStats(Player *player) {
    StatBlock *stats = &player->stats;
    Gear *gear = &player->gear;
    WeaponGrid *grid = &player->weapon_grid;
    // 1. Reset base stats and baseline max HP/MP
    stats->max_hp = stats->max_base_hp;
    stats->max_mp = stats->max_base_mp;

    for (int i = 0; i < STAT_COUNT; i++) {
        stats->current[i] = stats->base[i];
    }

    // 2. Add Weapon Stats and Bonuses
    if (grid->activeIndex != -1) {
        WeaponSlot *slot = &grid->slots[grid->activeIndex];
        if (slot->active) {
            stats->max_hp += slot->hp_bonus;
            stats->max_mp += slot->mp_bonus;
            for (int s = 0; s < STAT_COUNT; s++) {
                stats->current[s] += slot->stat_bonuses[s];
            }
        }
    }

    // 3. Add Accessory Stats and Bonuses dynamically
    for (int i = 0;
         i < MAX_ACCESSORY_SLOTS && i < stats->current[STAT_ACCESORY_COUNT];
         i++) {
        int acc_id = gear->accessory_ids[i];
        if (acc_id != -1) {
            ItemDefinition *acc = &ITEM_REGISTRY[acc_id];
            stats->max_hp += acc->hp_bonus;
            stats->max_mp += acc->mp_bonus;
            for (int s = 0; s < STAT_COUNT; s++) {
                stats->current[s] += acc->stat_bonuses[s];
            }
        }
    }

    // 4. Aggregate all active multivariate buffs
    for (int i = 0; i < MAX_ACTIVE_BUFFS; i++) {
        if (!stats->buffs[i].active)
            continue;
        stats->max_hp += stats->buffs[i].hpBonus;
        stats->max_mp += stats->buffs[i].mpBonus;
        for (int s = 0; s < STAT_COUNT; s++) {
            stats->current[s] += stats->buffs[i].modifiers[s];
        }
    }

    // 5. Cap HP and MP to max limits after all gear and buffs are aggregated
    if (stats->current_hp > stats->max_hp) {
        stats->current_hp = stats->max_hp;
    }
    if (stats->current_mp > stats->max_mp) {
        stats->current_mp = stats->max_mp;
    }
}

Player Get_Default_Player() {
    Player player = {0};
    player.item_inventory[9]++;
    player.item_inventory[3]++;
    player.item_inventory[4]++;
    player.item_inventory[13]++;
    player.item_inventory[17]++;
    player.item_inventory[18]++;
    player.speed = 250.0f;
    player.sprite = LoadTexture("data/sprites/sprite.png");
    player.targeting.target_id = -1;
    SetDefaultStat(&player);
    InitGear(&player.gear);
    InitWeaponGrid(&player.weapon_grid);
    RecalculateStats(&player);
    return player;
}

void ClosePlayer(Player *player) { UnloadTexture(player->sprite); }

void GiveItem(Player *player, int id) {
    if (id < 0)
        return;
    if (ITEM_REGISTRY[id].slot == 0) {
        AddWeapon(&player->weapon_grid, id);
        return;
    }
    player->item_inventory[id]++;
}

void RemoveOneFromInventory(Player *player, int id) {
    if (id < 0)
        return;
    if (player->item_inventory[id] > 0) {
        player->item_inventory[id]--;
    }
}

void ApplyPermanentStat(Player *player, ItemDefinition *item) {
    player->stats.max_base_hp += item->hp_bonus;
    player->stats.max_base_mp += item->mp_bonus;
    TraceLog(LOG_ERROR, "theres this : HP: %d, MP: %d", item->hp_bonus,
             item->mp_bonus);
    for (int s = 0; s < STAT_COUNT; s++) {
        player->stats.base[s] += item->stat_bonuses[s];
    }
}

bool AddActiveBuff(Player *player, ItemDefinition *item) {
    if (!player || !item)
        return false;

    StatBlock *stats = &player->stats;
    int empty_index = -1;

    // 1. Search for an existing buff from the same item source to refresh
    for (int i = 0; i < MAX_ACTIVE_BUFFS; i++) {
        if (stats->buffs[i].active && stats->buffs[i].id == item->id) {
            stats->buffs[i].duration = item->use_duration;
            stats->buffs[i].hpBonus = item->hp_bonus;
            stats->buffs[i].mpBonus = item->mp_bonus; // Update MP bonus

            for (int s = 0; s < STAT_COUNT; s++) {
                stats->buffs[i].modifiers[s] = item->stat_bonuses[s];
            }

            RecalculateStats(player);
            return true;
        }

        if (!stats->buffs[i].active && empty_index == -1) {
            empty_index = i;
        }
    }

    // 2. Populate a new slot
    if (empty_index != -1) {
        ActiveBuff *buff = &stats->buffs[empty_index];
        buff->id = item->id;
        buff->duration = item->use_duration;
        buff->hpBonus = item->hp_bonus;
        buff->mpBonus = item->mp_bonus; // Set MP bonus
        buff->active = true;

        for (int s = 0; s < STAT_COUNT; s++) {
            buff->modifiers[s] = item->stat_bonuses[s];
        }

        RecalculateStats(player);
        return true;
    }

    return false;
}

void UseItem(Player *player, ItemDefinition *item) {
    if (item->type != ITEM_CONSUME || item->use_type == USE_NONE)
        return;
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

        // Optional handler if you implement MP restoration items

    case USE_RESTORE_MP:
        if (player->stats.current_mp < player->stats.max_mp) {
            player->stats.current_mp += item->mp_bonus;
            if (player->stats.current_mp > player->stats.max_mp) {
                player->stats.current_mp = player->stats.max_mp;
            }
            used = true;
        }
        break;

    case USE_PERM_BOOST:
        ApplyPermanentStat(player, item);
        RecalculateStats(player);
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

void UpdateBuffs(Player *player, float dt) {
    if (!player)
        return;

    StatBlock *stats = &player->stats;
    bool needs_recalc = false;

    for (int i = 0; i < MAX_ACTIVE_BUFFS; i++) {
        if (!stats->buffs[i].active)
            continue;

        stats->buffs[i].duration -= dt;

        // Check if the buff has expired
        if (stats->buffs[i].duration <= 0.0f) {
            stats->buffs[i].duration = 0.0f;
            stats->buffs[i].hpBonus = 0;
            stats->buffs[i].mpBonus = 0; // Clear MP bonus on expiration
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
        RecalculateStats(player);
    }
}

void UsePlayerTarotSlot(Player *player, Gamestate *gamestate, int index) {
    if (index < 0 || index > 2)
        return;
    if (player->gear.tarot_ids[index] != -1) {
        TarotCard *t = GetTarotCardByItemId(PLAYER->gear.tarot_ids[index]);
        if (player->stats.current_mp >= t->use_cost) {
            player->stats.current_mp -= t->use_cost;
            ExecuteTarotCommand(t->id, gamestate);
        }
    }
    return;
}

void FullyRestPlayer(Player *player) {
    player->stats.current_hp = player->stats.max_hp;
    player->stats.current_mp = player->stats.max_mp;

    // Optional: If you want portals to also clear temporary status
    // debuffs/buffs ClearAllBuffs(player);
}
