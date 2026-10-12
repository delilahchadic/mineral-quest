#include "play/play_ui.h"

#include "core/camera_tools.h"
#include "defs/constants.h"
#include "defs/types_engine.h"
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"
#include "engine/palette.h"
#include "environment/map.h"
#include "play/play_inventory.h"
#include "raylib.h"
#include "registry/mineral_register.h"
#include "registry/register.h"
#include "rendering/map_renderer.h"
#include "systems/player.h"
#include "systems/script_manager.h"
#include "ui/dialog_box.h"
#include "ui/equip_menu.h"
#include "ui/node_menu.h"
#include "ui/plant_inventory.h"
#include "ui/recipe_menu.h"
#include "ui/stats_menu.h"
#include "ui/vhs_inventory.h"
#include "ui/weapons_menu.h"
#include <math.h>

void AdjustCamera(Gamestate* gamestate, bool dialog, float dt){
    // Keep bounds slightly inside the absolute edge so the player center doesn't clip out
    float max_w = (gamestate->map.columns - 0.5f) * TILE_SIZE;
    float max_h = (gamestate->map.rows - 0.5f) * TILE_SIZE;
    float min_w = 0.5f * TILE_SIZE;
    float min_h = 0.5f * TILE_SIZE;

    if (gamestate->map.player.position.x < min_w) gamestate->map.player.position.x = min_w;
    if (gamestate->map.player.position.y < min_h) gamestate->map.player.position.y = min_h;
    if (gamestate->map.player.position.x > max_w) gamestate->map.player.position.x = max_w;
    if (gamestate->map.player.position.y > max_h) gamestate->map.player.position.y = max_h;

    // 2. FIND THE EXACT POSITION OF THE PLAYER'S FEET
    Vector2 playerIso = GetWorldToIso(gamestate->map.player.position);

    // The player is drawn at playerIso.y - altitude.
    Vector2 targetPos = playerIso;
    targetPos.y -= gamestate->map.player.altitude;

    // 3. ONE SINGLE LERP
    float camSpeed = 15.0f;
    if (dt > 0.1f) dt = 0.1f;
    float lerpFactor = 1.0f - expf(-camSpeed * dt);

    gamestate->camera.target.x += (targetPos.x - gamestate->camera.target.x) * lerpFactor;
    gamestate->camera.target.y += (targetPos.y - gamestate->camera.target.y) * lerpFactor;

    if(dialog){
        gamestate->camera.zoom += (3.2f - gamestate->camera.zoom) * 0.05f;
    }else{
        gamestate->camera.zoom += (3.0f - gamestate->camera.zoom) * 0.05f;
    }
}

void DrawHUD(PlayState state) {
    Vector2 hud_start = (Vector2){SCREEN_WIDTH * 0.9, SCREEN_HEIGHT * 0.78};
    Vector2 hud_left = (Vector2){SCREEN_WIDTH * 0.05, SCREEN_HEIGHT * 0.78};

    // HP Bar coordinates (Right-to-left aligned)
    Vector2 max_hp_start =
        (Vector2){(hud_start.x + (RUSTY_HEADSHOT_SPRITE.width * 0.5)) -
                      PLAYER->stats.max_hp,
                  (hud_start.y + (RUSTY_HEADSHOT_SPRITE.height * 0.5)) + 10};
    Vector2 hp_start =
        (Vector2){(hud_start.x + (RUSTY_HEADSHOT_SPRITE.width * 0.5)) -
                      PLAYER->stats.current_hp,
                  (hud_start.y + (RUSTY_HEADSHOT_SPRITE.height * 0.5)) + 10};

    // MP Bar coordinates (Mirrored right-to-left alignment, placed 8 pixels
    // below HP)
    Vector2 max_mp_start =
        (Vector2){(hud_start.x + (RUSTY_HEADSHOT_SPRITE.width * 0.5)) -
                      PLAYER->stats.max_mp,
                  (hud_start.y + (RUSTY_HEADSHOT_SPRITE.height * 0.5)) + 18};
    Vector2 mp_start =
        (Vector2){(hud_start.x + (RUSTY_HEADSHOT_SPRITE.width * 0.5)) -
                      PLAYER->stats.current_mp,
                  (hud_start.y + (RUSTY_HEADSHOT_SPRITE.height * 0.5)) + 18};

    // Draw HUD elements
    DrawTextureEx(RUSTY_HEADSHOT_SPRITE, hud_start, 0.0, 0.5, WHITE);

    // Draw HP
    DrawRectangle(max_hp_start.x, max_hp_start.y, PLAYER->stats.max_hp, 6,
                  COLOR_SUNKEN_INK);
    DrawRectangle(hp_start.x, hp_start.y, PLAYER->stats.current_hp, 6,
                  COLOR_MAY_GREEN);

    // Draw MP
    DrawRectangle(max_mp_start.x, max_mp_start.y, PLAYER->stats.max_mp, 6,
                  COLOR_SUNKEN_INK);
    DrawRectangle(mp_start.x, mp_start.y, PLAYER->stats.current_mp, 6,
                  COLOR_CERULEAN_DUSTY);

    if (state == ADVENTURE_STATE) {
        // Draw Equipped Tarot Cards (Slots mapped to Z, X, C keys)
        float tarot_y = hud_left.y + 32;
        char slot_label[64];
        char tarot_keys[3] = {'Z', 'X', 'C'};

        for (int i = 0; i < 3; i++) {
            int tarot_id = PLAYER->gear.tarot_ids[i];
            if (tarot_id != -1) {
                const char *tarot_name = GetName(ENTITY_ITEM, tarot_id);
                snprintf(slot_label, sizeof(slot_label), "[%c] %s",
                         tarot_keys[i], tarot_name ? tarot_name : "Unknown");
            } else {
                snprintf(slot_label, sizeof(slot_label), "[%c] ---",
                         tarot_keys[i]);
            }

            DrawText(slot_label, (int)hud_left.x, (int)(tarot_y + (i * 32)), 30,
                     COLOR_SUNKEN_INK);
        }
    }
}

void DrawPlaySession(Gamestate *gamestate) {
    PlaySession *session = &gamestate->session;
    if (session->state == PLANT_INVENTORY) {
        DrawPlantInventory(&session->menu);
    } else if (session->state == VHS_MENU) {
        DrawVHSInventory(&session->menu);
    } else if (session->state == NODE_MENU) {
        DrawNodeSession(session);
    } else if (session->state == EQUIPMENT_MENU) {
        DrawEquipmentMenu(session, &session->equip_menu);
    } else if (session->state == STATS_MENU) {
        DrawStatsMenu(gamestate);
    } else if (session->state == INVENTORY_MENU) {
        DrawInventory(&session->menu);
        // DrawHUD(session->state);
    } else if (session->state == MINERAL_INVENTORY) {
        DrawMineralInventory(session->player);
    } else if (session->state == WEAPONS_MENU) {
        DrawWeaponsMenu(session);
    } else if (session->state == RECIPE_INVENTORY) {
        DrawRecipeInventory(session);
    } else {
        Draw_Map(&gamestate->map, &gamestate->camera, true);

        if (session->state == DIALOG_PROMPT) {
            DrawMessage(&session->manager);
        }
        if (session->state == GATHER_PROMPT) {
            DrawDialog("you found something!", session->pendingItemName);
        }
    }
    DrawHUD(session->state);
}
