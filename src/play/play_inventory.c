#include "play/play_inventory.h"
#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "engine/palette.h"
#include "play/play_session.h"
#include "registry/mineral_register.h"
#include "registry/register.h"
#include "systems/player.h"
#include "ui/menu.h"
#include "systems/gear.h"
#include <stdio.h>

void DrawInventory(Menu* menu) {
    if (!menu) return;

    MenuRenderData render_data = {
        .title = "ARCHAEOLOGY LOG",
        .empty_message = "YOUR LOGBOOK IS EMPTY...",
        .count = menu->count,
        .selected = menu->selected,
        .lore_title = "ITEM LORE:",
        .context_tag = "",
        .effect_count = 0
    };

    // Stack-allocated buffers to prevent heap allocation during drawing loops
    char label_buffers[64][40];
    const char* label_ptrs[64];

    char effect_buffers[16][32];
    const char* effect_ptrs[16];

    // Build item list labels
    for (int i = 0; i < menu->count && i < 64; i++) {
        int itemId = menu->itemIds[i];
        int count = PLAYER->item_inventory[itemId];

        snprintf(label_buffers[i], sizeof(label_buffers[i]), "%s x%d", GetName(menu->type, itemId), count);
        label_ptrs[i] = label_buffers[i];
    }
    render_data.item_labels = label_ptrs;

    // Build selected item payload
    if (menu->count > 0 && menu->selected < menu->count && menu->type == ENTITY_ITEM) {
        int selectedId = menu->itemIds[menu->selected];
        ItemDefinition* item = &ITEM_REGISTRY[selectedId];

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
            snprintf(context_buf, sizeof(context_buf), "[TEMP BUFF - %.0fs]", item->use_duration);
        } else if (item->use_type == USE_PERM_BOOST) {
            snprintf(context_buf, sizeof(context_buf), "[PERMANENT]");
        } else if (item->use_type == USE_RESTORE_HP) {
            snprintf(context_buf, sizeof(context_buf), "[CONSUMABLE]");
        }
        render_data.context_tag = context_buf;

        // Effect strings logic (Skip stat/HP display for Tarot cards per your design)
        int eff_idx = 0;

        if (item->slot != SLOT_TAROT) {
            if (item->hp_bonus != 0 && eff_idx < 16) {
                snprintf(effect_buffers[eff_idx], sizeof(effect_buffers[eff_idx]),
                         "HP %s%d", (item->hp_bonus > 0) ? "+" : "", item->hp_bonus);
                effect_ptrs[eff_idx] = effect_buffers[eff_idx];
                eff_idx++;
            }

            for (int s = 0; s < STAT_COUNT; s++) {
                int bonus = item->stat_bonuses[s];
                if (bonus != 0 && eff_idx < 16) {
                    snprintf(effect_buffers[eff_idx], sizeof(effect_buffers[eff_idx]),
                             "%s %s%d", STATS_NAMES[s], (bonus > 0) ? "+" : "", bonus);
                    effect_ptrs[eff_idx] = effect_buffers[eff_idx];
                    eff_idx++;
                }
            }
        }

        render_data.effect_lines = effect_ptrs;
        render_data.effect_count = eff_idx;
    }

    // 1. Draw the base menu logbook
    DrawMenu(&render_data);

    // 2. Render Sub-State Overlay Prompts
    if (menu->sub_state == ITEM_MENU_PROMPT_WEAPON) {
        int boxW = 340;
        int boxH = 110;
        int boxX = (SCREEN_WIDTH - boxW) / 2;
        int boxY = (SCREEN_HEIGHT - boxH) / 2;

        Gear* gear = &PLAYER->gear;
        const char* current_weapon_name = (gear->weapon_id != -1) ? GetName(ENTITY_ITEM, gear->weapon_id) : "NONE";

        char prompt_title[64];
        snprintf(prompt_title, sizeof(prompt_title), "REPLACE %s?", current_weapon_name);

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(COLOR_SUNKEN_INK, 0.4f));
        DrawRectangle(boxX, boxY, boxW, boxH, COLOR_PULP_PAPER);
        DrawRectangleLines(boxX, boxY, boxW, boxH, COLOR_SUNKEN_INK);

        DrawText(prompt_title, boxX + 20, boxY + 20, 18, COLOR_SUNKEN_INK);

        Color yesColor = (menu->prompt_selected == 0) ? COLOR_JADE : COLOR_SUNKEN_INK;
        Color noColor  = (menu->prompt_selected == 1) ? COLOR_JADE : COLOR_SUNKEN_INK;

        DrawText((menu->prompt_selected == 0) ? "> YES" : "  YES", boxX + 50, boxY + 65, 20, yesColor);
        DrawText((menu->prompt_selected == 1) ? "> NO"  : "  NO",  boxX + 180, boxY + 65, 20, noColor);
    }
    else if (menu->sub_state == ITEM_MENU_PROMPT_ACC) {
        int max_slots = PLAYER->stats.current[STAT_ACCESORY_COUNT];
        int boxW = 360;
        int boxH = 60 + (max_slots * 25);
        int boxX = (SCREEN_WIDTH - boxW) / 2;
        int boxY = (SCREEN_HEIGHT - boxH) / 2;

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(COLOR_SUNKEN_INK, 0.4f));
        DrawRectangle(boxX, boxY, boxW, boxH, COLOR_PULP_PAPER);
        DrawRectangleLines(boxX, boxY, boxW, boxH, COLOR_SUNKEN_INK);

        DrawText("REPLACE WHICH ACCESSORY?", boxX + 20, boxY + 15, 18, COLOR_SUNKEN_INK);

        Gear* gear = &PLAYER->gear;
        for (int i = 0; i < max_slots && i < 4; i++) {
            int acc_id = gear->accessory_ids[i];
            const char* acc_name = (acc_id != -1) ? GetName(ENTITY_ITEM, acc_id) : "[EMPTY]";
            Color slotColor = (i == menu->prompt_selected) ? COLOR_JADE : COLOR_SUNKEN_INK;

            char slot_str[64];
            snprintf(slot_str, sizeof(slot_str), "%sSlot %d: %s",
                     (i == menu->prompt_selected) ? "> " : "  ", i + 1, acc_name);

            DrawText(slot_str, boxX + 25, boxY + 45 + (i * 22), 16, slotColor);
        }
    }
    else if (menu->sub_state == ITEM_MENU_PROMPT_TAROT) {
        int max_slots = 3;
        int boxW = 360;
        int boxH = 60 + (max_slots * 25);
        int boxX = (SCREEN_WIDTH - boxW) / 2;
        int boxY = (SCREEN_HEIGHT - boxH) / 2;

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(COLOR_SUNKEN_INK, 0.4f));
        DrawRectangle(boxX, boxY, boxW, boxH, COLOR_PULP_PAPER);
        DrawRectangleLines(boxX, boxY, boxW, boxH, COLOR_SUNKEN_INK);

        DrawText("EQUIP TO WHICH TAROT SLOT?", boxX + 20, boxY + 15, 18, COLOR_SUNKEN_INK);

        Gear* gear = &PLAYER->gear;
        for (int i = 0; i < max_slots; i++) {
            int tarot_id = gear->tarot_ids[i];
            const char* tarot_name = (tarot_id != -1) ? GetName(ENTITY_ITEM, tarot_id) : "[EMPTY]";
            Color slotColor = (i == menu->prompt_selected) ? COLOR_JADE : COLOR_SUNKEN_INK;

            char slot_str[64];
            snprintf(slot_str, sizeof(slot_str), "%sSlot %d: %s",
                     (i == menu->prompt_selected) ? "> " : "  ", i + 1, tarot_name);

            DrawText(slot_str, boxX + 25, boxY + 45 + (i * 22), 16, slotColor);
        }
    }
}

void DrawLevelInventory() {
    ClearBackground(COLOR_PULP_PAPER);

    int margin = 50;
    int uiWidth = SCREEN_WIDTH - (margin * 2);

    DrawText("PORTAL LOG", margin, 40, 30, COLOR_SUNKEN_INK);
    DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK);

    int levelCount = GetEntityTypeCount(ENTITY_PORTAL);
    for (int i = 0; i < levelCount; i++) {
        Color textColor = COLOR_MOONGLOW;
        DrawText(GetName(ENTITY_PORTAL, i), 100, 120 + (i * 30), 20, textColor);
    }
}

void DrawMineralInventory(Player* player) {
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

void UpdateInventory(PlaySession* session, Input* input) {
    Menu* menu = &session->menu;
    Player* player = session->player;

    // --- SUB-STATE 1: Prompting for Weapon Swap (YES / NO) ---
    if (menu->sub_state == ITEM_MENU_PROMPT_WEAPON) {
        if (input->buttons_pressed & KEY_W_PRESSED || input->buttons_pressed & KEY_S_PRESSED) {
            menu->prompt_selected = !menu->prompt_selected;
        }
        if (input->buttons_pressed & BACKSPACE_PRESSED) {
            menu->sub_state = ITEM_MENU_BROWSE;
            return;
        }
        if (input->buttons_pressed & ENTER_PRESSED) {
            if (menu->prompt_selected == 0) {
                PlayerEquipWeapon(player, menu->pending_item_id);
                RebindItemMenu(session);
            }
            menu->sub_state = ITEM_MENU_BROWSE;
        }
        return;
    }

    // --- SUB-STATE 2: Prompting for Accessory Slot Selection ---
    if (menu->sub_state == ITEM_MENU_PROMPT_ACC) {
        int max_slots = player->stats.current[STAT_ACCESORY_COUNT];

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
            int current_acc_id = player->gear.accessory_ids[target_slot];

            // Prevent replacing an accessory that grants extra accessory slots
            if (current_acc_id != -1) {
                ItemDefinition* current_acc = &ITEM_REGISTRY[current_acc_id];
                if (current_acc->stat_bonuses[STAT_ACCESORY_COUNT] > 0) {
                    return;
                }
            }

            PlayerEquipAccessory(player, target_slot, menu->pending_item_id);
            RebindItemMenu(session);
            menu->sub_state = ITEM_MENU_BROWSE;
        }
        return;
    }

    // --- SUB-STATE 3: Prompting for Tarot Slot Selection ---
    if (menu->sub_state == ITEM_MENU_PROMPT_TAROT) {
        int max_slots = 3;

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
            PlayerEquipTarot(player, target_slot, menu->pending_item_id);
            RebindItemMenu(session);
            menu->sub_state = ITEM_MENU_BROWSE;
        }
        return;
    }

    // --- SUB-STATE 0: Normal Item Browsing ---
    if (!UpdateMenu(menu, input)) {
        session->state = ADVENTURE;
        return;
    }

    if (input->buttons_pressed & ENTER_PRESSED) {
        if (menu->count == 0) return;

        int selected = menu->selected;
        int itemId = menu->itemIds[selected];
        ItemDefinition* item = &ITEM_REGISTRY[itemId];

        // 1. Usable Consumable
        if (item->use_type != 0) {
            UseItem(player, item);
            RebindItemMenu(session);
            selected = (selected >= menu->count) ? menu->count - 1 : selected;
            menu->selected = (selected >= 0) ? selected : 0;
            return;
        }

        // 2. Weapons
        if (item->slot == SLOT_WEAPON) {
            if (player->gear.weapon_id != -1) {
                menu->sub_state = ITEM_MENU_PROMPT_WEAPON;
                menu->pending_item_id = itemId;
                menu->prompt_selected = 0;
            } else {
                PlayerEquipWeapon(player, itemId);
                RebindItemMenu(session);
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
                RebindItemMenu(session);
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
                RebindItemMenu(session);
            } else {
                menu->sub_state = ITEM_MENU_PROMPT_TAROT;
                menu->pending_item_id = itemId;
                menu->prompt_selected = 0;
            }
            return;
        }
    }
}

void UpdateMineralInventory(PlaySession* session, Input* input) {
    if (input->buttons_pressed & KEY_M_PRESSED) {
        session->state = ADVENTURE;
    }
}

void UpdateLevelInventory(PlaySession* session, Input* input) {
    if (input->buttons_pressed & KEY_P_PRESSED) {
        session->state = ADVENTURE;
    }
}
