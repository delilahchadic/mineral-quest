#include "ui/equip_menu.h"

#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"
#include "engine/palette.h"
#include "registry/register.h"
#include "systems/gear.h"
#include "systems/player.h"
#include "systems/weapon_grid.h"

#include <stdio.h>

void DrawEquipmentMenu(PlaySession *session, EquipMenu *equipMenu) {
    // 1. Background
    ClearBackground(COLOR_DUSTY_ROSE);

    int margin = 40;
    int uiWidth = SCREEN_WIDTH - (margin * 2);

    // 2. Title Line
    DrawText("Equipment", margin, 40, 30, COLOR_BONE_WHITE);
    DrawRectangle(margin, 80, uiWidth, 2, COLOR_BONE_WHITE);

    // --- Column Setup for Stats Alignment ---
    int stat_label_x = margin;
    int total_col_x = 180;
    int select_col_x = 300;
    int base_y = 100;

    WeaponGrid *grid = &session->player->weapon_grid;
    int acc_count = session->player->stats.current[STAT_ACCESORY_COUNT];

    // --- Aggregate Total Gear Bonuses ---
    int total_hp_bonus = 0;
    int total_mp_bonus = 0;
    int total_stat_bonuses[STAT_COUNT] = {0};

    // Weapon Bonuses (active weapon grid slot)
    if (grid->activeIndex != -1 && grid->slots[grid->activeIndex].active) {
        total_hp_bonus += grid->slots[grid->activeIndex].hp_bonus;
        total_mp_bonus += grid->slots[grid->activeIndex].mp_bonus;
        for (int s = 0; s < STAT_COUNT; s++) {
            total_stat_bonuses[s] +=
                grid->slots[grid->activeIndex].stat_bonuses[s];
        }
    }

    // Accessory Bonuses
    for (int i = 0; i < acc_count; i++) {
        int acc_id = session->player->gear.accessory_ids[i];
        if (acc_id != -1) {
            ItemDefinition *acc = &ITEM_REGISTRY[acc_id];
            total_hp_bonus += acc->hp_bonus;
            total_mp_bonus += acc->mp_bonus;
            for (int s = 0; s < STAT_COUNT; s++) {
                total_stat_bonuses[s] += acc->stat_bonuses[s];
            }
        }
    }

    // --- Determine Currently Selected Item Preview & Stats ---
    int preview_hp = 0;
    int preview_mp = 0;
    int preview_stats[STAT_COUNT] = {0};
    bool has_preview = false;

    if (equipMenu->mode == SLOT_NONE) {
        if (equipMenu->activeSlot == 0) {
            if (grid->activeIndex != -1 &&
                grid->slots[grid->activeIndex].active) {
                preview_hp = grid->slots[grid->activeIndex].hp_bonus;
                preview_mp = grid->slots[grid->activeIndex].mp_bonus;
                for (int s = 0; s < STAT_COUNT; s++) {
                    preview_stats[s] =
                        grid->slots[grid->activeIndex].stat_bonuses[s];
                }
                has_preview = true;
            }
        } else if (equipMenu->activeSlot <= acc_count) {
            int acc_index = equipMenu->activeSlot - 1;
            int acc_id = session->player->gear.accessory_ids[acc_index];
            if (acc_id != -1) {
                ItemDefinition *item = &ITEM_REGISTRY[acc_id];
                preview_hp = item->hp_bonus;
                preview_mp = item->mp_bonus;
                for (int s = 0; s < STAT_COUNT; s++) {
                    preview_stats[s] = item->stat_bonuses[s];
                }
                has_preview = true;
            }
        } else {
            int tarot_index = equipMenu->activeSlot - (1 + acc_count);
            int tarot_id = session->player->gear.tarot_ids[tarot_index];
            if (tarot_id != -1) {
                preview_hp = 0;
                preview_mp = 0;
                for (int s = 0; s < STAT_COUNT; s++) {
                    preview_stats[s] = 0;
                }
                has_preview = true;
            }
        }
    } else {
        if (equipMenu->mode == SLOT_WEAPON) {
            if (equipMenu->activeItemSlot > 0 &&
                (equipMenu->activeItemSlot - 1) < equipMenu->count) {
                int grid_idx =
                    equipMenu->itemIds[equipMenu->activeItemSlot - 1];
                preview_hp = grid->slots[grid_idx].hp_bonus;
                preview_mp = grid->slots[grid_idx].mp_bonus;
                for (int s = 0; s < STAT_COUNT; s++) {
                    preview_stats[s] = grid->slots[grid_idx].stat_bonuses[s];
                }
                has_preview = true;
            }
        } else if (equipMenu->mode == SLOT_ACCESSORY) {
            if (equipMenu->activeItemSlot > 0 &&
                (equipMenu->activeItemSlot - 1) < equipMenu->count) {
                int item_id = equipMenu->itemIds[equipMenu->activeItemSlot - 1];
                ItemDefinition *item = &ITEM_REGISTRY[item_id];
                preview_hp = item->hp_bonus;
                preview_mp = item->mp_bonus;
                for (int s = 0; s < STAT_COUNT; s++) {
                    preview_stats[s] = item->stat_bonuses[s];
                }
                has_preview = true;
            }
        } else if (equipMenu->mode == SLOT_TAROT) {
            if (equipMenu->activeItemSlot > 0 &&
                (equipMenu->activeItemSlot - 1) < equipMenu->count) {
                preview_hp = 0;
                preview_mp = 0;
                for (int s = 0; s < STAT_COUNT; s++) {
                    preview_stats[s] = 0;
                }
                has_preview = true;
            }
        }
    }

    // --- Header Section ---
    DrawText("STAT", stat_label_x, base_y, 16, Fade(COLOR_BONE_WHITE, 0.6f));
    DrawText("TOTAL", total_col_x, base_y, 16, COLOR_BONE_WHITE);
    DrawText("SELECTED", select_col_x, base_y, 16, COLOR_RED_OCHRE);

    DrawRectangle(stat_label_x, base_y + 22, 380, 1,
                  Fade(COLOR_BONE_WHITE, 0.4f));

    int current_y = base_y + 32;

    // --- HP Row ---
    DrawText("HP", stat_label_x, current_y, 16, COLOR_BONE_WHITE);
    char total_hp_buf[16];
    snprintf(total_hp_buf, sizeof(total_hp_buf), "%s%d",
             (total_hp_bonus > 0) ? "+" : "", total_hp_bonus);
    DrawText(total_hp_buf, total_col_x, current_y, 16, COLOR_BONE_WHITE);

    if (has_preview) {
        char sel_hp_buf[16];
        snprintf(sel_hp_buf, sizeof(sel_hp_buf), "%s%d",
                 (preview_hp > 0) ? "+" : "", preview_hp);
        DrawText(sel_hp_buf, select_col_x, current_y, 16, COLOR_BONE_WHITE);
    } else {
        DrawText("+0", select_col_x, current_y, 16,
                 Fade(COLOR_BONE_WHITE, 0.4f));
    }

    current_y += 22;

    // --- MP Row ---
    DrawText("MP", stat_label_x, current_y, 16, COLOR_BONE_WHITE);
    char total_mp_buf[16];
    snprintf(total_mp_buf, sizeof(total_mp_buf), "%s%d",
             (total_mp_bonus > 0) ? "+" : "", total_mp_bonus);
    DrawText(total_mp_buf, total_col_x, current_y, 16, COLOR_BONE_WHITE);

    if (has_preview) {
        char sel_mp_buf[16];
        snprintf(sel_mp_buf, sizeof(sel_mp_buf), "%s%d",
                 (preview_mp > 0) ? "+" : "", preview_mp);
        DrawText(sel_mp_buf, select_col_x, current_y, 16, COLOR_BONE_WHITE);
    } else {
        DrawText("+0", select_col_x, current_y, 16,
                 Fade(COLOR_BONE_WHITE, 0.4f));
    }

    current_y += 22;

    // --- Stat Rows ---
    for (int s = 0; s < STAT_COUNT; s++) {
        DrawText(STATS_NAMES[s], stat_label_x, current_y, 16, COLOR_BONE_WHITE);

        char total_buf[16];
        snprintf(total_buf, sizeof(total_buf), "%s%d",
                 (total_stat_bonuses[s] > 0) ? "+" : "", total_stat_bonuses[s]);
        DrawText(total_buf, total_col_x, current_y, 16, COLOR_BONE_WHITE);

        if (has_preview) {
            char sel_buf[16];
            snprintf(sel_buf, sizeof(sel_buf), "%s%d",
                     (preview_stats[s] > 0) ? "+" : "", preview_stats[s]);
            DrawText(sel_buf, select_col_x, current_y, 16, COLOR_BONE_WHITE);
        } else {
            DrawText("+0", select_col_x, current_y, 16,
                     Fade(COLOR_BONE_WHITE, 0.4f));
        }

        current_y += 22;
    }

    // --- Column 3 (x: 480): Weapon, Accessory & Tarot Slots ---
    int col3_x = 480;
    int col3_y = 100;

    // Weapon Slot
    DrawText("Weapon", col3_x, col3_y, 15, COLOR_BONE_WHITE);
    Color weaponTextColor =
        (equipMenu->mode == SLOT_NONE && equipMenu->activeSlot == 0)
            ? COLOR_RED_OCHRE
            : COLOR_BONE_WHITE;
    bool has_active_weapon =
        (grid->activeIndex != -1 && grid->slots[grid->activeIndex].active);
    char *weaponName =
        !has_active_weapon
            ? "------------"
            : GetName(ENTITY_ITEM, grid->slots[grid->activeIndex].weapon_id);
    DrawText(weaponName, col3_x, col3_y + 22, 16, weaponTextColor);

    // Accessory Slots
    int acc_start_y = col3_y + 55;
    DrawText("Accessories", col3_x, acc_start_y, 15, COLOR_BONE_WHITE);
    for (int i = 0; i < acc_count; i++) {
        int slot_idx = 1 + i;
        Color accessoryTextColor =
            (equipMenu->mode == SLOT_NONE && equipMenu->activeSlot == slot_idx)
                ? COLOR_RED_OCHRE
                : COLOR_BONE_WHITE;
        char *accName =
            session->player->gear.accessory_ids[i] == -1
                ? "------------"
                : GetName(ENTITY_ITEM, session->player->gear.accessory_ids[i]);
        DrawText(accName, col3_x, acc_start_y + 22 + (i * 22), 16,
                 accessoryTextColor);
    }

    // Tarot Slots
    int tarot_start_y = acc_start_y + 22 + (acc_count * 22) + 15;
    DrawText("Tarot Cards", col3_x, tarot_start_y, 15, COLOR_BONE_WHITE);
    for (int i = 0; i < 3; i++) {
        int slot_idx = 1 + acc_count + i;
        Color tarotTextColor =
            (equipMenu->mode == SLOT_NONE && equipMenu->activeSlot == slot_idx)
                ? COLOR_RED_OCHRE
                : COLOR_BONE_WHITE;
        int tarot_id = session->player->gear.tarot_ids[i];
        char *tarotName =
            tarot_id == -1 ? "------------" : GetName(ENTITY_ITEM, tarot_id);
        DrawText(tarotName, col3_x, tarot_start_y + 22 + (i * 22), 16,
                 tarotTextColor);
    }

    // --- Column 4: Picker Panel (Shifted right to prevent overlap) ---
    if (equipMenu->mode == SLOT_WEAPON || equipMenu->mode == SLOT_ACCESSORY ||
        equipMenu->mode == SLOT_TAROT) {
        int col4_x = 720; // Shifted right to avoid Column 3 name text overlap
        int col4_y = 80;
        int col4_width = 300;
        int col4_height = SCREEN_HEIGHT - 120;

        // Draw a background panel box container
        DrawRectangle(col4_x - 15, col4_y - 10, col4_width, col4_height,
                      Fade(COLOR_DUSTY_ROSE, 0.8f));
        DrawRectangleLines(col4_x - 15, col4_y - 10, col4_width, col4_height,
                           COLOR_BONE_WHITE);

        DrawText(equipMenu->mode == SLOT_WEAPON ? "SELECT MINERAL"
                                                : "SELECT ITEM",
                 col4_x, col4_y, 15, COLOR_BONE_WHITE);

        Color selectedColor =
            equipMenu->activeItemSlot == 0 ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
        DrawText("------------", col4_x, col4_y + 25, 16, selectedColor);

        // Loop through items with tight vertical spacing
        for (int i = 0; i < equipMenu->count; i++) {
            selectedColor = equipMenu->activeItemSlot == (i + 1)
                                ? COLOR_RED_OCHRE
                                : COLOR_BONE_WHITE;
            int display_id = -1;
            if (equipMenu->mode == SLOT_WEAPON) {
                int grid_idx = equipMenu->itemIds[i];
                display_id = grid->slots[grid_idx].weapon_id;
            } else {
                display_id = equipMenu->itemIds[i];
            }

            DrawText(GetName(ENTITY_ITEM, display_id), col4_x,
                     col4_y + 50 + (i * 22), 16, selectedColor);
        }
    }
}

void UpdateEquipMenu(PlaySession *session, Input *input) {
    EquipMenu *menu = &session->equip_menu;
    int acc_count = session->player->stats.current[STAT_ACCESORY_COUNT];
    int max_slots = 1 + acc_count + 3 - 1; // Weapon (0) + Accessories + Tarot

    // State 1: Browsing the equip slots
    if (menu->mode == SLOT_NONE) {
        if (input->buttons_pressed & KEY_W_PRESSED) {
            menu->activeSlot--;
            if (menu->activeSlot < 0) {
                menu->activeSlot = max_slots;
            }
        }
        if (input->buttons_pressed & KEY_S_PRESSED) {
            menu->activeSlot++;
            if (menu->activeSlot > max_slots) {
                menu->activeSlot = 0;
            }
        }

        // Press Enter to open the picker for the active slot
        if (input->buttons_pressed & ENTER_PRESSED) {
            menu->count = 0;
            menu->activeItemSlot = 0;

            if (menu->activeSlot == 0) {
                // Populate picker with active weapon grid slots from WeaponGrid
                menu->mode = SLOT_WEAPON;
                WeaponGrid *grid = &session->player->weapon_grid;
                for (int i = 0; i < 25; i++) {
                    if (grid->slots[i].active) {
                        menu->itemIds[menu->count++] =
                            i; // store the grid slot index
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

        if (input->buttons_pressed & KEY_G_PRESSED) {
            session->state = ADVENTURE_STATE;
        }
    }
    // State 2: Selecting an item from the filtered list (or unequipping)
    else if (menu->mode == SLOT_WEAPON || menu->mode == SLOT_ACCESSORY ||
             menu->mode == SLOT_TAROT) {
        int max_item_slot = menu->count;

        if (input->buttons_pressed & KEY_W_PRESSED) {
            menu->activeItemSlot--;
            if (menu->activeItemSlot < 0) {
                menu->activeItemSlot = max_item_slot;
            }
        }
        if (input->buttons_pressed & KEY_S_PRESSED) {
            menu->activeItemSlot++;
            if (menu->activeItemSlot > max_item_slot) {
                menu->activeItemSlot = 0;
            }
        }

        if (input->buttons_pressed & ENTER_PRESSED) {
            WeaponGrid *grid = &session->player->weapon_grid;

            if (menu->mode == SLOT_WEAPON) {
                if (menu->activeItemSlot == 0) {
                    // Selecting "------------" sets activeIndex to -1
                    grid->activeIndex = -1;
                } else {
                    int chosen_grid_idx =
                        menu->itemIds[menu->activeItemSlot - 1];
                    grid->activeIndex = chosen_grid_idx;
                }
            } else if (menu->mode == SLOT_ACCESSORY) {
                int acc_index = menu->activeSlot - 1;
                if (menu->activeItemSlot == 0) {
                    PlayerUnequipAccessory(session->player, acc_index);
                } else {
                    int chosen_item_id =
                        menu->itemIds[menu->activeItemSlot - 1];
                    PlayerEquipAccessory(session->player, acc_index,
                                           chosen_item_id);
                }
            } else if (menu->mode == SLOT_TAROT) {
                int tarot_index = menu->activeSlot - (1 + acc_count);
                if (menu->activeItemSlot == 0) {
                    PlayerUnequipTarot(session->player, tarot_index);
                } else {
                    int chosen_item_id =
                        menu->itemIds[menu->activeItemSlot - 1];
                    PlayerEquipTarot(session->player, tarot_index,
                                     chosen_item_id);
                }
            }

            menu->mode = SLOT_NONE;
        }

        if (input->buttons_pressed & BACKSPACE_PRESSED) {
            menu->mode = SLOT_NONE;
        }
    }
}
