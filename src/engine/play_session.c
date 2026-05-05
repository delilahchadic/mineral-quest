#include "engine/play_session.h"
#include "core/camera_tools.h"
#include "defs/types_engine.h"
#include "defs/types_minerals.h"
#include "defs/types_systems.h"
#include "environment/map.h"
#include "raylib.h"
#include "registry/mineral_register.h"

void InitPlaySession(PlaySession* session){
    session->state = ADVENTURE;
    session->player = Get_Default_Player();
    session->menu = (Menu){0};
    LoadMap("bx",&session->map);
    InitMap(&session->map);
    InitScriptManager(&session->manager,100);
    CenterCameraOn(&session->camera,session->map.player->position,3.0f, &session->map);
    int tx = (int)(session->map.player->position.x / TILE_SIZE);
    int ty = (int)(session->map.player->position.y / TILE_SIZE);
    // Set the altitude to the floor height immediately
    float startFloor = session->map.grid[ty][tx].height * 8.0f;
    session->map.player->altitude = startFloor;
    session->mineral_sound = LoadSound("data/audio/mineral.wav");
SetSoundVolume(session->mineral_sound, 0.33);
}

void UpdateMineralInventory(PlaySession* session, Input* input){
  if(input->buttons_pressed & MINERAL_PRESSED){
    session->state = ADVENTURE;
  }
}


void DrawMineralInventory(Player* player){
  // 1. Background - The Aged Paper
  ClearBackground(COLOR_PULP_PAPER);

  // Dynamic horizontal line based on screen width
  int margin = 50;
  int uiWidth = SCREEN_WIDTH - (margin * 2);

  // 2. Title - The "Ink" look
  DrawText("GEOLOGY LOG", margin, 40, 30, COLOR_SUNKEN_INK);
  DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK); // A simple line

  for(int i = 0;i< MINERAL_COUNT;i++){
      Color textColor = GetMineralColor(i);
      char count[5];
      sprintf(count, "%d", player->mineral_inventory[i]);
      DrawMineral(i, (Vector2){50, 120 + (i * 30) + 12});
      DrawText(GetMineralLabel(i), 100, 120 + (i * 30), 20, textColor);
      DrawText(count,250, 120 + (i * 30), 20, textColor);
  }
}

void UpdatePlaySession(PlaySession* session){
  Input input = CaptureInput();
  switch(session->state){
    case ADVENTURE:
    if(input.buttons_pressed & MINERAL_PRESSED){

      session->state = MINERAL_INVENTORY;
    }else if(input.buttons_pressed & INVENTORY_PRESSED){
          session->menu.type = ENTITY_ITEM;
          FillMenu(&session->menu, &session->player.inventory.itemIds, session->player.inventory.count);
          session->state = INVENTORY;
        } else if(input.buttons_pressed & INTERACT_PRESSED){
          int item = PollChest(&session->player,&session->map);
          if(item > 0){
            session->state = ITEM;
            sprintf(session->pendingItemName,"You got a %s !", GetName(ENTITY_ITEM, item));
          }else{
            InitDialog(&session->map, &session->manager);
            if(session->manager.active) session->state = TALKING;
          }
        }else{
            float dt = GetFrameTime();
                    if (dt > 0.1f) dt = 0.1f;

                    // 1. RUN PHYSICS FIRST
                    UpdatePhysics(&session->map, &input);

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


                    float max_w = (session->map.columns - 1) * TILE_SIZE;
                    float max_h = (session->map.rows - 1) * TILE_SIZE;


                    if (session->map.player->position.x < 0) session->map.player->position.x = 0;
                    if (session->map.player->position.y < 0) session->map.player->position.y = 0;
                    if (session->map.player->position.x > max_w) session->map.player->position.x = max_w;
                    if (session->map.player->position.y > max_h) session->map.player->position.y = max_h;
                    // 2. FIND THE EXACT POSITION OF THE PLAYER'S FEET
                    Vector2 playerIso = GetWorldToIso(session->map.player->position);

                    // The player is drawn at playerIso.y - altitude.
                    // We want the camera to center on the player's sprite, not the floor.
                    Vector2 targetPos = playerIso;
                    targetPos.y -= session->map.player->altitude; // ONLY subtract altitude

                    // 3. ONE SINGLE LERP
                    float camSpeed = 15.0f;
                    // float dt = GetFrameTime();
                    if (dt > 0.1f) dt = 0.1f;
                    float lerpFactor = 1.0f - expf(-camSpeed * dt);

                    session->camera.target.x += (targetPos.x - session->camera.target.x) * lerpFactor;
                    session->camera.target.y += (targetPos.y - session->camera.target.y) * lerpFactor;

        }
      AdjustCamera(session, false);
      break;
    case INVENTORY:
      UpdateInventory(session, &input);
      break;
    case MINERAL_INVENTORY:
        UpdateMineralInventory(session, &input);
        break;
    case TALKING:
      UpdateScriptManager(&session->manager, &input);
      session->state = session->manager.active ? TALKING : ADVENTURE;
      AdjustCamera(session,true);
      break;
    case ITEM:
      if(input.buttons_pressed & INTERACT_PRESSED){
        session->state= ADVENTURE;
      }
  }
}
  //if dialog manager is active input is disabled
void DrawPlaySession(PlaySession* session){
  if(session->state == INVENTORY){
    DrawInventory(&session->menu);
  }else if(session->state==MINERAL_INVENTORY){
    DrawMineralInventory(&session->player);
  }else {
    Draw_Map(&session->map,&session->camera);
    if(session->state == TALKING){
      DrawMessage(&session->manager);
    }else if(session->state == ITEM){
      DrawDialog("you found a christmas present", session->pendingItemName);
    }
  }
}


/// @brief manages input while game is in inventory mode
/// @param gamestate
void UpdateInventory(PlaySession* session, Input* input){
  if(!UpdateMenu(&session->menu, input)){
    session->state = ADVENTURE;
  }
}

void AdjustCamera(PlaySession* session, bool dialog){
  if(dialog){
    session->camera.zoom += (3.2f - session->camera.zoom) * 0.05f;
  }else{
    session->camera.zoom += (3.0f - session->camera.zoom) * 0.05f;
  }
}



void DrawInventory(Menu* menu) {
  DrawMenu(menu);
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

bool CheckMineralPickup(Vector2 playerPos, Vector2 mineralPos, float radius) {
    // We "stretch" the Y distance to turn a circle check into an oval check
    // float dx = playerPos.x - mineralPos.x;
    // float dy = (playerPos.y - mineralPos.y) * 2.0f; // 2.0 matches your 2:1 isometric ratio
    return Vector2Distance(playerPos, mineralPos) < radius;
    // return (dx * dx + dy * dy) <= (radius * radius);
}
