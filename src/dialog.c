#include "dialog.h"

void Init_Dialog_Manager(Dialog_Manager* manager, int initialCapacity){
  manager->count =0;
  manager->capacity = initialCapacity;
  manager->active =false;
  manager->currentID = -1;
  manager->active_dialog = NULL;\
};


void Set_Active_Dialog(Dialog_Manager* manager,int dialogId){
  manager->active_dialog = &DIALOG_REGISTRY[dialogId];
}

void Draw_Dialog(Dialog_Manager* manager){

  if(!manager->active || manager->active_dialog == NULL){
    return;
  }
  // 1. Draw the Main Box (Bottom of screen)
  int boxY = GetScreenHeight() - 150;
  DrawRectangle(50, boxY, GetScreenWidth() - 100, 120, Fade(COLOR_PULP_PAPER, 0.8f));
  DrawRectangleLines(50, boxY, GetScreenWidth() - 100, 120, COLOR_SUNKEN_INK);

  // 2. Draw the Character Name (slightly offset)
  DrawText(manager->active_dialog->character_name, 70, boxY + 10, 20, COLOR_SUNKEN_INK);

  // 3. Draw the Dialog Text
  DrawText(manager->active_dialog->text, 70, boxY + 40, 18, COLOR_SUNKEN_INK);
    
  // 4. Prompt to continue
  DrawText("Press [SPACE] to continue", GetScreenWidth() - 250, boxY + 90, 10, GRAY);
  DrawText(manager->active_dialog->text, 0, 0, 12, RED);
  
}

void Update_Dialog_Manager(Dialog_Manager* manager){
  if (manager->active && IsKeyPressed(KEY_SPACE)) {
    // For now, let's just close it. 
    // Later, this will call Advance_Dialogue to check for nextId
    if(manager->active_dialog->nextid != -1){
      Set_Active_Dialog(manager, manager->active_dialog->nextid);
      return;
    }
    manager->active = false;
    manager->active_dialog = NULL; 
  }
}