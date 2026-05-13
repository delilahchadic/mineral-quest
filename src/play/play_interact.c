#include "play/play_interact.h"
#include "raymath.h"
#include "environment/map.h"
#include "registry/register.h"
#include "systems/script_manager.h"

bool CheckMineralPickup(Vector2 playerPos, Vector2 mineralPos, float radius) {
    // We "stretch" the Y distance to turn a circle check into an oval check
    float dx = playerPos.x - mineralPos.x;
    float dy = (playerPos.y - mineralPos.y) * 2.0f; // 2.0 matches your 2:1 isometric ratio

    return (dx * dx + dy * dy) <= (radius * radius);
}

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

void CheckForMineralCollision(PlaySession* session){
    MapEntity* e = session->map.entities;
    while(e!=NULL){
        if (e->type == ENTITY_MINERAL) {
            // Ensure we are comparing WORLD coordinates to WORLD coordinates
            // If e->position is stored as Iso, you'd need GetIsoToWorld(e->position)
            Vector2 playerPos = session->map.player->position;
            Vector2 mineralPos = e->position;

            // 15.0f to 20.0f is usually the "sweet spot" for 32px tiles
            if (CheckMineralPickup(playerPos, mineralPos, 30.0f)) {
                float pitch = 0.95f + ((float)(e->id % 10)/ 100.0f); // 0.95 to 1.05
                    SetSoundPitch(session->mineral_sound, pitch);

                    PlaySound(session->mineral_sound);
                session->player.mineral_inventory[e->id]++;
                Remove_Entity(&session->map, e);
                break;
            }
        }
        e = e->next;
    }
}

int PollChest(Player* player,Map* map){
  // get characterid
  // set dialg
  MapEntity* p = PollTrait(map, TRAIT_GATHER, 50.0f);
  if(p && p->type == ENTITY_ITEM){

    GiveItem(player, p->id);
    Remove_Entity(map, p);
    return p->id;
  }
  return -1;
}
