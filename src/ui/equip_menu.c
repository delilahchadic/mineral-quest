#include "ui/equip_menu.h"

#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"
#include "engine/palette.h"
#include "registry/register.h"
#include "systems/player.h"

#include "ui/equip_menu.h"
#include <stdio.h>

#include "ui/equip_menu.h"

#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"
#include "engine/palette.h"
#include "registry/register.h"
#include "systems/player.h"

#include <stdio.h>

void DrawEquipmentMenu(PlaySession* session, EquipMenu* equipMenu) {
    // 1. Background
    ClearBackground(COLOR_DUSTY_ROSE);

    int margin = 40;
    int uiWidth = SCREEN_WIDTH - (margin * 2);

    // 2. Title Line
    DrawText("Equipment", margin, 40, 30, COLOR_BONE_WHITE);
    DrawRectangle(margin, 80, uiWidth, 2, COLOR_BONE_WHITE);

    // --- Column Setup for Stats Alignment ---
    // Col A (x: 40)  -> Stat Labels ("HP", "STR", "DEF", etc.)
    // Col B (x: 180) -> Total Gear Bonuses
    // Col C (x: 300) -> Selected Item Bonuses
    int stat_label_x = margin;
    int total_col_x  = 180;
    int select_col_x = 300;
    int base_y       = 100;

    // --- Aggregate Total Gear Bonuses ---
    int total_hp_bonus = 0;
    int total_stat_bonuses[STAT_COUNT] = {0};

    if (session->player->gear.weapon_id != -1) {
        ItemDefinition* weapon = &ITEM_REGISTRY[session->player->gear.weapon_id];
        total_hp_bonus += weapon->hp_bonus;
        for (int s = 0; s < STAT_COUNT; s++) {
            total_stat_bonuses[s] += weapon->stat_bonuses[s];
        }
    }

    for (int i = 0; i < session->player->stats.current[STAT_ACCESORY_COUNT]; i++) {
        int acc_id = session->player->gear.accessory_ids[i];
        if (acc_id != -1) {
            ItemDefinition* acc = &ITEM_REGISTRY[acc_id];
            total_hp_bonus += acc->hp_bonus;
            for (int s = 0; s < STAT_COUNT; s++) {
                total_stat_bonuses[s] += acc->stat_bonuses[s];
            }
        }
    }

    // --- Determine Currently Selected Item ---
    int selected_item_id = -1;
    if (equipMenu->mode == SLOT_NONE) {
        if (equipMenu->activeSlot == 0) {
            selected_item_id = session->player->gear.weapon_id;
        } else {
            int acc_index = equipMenu->activeSlot - 1;
            if (acc_index < session->player->stats.current[STAT_ACCESORY_COUNT]) {
                selected_item_id = session->player->gear.accessory_ids[acc_index];
            }
        }
    } else {
        if (equipMenu->activeItemSlot > 0 && (equipMenu->activeItemSlot - 1) < equipMenu->count) {
            selected_item_id = equipMenu->itemIds[equipMenu->activeItemSlot - 1];
        }
    }

    // --- Header Section ---
    DrawText("STAT", stat_label_x, base_y, 16, Fade(COLOR_BONE_WHITE, 0.6f));
    DrawText("TOTAL", total_col_x, base_y, 16, COLOR_BONE_WHITE);
    DrawText("SELECTED", select_col_x, base_y, 16, COLOR_RED_OCHRE);

    DrawRectangle(stat_label_x, base_y + 22, 380, 1, Fade(COLOR_BONE_WHITE, 0.4f));

    int current_y = base_y + 32;

    // --- HP Row ---
    DrawText("HP", stat_label_x, current_y, 16, COLOR_BONE_WHITE);

    // Total HP Bonus
    char total_hp_buf[16];
    snprintf(total_hp_buf, sizeof(total_hp_buf), "%s%d", (total_hp_bonus > 0) ? "+" : "", total_hp_bonus);
    DrawText(total_hp_buf, total_col_x, current_y, 16, COLOR_BONE_WHITE);

    // Selected Item HP Bonus
    if (selected_item_id != -1) {
        ItemDefinition* item = &ITEM_REGISTRY[selected_item_id];
        char sel_hp_buf[16];
        snprintf(sel_hp_buf, sizeof(sel_hp_buf), "%s%d", (item->hp_bonus > 0) ? "+" : "", item->hp_bonus);
        DrawText(sel_hp_buf, select_col_x, current_y, 16, COLOR_BONE_WHITE);
    } else {
        DrawText("+0", select_col_x, current_y, 16, Fade(COLOR_BONE_WHITE, 0.4f));
    }

    current_y += 22;

    // --- Stat Rows ---
    for (int s = 0; s < STAT_COUNT; s++) {
        // Label Column
        DrawText(STATS_NAMES[s], stat_label_x, current_y, 16, COLOR_BONE_WHITE);

        // Total Column
        char total_buf[16];
        snprintf(total_buf, sizeof(total_buf), "%s%d", (total_stat_bonuses[s] > 0) ? "+" : "", total_stat_bonuses[s]);
        DrawText(total_buf, total_col_x, current_y, 16, COLOR_BONE_WHITE);

        // Selected Item Column
        if (selected_item_id != -1) {
            ItemDefinition* item = &ITEM_REGISTRY[selected_item_id];
            char sel_buf[16];
            snprintf(sel_buf, sizeof(sel_buf), "%s%d", (item->stat_bonuses[s] > 0) ? "+" : "", item->stat_bonuses[s]);
            DrawText(sel_buf, select_col_x, current_y, 16, COLOR_BONE_WHITE);
        } else {
            DrawText("+0", select_col_x, current_y, 16, Fade(COLOR_BONE_WHITE, 0.4f));
        }

        current_y += 22;
    }

    // --- Column 3 (x: 480): Weapon & Accessory Slots ---
    int col3_x = 480;
    int col3_y = 100;

    Color weaponTextColor = (equipMenu->mode == SLOT_NONE && equipMenu->activeSlot == 0) ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
    DrawText("Weapon", col3_x, col3_y, 15, COLOR_BONE_WHITE);
    char* weaponName = session->player->gear.weapon_id == -1 ? "------------" : GetName(ENTITY_ITEM, session->player->gear.weapon_id);
    DrawText(weaponName, col3_x, col3_y + 22, 16, weaponTextColor);

    col3_y += 65;
    DrawText("Accessories", col3_x, col3_y, 15, COLOR_BONE_WHITE);
    for (int i = 0; i < session->player->stats.current[STAT_ACCESORY_COUNT]; i++) {
        Color accessoryTextColor = (equipMenu->mode == SLOT_NONE && equipMenu->activeSlot == i + 1) ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
        if (session->player->gear.accessory_ids[i] == -1) {
            DrawText("------------", col3_x, col3_y + 22 + (i * 26), 16, accessoryTextColor);
            continue;
        }
        DrawText(GetName(ENTITY_ITEM, session->player->gear.accessory_ids[i]), col3_x, col3_y + 22 + (i * 26), 16, accessoryTextColor);
    }

    // --- Column 4 (x: 680): Item Picker (State 2) ---
    if (equipMenu->mode == SLOT_WEAPON || equipMenu->mode == SLOT_ACCESSORY) {
        int col4_x = 680;
        int col4_y = 100;

        DrawText("Inventory", col4_x, col4_y, 15, COLOR_BONE_WHITE);
        Color selectedColor = equipMenu->activeItemSlot == 0 ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
        DrawText("------------", col4_x, col4_y + 22, 16, selectedColor);

        for (int i = 0; i < equipMenu->count; i++) {
            selectedColor = equipMenu->activeItemSlot == (i + 1) ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
            DrawText(GetName(ENTITY_ITEM, equipMenu->itemIds[i]), col4_x, col4_y + 48 + (i * 26), 16, selectedColor);
        }
    }
}

void UpdateEquipMenu(PlaySession* session, Input* input) {
    EquipMenu* menu = &session->equip_menu;

    // State 1: Browsing the equip slots (Weapon or Accessories)
    if (menu->mode == SLOT_NONE) {
        if (input->buttons_pressed & KEY_W_PRESSED) {
            menu->activeSlot--;
            int max_slots = session->player->stats.current[STAT_ACCESORY_COUNT];
            if (menu->activeSlot < 0) {
                menu->activeSlot = max_slots; // wrap to last accessory slot
            }
        }
        if (input->buttons_pressed & KEY_S_PRESSED) {
            menu->activeSlot++;
            int max_slots = session->player->stats.current[STAT_ACCESORY_COUNT];
            if (menu->activeSlot > max_slots) {
                menu->activeSlot = 0; // wrap back to weapon
            }
        }

        // Press Enter to open the inventory filter for the active slot
        if (input->buttons_pressed & ENTER_PRESSED) {
            menu->count = 0;
            menu->activeItemSlot = 0; // Reset item selection cursor to top ("------------")

            // When pressing ENTER on a slot (State 1)
            if (menu->activeSlot == 0) {
                menu->mode = SLOT_WEAPON;
                // Filter item_inventory for weapons
                for (int i = 0; i < 100; i++) {
                    if (session->player->item_inventory[i] > 0) {
                        // i represents the item_id directly now!
                        if (GetAccesorySlot(ENTITY_ITEM, i) == SLOT_WEAPON) {
                            menu->itemIds[menu->count++] = i;
                        }
                    }
                }
            } else {
                menu->mode = SLOT_ACCESSORY;
                // Filter item_inventory for accessories
                for (int i = 0; i < 100; i++) {
                    if (session->player->item_inventory[i] > 0) {
                        int item_id = i;
                        if (GetAccesorySlot(ENTITY_ITEM, item_id) == SLOT_ACCESSORY) {
                            menu->itemIds[menu->count++] = item_id;
                        }
                    }
                }
            }
        }

        if (input->buttons_pressed & KEY_E_PRESSED) {
            session->state = ADVENTURE;
        }
    }
    // State 2: Selecting an item from the filtered list (or unequipping)
    else if (menu->mode == SLOT_WEAPON || menu->mode == SLOT_ACCESSORY) {
        // Max index is menu->count because index 0 is "------------" and 1..count are items
        int max_item_slot = menu->count;

        if (input->buttons_pressed & KEY_W_PRESSED) {
            menu->activeItemSlot--;
            if (menu->activeItemSlot < 0) {
                menu->activeItemSlot = max_item_slot; // wrap to bottom
            }
        }
        if (input->buttons_pressed & KEY_S_PRESSED) {
            menu->activeItemSlot++;
            if (menu->activeItemSlot > max_item_slot) {
                menu->activeItemSlot = 0; // wrap to top ("------------")
            }
        }

        if (input->buttons_pressed & ENTER_PRESSED) {
            // Inside State 2 (when you select an item to equip)
            if (menu->mode == SLOT_WEAPON) {
                if (menu->activeItemSlot == 0) {
                    // Unequip weapon entirely
                    if (session->player->gear.weapon_id != -1) {
                        GiveItem(session->player, session->player->gear.weapon_id); // Increments count
                        session->player->gear.weapon_id = -1;
                    }
                } else {
                    int chosen_item_id = menu->itemIds[menu->activeItemSlot - 1];

                    // Return old weapon to inventory if equipped
                    if (session->player->gear.weapon_id != -1) {
                        GiveItem(session->player, session->player->gear.weapon_id);
                    }
                    // Equip new weapon
                    session->player->gear.weapon_id = chosen_item_id;

                    // Decrement item quantity directly from the inventory array
                    session->player->item_inventory[chosen_item_id]--;
                }
            }
            else if (menu->mode == SLOT_ACCESSORY) {
                int acc_index = menu->activeSlot - 1;

                if (menu->activeItemSlot == 0) {
                    // Unequip accessory slot
                    if (session->player->gear.accessory_ids[acc_index] != -1) {
                        GiveItem(session->player, session->player->gear.accessory_ids[acc_index]);
                        session->player->gear.accessory_ids[acc_index] = -1;
                    }
                } else {
                    int chosen_item_id = menu->itemIds[menu->activeItemSlot - 1];

                    // Return old accessory to inventory if present
                    if (session->player->gear.accessory_ids[acc_index] != -1) {
                        GiveItem(session->player, session->player->gear.accessory_ids[acc_index]);
                    }
                    // Equip new accessory
                    session->player->gear.accessory_ids[acc_index] = chosen_item_id;

                    // Decrement item quantity directly
                    session->player->item_inventory[chosen_item_id]--;
                }
            }

            // Right after RecalculateStats in State 2:
            RecalculateStats(&session->player->stats, &session->player->gear);

            // Clamp activeSlot so cursor doesn't get stuck on a hidden accessory slot
            if (menu->activeSlot > session->player->stats.current[STAT_ACCESORY_COUNT]) {
                menu->activeSlot = session->player->stats.current[STAT_ACCESORY_COUNT];
            }
            menu->mode = SLOT_NONE;
        }

        // Backspace cancels out of item selection back to slot view
        if (input->buttons_pressed & BACKSPACE_PRESSED) {
            menu->mode = SLOT_NONE;
        }
    }
}
