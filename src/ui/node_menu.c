#include "ui/node_menu.h"

#include "defs/types_systems.h"
#include "registry/register.h"
void UpdateNodeMenu(PlaySession* session, Input* input){
    if(session->node_menu.state == NODE_MENU_BROWSE){
        if(input->buttons_pressed & KEY_W_PRESSED){
            session->node_menu.selected_index--;
            if(session->node_menu.selected_index == -1){
                session->node_menu.selected_index= NODE_REGISTRY[session->node_menu.node_id].count -1;
            }
        }
        if(input->buttons_pressed & KEY_S_PRESSED){
            session->node_menu.selected_index++;
            if(session->node_menu.selected_index == NODE_REGISTRY[session->node_menu.node_id].count){
                session->node_menu.selected_index = 0;
            }
        }

        if(input->buttons_pressed & ENTER_PRESSED){
            // session->node_menu.state = NODE_MENU_CONFIRM;
            ExchangeNode* n= &NODE_REGISTRY[session->node_menu.node_id];
            Exchange* e = &EXCHANGE_REGISTRY[n->exchange_ids[session->node_menu.selected_index]];
            int canMake = 1;
            for(int i = 0; i < 3;i++){
                if(e->cost_slots[i].type == ENTITY_NONE) break;
                if(!CanAfford(session->player, e->cost_slots[i])){
                    canMake = 0;
                    break;
                }
            }
            if(canMake){
                session->node_menu.state = NODE_MENU_CONFIRM;
                session->node_menu.confirm_index = 0;
                return;
            }
        }

        if(input->buttons_pressed & KEY_E_PRESSED){
            session->state = ADVENTURE;
            return;
        }
    }else{
        if(input->buttons_pressed & KEY_W_PRESSED){
            session->node_menu.confirm_index--;
            if(session->node_menu.confirm_index == -1) session->node_menu.confirm_index= 1;
        }
        if(input->buttons_pressed & KEY_S_PRESSED){
            session->node_menu.confirm_index++;
            if(session->node_menu.confirm_index == 2)session->node_menu.confirm_index = 0;
        }
        if(input->buttons_pressed & ENTER_PRESSED){
            if(session->node_menu.confirm_index == 0 ){
                ExchangeNode* n= &NODE_REGISTRY[session->node_menu.node_id];
                Exchange* e = &EXCHANGE_REGISTRY[n->exchange_ids[session->node_menu.selected_index]];
                ProcessRecipe(session->player, *e);
            }
            session->node_menu.state = NODE_MENU_BROWSE;
        }
    }
}

void DrawNodeSession(PlaySession* session){
      // 1. Background - The Aged Paper
      ClearBackground(COLOR_CELADON);
      ExchangeNode* n= &NODE_REGISTRY[session->node_menu.node_id];
      // Dynamic horizontal line based on screen width
      int margin = 50;
      int uiWidth = SCREEN_WIDTH - (margin * 2);

      // 2. Title - The "Ink" look
      DrawText(n->name, margin, 40, 30, COLOR_BONE_WHITE);
      DrawRectangle(50, 80, uiWidth, 2, COLOR_BONE_WHITE); // A simple line

      for(int i = 0;i< n->count;i++){
          Color textColor =  session->node_menu.selected_index == i? COLOR_BONE_WHITE: COLOR_RED_OCHRE;
          Exchange* e = &EXCHANGE_REGISTRY[n->exchange_ids[i]];
          DrawText(GetName(ENTITY_ITEM, e->item_id),250, 120 + (i * 30), 20, textColor);
          if(session->node_menu.selected_index == i && e->cost_slots[0].type != ENTITY_NONE){
              Color costColor = CanAfford(session->player, e->cost_slots[0])? COLOR_SAP_GREEN : COLOR_RED_OCHRE;
              DrawText(GetName(e->cost_slots[0].type, e->cost_slots[0].id),
                  500, 120 + (i * 30), 20, costColor);
              char cost1_amount[5];
              sprintf(cost1_amount, "%d", e->cost_slots[0].amount);
              DrawText(cost1_amount,
                  600, 120 + (i * 30), 20, COLOR_BONE_WHITE);
              if(session->node_menu.state ==  NODE_MENU_CONFIRM){
                  DrawText("Do you want to buy this?",600, 120 + (i * 30), 20, COLOR_BONE_WHITE);
                  Color yesColor = session->node_menu.confirm_index == 0? COLOR_BONE_WHITE: COLOR_RED_OCHRE;
                  Color noColor = session->node_menu.confirm_index == 1? COLOR_BONE_WHITE: COLOR_RED_OCHRE;
                  DrawText("Yes",600, 140 + (i * 30), 20, yesColor);
                  DrawText("No",600, 160 + (i * 30), 20, noColor);
              }

          }
          // DrawText(current,450, 120 + (i * 30), 20, COLOR_BONE_WHITE);
      }
}
