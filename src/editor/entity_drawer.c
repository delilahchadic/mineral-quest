#include "editor/entity_drawer.h"
#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"
#include "engine/palette.h"
#include "raylib.h"
#include "registry/mineral_register.h"
#include "registry/register.h"
#include "systems/physics.h"
#include "ui/ui_helpers.h"
#include "environment/map.h"

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
    drawer->mineral_button = (Rectangle) {start_x + 210, start_y, 64,32};
    drawer->enemy_button = (Rectangle) {start_x + 280, start_y, 64,32};
    drawer->portal_button = (Rectangle) {start_x, start_y+48, 64,32};
    drawer->prev_page_button = (Rectangle) {start_x, start_y+96, 64,32};
    drawer->next_page_button = (Rectangle) {start_x + 128, start_y +64, 64,32};
    int spacing = 70; // 64px button + 6px padding
    int grid_start_y = start_y + 128;

        for (int i = 0; i < drawer->entities_per_page; i++) {
            int row = i / 3; // Integer division gives you the row (0, 0, 0, 1, 1, 1...)
            int col = i % 3; // Modulo gives you the column (0, 1, 2, 0, 1, 2...)

            drawer->buttons[i] = (Rectangle) {
                (float)start_x + (col * spacing),
                (float)grid_start_y + (row * spacing),
                64, 64
            };
        }

}

void SetPage(EntityDrawer* drawer){
    EntityType type = drawer->current_type;
    int page = drawer->current_page;
    int count = type == ENTITY_MINERAL ? MINERAL_COUNT: GetEntityTypeCount(type);
    int pagestart = page * drawer->entities_per_page;

    for (int k = 0; k < drawer->entities_per_page; k++) {
            drawer->ids[k] = -1;
    }

    if(pagestart < count){
    for(int i = pagestart,j = 0; i < pagestart + drawer->entities_per_page && i < count;i++,j++){
        drawer->ids[j] = i;
    }}
}

bool HandleMapPlacement(Map* map, EntityDrawer* drawer, Camera2D* camera, Input* input) {
    if (drawer->selected_id == -1) return false;
    if (input->mouse.x > (SCREEN_WIDTH * 0.66f)) return false;

    if (input->buttons_pressed & LEFT_MOUSE_CLICKED) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(input->mouse, *camera);
        Vector2 gridCoords = GetIsoWorldToGridWithHeight(map, mouseWorldPos);

        int ix = (int)gridCoords.x;
        int iy = (int)gridCoords.y;

        MapEntity* newEntity = malloc(sizeof(MapEntity));
        newEntity->type = drawer->current_type;
        newEntity->id = drawer->selected_id;

        // FIX: Capture the height of the tile and convert to pixels (8.0f step)
        newEntity->altitude = map->grid[iy][ix].height * 8.0f;

        newEntity->state = NORMAL_STATE;
        newEntity->jumpoffset = 0.0f;
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

        if(CheckCollisionPointRec(input->mouse, drawer->portal_button)){
            drawer->current_type = ENTITY_PORTAL;
            SetPage(drawer);
        }
        if(CheckCollisionPointRec(input->mouse, drawer->item_button)){
            drawer->current_type = ENTITY_ITEM;
            SetPage(drawer);
        }
        if(CheckCollisionPointRec(input->mouse, drawer->mineral_button)){
            drawer->current_type = ENTITY_MINERAL;
            SetPage(drawer);
        }
        if(CheckCollisionPointRec(input->mouse, drawer->enemy_button)){
            drawer->current_type = ENTITY_ENEMY;
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
    drawer->entities_per_page = 9;
    InitDrawerButtons(drawer, start_x , start_y);
    SetPage(drawer);
}

#include <math.h>

void DrawTextureProFit(Texture2D texture, Rectangle box) {
    // Find the single uniform scale factor that fits inside the box
    float scale = fminf(box.width / (float)texture.width, box.height / (float)texture.height);

    // Calculate scaled dimensions
    float w = texture.width * scale;
    float h = texture.height * scale;

    // Center it inside the box
    Rectangle dest = {
        box.x + (box.width - w) / 2.0f,
        box.y + (box.height - h) / 2.0f,
        w,
        h
    };

    DrawTexturePro(texture, (Rectangle){ 0.0f, 0.0f, (float)texture.width, (float)texture.height }, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
}

void DrawEnitityDrawer(EntityDrawer* drawer) {
    int s = (int)(SCREEN_WIDTH  * 0.66f);
    int startX = s + 40;
    DrawText("Entity Drawer", startX, 100, 30, COLOR_DUSTY_CORAL);
    DrawButton(drawer->plant_button, "Plants", COLOR_SAP_GREEN, COLOR_PULP_PAPER);
    DrawButton(drawer->character_button, "Character", COLOR_CERULEAN_COBALT, COLOR_PULP_PAPER);
    DrawButton(drawer->mineral_button, "Minerals", COLOR_ROSE, COLOR_PULP_PAPER);
    DrawButton(drawer->enemy_button, "Enemies", COLOR_NEON_CRIMSON, COLOR_PULP_PAPER);
    DrawButton(drawer->item_button, "Items", COLOR_NICKEL_TITANITE, COLOR_PULP_PAPER);
    DrawButton(drawer->portal_button, "Portals", COLOR_CERULEAN_BERYL, COLOR_PULP_PAPER);
    DrawButton(drawer->prev_page_button, "Previous", COLOR_AMBER, COLOR_PULP_PAPER);
    DrawButton(drawer->next_page_button, "Next", COLOR_RED_OCHRE, COLOR_PULP_PAPER);

    for(int i=0;i<10 && drawer->ids[i] > -1;i++){
        if(drawer->current_type == ENTITY_MINERAL){
            DrawMineral(drawer->ids[i],(Vector2){drawer->buttons[i].x,drawer->buttons[i].y});
        }else{
            DrawTextureProFit(*GetSprite(drawer->current_type, drawer->ids[i]), drawer->buttons[i]);
        }

    }
    Rectangle panel = (Rectangle){s,(SCREEN_HEIGHT * 0.66f) + 30 ,(SCREEN_WIDTH * 0.33f)-0, (SCREEN_HEIGHT * 0.33f) -60};

    DrawRectangleRec(panel, COLOR_PULP_PAPER);
    DrawRectangleLinesEx(panel, 1.0f, COLOR_SUNKEN_INK);
    DrawText("Current Entity", panel.x+20, panel.y+25, 15.0f, COLOR_SUNKEN_INK);
    int selected_id = drawer->selected_id;
    if(selected_id >-1){
        DrawText(GetName(drawer->current_type, selected_id), panel.x+10, panel.y+40, 15.0f, COLOR_SUNKEN_INK);
        if(drawer->current_type == ENTITY_MINERAL){
            DrawMineral(selected_id,GetMousePosition());
        }else{
            DrawTexture(*GetSprite(drawer->current_type, selected_id), GetMousePosition().x, GetMousePosition().y, WHITE);
        }
    }

}
