#include "editor/entity_drawer.h"
#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"
#include "engine/palette.h"
#include "raylib.h"
#include "registry/register.h"
#include "systems/physics.h"
#include "ui/ui_helpers.h"

void DrawEnityTypeLabel(EntityType type, int start_x, int start_y){
    switch (type) {
        case ENTITY_PLANT:
            DrawText("Plants:", start_x, start_y  , 14.0f, COLOR_SUNKEN_INK);
            break;
        case ENTITY_ITEM:
            DrawText("Items:", start_x, start_y  , 14.0f, COLOR_SUNKEN_INK);
            break;
        case ENTITY_CHARACTER:
            DrawText("Characters:", start_x, start_y  , 14.0f, COLOR_SUNKEN_INK);
            break;
        case ENTITY_PLAYER:
            DrawText("Players:", start_x, start_y  , 14.0f, COLOR_SUNKEN_INK);
            break;
        default:
            break;
    }
}

void InitDrawerButtons(EntityDrawer* drawer, int start_x, int start_y){
    drawer->plant_button = (Rectangle) {start_x, start_y, 64,32};
    drawer->character_button = (Rectangle) {start_x + 70, start_y, 64,32};
    drawer->item_button = (Rectangle) {start_x + 140, start_y, 64,32};
    drawer->prev_page_button = (Rectangle) {start_x, start_y+32, 64,32};
    drawer->next_page_button = (Rectangle) {start_x + 128, start_y +32, 64,32};
    for(int i =0;i< 2;i++){
        for(int j=0;j<5;j++){
            drawer->buttons[(i *5) +j] = (Rectangle) {(start_x) + (i*64), (start_y+64) + (j*64), 64, 64};
        }
    }

}

void SetPage(EntityDrawer* drawer){
    EntityType type = drawer->current_type;
    int page = drawer->current_page;
    int count = GetEntityTypeCount(type);
    int pagestart = page * drawer->entities_per_page;

    for (int k = 0; k < drawer->entities_per_page; k++) {
            drawer->ids[k] = -1;
    }

    if(pagestart < count){
    for(int i = pagestart,j = 0; i<pagestart + drawer->entities_per_page && i < count;i++,j++){
        drawer->ids[j] = i;
    }}
}

bool HandleMapPlacement(Map* map, EntityDrawer* drawer, Camera2D* camera, Input* input) {
    // 1. Safety Check: Only place if something is selected
    if (drawer->selected_id == -1) return false;

    // 2. Prevent "clicking through" the UI
    // If the mouse is inside the area where the Entity Drawer is drawn, don't place.
    if (input->mouse.x > (SCREEN_WIDTH * 0.66f)) return false;

    if (input->buttons_pressed & LEFT_MOUSE_CLICKED) {
        // 3. Convert Screen Mouse -> World Coordinates
        Vector2 mouseWorldPos = GetScreenToWorld2D(input->mouse, *camera);

        // 4. Use your height-aware math to find the exact tile
        Vector2 gridCoords = GetIsoWorldToGridWithHeight(map, mouseWorldPos);

        // 5. Create and add the entity
        MapEntity* newEntity = malloc(sizeof(MapEntity));
        newEntity->type = drawer->current_type;
        newEntity->id = drawer->selected_id;
        newEntity->state = NORMAL_STATE;
        newEntity->jumpoffset = 0.0f;

        // Convert grid indices back to "Square World" coordinates for storage
        newEntity->position.x = gridCoords.x * TILE_SIZE + (TILE_SIZE / 2.0f);
        newEntity->position.y = gridCoords.y * TILE_SIZE + (TILE_SIZE / 2.0f);

        Add_Entity(map, newEntity);
        return true;
    }
    return false;
}

bool UpdateEnitityDrawer(EntityDrawer* drawer, Map* map, Input* input,Camera2D* camera){
    int count = GetEntityTypeCount(drawer->current_type);
    if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
        if(CheckCollisionPointRec(input->mouse, drawer->prev_page_button)){
            if(drawer->current_page > 0){
                drawer->current_page--;
                SetPage(drawer);
            }
        } else if(CheckCollisionPointRec(input->mouse, drawer->next_page_button)){
            if((drawer->current_page + 1)* drawer->entities_per_page < count){
                drawer->current_page++;
                SetPage(drawer);
            }
        }

        if(CheckCollisionPointRec(input->mouse, drawer->plant_button)){
            drawer->current_type = ENTITY_PLANT;
            SetPage(drawer);
        }

        if(CheckCollisionPointRec(input->mouse, drawer->character_button)){
            drawer->current_type = ENTITY_CHARACTER;
            SetPage(drawer);
        }
        if(CheckCollisionPointRec(input->mouse, drawer->item_button)){
            drawer->current_type = ENTITY_ITEM;
            SetPage(drawer);
        }
        for(int i =0;i<drawer->entities_per_page && drawer->ids[i] > -1;i++){
            if(CheckCollisionPointRec(input->mouse, drawer->buttons[i])){
                drawer->selected_id = drawer->ids[i];
            }
        }

    }
    return HandleMapPlacement(map,drawer,camera, input);
}

void InitEnitityDrawer(EntityDrawer* drawer, int start_x, int start_y){
    drawer->entities_per_page = 10;
    InitDrawerButtons(drawer, start_x , start_y + 64);
    SetPage(drawer);
}

void DrawEnitityDrawer(EntityDrawer* drawer) {
    int s = (int)(SCREEN_WIDTH  * 0.66f);
    int startX = s + 40;
    int startY = 160;
    DrawText("Entity Drawer", startX, 100, 30, COLOR_DUSTY_CORAL);

    DrawEnityTypeLabel(drawer->current_type,startX, startY);
    char count_ch[5];
    sprintf(count_ch, "%d", GetEntityTypeCount(drawer->current_type));
    DrawText(count_ch, startX, startY+40, 14.0f, COLOR_SUNKEN_INK);
    DrawButton(drawer->plant_button, "Plants", COLOR_SAP_GREEN, COLOR_PULP_PAPER);
    DrawButton(drawer->character_button, "Character", COLOR_CERULEAN_COBALT, COLOR_PULP_PAPER);
    DrawButton(drawer->item_button, "Items", COLOR_NICKEL_TITANITE, COLOR_PULP_PAPER);
    DrawButton(drawer->prev_page_button, "Previous", COLOR_AMBER, COLOR_PULP_PAPER);
    DrawButton(drawer->next_page_button, "Next", COLOR_RED_OCHRE, COLOR_PULP_PAPER);
    for(int i=0;i<10 && drawer->ids[i] > -1;i++){
        DrawTexture(*GetSprite(drawer->current_type, drawer->ids[i]), drawer->buttons[i].x, drawer->buttons[i].y, WHITE);
    }
    Rectangle panel = (Rectangle){s,(SCREEN_HEIGHT *2 /3) + 30 ,(SCREEN_WIDTH * 0.33f)-0, (SCREEN_HEIGHT * 0.33f) -60};

    DrawRectangleRec(panel, COLOR_PULP_PAPER);
    DrawRectangleLinesEx(panel, 1.0f, COLOR_SUNKEN_INK);
    DrawText("Current Entity", panel.x+10, panel.y+15, 15.0f, COLOR_SUNKEN_INK);
    int selected_id =drawer->selected_id;
    if(selected_id >-1){
        DrawText(GetName(drawer->current_type, selected_id), panel.x+10, panel.y+30, 15.0f, COLOR_SUNKEN_INK);
    }
    DrawTexture(*GetSprite(drawer->current_type, drawer->selected_id), GetMousePosition().x, GetMousePosition().y, WHITE);
}
