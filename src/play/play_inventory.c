#include "play/play_inventory.h"
#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "engine/palette.h"
#include "play/play_session.h"
#include "registry/mineral_register.h"
#include "registry/register.h"
#include "systems/gear.h"
#include "systems/player.h"
#include "systems/weapon_grid.h"
#include "ui/menu.h"
#include <stdio.h>

void UpdateInventory(PlaySession *session, Input *input) {
    Menu *menu = &session->menu;
    Player *player = session->player;

    // --- SUB-STATE 1: Prompting for Weapon Slot Selection / Replacement ---
    if (menu->sub_state == ITEM_MENU_PROMPT_WEAPON) {
        WeaponGrid *grid = &player->weapon_grid;
        // Let's allow selecting among available slots or up to 25 slots (or grid->count / 25 limit)
        int max_slots = 25;

        if (input->buttons_pressed & KEY_W_PRESSED) {
            menu->prompt_selected = (menu->prompt_selected - 1 + max_slots) % max_slots;
        }
        if (input->buttons_pressed & KEY_S_PRESSED) {
            menu->prompt_selected = (menu->prompt_selected + 1) % max_slots;
        }
        if (input->buttons_pressed & BACKSPACE_PRESSED) {
            menu->sub_state = ITEM_MENU_BROWSE;
            return;
        }
        if (input->buttons_pressed & ENTER_PRESSED) {
            int target_slot = menu->prompt_selected;

            // If the slot is inactive, we can just add/activate it there. If active, it replaces it.
            grid->slots[target_slot].active = true;
            grid->slots[target_slot].weapon_id = menu->pending_item_id;
            // Reset minerals/plants on slot change if desired, or keep them. Let's call CalculateWeapon.
            CalculateWeapon(grid, target_slot);

            // Update grid count if it was previously inactive
            int active_count = 0;
            for(int i=0; i<25; i++) {
                if(grid->slots[i].active) active_count++;
            }
            grid->count = active_count;
            if (grid->activeIndex == -1) {
                grid->activeIndex = target_slot;
            }

            RebindItemMenu(menu, player);
            menu->sub_state = ITEM_MENU_BROWSE;
        }
        return;
    }

    // --- SUB-STATE 2: Prompting for Accessory Slot Selection ---
    if (menu->sub_state == ITEM_MENU_PROMPT_ACC) {
        int max_slots = player->stats.current[STAT_ACCESORY_COUNT];

        if (input->buttons_pressed & KEY_W_PRESSED) {
            menu->prompt_selected =
                (menu->prompt_selected - 1 + max_slots) % max_slots;
        }
        if (input->buttons_pressed & KEY_S_PRESSED) {
            menu->prompt_selected = (menu->prompt_selected + 1) % max_slots;
        }
        if (input->buttons_pressed & BACKSPACE_PRESSED) {
            menu->sub_state = ITEM_MENU_BROWSE;
            return;
        }
        if (input->buttons_pressed & ENTER_PRESSED) {
            int target_slot = menu->prompt_selected;
            int current_acc_id = player->gear.accessory_ids[target_slot];

            // Prevent replacing an accessory that grants extra accessory slots
            if (current_acc_id != -1) {
                ItemDefinition *current_acc = &ITEM_REGISTRY[current_acc_id];
                if (current_acc->stat_bonuses[STAT_ACCESORY_COUNT] > 0) {
                    return;
                }
            }

            PlayerEquipAccessory(player, target_slot, menu->pending_item_id);
            RebindItemMenu(menu, player);
            menu->sub_state = ITEM_MENU_BROWSE;
        }
        return;
    }

    // --- SUB-STATE 3: Prompting for Tarot Slot Selection ---
    if (menu->sub_state == ITEM_MENU_PROMPT_TAROT) {
        int max_slots = 3;

        if (input->buttons_pressed & KEY_W_PRESSED) {
            menu->prompt_selected =
                (menu->prompt_selected - 1 + max_slots) % max_slots;
        }
        if (input->buttons_pressed & KEY_S_PRESSED) {
            menu->prompt_selected = (menu->prompt_selected + 1) % max_slots;
        }
        if (input->buttons_pressed & BACKSPACE_PRESSED) {
            menu->sub_state = ITEM_MENU_BROWSE;
            return;
        }
        if (input->buttons_pressed & ENTER_PRESSED) {
            int target_slot = menu->prompt_selected;
            PlayerEquipTarot(player, target_slot, menu->pending_item_id);
            RebindItemMenu(menu, player);
            menu->sub_state = ITEM_MENU_BROWSE;
        }
        return;
    }

    // --- SUB-STATE 0: Normal Item Browsing ---
    if (!UpdateMenu(menu, input)) {
        session->state = ADVENTURE_STATE;
        return;
    }

    if (input->buttons_pressed & ENTER_PRESSED) {
        if (menu->count == 0)
            return;

        int selected = menu->selected;
        int itemId = menu->itemIds[selected];
        ItemDefinition *item = &ITEM_REGISTRY[itemId];

        // 1. Usable Consumable
        if (item->use_type != 0) {
            UseItem(player, item);
            RebindItemMenu(menu, player);
            selected = (selected >= menu->count) ? menu->count - 1 : selected;
            menu->selected = (selected >= 0) ? selected : 0;
            return;
        }

        // 2. Weapons (Using WeaponGrid)
        if (item->slot == SLOT_WEAPON) {
            WeaponGrid *grid = &player->weapon_grid;
            int empty_slot = -1;

            for (int i = 0; i < 25; i++) {
                if (!grid->slots[i].active) {
                    empty_slot = i;
                    break;
                }
            }

            if (empty_slot != -1) {
                // Automatically equip into the first available weapon grid slot
                grid->slots[empty_slot].active = true;
                grid->slots[empty_slot].weapon_id = itemId;
                CalculateWeapon(grid, empty_slot);
                grid->count++;
                if (grid->activeIndex == -1) {
                    grid->activeIndex = empty_slot;
                }
                RebindItemMenu(menu, player);
            } else {
                // If all 25 slots are full, prompt user to choose which slot to replace
                menu->sub_state = ITEM_MENU_PROMPT_WEAPON;
                menu->pending_item_id = itemId;
                menu->prompt_selected = 0;
            }
            return;
        }

        // 3. Accessories
        if (item->slot == SLOT_ACCESSORY) {
            int max_slots = player->stats.current[STAT_ACCESORY_COUNT];
            int empty_slot = -1;

            for (int i = 0; i < max_slots; i++) {
                if (player->gear.accessory_ids[i] == -1) {
                    empty_slot = i;
                    break;
                }
            }

            if (empty_slot != -1) {
                PlayerEquipAccessory(player, empty_slot, itemId);
                RebindItemMenu(menu, player);
            } else {
                menu->sub_state = ITEM_MENU_PROMPT_ACC;
                menu->pending_item_id = itemId;
                menu->prompt_selected = 0;
            }
            return;
        }

        // 4. Tarot Cards
        if (item->slot == SLOT_TAROT) {
            int empty_slot = -1;

            for (int i = 0; i < 3; i++) {
                if (player->gear.tarot_ids[i] == -1) {
                    empty_slot = i;
                    break;
                }
            }

            if (empty_slot != -1) {
                PlayerEquipTarot(player, empty_slot, itemId);
                RebindItemMenu(menu, player);
            } else {
                menu->sub_state = ITEM_MENU_PROMPT_TAROT;
                menu->pending_item_id = itemId;
                menu->prompt_selected = 0;
            }
            return;
        }
    }
}

void DrawInventory(Menu *menu) {
    if (!menu)
        return;

    MenuRenderData render_data = {.title = "ARCHAEOLOGY LOG",
                                  .empty_message = "YOUR LOGBOOK IS EMPTY...",
                                  .count = menu->count,
                                  .selected = menu->selected,
                                  .lore_title = "ITEM LORE:",
                                  .context_tag = "",
                                  .effect_count = 0};

    // Stack-allocated buffers to prevent heap allocation during drawing loops
    char label_buffers[64][40];
    const char *label_ptrs[64];

    char effect_buffers[16][32];
    const char *effect_ptrs[16];

    // Build item list labels
    for (int i = 0; i < menu->count && i < 64; i++) {
        int itemId = menu->itemIds[i];
        int count = PLAYER->item_inventory[itemId];

        snprintf(label_buffers[i], sizeof(label_buffers[i]), "%s x%d",
                 GetName(menu->type, itemId), count);
        label_ptrs[i] = label_buffers[i];
    }
    render_data.item_labels = label_ptrs;

    // Build selected item payload
    if (menu->count > 0 && menu->selected < menu->count &&
        menu->type == ENTITY_ITEM) {
        int selectedId = menu->itemIds[menu->selected];
        ItemDefinition *item = &ITEM_REGISTRY[selectedId];

        render_data.lore_body = GetDescription(menu->type, selectedId);

        // Context tag logic
        static char context_buf[32];
        context_buf[0] = '\0';

        if (item->slot == SLOT_WEAPON) {
            snprintf(context_buf, sizeof(context_buf), "[WEAPON]");
        } else if (item->slot == SLOT_ACCESSORY) {
            snprintf(context_buf, sizeof(context_buf), "[ACCESSORY]");
        } else if (item->slot == SLOT_TAROT) {
            snprintf(context_buf, sizeof(context_buf), "[TAROT]");
        } else if (item->use_type == USE_TEMP_BUFF) {
            snprintf(context_buf, sizeof(context_buf), "[TEMP BUFF - %.0fs]",
                     item->use_duration);
        } else if (item->use_type == USE_PERM_BOOST) {
            snprintf(context_buf, sizeof(context_buf), "[PERMANENT]");
        } else if (item->use_type == USE_RESTORE_HP) {
            snprintf(context_buf, sizeof(context_buf), "[CONSUMABLE]");
        }
        render_data.context_tag = context_buf;

        // Effect strings logic
        int eff_idx = 0;

        if (item->slot != SLOT_TAROT) {
            if (item->hp_bonus != 0 && eff_idx < 16) {
                snprintf(effect_buffers[eff_idx],
                         sizeof(effect_buffers[eff_idx]), "HP %s%d",
                         (item->hp_bonus > 0) ? "+" : "", item->hp_bonus);
                effect_ptrs[eff_idx] = effect_buffers[eff_idx];
                eff_idx++;
            }

            if (item->mp_bonus != 0 && eff_idx < 16) {
                snprintf(effect_buffers[eff_idx],
                         sizeof(effect_buffers[eff_idx]), "MP %s%d",
                         (item->mp_bonus > 0) ? "+" : "", item->mp_bonus);
                effect_ptrs[eff_idx] = effect_buffers[eff_idx];
                eff_idx++;
            }

            for (int s = 0; s < STAT_COUNT; s++) {
                int bonus = item->stat_bonuses[s];
                if (bonus != 0 && eff_idx < 16) {
                    snprintf(effect_buffers[eff_idx],
                             sizeof(effect_buffers[eff_idx]), "%s %s%d",
                             STATS_NAMES[s], (bonus > 0) ? "+" : "", bonus);
                    effect_ptrs[eff_idx] = effect_buffers[eff_idx];
                    eff_idx++;
                }
            }
        }

        render_data.effect_lines = effect_ptrs;
        render_data.effect_count = eff_idx;
    }

    // 1. Draw the base menu logbook
    DrawMenu(&render_data, COLOR_SNOOT_PINK);

    // 2. Render Sub-State Overlay Prompts
    if (menu->sub_state == ITEM_MENU_PROMPT_WEAPON) {
        int max_slots = 25;
        int boxW = 380;
        int boxH = 60 + (6 * 22); // Show a scrollable-like list of slots
        int boxX = (SCREEN_WIDTH - boxW) / 2;
        int boxY = (SCREEN_HEIGHT - boxH) / 2;

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                      Fade(COLOR_SUNKEN_INK, 0.4f));
        DrawRectangle(boxX, boxY, boxW, boxH, COLOR_PULP_PAPER);
        DrawRectangleLines(boxX, boxY, boxW, boxH, COLOR_SUNKEN_INK);

        DrawText("EQUIP TO WHICH WEAPON SLOT?", boxX + 20, boxY + 15, 16,
                 COLOR_SUNKEN_INK);

        WeaponGrid *grid = &PLAYER->weapon_grid;
        // Display a window around prompt_selected for visibility
        int start_idx = menu->prompt_selected - 2;
        if (start_idx < 0) start_idx = 0;
        if (start_idx > max_slots - 5) start_idx = max_slots - 5;
        if (start_idx < 0) start_idx = 0;

        int display_count = 5;
        for (int i = 0; i < display_count && (start_idx + i) < max_slots; i++) {
            int slot_idx = start_idx + i;
            bool active = grid->slots[slot_idx].active;
            int wid = grid->slots[slot_idx].weapon_id;
            const char *w_name = active ? GetName(ENTITY_ITEM, wid) : "[EMPTY]";

            Color slotColor = (slot_idx == menu->prompt_selected) ? COLOR_JADE : COLOR_SUNKEN_INK;

            char slot_str[64];
            snprintf(slot_str, sizeof(slot_str), "%sSlot %d: %s",
                     (slot_idx == menu->prompt_selected) ? "> " : "  ", slot_idx + 1,
                     w_name);

            DrawText(slot_str, boxX + 25, boxY + 45 + (i * 22), 16, slotColor);
        }
    } else if (menu->sub_state == ITEM_MENU_PROMPT_ACC) {
        int max_slots = PLAYER->stats.current[STAT_ACCESORY_COUNT];
        int boxW = 360;
        int boxH = 60 + (max_slots * 25);
        int boxX = (SCREEN_WIDTH - boxW) / 2;
        int boxY = (SCREEN_HEIGHT - boxH) / 2;

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                      Fade(COLOR_SUNKEN_INK, 0.4f));
        DrawRectangle(boxX, boxY, boxW, boxH, COLOR_PULP_PAPER);
        DrawRectangleLines(boxX, boxY, boxW, boxH, COLOR_SUNKEN_INK);

        DrawText("REPLACE WHICH ACCESSORY?", boxX + 20, boxY + 15, 18,
                 COLOR_SUNKEN_INK);

        Gear *gear = &PLAYER->gear;
        for (int i = 0; i < max_slots && i < 4; i++) {
            int acc_id = gear->accessory_ids[i];
            const char *acc_name =
                (acc_id != -1) ? GetName(ENTITY_ITEM, acc_id) : "[EMPTY]";
            Color slotColor =
                (i == menu->prompt_selected) ? COLOR_JADE : COLOR_SUNKEN_INK;

            char slot_str[64];
            snprintf(slot_str, sizeof(slot_str), "%sSlot %d: %s",
                     (i == menu->prompt_selected) ? "> " : "  ", i + 1,
                     acc_name);

            DrawText(slot_str, boxX + 25, boxY + 45 + (i * 22), 16, slotColor);
        }
    } else if (menu->sub_state == ITEM_MENU_PROMPT_TAROT) {
        int max_slots = 3;
        int boxW = 360;
        int boxH = 60 + (max_slots * 25);
        int boxX = (SCREEN_WIDTH - boxW) / 2;
        int boxY = (SCREEN_HEIGHT - boxH) / 2;

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                      Fade(COLOR_SUNKEN_INK, 0.4f));
        DrawRectangle(boxX, boxY, boxW, boxH, COLOR_PULP_PAPER);
        DrawRectangleLines(boxX, boxY, boxW, boxH, COLOR_SUNKEN_INK);

        DrawText("EQUIP TO WHICH TAROT SLOT?", boxX + 20, boxY + 15, 18,
                 COLOR_SUNKEN_INK);

        Gear *gear = &PLAYER->gear;
        for (int i = 0; i < max_slots; i++) {
            int tarot_id = gear->tarot_ids[i];
            const char *tarot_name =
                (tarot_id != -1) ? GetName(ENTITY_ITEM, tarot_id) : "[EMPTY]";
            Color slotColor =
                (i == menu->prompt_selected) ? COLOR_JADE : COLOR_SUNKEN_INK;

            char slot_str[64];
            snprintf(slot_str, sizeof(slot_str), "%sSlot %d: %s",
                     (i == menu->prompt_selected) ? "> " : "  ", i + 1,
                     tarot_name);

            DrawText(slot_str, boxX + 25, boxY + 45 + (i * 22), 16, slotColor);
        }
    }
}

void UpdateMineralInventory(PlaySession *session, Input *input) {
    if (input->buttons_pressed & KEY_M_PRESSED) {
        session->state = ADVENTURE_STATE;
    }
}

void DrawMineralInventory(Player *player) {
    ClearBackground(COLOR_PULP_PAPER);

    int margin = 50;
    int uiWidth = SCREEN_WIDTH - (margin * 2);

    DrawText("GEOLOGY LOG", margin, 40, 30, COLOR_SUNKEN_INK);
    DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK);

    for (int i = 0; i < MINERAL_COUNT; i++) {
        Color textColor = GetMineralColor(i);
        char count[8];
        snprintf(count, sizeof(count), "%d", player->mineral_inventory[i]);

        DrawMineral(i, (Vector2){50, 120 + (i * 30) + 12});
        DrawText(GetMineralLabel(i), 100, 120 + (i * 30), 20, textColor);
        DrawText(count, 250, 120 + (i * 30), 20, textColor);
    }
}
