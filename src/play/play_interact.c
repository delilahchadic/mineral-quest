#include "play/play_interact.h"
#include "defs/types_entities.h"
#include "raymath.h"
#include "environment/map.h"
#include "registry/register.h"
#include "systems/script_manager.h"
#include <stdbool.h>

void InitDialog(Map* map, ScriptManager* manager){
  // get characterid
  // set dialg
  MapEntity* p = PollTrait(map, TRAIT_TALK, 50.0f);
  if(p){
    int dialogID = GetDialogID(ENTITY_CHARACTER, p->id);
    SetActiveMessage(manager, dialogID);
    manager->active = true;
  }
}

void CheckAndCollectMinerals(Map* map) {
    float collectionRadius = 30.0f + ((float) GLOBAL_PLAYER.stats.current[STAT_GEOLOGY] * 0.5f);
    MapEntity* entity = map->entities;
    while (entity != NULL) {
        // Check if it's a mineral (or has your collection trait/type)
        if (entity->type == ENTITY_MINERAL && !entity->isCollecting) {
            float dist = Vector2Distance(map->player->position, entity->position);
            if (dist < collectionRadius) {
                entity->isCollecting = true; // Flag ALL minerals in range
            }
        }
        entity = entity->next;
    }
}

int PollChest(Player* player,Map* map){
  // get characterid
  // set dialg
  MapEntity* p = PollTrait(map, TRAIT_GATHER, 50.0f);

  if(p && p->type == ENTITY_ITEM){
    int item_id = p->id;
    GiveItem(player, item_id);
    Remove_Entity(map, p);
    return item_id;
  }
  return -1;
}
