#include "ui/equip_menu.h"

#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"
#include "engine/palette.h"
#include "registry/register.h"
#include "systems/player.h"

void DrawEquipmentMenu(PlaySession* session, EquipMenu* equipMenu){
      // 1. Background - The Aged Paper
      ClearBackground(COLOR_DUSTY_ROSE);

      // Dynamic horizontal line based on screen width
      int margin = 50;
      int uiWidth = SCREEN_WIDTH - (margin * 2);

      // 2. Title - The "Ink" look
      DrawText("Equipment", margin, 40, 30, COLOR_BONE_WHITE);
      DrawRectangle(50, 80, uiWidth, 2, COLOR_BONE_WHITE); // A simple line

      Color weaponTextColor = (equipMenu->mode == SLOT_NONE && equipMenu->activeSlot == 0) ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
      DrawText("Weapon", 250,90,15,COLOR_BONE_WHITE);
      char* weaponName = session->player->gear.weapon_id == -1 ? "------------" : GetName(ENTITY_ITEM, session->player->gear.weapon_id);
      DrawText(weaponName, 250,120,15,weaponTextColor);

      DrawText("Current Accesories", 450,90,15,COLOR_BONE_WHITE);
      for(int i = 0; i < session->player->stats.base[STAT_ACCESORY_COUNT]; i++){
          Color accesoryTextColor = (equipMenu->mode == SLOT_NONE && equipMenu->activeSlot == i + 1) ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
          if(session->player->gear.accessory_ids[i] == -1){
              DrawText("------------", 450, 120 + (i * 30), 20, accesoryTextColor);
              continue;
          }
          DrawText(GetName(ENTITY_ITEM, session->player->gear.accessory_ids[i]), 450, 120 + (i * 30), 20, accesoryTextColor);
      }

      // Draw item selection column if we are choosing an item for a weapon or accessory slot
      if(equipMenu->mode == SLOT_WEAPON || equipMenu->mode == SLOT_ACCESSORY){
          // Index 0 is the "unequip" option ("------------")
          Color selectedColor = equipMenu->activeItemSlot == 0 ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
          DrawText("------------", 650, 90, 20, selectedColor);

          // Indices 1 to count map to the filtered item list
          for(int i = 0; i < equipMenu->count; i++){
              selectedColor = equipMenu->activeItemSlot == (i + 1) ? COLOR_RED_OCHRE : COLOR_BONE_WHITE;
              DrawText(GetName(ENTITY_ITEM, equipMenu->itemIds[i]), 650, 120 + (i * 30), 20, selectedColor);
          }
      }
}

void UpdateEquipMenu(PlaySession* session, Input* input) {
    EquipMenu* menu = &session->equip_menu;

    // State 1: Browsing the equip slots (Weapon or Accessories)
    if (menu->mode == SLOT_NONE) {
        if (input->buttons_pressed & KEY_W_PRESSED) {
            menu->activeSlot--;
            int max_slots = session->player->stats.base[STAT_ACCESORY_COUNT];
            if (menu->activeSlot < 0) {
                menu->activeSlot = max_slots; // wrap to last accessory slot
            }
        }
        if (input->buttons_pressed & KEY_S_PRESSED) {
            menu->activeSlot++;
            int max_slots = session->player->stats.base[STAT_ACCESORY_COUNT];
            if (menu->activeSlot > max_slots) {
                menu->activeSlot = 0; // wrap back to weapon
            }
        }

        // Press Enter to open the inventory filter for the active slot
        if (input->buttons_pressed & ENTER_PRESSED) {
            menu->count = 0;
            menu->activeItemSlot = 0; // Reset item selection cursor to top ("------------")

            if (menu->activeSlot == 0) {
                menu->mode = SLOT_WEAPON;
                // Filter inventory for weapons
                for (int i = 0; i < session->player->inventory.count; i++) {
                    int item_id = session->player->inventory.itemIds[i];
                    if (GetAccesorySlot(ENTITY_ITEM, item_id) == SLOT_WEAPON) {
                        menu->itemIds[menu->count++] = item_id;
                    }
                }
            } else {
                menu->mode = SLOT_ACCESSORY;
                // Filter inventory for accessories
                for (int i = 0; i < session->player->inventory.count; i++) {
                    int item_id = session->player->inventory.itemIds[i];
                    if (GetAccesorySlot(ENTITY_ITEM, item_id) == SLOT_ACCESSORY) {
                        menu->itemIds[menu->count++] = item_id;
                    }
                }
            }
        }

        if (input->buttons_pressed & BACKSPACE_PRESSED) {
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
            if (menu->mode == SLOT_WEAPON) {
                if (menu->activeItemSlot == 0) {
                    // Chosen "------------" -> Unequip weapon entirely
                    if (session->player->gear.weapon_id != -1) {
                        GiveItem(
                            session->player, session->player->gear.weapon_id);
                        session->player->gear.weapon_id = -1;
                    }
                } else {
                    // Chosen a specific item from the list (adjust index by -1 because 0 is unequip)
                    int chosen_item_id = menu->itemIds[menu->activeItemSlot - 1];

                    // Return old weapon to inventory if equipped
                    if (session->player->gear.weapon_id != -1) {
                        GiveItem(session->player, session->player->gear.weapon_id);
                    }
                    // Equip new weapon
                    session->player->gear.weapon_id = chosen_item_id;

                    // Remove chosen item from raw inventory bag
                    for (int i = 0; i < session->player->inventory.count; i++) {
                        if (session->player->inventory.itemIds[i] == chosen_item_id) {
                            RemoveItemAt(session->player, i);
                            break;
                        }
                    }
                }
            }
            else if (menu->mode == SLOT_ACCESSORY) {
                int acc_index = menu->activeSlot - 1; // Map activeSlot to accessory array index

                if (menu->activeItemSlot == 0) {
                    // Chosen "------------" -> Unequip this accessory slot
                    if (session->player->gear.accessory_ids[acc_index] != -1) {
                        GiveItem(session->player, session->player->gear.accessory_ids[acc_index]);
                        session->player->gear.accessory_ids[acc_index] = -1;
                    }
                } else {
                    // Chosen a specific accessory item
                    int chosen_item_id = menu->itemIds[menu->activeItemSlot - 1];

                    // Return old accessory to inventory if present
                    if (session->player->gear.accessory_ids[acc_index] != -1) {
                        GiveItem(session->player, session->player->gear.accessory_ids[acc_index]);
                    }
                    // Equip new accessory
                    session->player->gear.accessory_ids[acc_index] = chosen_item_id;

                    // Remove chosen item from raw inventory bag
                    for (int i = 0; i < session->player->inventory.count; i++) {
                        if (session->player->inventory.itemIds[i] == chosen_item_id) {
                            RemoveItemAt(session->player, i);
                            break;
                        }
                    }
                }
            }

            // Recalculate stats and return to slot-browsing mode
            RecalculateStats(&session->player->stats, &session->player->gear);
            menu->mode = SLOT_NONE;
        }

        // Backspace cancels out of item selection back to slot view
        if (input->buttons_pressed & BACKSPACE_PRESSED) {
            menu->mode = SLOT_NONE;
        }
    }
}
