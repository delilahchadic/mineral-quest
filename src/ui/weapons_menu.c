#include "ui/weapons_menu.h"
#include "defs/types_systems.h"
#include "engine/palette.h"
#include "registry/mineral_register.h"
#include "registry/register.h"
#include "systems/player.h"
#include "systems/weapon_grid.h"

// Forward declaration for large weapon drawing
void DrawLargeWeapon(int weapon_id, Vector2 position, float rotation);

void UpdateWeaponsMenu(PlaySession *session, Input *input) {
    Player *player = session->player;
    WeaponGrid *grid = &player->weapon_grid;
    WeaponsMenu *menu = &session->weapons_menu;
    int max_slots = 25;
    int cols = 5;
    int target = menu->selected_slot;

    // --- SUB-STATE 2: SELECT SPECIFIC MATERIAL FROM INVENTORY ---
    if (menu->sub_state == WEAPON_MENU_SELECT_MATERIAL) {
        int total_types = (menu->material_category == 0)
                              ? GetEntityTypeCount(ENTITY_PLANT)
                              : GetEntityTypeCount(ENTITY_MINERAL);

        // Count how many valid owned items exist
        int owned_count = 0;
        for (int i = 0; i < total_types; i++) {
            if (menu->material_category == 0
                    ? player->plant_inventory[i] > 0
                    : player->mineral_inventory[i] > 0) {
                owned_count++;
            }
        }

        if (input->buttons_pressed & BACKSPACE_PRESSED) {
            menu->sub_state = WEAPON_MENU_SELECT_CATEGORY;
            return;
        }

        if (owned_count > 0) {
            // Navigate list up/down among owned items
            if (input->buttons_pressed & KEY_W_PRESSED) {
                menu->material_list_index =
                    (menu->material_list_index - 1 + owned_count) % owned_count;
            }
            if (input->buttons_pressed & KEY_S_PRESSED) {
                menu->material_list_index =
                    (menu->material_list_index + 1) % owned_count;
            }

            // Confirm applying item from inventory to weapon slot
            if (input->buttons_pressed & ENTER_PRESSED) {
                // Map the filtered list index back to the actual
                // inventory/registry item ID
                int current_owned_index = 0;
                int actual_mat_id = -1;
                for (int i = 0; i < total_types; i++) {
                    if (menu->material_category == 0
                            ? player->plant_inventory[i] > 0
                            : player->mineral_inventory[i] > 0) {
                        if (current_owned_index == menu->material_list_index) {
                            actual_mat_id = i;
                            break;
                        }
                        current_owned_index++;
                    }
                }

                if (actual_mat_id != -1) {
                    WeaponSlot *slot = &grid->slots[target];

                    if (menu->material_category == 0) {
                        if (player->plant_inventory[actual_mat_id] > 0) {
                            if (slot->current_socket +
                                    GetCost(ENTITY_PLANT, actual_mat_id) <=
                                slot->max_socket) {
                                player->plant_inventory[actual_mat_id]--;
                                AddMaterial(grid, target, ENTITY_PLANT,
                                            actual_mat_id, 1);
                                CalculateWeapon(grid, target);
                                RecalculateStats(player);

                                if (slot->current_socket >= slot->max_socket) {
                                    menu->sub_state = WEAPON_MENU_BROWSE;
                                    return;
                                }
                                // Re-clamp index if list shrank
                                int new_owned_count = 0;
                                for (int i = 0; i < total_types; i++) {
                                    if (player->plant_inventory[i] > 0)
                                        new_owned_count++;
                                }
                                if (menu->material_list_index >=
                                        new_owned_count &&
                                    new_owned_count > 0) {
                                    menu->material_list_index =
                                        new_owned_count - 1;
                                }
                            }
                        }
                    } else {
                        if (player->mineral_inventory[actual_mat_id] > 0) {
                            if (slot->current_socket +
                                    GetCost(ENTITY_MINERAL, actual_mat_id) <=
                                slot->max_socket) {
                                player->mineral_inventory[actual_mat_id]--;
                                AddMaterial(grid, target, ENTITY_MINERAL,
                                            actual_mat_id, 1);
                                CalculateWeapon(grid, target);
                                RecalculateStats(player);

                                if (slot->current_socket >= slot->max_socket) {
                                    menu->sub_state = WEAPON_MENU_BROWSE;
                                    return;
                                }

                                // Re-clamp index if list shrank
                                int new_owned_count = 0;
                                for (int i = 0; i < total_types; i++) {
                                    if (player->mineral_inventory[i] > 0)
                                        new_owned_count++;
                                }
                                if (menu->material_list_index >=
                                        new_owned_count &&
                                    new_owned_count > 0) {
                                    menu->material_list_index =
                                        new_owned_count - 1;
                                }
                            }
                        }
                    }
                }
            }
        }
        return;
    }

    // --- SUB-STATE 1: CHOOSE CATEGORY (PLANTS VS MINERALS) ---
    if (menu->sub_state == WEAPON_MENU_SELECT_CATEGORY) {
        if (input->buttons_pressed & BACKSPACE_PRESSED) {
            menu->sub_state = WEAPON_MENU_BROWSE;
            return;
        }

        if (input->buttons_pressed & KEY_W_PRESSED ||
            input->buttons_pressed & KEY_S_PRESSED) {
            menu->material_category =
                1 - menu->material_category; // Toggle between 0 and 1
        }

        if (input->buttons_pressed & ENTER_PRESSED) {
            menu->material_list_index = 0;
            menu->sub_state = WEAPON_MENU_SELECT_MATERIAL;
        }
        return;
    }

    // --- SUB-STATE 0: BROWSE GRID ---
    if (input->buttons_pressed & KEY_W_PRESSED) {
        menu->selected_slot =
            (menu->selected_slot - cols + max_slots) % max_slots;
    }
    if (input->buttons_pressed & KEY_S_PRESSED) {
        menu->selected_slot = (menu->selected_slot + cols) % max_slots;
    }
    if (input->buttons_pressed & KEY_A_PRESSED) {
        if (menu->selected_slot % cols != 0)
            menu->selected_slot--;
    }
    if (input->buttons_pressed & KEY_D_PRESSED) {
        if ((menu->selected_slot + 1) % cols != 0)
            menu->selected_slot++;
    }

    if (input->buttons_pressed & BACKSPACE_PRESSED) {
        session->state = ADVENTURE_STATE;
        return;
    }

    if (input->buttons_pressed & ENTER_PRESSED) {
        if (grid->slots[target].active) {
            WeaponSlot *slot = &grid->slots[target];
            // Only allow entering material menu if sockets are not full
            if (slot->current_socket < slot->max_socket) {
                menu->sub_state = WEAPON_MENU_SELECT_CATEGORY;
                menu->material_category = 0; // Default to Plants
            }
        }
    }
}

void DrawWeaponsMenu(PlaySession *session) {
    Player *player = session->player;
    WeaponGrid *grid = &player->weapon_grid;
    WeaponsMenu *menu = &session->weapons_menu;

    ClearBackground(COLOR_CAMO_WOODLAND_GREEN);

    // Header
    DrawText("WEAPON GRID & LOADOUT", 50, 40, 30, COLOR_PYRITE_BRASS);
    DrawRectangle(50, 80, SCREEN_WIDTH - 100, 2, COLOR_PYRITE_BRASS);

    // Grid Slot Dimensions
    int startX = 60;
    int startY = 120;
    int slotSize = 80;
    int padding = 12;

    for (int i = 0; i < 25; i++) {
        int r = i / 5;
        int c = i % 5;
        int x = startX + c * (slotSize + padding);
        int y = startY + r * (slotSize + padding);

        bool isSelected = (i == menu->selected_slot);
        bool isActiveSlot = (grid->activeIndex == i);
        bool hasWeapon = grid->slots[i].active;

        DrawRectangle(x, y, slotSize, slotSize, COLOR_PULP_PAPER);
        DrawRectangleLines(x, y, slotSize, slotSize, COLOR_SUNKEN_INK);

        if (isActiveSlot) {
            DrawRectangleLines(x + 3, y + 3, slotSize - 6, slotSize - 6,
                               COLOR_PYRITE_BRASS);
        }

        if (isSelected) {
            DrawRectangleLinesEx((Rectangle){(float)x - 2, (float)y - 2,
                                             (float)slotSize + 4,
                                             (float)slotSize + 4},
                                 2.5f, COLOR_PYRITE_BRASS);
        }

        if (hasWeapon) {
            int wid = grid->slots[i].weapon_id;
            Vector2 drawPos = {(float)(x + slotSize / 2),
                               (float)(y + slotSize / 2)};
            DrawWeapon(wid, drawPos, 0.0f);
        } else {
            DrawText("[+]", x + 28, y + 30, 16, COLOR_SUNKEN_INK);
        }

        char slotNum[8];
        snprintf(slotNum, sizeof(slotNum), "%d", i + 1);
        DrawText(slotNum, x + 6, y + 6, 10, COLOR_SUNKEN_INK);
    }

    // Information Panel on the Right
    int selected = menu->selected_slot;
    int panelX = 520;
    int panelY = 120;
    int panelW = 360;
    int panelH = 540;

    DrawRectangle(panelX, panelY, panelW, panelH, COLOR_PULP_PAPER);
    DrawRectangleLines(panelX, panelY, panelW, panelH, COLOR_SUNKEN_INK);

    if (menu->sub_state == WEAPON_MENU_SELECT_CATEGORY) {
        DrawText("CHOOSE MATERIAL TYPE", panelX + 20, panelY + 20, 16,
                 COLOR_SUNKEN_INK);

        Color pColor = (menu->material_category == 0) ? COLOR_PYRITE_BRASS
                                                      : COLOR_SUNKEN_INK;
        Color mColor = (menu->material_category == 1) ? COLOR_PYRITE_BRASS
                                                      : COLOR_SUNKEN_INK;

        DrawText("> Plants", panelX + 30, panelY + 80, 16, pColor);
        DrawText("> Minerals", panelX + 30, panelY + 110, 16, mColor);
        DrawText("\n[ENTER] Select Category\n[BACKSPACE] Back to Slot",
                 panelX + 20, panelY + 180, 12, COLOR_SUNKEN_INK);

    } else if (menu->sub_state == WEAPON_MENU_SELECT_MATERIAL) {
        char title[64];
        snprintf(title, sizeof(title), "SELECT %s",
                 menu->material_category == 0 ? "PLANT" : "MINERAL");
        DrawText(title, panelX + 20, panelY + 20, 16, COLOR_SUNKEN_INK);

        int total_types = (menu->material_category == 0)
                              ? GetEntityTypeCount(ENTITY_PLANT)
                              : GetEntityTypeCount(ENTITY_MINERAL);
        int startListY = panelY + 60;
        int drawnItems = 0;
        int actual_rendered_ids[32]; // Track which item IDs are drawn to handle
                                     // preview mapping

        for (int i = 0; i < total_types && drawnItems < 14; i++) {
            int qty = (menu->material_category == 0)
                          ? player->plant_inventory[i]
                          : player->mineral_inventory[i];

            if (qty > 0) {
                bool isHighlighted = (drawnItems == menu->material_list_index);
                char rowText[96];
                int currentItemY = startListY + (drawnItems * 22);

                actual_rendered_ids[drawnItems] = i;

                if (menu->material_category == 0) {
                    snprintf(rowText, sizeof(rowText), "%s (x%d)",
                             PLANT_REGISTRY[i].species_name, qty);
                    Color col =
                        isHighlighted ? COLOR_NAPLES_YELLOW : COLOR_SUNKEN_INK;

                    Texture2D *plantSprite = GetSprite(ENTITY_PLANT, i);
                    if (plantSprite && plantSprite->id > 0) {
                        float maxAllowedHeight = 18.0f;
                        float scale = 1.0f;
                        if ((float)plantSprite->height > maxAllowedHeight) {
                            scale =
                                maxAllowedHeight / (float)plantSprite->height;
                        }
                        float finalHeight = (float)plantSprite->height * scale;
                        Rectangle source = {0.0f, 0.0f,
                                            (float)plantSprite->width,
                                            (float)plantSprite->height};
                        Rectangle dest = {
                            (float)(panelX + 38), (float)(currentItemY + 11),
                            (float)plantSprite->width * scale, finalHeight};
                        DrawTexturePro(*plantSprite, source, dest,
                                       (Vector2){0, finalHeight / 2.0f}, 0.0f,
                                       WHITE);
                    }

                    DrawText(rowText, panelX + 58, currentItemY, 14, col);
                } else {
                    snprintf(rowText, sizeof(rowText), "%s (x%d)",
                             GetMineralLabel(i), qty);
                    Color col = isHighlighted ? COLOR_NAPLES_YELLOW
                                              : GetMineralColor(i);

                    DrawMineral(i, (Vector2){(float)(panelX + 38),
                                             (float)(currentItemY + 7)});
                    DrawText(rowText, panelX + 58, currentItemY, 14, col);
                }
                drawnItems++;
            }
        }

        if (drawnItems == 0) {
            char noneMsg[64];
            snprintf(noneMsg, sizeof(noneMsg), "No %s available in inventory.",
                     menu->material_category == 0 ? "plants" : "minerals");
            DrawText(noneMsg, panelX + 20, startListY + 20, 13,
                     COLOR_SUNKEN_INK);
            DrawText("[BACKSPACE] Back", panelX + 20, startListY + 60, 12,
                     COLOR_PYRITE_BRASS);
        } else {
            // --- STAT PREVIEW BOX FOR HOVERED MATERIAL ---
            int statPreviewY = startListY + (14 * 22) + 10;
            DrawRectangle(panelX + 15, statPreviewY, panelW - 30, 110,
                          COLOR_CAMO_WOODLAND_GREEN);
            DrawRectangleLines(panelX + 15, statPreviewY, panelW - 30, 110,
                               COLOR_SUNKEN_INK);

            int hoveredIndex = menu->material_list_index;
            if (hoveredIndex >= 0 && hoveredIndex < drawnItems) {
                int actual_id = actual_rendered_ids[hoveredIndex];
                int hp_b = 0;
                int mp_b = 0;
                int stat_b[STAT_COUNT] = {0};

                if (menu->material_category == 0) {
                    Plant *plant = &PLANT_REGISTRY[actual_id];
                    hp_b = plant->hp_bonus;
                    mp_b = plant->mp_bonus;
                    for (int j = 0; j < STAT_COUNT; j++) {
                        stat_b[j] = plant->stat_bonuses[j];
                    }
                } else {
                    BasicStatBlock *mineral = &MINERAL_STATS[actual_id];
                    hp_b = mineral->hp_bonus;
                    mp_b = mineral->mp_bonus;
                    for (int j = 0; j < STAT_COUNT; j++) {
                        stat_b[j] = mineral->stat_bonuses[j];
                    }
                }

                DrawText("Stat Bonuses:", panelX + 25, statPreviewY + 8, 12,
                         COLOR_PYRITE_BRASS);

                char statLine1[64];
                snprintf(statLine1, sizeof(statLine1), "HP: %+d  |  MP: %+d",
                         hp_b, mp_b);
                DrawText(statLine1, panelX + 25, statPreviewY + 24, 12,
                         COLOR_PULP_PAPER);

                // Dynamic stats from StatType enum (STR, DEF, MAG_OFF, MAG_DEF,
                // SPEED, etc.)
                char statLine2[96];
                snprintf(statLine2, sizeof(statLine2),
                         "STR:%+d DEF:%+d M.Off:%+d M.Def:%+d",
                         stat_b[STAT_STR], stat_b[STAT_DEF],
                         stat_b[STAT_MAG_OFF], stat_b[STAT_MAG_DEF]);
                DrawText(statLine2, panelX + 25, statPreviewY + 40, 11,
                         COLOR_PULP_PAPER);

                char statLine3[96];
                snprintf(statLine3, sizeof(statLine3),
                         "SPD:%+d Geo:%+d Bot:%+d Alc:%+d Aer:%+d",
                         stat_b[STAT_SPEED], stat_b[STAT_GEOLOGY],
                         stat_b[STAT_BOTANY], stat_b[STAT_ALCHEMY],
                         stat_b[STAT_AEROBICS]);
                DrawText(statLine3, panelX + 25, statPreviewY + 56, 11,
                         COLOR_PULP_PAPER);

                char costLine[64];
                int cost =
                    GetCost(menu->material_category == 0 ? ENTITY_PLANT
                                                         : ENTITY_MINERAL,
                            actual_id);
                snprintf(costLine, sizeof(costLine), "Socket Cost: %d", cost);
                DrawText(costLine, panelX + 25, statPreviewY + 76, 12,
                         COLOR_NAPLES_YELLOW);
            }

            int confirmY = statPreviewY + 115;
            DrawText("[ENTER] Apply Item  [BACKSPACE] Back", panelX + 20,
                     confirmY, 11, COLOR_SUNKEN_INK);
        }

    } else {
        // Standard Slot Inspection View
        DrawText("SLOT INSPECTION", panelX + 20, panelY + 20, 18,
                 COLOR_SUNKEN_INK);

        if (grid->slots[selected].active) {
            WeaponSlot *slot = &grid->slots[selected];
            int wid = slot->weapon_id;

            int previewBoxX = panelX + 20;
            int previewBoxY = panelY + 50;
            int previewBoxW = 100;
            int previewBoxH = 180;
            DrawRectangle(previewBoxX, previewBoxY, previewBoxW, previewBoxH,
                          COLOR_PULP_PAPER);
            DrawRectangleLines(previewBoxX, previewBoxY, previewBoxW,
                               previewBoxH, COLOR_SUNKEN_INK);

            Vector2 previewDrawPos = {(float)(previewBoxX + previewBoxW / 2),
                                      (float)(previewBoxY + previewBoxH / 3)};
            DrawLargeWeapon(wid, previewDrawPos, PI / 2);

            Vector2 textBeginning = {(float)(panelX + 20),
                                     (float)(previewBoxY + previewBoxH + 20)};
            char nameBuffer[64];
            snprintf(nameBuffer, sizeof(nameBuffer), "%s",
                     GetName(ENTITY_ITEM, wid));
            DrawText(nameBuffer, textBeginning.x, textBeginning.y, 14,
                     COLOR_SUNKEN_INK);

            char dmgBuffer[64];
            snprintf(dmgBuffer, sizeof(dmgBuffer), "Base Power: %d",
                     ITEM_REGISTRY[wid].stat_bonuses[STAT_STR]);
            DrawText(dmgBuffer, textBeginning.x, textBeginning.y + 18, 12,
                     COLOR_SUNKEN_INK);

            // Display Level & Exp
            char statsBuffer[64];
            snprintf(statsBuffer, sizeof(statsBuffer), "Level: %d (Exp: %d/%d)",
                     slot->level, slot->exp, slot->next_level_exp);
            DrawText(statsBuffer, textBeginning.x, textBeginning.y + 34, 12,
                     COLOR_SUNKEN_INK);

            // Display Socket Usage (current / max)
            char socketBuffer[64];
            snprintf(socketBuffer, sizeof(socketBuffer), "Sockets: %d / %d",
                     slot->current_socket, slot->max_socket);
            DrawText(socketBuffer, textBeginning.x, textBeginning.y + 50, 12,
                     COLOR_SUNKEN_INK);

            DrawText("Applied Materials:", textBeginning.x,
                     textBeginning.y + 70, 14, COLOR_PYRITE_BRASS);

            int matY = textBeginning.y + 90;
            int drawnCount = 0;

            // Loop through applied plants
            int plantCount = GetEntityTypeCount(ENTITY_PLANT);
            for (int i = 0; i < plantCount && drawnCount < 5; i++) {
                if (slot->plant[i] > 0) {
                    char rowText[96];
                    snprintf(rowText, sizeof(rowText), "%s (x%d)",
                             PLANT_REGISTRY[i].species_name, slot->plant[i]);

                    int currentItemY = matY + (drawnCount * 22);
                    Texture2D *plantSprite = GetSprite(ENTITY_PLANT, i);
                    if (plantSprite && plantSprite->id > 0) {
                        float maxAllowedHeight = 16.0f;
                        float scale = 1.0f;
                        if ((float)plantSprite->height > maxAllowedHeight) {
                            scale =
                                maxAllowedHeight / (float)plantSprite->height;
                        }
                        float finalHeight = (float)plantSprite->height * scale;
                        Rectangle source = {0.0f, 0.0f,
                                            (float)plantSprite->width,
                                            (float)plantSprite->height};
                        Rectangle dest = {
                            (float)(panelX + 35), (float)(currentItemY + 8),
                            (float)plantSprite->width * scale, finalHeight};
                        DrawTexturePro(*plantSprite, source, dest,
                                       (Vector2){0, finalHeight / 2.0f}, 0.0f,
                                       WHITE);
                    }

                    DrawText(rowText, panelX + 50, currentItemY, 12,
                             COLOR_SUNKEN_INK);
                    drawnCount++;
                }
            }

            // Loop through applied minerals
            int mineralCount = GetEntityTypeCount(ENTITY_MINERAL);
            for (int i = 0; i < mineralCount && drawnCount < 5; i++) {
                if (slot->minerals[i] > 0) {
                    char rowText[96];
                    snprintf(rowText, sizeof(rowText), "%s (x%d)",
                             GetMineralLabel(i), slot->minerals[i]);

                    int currentItemY = matY + (drawnCount * 22);
                    DrawMineral(i, (Vector2){(float)(panelX + 30),
                                             (float)(currentItemY + 6)});
                    DrawText(rowText, panelX + 50, currentItemY, 12,
                             GetMineralColor(i));
                    drawnCount++;
                }
            }

            if (drawnCount == 0) {
                DrawText("- None applied", panelX + 30, matY, 12,
                         COLOR_SUNKEN_INK);
                drawnCount = 1;
            }

            int confirmY = matY + (drawnCount * 22) + 10;
            if (slot->current_socket >= slot->max_socket) {
                DrawText("Sockets are full!", panelX + 20, confirmY, 13,
                         COLOR_NAPLES_YELLOW);
            } else {
                DrawText("Press [ENTER] to apply\nplants or minerals.",
                         panelX + 20, confirmY, 13, COLOR_PYRITE_BRASS);
            }
        } else {
            DrawText("Slot is currently empty.\nEquip a weapon from "
                     "your\ninventory to fill this slot.",
                     panelX + 20, panelY + 60, 14, COLOR_SUNKEN_INK);
        }
    }
}
