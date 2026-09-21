#include "play/play_interact.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "raylib.h"
#include "raymath.h"
#include "environment/map.h"
#include "registry/register.h"
#include "systems/script_manager.h"
#include <stdbool.h>

void InitDialog(Map* map, ScriptManager* manager){
  // get characterid
  // set dialg
  int characterIndex = PollTrait(map, TRAIT_TALK, 50.0f);
  if(characterIndex >-1){
    MapEntity* p = &map->entities[characterIndex];
    int dialogID = GetDialogID(ENTITY_CHARACTER, p->entity_id);
    SetActiveMessage(manager, dialogID);
    manager->active = true;
  }
}

void CheckAndCollectMinerals(Map* map) {
    float collectionRadius = 30.0f + ((float) GLOBAL_PLAYER.stats.current[STAT_GEOLOGY] * 0.5f);
    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *entity = &map->entities[i];
        // Check if it's a mineral (or has your collection trait/type)
        if (entity->type == ENTITY_MINERAL && !entity->isCollecting) {
            float dist = Vector2Distance(map->player.position, entity->position);
            if (dist < collectionRadius) {
                entity->isCollecting = true; // Flag ALL minerals in range
            }
        }
    }
}

int PollChest(Player* player,Map* map){
  // get characterid
  // set dialg
  int index = PollTrait(map, TRAIT_GATHER, 50.0f);

  if(index>-1){
    MapEntity* p = &map->entities[index];
    int item_id = p->entity_id;
    GiveItem(player, item_id);
    RemoveEntityAt(map, index);
    return item_id;
  }
  return -1;
}
