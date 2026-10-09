#include "ui/vhs_inventory.h"
#include "engine/palette.h"
#include "registry/register.h"
#include <stdio.h>
#include <string.h>
#include "defs/types_entities.h"
#include "ui/menu.h"


void RebindVHSMenu(Menu* menu, Player* player) {
    menu->type = ENTITY_VHS;
    menu->exit_button = KEY_V_PRESSED; // Change to whatever key opens your VHS menu

    int active_item_ids[100];
    int active_count = 0;

    // Gather all non-zero VHS IDs from the player's inventory
    for (int i = 0; i < GetEntityTypeCount(ENTITY_VHS); i++) {
        if (player->vhs_inventory[i] > 0) {
            active_item_ids[active_count++] = i;
        }
    }

    FillMenu(menu, active_item_ids, active_count);
}

void DrawVHSInventory(Menu* menu) {
    if (!menu) return;

    MenuRenderData render_data = {
        .title = "VHS TAPE COLLECTION",
        .empty_message = "YOU HAVE NO VHS TAPES...",
        .count = menu->count,
        .selected = menu->selected,
        .lore_title = "TAPE DETAILS:",
        .lore_body = "",
        .context_tag = "",
        .effect_count = 0
    };

    // Stack-allocated buffers to prevent heap allocation during drawing loops
    char label_buffers[64][40];
    const char* label_ptrs[64];

    // Build VHS list labels
    for (int i = 0; i < menu->count && i < 64; i++) {
        int vhsId = menu->itemIds[i];
        int count = PLAYER->vhs_inventory[vhsId];

        snprintf(label_buffers[i], sizeof(label_buffers[i]), "%s x%d", GetName(menu->type, vhsId), count);
        label_ptrs[i] = label_buffers[i];
    }
    render_data.item_labels = label_ptrs;

    // Build selected tape payload for the side panel/lore area
    if (menu->count > 0 && menu->selected < menu->count && menu->type == ENTITY_VHS) {
        int selectedId = menu->itemIds[menu->selected];
        int portal_id = VHS_REGISTRY[selectedId].portal_id;

        // Fetch description from the registry
        render_data.lore_body = VHS_REGISTRY[selectedId].description;

        static char context_buf[64];

        // Portal reveal logic: if not discovered/unlocked, it's ???
        // (Note: if PORTAL_REGISTRY is an array of structs, you may need to append
        // a specific flag here like !PORTAL_REGISTRY[portal_id].is_discovered depending on your engine)
        if (!PORTAL_REGISTRY[portal_id].locked) {
            snprintf(context_buf, sizeof(context_buf), "DESTINATION: %s", GetName(ENTITY_PORTAL, portal_id));
        } else {
            snprintf(context_buf, sizeof(context_buf), "DESTINATION: ???");
        }

        render_data.context_tag = context_buf;
    }

    // Draw the base menu logbook (Feel free to swap COLOR_CELADON for something staticy or purple)
    DrawMenu(&render_data, COLOR_MUTED_FUCHSIA);
}

void UpdateVHSInventory(PlaySession* session, Input* input) {
    Menu* menu = &session->menu;

    // --- Normal VHS Menu Browsing ---
    if (!UpdateMenu(menu, input)) {
        session->state = ADVENTURE_STATE;
        return;
    }

    // Handle pressing ENTER on a specific VHS tape
    if (input->buttons_pressed & ENTER_PRESSED) {
        if (menu->count == 0) return;

        int selected = menu->selected;
        int vhsId = menu->itemIds[selected];

        // Future logic for playing the tape, setting a waypoint, or fast-traveling
        printf("Selected VHS ID: %d (Portal ID: %d)\n", vhsId, VHS_REGISTRY[vhsId].portal_id);
    }
}
