#include "ui/recipe_menu.h"

#include "defs/types_entities.h"
#include "engine/palette.h"
#include "registry/register.h"
#include "ui/menu.h"
#include <stdio.h>

void DrawRecipeInventory(PlaySession *session) {
    if (!session)
        return;
    Menu *menu = &session->menu;

    // 1. Background & Header Setup
    ClearBackground(COLOR_RED_OCHRE);

    int margin = 50;
    int uiWidth = SCREEN_WIDTH - (margin * 2);

    DrawText("ALCHEMY RECIPES", margin, 40, 30, COLOR_SNOOT_PINK);
    DrawRectangle(50, 80, uiWidth, 2, COLOR_SNOOT_PINK);

    if (menu->count == 0) {
        DrawText("YOUR RECIPE BOOK IS EMPTY...", margin, 130, 20,
                 COLOR_RED_OCHRE);
        return;
    }

    // 2. Render Recipe List, Output Items, and Costs
    for (int i = 0; i < menu->count && i < 64; i++) {
        int recipeId = menu->itemIds[i];
        Recipe *r = &RECIPE_REGISTRY[recipeId];
        Exchange *e = &EXCHANGE_REGISTRY[r->exchangeId];

        Color textColor =
            (menu->selected == i) ? COLOR_CHROME_YELLOW : COLOR_SNOOT_PINK;

        // Draw Recipe Name/ID
        char recipe_buf[64];
        snprintf(recipe_buf, sizeof(recipe_buf), "Recipe #%d: %s", recipeId + 1,
                 GetName(ENTITY_ITEM, e->item_id));
        DrawText(recipe_buf, margin, 120 + (i * 35), 20, textColor);

        // If this row is selected, render cost slots and affordability
        if (menu->selected == i) {
            int cost_x = margin + 550;

            for (int c = 0; c < 3; c++) {
                if (e->cost_slots[c].type == ENTITY_NONE)
                    break;

                // Check affordability for each cost slot
                Color costColor = CanAfford(session->player, e->cost_slots[c])
                                      ? COLOR_SAP_GREEN
                                      : COLOR_SUNKEN_INK;

                // Draw Cost Item Name
                DrawText(GetName(e->cost_slots[c].type, e->cost_slots[c].id),
                         cost_x, 120 + (i * 35), 18, costColor);

                // Draw Cost Amount
                char amount_buf[16];
                snprintf(amount_buf, sizeof(amount_buf), "x%d",
                         e->cost_slots[c].amount);
                DrawText(amount_buf, cost_x + 110, 120 + (i * 35), 18,
                         COLOR_SNOOT_PINK);

                cost_x += 160;
            }
        }
    }

    // Footer Help Text
    DrawText("Press ENTER to Craft | Press T to Exit", margin,
             SCREEN_HEIGHT - 60, 18, COLOR_SNOOT_PINK);
}

void UpdateRecipeInventory(PlaySession *session, Input *input) {
    if (!session)
        return;
    Menu *menu = &session->menu;

    // Exit menu using Key T
    if (input->buttons_pressed & KEY_T_PRESSED) {
        session->state = ADVENTURE_STATE;
        return;
    }

    // Menu Navigation (W / S)
    if (input->buttons_pressed & KEY_W_PRESSED) {
        menu->selected--;
        if (menu->selected < 0) {
            menu->selected = menu->count > 0 ? menu->count - 1 : 0;
        }
    }
    if (input->buttons_pressed & KEY_S_PRESSED) {
        menu->selected++;
        if (menu->selected >= menu->count) {
            menu->selected = 0;
        }
    }

    // Crafting / Executing Exchange on ENTER
    if (input->buttons_pressed & ENTER_PRESSED) {
        if (menu->count == 0 || menu->selected < 0 ||
            menu->selected >= menu->count)
            return;

        int recipeId = menu->itemIds[menu->selected];
        Recipe *r = &RECIPE_REGISTRY[recipeId];
        Exchange *e = &EXCHANGE_REGISTRY[r->exchangeId];

        // Validate affordability across all active cost slots
        int canMake = 1;
        for (int i = 0; i < 3; i++) {
            if (e->cost_slots[i].type == ENTITY_NONE)
                break;
            if (!CanAfford(session->player, e->cost_slots[i])) {
                canMake = 0;
                break;
            }
        }

        if (canMake) {
            ProcessRecipe(session->player, *e);
        }
    }
}

void RebindRecipeMenu(Menu *menu, Player *player) {
    menu->type = ENTITY_RECIPE;
    menu->exit_button = KEY_T_PRESSED; // Uses Key T to exit the recipe log

    // Gather all unlocked recipe IDs into a temporary list for the menu
    int active_recipe_ids[100];
    int active_count = 0;

    for (int i = 0; i < 100; i++) {
        // Assuming your player struct tracks recipes via an array (e.g.,
        // recipe_inventory or unlocked_recipes)
        if (player->recipe_inventory[i] > 0) {
            active_recipe_ids[active_count++] = i;
        }
    }

    FillMenu(menu, active_recipe_ids, active_count);

    return;
}
