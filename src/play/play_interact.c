#include "play/play_interact.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "environment/map.h"
#include "raylib.h"
#include "raymath.h"
#include "registry/register.h"
#include "systems/script_manager.h"
#include <stdbool.h>

void InitDialog(Map *map, ScriptManager *manager) {
    // get characterid
    // set dialg
    int characterIndex = PollTrait(map, TRAIT_TALK, 50.0f);
    if (characterIndex > -1) {
        MapEntity *p = &map->entities[characterIndex];
        int dialogID = GetDialogID(ENTITY_CHARACTER, p->entity_id);
        SetActiveMessage(manager, dialogID);
        manager->active = true;
    }
}

void CheckAndCollectMinerals(Map *map) {
    float collectionRadius =
        30.0f + ((float)GLOBAL_PLAYER.stats.current[STAT_GEOLOGY] * 0.5f);
    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *entity = &map->entities[i];
        // Check if it's a mineral (or has your collection trait/type)
        if (entity->type == ENTITY_MINERAL && !entity->isCollecting) {
            float dist =
                Vector2Distance(map->player.position, entity->position);
            if (dist < collectionRadius) {
                entity->isCollecting = true; // Flag ALL minerals in range
            }
        }
    }
}

char *GatherEntity(Player *player, Map *map) {
    int index = PollTrait(map, TRAIT_GATHER, 50.0f);

    if (index > -1) {
        if (map->entities[index].type == ENTITY_ITEM) {
            MapEntity *p = &map->entities[index];
            int item_id = p->entity_id;
            GiveItem(player, item_id);
            RemoveEntityAt(map, index);
            return GetName(ENTITY_ITEM, item_id);
            ;
        } else if (map->entities[index].type == ENTITY_PLANT) {
            MapEntity *p = &map->entities[index];
            Plant *pl = &PLANT_REGISTRY[p->entity_id];
            int plant_id = p->entity_id;
            if (PLAYER->stats.current[STAT_BOTANY] >= pl->gather_level) {
                PLAYER->plant_inventory[p->entity_id]++;
                RemoveEntityAt(map, index);
                return GetName(ENTITY_PLANT, plant_id);
            }
        }
    }
    return NULL;
}

void CheckHazards(Map *map, float dt) {
    // Tick down damage cooldown using whatever player reference or timer you use
    if (PLAYER->damage_cooldown > 0.0f) {
        PLAYER->damage_cooldown -= dt;
        return;
    }

    int painPoint = PollTrait(map, TRAIT_DAMAGE, 30.0f);
    if (painPoint > -1) {
        Plant *p = &PLANT_REGISTRY[map->entities[painPoint].entity_id];
        int player_botany = PLAYER->stats.current[STAT_BOTANY];

        // Negative damage amount = healing (like upcoming Mint)
        if (p->damage_amount < 0) {
            int heal_amount = -p->damage_amount;
            PLAYER->stats.current_hp = (PLAYER->stats.current_hp + heal_amount > PLAYER->stats.max_hp)
                                       ? PLAYER->stats.max_hp
                                       : PLAYER->stats.current_hp + heal_amount;

            PLAYER->damage_cooldown = 1.0f;
            printf("Stepped on %s and recovered %d HP!\n", p->species_name, heal_amount);
            return;
        }

        // Check if player's botany bypasses the damage/knockback
        if (player_botany < p->damage_waive_level) {
            PLAYER->stats.current_hp -= p->damage_amount;
            PLAYER->damage_cooldown = 0.5f;

            // Apply Knockback
            Vector2 plantCenter = GetEntityCenter(&map->entities[painPoint]);
            Vector2 playerCenter = GetEntityCenter(&map->player);
            Vector2 pushDir = Vector2Normalize(Vector2Subtract(playerCenter, plantCenter));

            if (Vector2LengthSqr(pushDir) == 0.0f) {
                pushDir = (Vector2){0.0f, 1.0f};
            }

            map->player.position = Vector2Add(map->player.position, Vector2Scale(pushDir, 40.0f));
        }
    }
}
