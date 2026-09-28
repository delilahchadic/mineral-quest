#include "play/play_session.h"
#include "core/camera_tools.h"
#include "defs/types_engine.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "defs/types_minerals.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"
#include "environment/map.h"
#include "environment/map_loader.h"
#include "play/play_combat.h"
#include "play/play_interact.h"
#include "play/play_inventory.h"
#include "play/play_ui.h"
#include "raylib.h"
#include "raymath.h"
#include "registry/mineral_register.h"
#include "registry/register.h"
#include "registry/tarot_register.h"
#include "systems/input.h"
#include "systems/physics.h"
#include "systems/player.h"
#include "systems/script_manager.h"
#include "systems/targeting.h"
#include "ui/dialog_box.h"
#include "ui/equip_menu.h"
#include "ui/menu.h"
#include "ui/node_menu.h"
#include "ui/plant_inventory.h"
#include "ui/stats_menu.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

void InitPlaySession(Gamestate *gamestate) {
    PlaySession *session = &gamestate->session;
    session->state = ADVENTURE_STATE;
    session->player = PLAYER;
    session->menu = (Menu){0};
    int startPortalId = 12; // romantic treasure room
    LoadMap(GetWorldName(GetWorldIDFromPortal(startPortalId)), &gamestate->map);
    InitMap(&gamestate->map);
    InitScriptManager(&session->manager, 100);
    gamestate->map.player.position =
        GetDestination(ENTITY_PORTAL, startPortalId);

    Vector2 player_center = GetEntityCenter(&gamestate->map.player);

    CenterCameraOn(&gamestate->camera, player_center, 3.0f, &gamestate->map);
    int tx = (int)(player_center.x / TILE_SIZE);
    int ty = (int)(player_center.y / TILE_SIZE);
    // Set the altitude to the floor height immediately
    //
    float startFloor = gamestate->map.grid[ty][tx].height * 8.0f;
    gamestate->map.player.altitude = startFloor;

    session->equip_menu.activeSlot = 0;
    session->equip_menu.activeItemSlot = 0;
    session->equip_menu.mode = SLOT_NONE;
}

void ChangeMap(Gamestate *gamestate, char *map_name, Vector2 destination) {
    LoadMap(map_name, &gamestate->map);
    InitMap(&gamestate->map);
    gamestate->map.player.position = destination;
}

void UpdateTalking(Gamestate *gamestate, Input *input, float dt) {
    PlaySession *session = &gamestate->session;
    UpdateScriptManager(&session->manager, input);
    session->state = session->manager.active ? DIALOG_PROMPT : ADVENTURE_STATE;
    AdjustCamera(gamestate, true, dt);
}

void UpdateAdventure(Gamestate *gamestate, Input *input, float dt) {
    PlaySession *session = &gamestate->session;
    // Refresh nearby targets every frame
    UpdatePlayerTargets(&gamestate->map, &gamestate->session.player->targeting);
    // int worrld
    if (PLAYER->stats.current_hp <= 0) {
        session->state = GAME_OVER;
        return;
    }
    if (input->buttons_pressed & BACKSPACE_PRESSED) {
        session->state = GAME_OVER;
        return;
    }
    if (input->attack_dir.x != 0.0f || input->attack_dir.y != 0.0f) {
        ExecuteDirectionalAttack(&gamestate->map,input);

    }

    if (input->buttons_pressed & KEY_N_PRESSED) {
        RebindItemMenu(&session->menu, session->player);
        session->state = INVENTORY_MENU;
        return;
    }

    if (input->buttons_pressed & KEY_M_PRESSED) {
        session->state = MINERAL_INVENTORY;
        return;
    }

    if (input->buttons_pressed & KEY_B_PRESSED) {
        RebindPlantMenu(&session->menu, session->player);
        session->state = PLANT_INVENTORY;
        return;
    }

    if (input->buttons_pressed & SHIFT_PRESSED) {
        session->state = STATS_MENU;
        return;
    }

    if (input->buttons_pressed & KEY_U_PRESSED) {
        PLAYER->targeting.locked = !PLAYER->targeting.locked;
        return;
    }
    if (input->buttons_pressed & KEY_Z_PRESSED) {
        UsePlayerTarotSlot(session->player, gamestate, 0);
    }
    if (input->buttons_pressed & KEY_X_PRESSED) {
        UsePlayerTarotSlot(session->player, gamestate, 1);
    }
    if (input->buttons_pressed & KEY_C_PRESSED) {
        UsePlayerTarotSlot(session->player, gamestate, 2);
    }
    if (input->buttons_pressed & KEY_O_PRESSED) {
        if (PLAYER->targeting.locked) {
            CycleTarget(&PLAYER->targeting);
            return;
        }
    }
    if (input->buttons_pressed & KEY_P_PRESSED) {
        ExecuteTargetedAttack(&gamestate->map, gamestate->session.player);
    }

    if (input->buttons_pressed & KEY_G_PRESSED) {
        session->state = EQUIPMENT_MENU;
    }

    if (input->buttons_pressed & KEY_E_PRESSED) {
        HandleInteract(gamestate);
    }

    if (gamestate->map.hitstop_timer > 0.0f) {
        // If we are in hitstop, count down the timer but SKIP updating the
        // world
        gamestate->map.hitstop_timer -= dt;
    } else {
        UpdatePlayerCombatAnimation(&gamestate->map.player, dt);
        // Update facing direction based on movement
        if (input->buttons_pressed &
            (KEY_W_PRESSED | KEY_S_PRESSED | MOVEMENT_PRESSED)) {
            if (input->dir.x != 0.0f || input->dir.y != 0.0f) {
                gamestate->map.player.combat.facing_direction =
                    atan2f(input->dir.y, input->dir.x);
            }
        }
        CheckHazards(&gamestate->map, dt);
        UpdatePhysics(&gamestate->map, input);
        UpdateBuffs(session->player, dt);
        UpdateCombat(&gamestate->map);
        CheckAndCollectMinerals(&gamestate->map);
        UpdateEntityMovement(&gamestate->map, dt);
    }
    AdjustCamera(gamestate, false, dt);

    int portal_index = PollTrait(&gamestate->map, TRAIT_TELEPORT, 32.0f);
    if (portal_index > -1) {

        MapEntity *entity = &gamestate->map.entities[portal_index];
        if (GetPortalType(entity->entity_id) == PORTAL_CRYSTAL) {
            PLAYER->stats.current_hp = PLAYER->stats.max_hp;
        }
        ChangeMap(gamestate, GetWorldNameFromPortalId(entity->entity_id),
                  GetDestination(ENTITY_PORTAL, entity->entity_id));
    }
}

void UpdateItemPopup(PlaySession *session, Input *input) {
    if (input->buttons_pressed & KEY_E_PRESSED) {
        session->state = ADVENTURE_STATE;
    }
}

void UpdatePlaySession(Gamestate *gamestate) {
    PlaySession *session = &gamestate->session;
    Input input = CaptureInput();
    float dt = GetFrameTime();
    if (dt > 0.1f)
        dt = 0.1f;
    switch (session->state) {
    case ADVENTURE_STATE:
        UpdateAdventure(gamestate, &input, dt);
        break;
    case INVENTORY_MENU:
        UpdateInventory(session, &input);
        break;
    case MINERAL_INVENTORY:
        UpdateMineralInventory(session, &input);
        break;
    case DIALOG_PROMPT:
        UpdateTalking(gamestate, &input, dt);
        break;
    case GATHER_PROMPT:
        UpdateItemPopup(session, &input);
        break;
    case STATS_MENU:
        UpdateStatsMenu(session, &input);
        break;
    case EQUIPMENT_MENU:
        UpdateEquipMenu(session, &input);
        break;
    case NODE_MENU:
        UpdateNodeMenu(session, &input);
        break;
    case PLANT_INVENTORY:
        UpdatePlantInventory(session, &input);
    default:
        return;
    }
}
