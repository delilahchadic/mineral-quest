#include "ui/stats_menu.h"
#include <stdio.h>
#include "defs/types_systems.h"
#include "engine/palette.h"
#include "defs/types_entities.h"
#include "registry/register.h"

void UpdateStatsMenu(PlaySession* session, Input* input){
  if(input->buttons_pressed & KEY_P_PRESSED){
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
}
