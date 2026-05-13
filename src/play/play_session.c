#include "play/play_session.h"

#include <stdio.h>
#include "raylib.h"
#include "core/camera_tools.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "defs/types_minerals.h"
#include "defs/types_systems.h"
#include "environment/map.h"
#include "environment/map_loader.h"
#include "play/play_interact.h"
#include "play/play_inventory.h"
#include "play/play_combat.h"
#include "play/play_ui.h"
#include "registry/mineral_register.h"
#include "systems/script_manager.h"
#include "systems/input.h"
#include "systems/player.h"
#include "systems/physics.h"
#include "ui/menu.h"

void InitPlaySession(PlaySession* session){
    session->state = ADVENTURE;
    session->player = Get_Default_Player();
    session->menu = (Menu){0};
    LoadMap("rivers",&session->map);
    InitMap(&session->map);
    InitScriptManager(&session->manager,100);

    CenterCameraOn(&session->camera,session->map.player->position,3.0f, &session->map);
    int tx = (int)(session->map.player->position.x / TILE_SIZE);
    int ty = (int)(session->map.player->position.y / TILE_SIZE);
    // Set the altitude to the floor height immediately
    //
    float startFloor = session->map.grid[ty][tx].height * 8.0f;
    session->map.player->altitude = startFloor;
    session->mineral_sound = LoadSound("data/audio/mineral.wav");
    SetSoundVolume(session->mineral_sound, 0.33);
}

void UpdateTalking(PlaySession* session, Input* input, float dt){
    UpdateScriptManager(&session->manager, input);
    session->state = session->manager.active ? TALKING : ADVENTURE;
    AdjustCamera(session,true,dt);
}

void UpdateAdventure(PlaySession* session, Input* input, float dt){
    if(input->buttons_pressed & MINERAL_PRESSED){
        session->state = MINERAL_INVENTORY;
        return;
    }
    if(input->buttons_pressed & INVENTORY_PRESSED){
        session->menu.type = ENTITY_ITEM;
        FillMenu(&session->menu, &session->player.inventory.itemIds, session->player.inventory.count);
        session->state = INVENTORY;
        return;
    }
    if(input->buttons_pressed & INTERACT_PRESSED){
        int item = PollChest(&session->player,&session->map);
        if(item > 0){
            session->state = ITEM;
            sprintf(session->pendingItemName,"You got a %s !", GetName(ENTITY_ITEM, item));
        }else{
            InitDialog(&session->map, &session->manager);
            if(session->manager.active) session->state = TALKING;
            else{
                session->map.player->combat.isAttacking = true;
                session->map.player->combat.attackTimer = 0;
                ResetAllHitFlags(&session->map);
            }
        }
    }

    UpdatePlayerCombatAnimation(session->map.player, dt);
    UpdatePhysics(&session->map, input);
    UpdateCombat(&session->map);
    CheckForMineralCollision(session);
    AdjustCamera(session, false,dt);
}

void UpdateItemPopup(PlaySession* session, Input* input){
    if(input->buttons_pressed & INTERACT_PRESSED){
      session->state= ADVENTURE;
    }
}

void UpdatePlaySession(PlaySession* session){
  Input input = CaptureInput();
  float dt = GetFrameTime();
  if (dt > 0.1f) dt = 0.1f;
  switch(session->state){
    case ADVENTURE: UpdateAdventure(session,&input, dt);break;
    case INVENTORY: UpdateInventory(session, &input);break;
    case MINERAL_INVENTORY: UpdateMineralInventory(session, &input);break;
    case TALKING: UpdateTalking(session, &input,dt);break;
    case ITEM: UpdateItemPopup(session, &input);break;
  }
}
