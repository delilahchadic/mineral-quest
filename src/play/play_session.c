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
#include "systems/input.h"
#include "systems/physics.h"
#include "systems/player.h"
#include "systems/script_manager.h"
#include "ui/dialog_box.h"
#include "ui/equip_menu.h"
#include "ui/menu.h"
#include "ui/node_menu.h"
#include "ui/stats_menu.h"
#include <math.h>
#include <stdbool.h>
#include "systems/targeting.h"
#include <stdio.h>
#include "registry/tarot_register.h"

void InitPlaySession(Gamestate *gamestate) {
    PlaySession *session = &gamestate->session;
    session->state = ADVENTURE;
    session->player = PLAYER;
    session->menu = (Menu){0};
    int startPortalId = 12; // romantic treasure room
    LoadMap(GetWorldName(GetWorldIDFromPortal(startPortalId)), &gamestate->map);
    InitMap(&gamestate->map);
    InitScriptManager(&session->manager, 100);
    gamestate->map.player.position =
        GetDestination(ENTITY_PORTAL, startPortalId);

    Vector2 player_center = GetEntityCenter(&gamestate->map.player);

    CenterCameraOn(&gamestate->camera, player_center, 3.0f,
                   &gamestate->map);
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
    session->state = session->manager.active ? TALKING : ADVENTURE;
    AdjustCamera(gamestate, true, dt);
}

void RebindItemMenu(PlaySession *session) {
    session->menu.type = ENTITY_ITEM;
    session->menu.exit_button = KEY_N_PRESSED;
    // Gather all non-zero item IDs from the frequency map into a temporary list
    // for the menu
    int active_item_ids[100];
    int active_count = 0;
    for (int i = 0; i < 100; i++) {
        if (session->player->item_inventory[i] > 0) {
            active_item_ids[active_count++] = i;
        }
    }

    FillMenu(&session->menu, active_item_ids, active_count);
    session->state = INVENTORY;
    return;
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
        // 1. Update facing direction immediately to match the IJKL input
        gamestate->map.player.combat.facing_direction =
            atan2f(input->attack_dir.y, input->attack_dir.x);

        // 2. Trigger the attack combo (InitCombat handles combo states 1, 2,
        // and 3)
        InitCombat(&gamestate->map);
    }

    if (input->buttons_pressed & KEY_N_PRESSED) {
        RebindItemMenu(session);
        return;
    }

    if (input->buttons_pressed & KEY_M_PRESSED) {
        session->state = MINERAL_INVENTORY;
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
    if(input->buttons_pressed & KEY_Z_PRESSED){
        if(PLAYER->gear.tarot_ids[0] != -1){
            TarotCard* t =GetTarotCardByItemId(PLAYER->gear.tarot_ids[0]);
            ExecuteTarotCommand(t->id, gamestate);
        }
        return;
    }
    if(input->buttons_pressed & KEY_X_PRESSED){
        if(PLAYER->gear.tarot_ids[1] != -1){
            TarotCard* t =GetTarotCardByItemId(PLAYER->gear.tarot_ids[1]);
            ExecuteTarotCommand(t->id, gamestate);
        }
        return;
    }
    if(input->buttons_pressed & KEY_C_PRESSED){
        if(PLAYER->gear.tarot_ids[2] != -1){
            TarotCard* t =GetTarotCardByItemId(PLAYER->gear.tarot_ids[2]);
            ExecuteTarotCommand(t->id, gamestate);
        }
        return;
    }
    if (input->buttons_pressed & KEY_O_PRESSED) {
        if (PLAYER->targeting.locked) {
            CycleTarget(&PLAYER->targeting);
            return;
        }
    }
    if (input->buttons_pressed & KEY_P_PRESSED) {
        // session->state = LEVEL_INVENTORY;
        if (PLAYER->targeting.target_id != -1) {
            MapEntity *target =
                GetTargetEntity(&gamestate->map, PLAYER->targeting.target_id);
            if (target) {
                // Point player straight at the target
                Vector2 diff = Vector2Subtract(GetEntityCenter(target),
                                               GetEntityCenter(&gamestate->map.player));
                gamestate->map.player.combat.facing_direction =
                    atan2f(diff.y, diff.x);

                // Trigger a combat swing/lunge immediately
                InitCombat(&gamestate->map);
            }
        }else{
            InitCombat(&gamestate->map);
        }
        return;
    }

    if (input->buttons_pressed & KEY_E_PRESSED) {
        int item = PollChest(session->player, &gamestate->map);
        if (item >= 0) {
            session->state = ITEM;
            sprintf(session->pendingItemName, "You got a %s !",
                    GetName(ENTITY_ITEM, item));
        } else {
            int node_index = PollTrait(&gamestate->map, TRAIT_NODE, 50.0f);
            if (node_index > -1) {
                MapEntity *nodecharacter = &gamestate->map.entities[node_index];
                for (int i = 0; i < gamestate->map.node_count; i++) {
                    if (GetCharacterId(gamestate->map.active_nodes[i]) ==
                        nodecharacter->entity_id) {
                        session->node_menu.node_id =
                            gamestate->map.active_nodes[i];
                        session->state = NODE_MENU;
                        session->node_menu.selected_index = 0;
                        session->node_menu.state = NODE_MENU_BROWSE;
                    }
                }

            } else {
                InitDialog(&gamestate->map, &session->manager);
                if (session->manager.active)
                    session->state = TALKING;
                else
                    session->state = EQUIPMENT_MENU;
            }
        }
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

        UpdatePhysics(&gamestate->map, input);
        UpdateBuffs(session->player, dt);
        UpdateCombat(&gamestate->map);
        CheckAndCollectMinerals(&gamestate->map);
        UpdateEntityMovement(&gamestate->map, dt);
    }
    AdjustCamera(gamestate, false, dt);

    int portal_index = PollTrait(&gamestate->map, TRAIT_TELEPORT, 20.0f);
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
        session->state = ADVENTURE;
    }
}

void UpdatePlaySession(Gamestate *gamestate) {
    PlaySession *session = &gamestate->session;
    Input input = CaptureInput();
    float dt = GetFrameTime();
    if (dt > 0.1f)
        dt = 0.1f;
    switch (session->state) {
    case ADVENTURE:
        UpdateAdventure(gamestate, &input, dt);
        break;
    case INVENTORY:
        UpdateInventory(session, &input);
        break;
    case MINERAL_INVENTORY:
        UpdateMineralInventory(session, &input);
        break;
    case TALKING:
        UpdateTalking(gamestate, &input, dt);
        break;
    case ITEM:
        UpdateItemPopup(session, &input);
        break;
    case LEVEL_INVENTORY:
        UpdateLevelInventory(session, &input);
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
    default:
        return;
    }
}
