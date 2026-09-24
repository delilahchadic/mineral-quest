#include "ui/plant_inventory.h"
#include "engine/palette.h"
#include "registry/register.h"
#include <stdio.h>
#include "defs/types_entities.h"
#include "ui/menu.h"

void DrawPlantInventory(Menu* menu) {
    if (!menu) return;

    MenuRenderData render_data = {
        .title = "BOTANY LOG",
        .empty_message = "YOUR BOTANY LOG IS EMPTY...",
        .count = menu->count,
        .selected = menu->selected,
        .lore_title = "PLANT DETAILS:",
        .lore_body = "",
        .context_tag = "",
        .effect_count = 0
    };

    // Stack-allocated buffers to prevent heap allocation during drawing loops
    char label_buffers[64][40];
    const char* label_ptrs[64];

    // Build plant list labels (using menu->itemIds to store plant IDs)
    for (int i = 0; i < menu->count && i < 64; i++) {
        int plantId = menu->itemIds[i];
        int count = PLAYER->plant_inventory[plantId];

        snprintf(label_buffers[i], sizeof(label_buffers[i]), "%s x%d", GetName(menu->type, plantId), count);
        label_ptrs[i] = label_buffers[i];
    }
    render_data.item_labels = label_ptrs;

    // Build selected plant payload for the side panel/lore area
    if (menu->count > 0 && menu->selected < menu->count && menu->type == ENTITY_PLANT) {
        int selectedId = menu->itemIds[menu->selected];

        // Fetch description or fallback to species name
        render_data.lore_body = GetDescription(menu->type, selectedId);

        static char context_buf[32];
        snprintf(context_buf, sizeof(context_buf), "[PLANT]");
        render_data.context_tag = context_buf;
    }

    // Draw the base menu logbook
    DrawMenu(&render_data, COLOR_CELADON);
}

void UpdatePlantInventory(PlaySession* session, Input* input) {
    Menu* menu = &session->menu;

    // --- Normal Botany Log Browsing ---
    if (!UpdateMenu(menu, input)) {
        session->state = ADVENTURE; // Or your default adventure state
        return;
    }

    // Optional: If you want pressing ENTER on a plant to do something later (like inspect or view details),
    // you can handle it here. For now, it just respects the menu loop navigation.
    if (input->buttons_pressed & ENTER_PRESSED) {
        if (menu->count == 0) return;

        // Future plant interaction logic can go here (e.g., viewing species info)
        int selected = menu->selected;
        int plantId = menu->itemIds[selected];

        // Example debug print
        printf("Selected plant ID: %d (Count: %d)\n", plantId, session->player->plant_inventory[plantId]);
    }
}
