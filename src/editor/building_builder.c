#include "editor/building_builder.h"
#include "defs/types_core.h"
#include "defs/types_env.h"
#include "defs/types_ui.h"
#include "engine/palette.h"
#include "environment/map.h"
#include "raylib.h"
#include "ui/ui_helpers.h"
#include <stdio.h>
#include <stdlib.h>
void InitBuildingBuilder(BuildingBuilder* builder, int start_x, int start_y){
    builder->minus_5 = (Rectangle) {start_x, start_y + 50, 60, 40};
    builder->minus_1 = (Rectangle) {start_x+100, start_y + 50, 60, 40};
    builder->plus_1 = (Rectangle) {start_x+200, start_y + 50, 60, 40};
    builder->plus_5 = (Rectangle){start_x+300, start_y+50, 60,40};
    builder->set_tile_1 = (Rectangle) {start_x, start_y + 100, 60, 40};
    builder->set_tile_2 = (Rectangle) {start_x+100, start_y + 100, 60, 40};
    builder->set_min_height = (Rectangle) {start_x+200, start_y + 100, 60, 40};
    builder->set_max_height = (Rectangle) {start_x+300, start_y + 100, 60, 40};
    builder->cycle_selected_building = (Rectangle) {start_x, start_y + 150, 60, 40};
    builder->add_building_button = (Rectangle) {start_x, start_y + 200, 60, 40};
    builder->remove_building = (Rectangle) {start_x+100, start_y + 200, 60, 40};
    builder->current_building = NULL;
}

void DrawBuildingBuilder(BuildingBuilder* builder){
    int s = (int)(SCREEN_WIDTH  * 0.66f);

    int startX = s + 40;
    int startY = 100;
    char x1_str[5], y1_str[5] ,x2_str[5], y2_str[5];
    char min_str[5], max_str[5], curr_str[5];
    char last_x[5], last_y[5];
    snprintf(x1_str, sizeof(x1_str), "%d", builder->x1);
    snprintf(y1_str, sizeof(y1_str), "%d", builder->y1);
    snprintf(x2_str, sizeof(x2_str), "%d", builder->x2);
    snprintf(y2_str, sizeof(y2_str), "%d", builder->y2);
    snprintf(last_x, sizeof(last_x), "%d", builder->last_x);
    snprintf(last_y, sizeof(last_y), "%d", builder->last_y);
    snprintf(min_str, sizeof(min_str),"%d", builder->min_height);
    snprintf(max_str, sizeof(max_str),"%d", builder->max_height);
    snprintf(curr_str, sizeof(curr_str),"%d", builder->curr_height);
    DrawText("Building Builder", startX, startY, 30, COLOR_DUSTY_SALMON);
    DrawText("x1", startX-20, startY + 45, 12, COLOR_SUNKEN_INK);
    DrawText(x1_str, startX, startY + 45, 12, COLOR_SUNKEN_INK);
    DrawText("y1", startX+30, startY + 45, 12, COLOR_SUNKEN_INK);
    DrawText(y1_str, startX + 50, startY + 45, 12, COLOR_SUNKEN_INK);
    DrawText("x2", startX-20, startY + 60, 12, COLOR_SUNKEN_INK);
    DrawText(x2_str, startX, startY + 60, 12, COLOR_SUNKEN_INK);
    DrawText("y2", startX+30, startY + 60, 12, COLOR_SUNKEN_INK);
    DrawText(y2_str, startX +50, startY + 60, 12, COLOR_SUNKEN_INK);
    DrawText("min", startX-20, startY + 75, 12, COLOR_SUNKEN_INK);
    DrawText(min_str, startX, startY + 75, 12, COLOR_SUNKEN_INK);
    DrawText("max", startX+30, startY + 75, 12, COLOR_SUNKEN_INK);
    DrawText(max_str, startX +50, startY + 75, 12, COLOR_SUNKEN_INK);
    DrawText("curr", startX-20, startY + 90, 12, COLOR_SUNKEN_INK);
    DrawText(curr_str, startX, startY + 90, 12, COLOR_SUNKEN_INK);
    DrawText("last x", startX-20, startY + 120, 12, COLOR_SUNKEN_INK);
    DrawText(last_x, startX+20, startY + 120, 12, COLOR_SUNKEN_INK);
    DrawText("last y", startX+60, startY + 120, 12, COLOR_SUNKEN_INK);
    DrawText(last_y, startX+100, startY + 120, 12, COLOR_SUNKEN_INK);
    DrawButton(builder->minus_5, "-5", COLOR_RED_OCHRE, COLOR_PULP_PAPER);
    DrawButton(builder->minus_1, "-1", COLOR_POTTERS_PINK, COLOR_PULP_PAPER);
    DrawButton(builder->plus_1, "+1", COLOR_CELADON, COLOR_PULP_PAPER);
    DrawButton(builder->plus_5, "+5", COLOR_VIRIDIAN, COLOR_PULP_PAPER);
    DrawButton(builder->set_tile_1,"set 1", COLOR_SHELL_PINK, COLOR_PULP_PAPER);
    DrawButton(builder->set_tile_2,"set 2", COLOR_BEAVIS_SHIRT, COLOR_PULP_PAPER);
    DrawButton(builder->set_min_height,"min", COLOR_AUREOLIN, COLOR_PULP_PAPER);
    DrawButton(builder->set_max_height,"max", COLOR_MAY_GREEN, COLOR_PULP_PAPER);
    DrawButton(builder->cycle_selected_building, "+", COLOR_CERULEAN_CORE, COLOR_PULP_PAPER);
    DrawButton(builder->remove_building, "remove", COLOR_PERIWINKLE_SLATE, COLOR_PULP_PAPER);
    DrawButton(builder->add_building_button, "add building", COLOR_MAGENTA, COLOR_PULP_PAPER);


    // BuildingZone b =
}


void UpdateBuildingBuilder(BuildingBuilder* builder, Map* map, Input* input, int last_x, int last_y){
    builder->last_x =last_x;
    builder->last_y = last_y;
    if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
        if(CheckCollisionPointRec(input->mouse, builder->minus_5)) builder->curr_height -= 5;
        if(CheckCollisionPointRec(input->mouse, builder->minus_1)) builder->curr_height -=  1;
        if(CheckCollisionPointRec(input->mouse, builder->plus_1)) builder->curr_height +=  1;
        if(CheckCollisionPointRec(input->mouse, builder->plus_5)) builder->curr_height +=  5;
        if(CheckCollisionPointRec(input->mouse, builder->set_tile_1)) {
            builder->x1 = builder->last_x;
            builder->y1 = builder->last_y;
        }
        if(CheckCollisionPointRec(input->mouse, builder->set_tile_2)) {
            builder->x2 = builder->last_x;
            builder->y2 = builder->last_y;
        }
        if(CheckCollisionPointRec(input->mouse, builder->set_min_height)) builder->min_height = builder->curr_height;
        if(CheckCollisionPointRec(input->mouse, builder->set_max_height)) builder->max_height = builder->curr_height;
        if(CheckCollisionPointRec(input->mouse, builder->add_building_button)){
            BuildingZone* b = malloc(sizeof(BuildingZone));
            if(b==NULL) return;
            b->id = map->building_count;
            b->x1 = builder->x1;
            b->y1 = builder->y1;
            b->x2 = builder->x2;
            b->y2 = builder->y2;
            b->min_height= builder->min_height;
            b->max_height= builder->max_height;
            b->total_floors = 0;

            b->next=map->buildings;
            map->buildings=b;
            map->building_count++;
        }

        if(CheckCollisionPointRec(input->mouse, builder->cycle_selected_building)){
            if(builder->current_building == NULL){
                builder->current_building = map->buildings;
            }else if(builder->current_building->next){
                builder->current_building = builder->current_building->next;
            }else{
                builder->current_building = map->buildings;
            }
        }

        if(CheckCollisionPointRec(input->mouse, builder->remove_building)){
            if(builder->current_building){
                Remove_Building(map, builder->current_building);
                builder->current_building = map->buildings;
            }
        }

        if(!builder->current_building && map->buildings){
            builder->current_building =  map->buildings;
        }
        BuildingZone* b = map->buildings;
        while(b && builder->current_building){
            if(b->id == builder->current_building->id){
                b->color =  COLOR_CERULEAN_CORE;
            }else{
                b->color =  COLOR_SHELL_PINK;
            }
        b = b->next;
        }

    }
}
