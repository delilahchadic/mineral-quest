#include "ui/stats_menu.h"
#include <stdio.h>
#include "defs/types_systems.h"
#include "engine/palette.h"
#include "defs/types_entities.h"
#include "registry/register.h"

void UpdateStatsMenu(PlaySession* session, Input* input){
  if(input->buttons_pressed & SHIFT_PRESSED){
    session->state = ADVENTURE;
  }
}

void DrawStatsMenu(PlaySession* session){
    // 1. Background - The Aged Paper
    ClearBackground(COLOR_CERULEAN_COBALT);

    int margin_x = 50, padding_y = 40;
    int uiWidth = SCREEN_WIDTH - (margin_x * 2);
    int text_start_x = 250;

    // Track vertical position dynamically as we draw downwards
    int current_y = (padding_y * 2) + 10;

    // 2. Title & Header Line
    DrawText("Stats", margin_x, 40, 30, COLOR_BONE_WHITE);
    DrawRectangle(margin_x, padding_y + 40, uiWidth, 2, COLOR_BONE_WHITE);

    // 3. HP Section
    DrawText("Current HP", text_start_x, current_y, 15, COLOR_BONE_WHITE);
    DrawText("Max HP", text_start_x + 200, current_y, 15, COLOR_BONE_WHITE);
    current_y += 20; // Move down for values

    char current_hp[16], max_hp[16];
    snprintf(current_hp, sizeof(current_hp), "%d", session->map.player->stats->current_hp);
    snprintf(max_hp, sizeof(max_hp), "%d", session->map.player->stats->max_hp);

    DrawText(current_hp, text_start_x, current_y, 20, COLOR_BONE_WHITE);
    DrawText(max_hp, text_start_x + 200, current_y, 20, COLOR_BONE_WHITE);
    current_y += 35; // Spacing before table headers

    // 4. Stat Table Labels
    DrawText("Current", text_start_x, current_y, 15, COLOR_BONE_WHITE);
    DrawText("Base", text_start_x + 200, current_y, 15, COLOR_BONE_WHITE);
    current_y += 25; // Spacing before loop rows

    // 5. Stat Loop
    for(int i = 0; i < STAT_COUNT; i++) {
        char base[16], current[16];
        snprintf(current, sizeof(current), "%d", session->map.player->stats->current[i]);
        snprintf(base, sizeof(base), "%d", session->map.player->stats->base[i]);

        DrawText(STATS_NAMES[i], margin_x, current_y, 15, COLOR_BONE_WHITE);
        DrawText(base, text_start_x, current_y, 20, COLOR_BONE_WHITE);
        DrawText(current, text_start_x + 200, current_y, 20, COLOR_BONE_WHITE);

        current_y += 30; // Advance row cleanly
    }

    // 6. Active Buffs Section
    current_y += 10;
    DrawText("Active Buffs", margin_x, current_y, 20, COLOR_BONE_WHITE);
    current_y += 25;

    bool found_buff = false;
    for(int b = 0; b < MAX_ACTIVE_BUFFS; b++) {
        ActiveBuff* buff = &session->map.player->stats->buffs[b];
        if(!buff->active) continue;

        found_buff = true;

        // Display Item Name from Registry along with remaining duration
        ItemDefinition* item = &ITEM_REGISTRY[buff->id];
        char buff_info[64];
        snprintf(buff_info, sizeof(buff_info), "%s (%.1fs)", item->name, buff->duration);
        DrawText(buff_info, margin_x, current_y, 15, COLOR_BONE_WHITE);

        // Display individual non-zero stat modifiers for this exact buff
        int offset_x = margin_x + 200;
        for(int s = 0; s < STAT_COUNT; s++) {
            int mod = buff->modifiers[s];
            if(mod != 0) {
                char mod_text[32];
                snprintf(mod_text, sizeof(mod_text), "%s %s%d", STATS_NAMES[s], (mod > 0) ? "+" : "", mod);
                DrawText(mod_text, offset_x, current_y, 15, COLOR_BONE_WHITE);
                offset_x += 120;
            }
        }

        current_y += 22;
    }

    if(!found_buff) {
        DrawText("None", margin_x, current_y, 15, COLOR_BONE_WHITE);
    }
}
