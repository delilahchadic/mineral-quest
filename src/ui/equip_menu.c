#include "ui/equip_menu.h"

#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"
#include "engine/palette.h"
#include "registry/register.h"
#include "systems/player.h"
#include "systems/gear.h"

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
    int stat_label_x = margin;
    int total_col_x  = 180;
    int select_col_x = 300;
    int base_y       = 100;

    // --- Aggregate Total Gear Bonuses (Weapons & Accessories only) ---
    int total_hp_bonus = 0;
    int total_stat_bonuses[STAT_COUNT] = {0};

    // Weapon Bonuses
    if (session->player->gear.weapon_id != -1) {
        ItemDefinition* weapon = &ITEM_REGISTRY[session->player->gear.weapon_id];
        total_hp_bonus += weapon->hp_bonus;
        for (int s = 0; s < STAT_COUNT; s++) {
            total_stat_bonuses[s] += weapon->stat_bonuses[s];
        }
    }

    // Accessory Bonuses
    int acc_count = session->player->stats.current[STAT_ACCESORY_COUNT];
    for (int i = 0; i < acc_count; i++) {
        int acc_id = session->player->gear.accessory_ids[i];
        if (acc_id != -1) {
            ItemDefinition* acc = &ITEM_REGISTRY[acc_id];
            total_hp_bonus += acc->hp_bonus;
            for (int s = 0; s < STAT_COUNT; s++) {
                total_stat_bonuses[s] += acc->stat_bonuses[s];
            }
        }
    }
    // Note: Tarot card bonuses are excluded here for now as they govern behavior rather than player stats.

    // --- Determine Currently Selected Item ---
    int selected_item_id = -1;
    if (equipMenu->mode == SLOT_NONE) {
        if (equipMenu->activeSlot == 0) {
            selected_item_id = session->player->gear.weapon_id;
        } else if (equipMenu->activeSlot <= acc_count) {
            int acc_index = equipMenu->activeSlot - 1;
            selected_item_id = session->player->gear.accessory_ids[acc_index];
        } else {
            int tarot_index = equipMenu->activeSlot - (acc_count + 1);
            selected_item_id = session->player->gear.tarot_ids[tarot_index];
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
    char total_hp_buf[16];
    snprintf(total_hp_buf, sizeof(total_hp_buf), "%s%d", (total_hp_bonus > 0) ? "+" : "", total_hp_bonus);
    DrawText(total_hp_buf, total_col_x, current_y, 16, COLOR_BONE_WHITE);

    if (selected_item_id != -1) {
        ItemDefinition* item = &ITEM_REGISTRY[selected_item_id];
        // If viewing a tarot card, its stat/HP modifiers are ignored in preview for now
        int preview_hp = (equipMenu->mode == SLOT_TAROT) ? 0 : item->hp_bonus;
        char sel_hp_buf[16];
        snprintf(sel_hp_buf, sizeof(sel_hp_buf), "%s%d", (preview_hp > 0) ? "+" : "", preview_hp);
        DrawText(sel_hp_buf, select_col_x, current_y, 16, COLOR_BONE_WHITE);
    } else {
        DrawText("+0", select_col_x, current_y, 16, Fade(COLOR_BONE_WHITE, 0.4f));
    }

    current_y += 22;

    // --- Stat Rows ---
    for (int s = 0; s < STAT_COUNT; s++) {
        DrawText(STATS_NAMES[s], stat_label_x, current_y, 16, COLOR_BONE_WHITE);

        char total_buf[16];
        snprintf(total_buf, sizeof(total_buf), "%s%d", (total_stat_bonuses[s] > 0) ? "+" : "", total_stat_bonuses[s]);
        DrawText(total_buf, total_col_x, current_y, 16, COLOR_BONE_WHITE);

        if (selected_item_id != -1) {
            ItemDefinition* item = &ITEM_REGISTRY[selected_item_id];
            int preview_stat = (equipMenu->mode == SLOT_TAROT) ? 0 : item->stat_bonuses[s];
            char sel_buf[16];
            snprintf(sel_buf, sizeof(sel_buf), "%s%d", (preview_stat > 0) ? "+" : "", preview_stat);
            DrawText(sel_buf, select_col_x, current_y, 16, COLOR_BONE_WHITE);
        } else {
            DrawText("+0", select_col_x, current_y, 16, Fade(COLOR_BONE_WHITE, 0.4f));
        }

        current_y += 22;
    }

    // --- Column 3 (x: 480): Weapon, Accessory & Tarot Slots ---
    int col3_x = 480;
    int col3_y = 100;

    // Weapon Slot
    Color weaponTextColor = (equipMenu->mode == SLOT_NONE && equipMenu->activeSlot == 0) ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
    DrawText("Weapon", col3_x, col3_y, 15, COLOR_BONE_WHITE);
    char* weaponName = session->player->gear.weapon_id == -1 ? "------------" : GetName(ENTITY_ITEM, session->player->gear.weapon_id);
    DrawText(weaponName, col3_x, col3_y + 22, 16, weaponTextColor);

    // Accessory Slots
    col3_y += 65;
    DrawText("Accessories", col3_x, col3_y, 15, COLOR_BONE_WHITE);
    for (int i = 0; i < acc_count; i++) {
        int slot_idx = i + 1;
        Color accessoryTextColor = (equipMenu->mode == SLOT_NONE && equipMenu->activeSlot == slot_idx) ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
        char* accName = session->player->gear.accessory_ids[i] == -1 ? "------------" : GetName(ENTITY_ITEM, session->player->gear.accessory_ids[i]);
        DrawText(accName, col3_x, col3_y + 22 + (i * 26), 16, accessoryTextColor);
    }

    // Tarot Slots
    col3_y += 22 + (acc_count * 26) + 15;
    DrawText("Tarot Cards", col3_x, col3_y, 15, COLOR_BONE_WHITE);
    for (int i = 0; i < 3; i++) {
        int slot_idx = acc_count + 1 + i;
        Color tarotTextColor = (equipMenu->mode == SLOT_NONE && equipMenu->activeSlot == slot_idx) ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
        int tarot_id = session->player->gear.tarot_ids[i];
        char* tarotName = tarot_id == -1 ? "------------" : GetName(ENTITY_ITEM, tarot_id);
        DrawText(tarotName, col3_x, col3_y + 22 + (i * 26), 16, tarotTextColor);
    }

    // --- Column 4 (x: 680): Item Picker (State 2) ---
    if (equipMenu->mode == SLOT_WEAPON || equipMenu->mode == SLOT_ACCESSORY || equipMenu->mode == SLOT_TAROT) {
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
    int acc_count = session->player->stats.current[STAT_ACCESORY_COUNT];
    int max_slots = acc_count + 3; // Weapon (0) + Accessories (1..acc_count) + Tarot (acc_count+1 .. acc_count+3)

    // State 1: Browsing the equip slots
    if (menu->mode == SLOT_NONE) {
        if (input->buttons_pressed & KEY_W_PRESSED) {
            menu->activeSlot--;
            if (menu->activeSlot < 0) {
                menu->activeSlot = max_slots; // wrap to last tarot slot
            }
        }
        if (input->buttons_pressed & KEY_S_PRESSED) {
            menu->activeSlot++;
            if (menu->activeSlot > max_slots) {
                menu->activeSlot = 0; // wrap back to weapon
            }
        }

        // Press Enter to open the inventory filter for the active slot
        if (input->buttons_pressed & ENTER_PRESSED) {
            menu->count = 0;
            menu->activeItemSlot = 0; // Reset item selection cursor

            if (menu->activeSlot == 0) {
                menu->mode = SLOT_WEAPON;
                for (int i = 0; i < 100; i++) {
                    if (session->player->item_inventory[i] > 0) {
                        if (GetAccesorySlot(ENTITY_ITEM, i) == SLOT_WEAPON) {
                            menu->itemIds[menu->count++] = i;
                        }
                    }
                }
            } else if (menu->activeSlot <= acc_count) {
                menu->mode = SLOT_ACCESSORY;
                for (int i = 0; i < 100; i++) {
                    if (session->player->item_inventory[i] > 0) {
                        if (GetAccesorySlot(ENTITY_ITEM, i) == SLOT_ACCESSORY) {
                            menu->itemIds[menu->count++] = i;
                        }
                    }
                }
            } else {
                menu->mode = SLOT_TAROT;
                for (int i = 0; i < 100; i++) {
                    if (session->player->item_inventory[i] > 0) {
                        if (GetAccesorySlot(ENTITY_ITEM, i) == SLOT_TAROT) {
                            menu->itemIds[menu->count++] = i;
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
    else if (menu->mode == SLOT_WEAPON || menu->mode == SLOT_ACCESSORY || menu->mode == SLOT_TAROT) {
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
            if (menu->mode == SLOT_WEAPON) {
                if (menu->activeItemSlot == 0) {
                    PlayerUnequipWeapon(session->player);
                } else {
                    int chosen_item_id = menu->itemIds[menu->activeItemSlot - 1];
                    PlayerEquipWeapon(session->player, chosen_item_id);
                }
            }
            else if (menu->mode == SLOT_ACCESSORY) {
                int acc_index = menu->activeSlot - 1;
                if (menu->activeItemSlot == 0) {
                    PlayerUnequipAccessory(session->player, acc_index);
                } else {
                    int chosen_item_id = menu->itemIds[menu->activeItemSlot - 1];
                    PlayerEquipAccessory(session->player, acc_index, chosen_item_id);
                }
            }
            else if (menu->mode == SLOT_TAROT) {
                int tarot_index = menu->activeSlot - (acc_count + 1);
                if (menu->activeItemSlot == 0) {
                    PlayerUnequipTarot(session->player, tarot_index);
                } else {
                    int chosen_item_id = menu->itemIds[menu->activeItemSlot - 1];
                    PlayerEquipTarot(session->player, tarot_index, chosen_item_id);
                }
            }

            int current_max_slots = session->player->stats.current[STAT_ACCESORY_COUNT] + 3;
            if (menu->activeSlot > current_max_slots) {
                menu->activeSlot = current_max_slots;
            }
            menu->mode = SLOT_NONE;
        }

        // Backspace cancels out of item selection back to slot view
        if (input->buttons_pressed & BACKSPACE_PRESSED) {
            menu->mode = SLOT_NONE;
        }
    }
}
