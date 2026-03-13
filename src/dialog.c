#include "dialog.h"

void Init_Dialog_Manager(Dialog_Manager* manager, int initialCapacity){
  manager->count =0;
  manager->capacity = initialCapacity;
  manager->active =false;
  manager->currentID = -1;
  manager->active_dialog = NULL;
  manager->database = (Dialog*) calloc(sizeof(Dialog), initialCapacity);
};

void Load_Dialogs_From_CSV(Dialog_Manager* manager, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
      TraceLog(LOG_ERROR, "Dialogs not loaded");
      return;
    }
    TraceLog(LOG_INFO,"File Loaded - dialog.csv");
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char* idToken = strtok(line,",");
        if(!idToken) continue;
        char* nextidToken = strtok(NULL,",");
        if(!nextidToken) continue;
        char* nameToken = strtok(NULL,",");
        if(!nameToken) continue;
        char* textToken = strtok(NULL,",");
        if(!textToken) continue;
        int len = strlen(textToken);
        if (len > 0 && textToken[len - 1] == '\n') {
            textToken[len - 1] = '\0'; // Overwrite it with the "end of string" marker
        }
        Dialog d;
        d.id = atoi(idToken);
        d.nextid = atoi(nextidToken);
        d.character_name = strdup(nameToken);
        d.text = strdup(textToken);
        manager->database[manager->count++]=d;
    }
    fclose(file);
}

void Set_Active_Dialog(Dialog_Manager* manager,int dialogId){
  for(int i = 0;i<manager->count; i++){
    if(manager->database[i].id == dialogId){
      manager->active_dialog = &manager->database[i];
    } 
  }
}

void Draw_Dialog(Dialog_Manager* manager){

  if(!manager->active || manager->active_dialog == NULL){
    return;
  }
  // 1. Draw the Main Box (Bottom of screen)
  int boxY = GetScreenHeight() - 150;
  DrawRectangle(50, boxY, GetScreenWidth() - 100, 120, Fade(BLACK, 0.8f));
  DrawRectangleLines(50, boxY, GetScreenWidth() - 100, 120, WHITE);

  // 2. Draw the Character Name (slightly offset)
  DrawText(manager->active_dialog->character_name, 70, boxY + 10, 20, YELLOW);

  // 3. Draw the Dialog Text
  DrawText(manager->active_dialog->text, 70, boxY + 40, 18, WHITE);
    
  // 4. Prompt to continue
  DrawText("Press [SPACE] to continue", GetScreenWidth() - 250, boxY + 90, 10, GRAY);
  DrawText(manager->active_dialog->text, 0, 0, 12, RED);
  
}

void Close_Dialog_Manager(Dialog_Manager* manager){
  for(int i =0;i< manager->count;i++){
    TraceLog(LOG_INFO, manager->database[i].text);
    free(manager->database[i].character_name);
    free(manager->database[i].text);
  }
  free(manager->database);
  manager->database = NULL;
  manager->count = 0;
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