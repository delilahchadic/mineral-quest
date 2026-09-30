#include "play/play_interact.h"
#include "defs/types_engine.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "environment/map.h"
#include "raylib.h"
#include "raymath.h"
#include "registry/register.h"
#include "systems/script_manager.h"
#include <stdbool.h>

void InitDialog(Player* player,Map *map, ScriptManager *manager) {
    // get characterid
    // set dialg
    int characterIndex = CheckTargetTrait(player, map, TRAIT_TALK);
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

char *GatherTarget(Player *player, Map *map){
    int target = player->targeting.target_id;
    if (target == -1) return NULL;
    int index = CheckTargetTrait(player, map, TRAIT_GATHER);
    if(index != -1){
        MapEntity* entity = &map->entities[index];
        if(entity->type ==  ENTITY_ITEM){
            int item_id = entity->entity_id;
            GiveItem(player, item_id);
            RemoveEntityAt(map, index);
            return GetName(ENTITY_ITEM, item_id);
        }else if (map->entities[index].type == ENTITY_PLANT){
            Plant *pl = &PLANT_REGISTRY[entity->entity_id];
            int plant_id = entity->entity_id;
            if (PLAYER->stats.current[STAT_BOTANY] >= pl->gather_level){
                PLAYER->plant_inventory[entity->entity_id]++;
                RemoveEntityAt(map, index);
                return GetName(ENTITY_PLANT, plant_id);
            }
        }else if(map->entities[index].type == ENTITY_RECIPE){
            Recipe* recipe = &RECIPE_REGISTRY[entity->entity_id];
            int recipe_id = recipe->id;
            player->recipe_inventory[recipe_id] = 1;
            RemoveEntityAt(map, index);
            return GetName(ENTITY_RECIPE, recipe_id);
        }else{
            return NULL;
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

void HandleInteract(Gamestate* gamestate){
    PlaySession *session = &gamestate->session;
    char *item = GatherTarget(session->player, &gamestate->map);
    if (item) {
        session->state = GATHER_PROMPT;
        sprintf(session->pendingItemName, "You got a %s !", item);
        return;
    }
    int node_index =
        CheckTargetTrait(session->player, &gamestate->map, TRAIT_NODE);
    if (node_index > -1) {
        MapEntity *nodecharacter = &gamestate->map.entities[node_index];
        for (int i = 0; i < gamestate->map.node_count; i++) {
            if (GetCharacterId(gamestate->map.active_nodes[i]) ==
                nodecharacter->entity_id) {
                session->node_menu.node_id = gamestate->map.active_nodes[i];
                session->state = NODE_MENU;
                session->node_menu.selected_index = 0;
                session->node_menu.state = NODE_MENU_BROWSE;
            }
        }

    } else {
        InitDialog(session->player, &gamestate->map, &session->manager);
        if (session->manager.active)
            session->state = DIALOG_PROMPT;
    }
}
