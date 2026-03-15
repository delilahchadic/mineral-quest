#include "engine/register.h"

ItemDefinition ITEM_REGISTRY[100] = {0};
Dialog DIALOG_REGISTRY[500] = {0};
Character CHARACTER_REGISTRY[200] = {0};

void ParseCharacterRow(char* line) {
  char* idToken = strtok(line,",");
 
  char* nameToken = strtok(NULL,",");
  char* dialogToken = strtok(NULL,",");
  char* textToken = strtok(NULL,",");
  if(idToken && nameToken && dialogToken && textToken){
    int id = atoi(idToken);
    Character* d =&CHARACTER_REGISTRY[id];

    Image image = LoadImage(textToken);
    d->sprite = LoadTextureFromImage(image);
    UnloadImage(image);
    d->dialogId = atoi(dialogToken);
    strncpy(d->name, nameToken, sizeof(d->name) - 1);
    d->name[sizeof(d->name) - 1] = '\0'; 
    
  }     
}

void ParseItemRow(char* line) {
  char* idToken = strtok(line,",");
  char* nameToken = strtok(NULL,",");
  char* descToken = strtok(NULL,",");
  char* typeToken = strtok(NULL,",");
  if(idToken && nameToken && descToken && typeToken){
    int id = atoi(idToken);
    ItemDefinition* d = &ITEM_REGISTRY[id];
    d->id = id;
    d->type = atoi(typeToken);
    strncpy(d->name, nameToken, sizeof(d->name) - 1);
    d->name[sizeof(d->name) - 1] = '\0'; // Safety null terminator

    strncpy(d->description, descToken, sizeof(d->description) - 1);
    d->description[sizeof(d->description) - 1] = '\0';
  }
}

void ParseDialogRow(char* line) {
  char* idToken = strtok(line,",");
  char* nextidToken = strtok(NULL,",");
  char* nameToken = strtok(NULL,",");
  char* textToken = strtok(NULL,",");
  if(idToken && nameToken && nextidToken && textToken){
    int id = atoi(idToken);
    Dialog* d =&DIALOG_REGISTRY[id];
    d->id = id;
    d->nextid = atoi(nextidToken);

    strncpy(d->character_name, nameToken, sizeof(d->character_name) - 1);
    d->character_name[sizeof(d->character_name) - 1] = '\0'; 
    
    strncpy(d->text, textToken, sizeof(d->text) - 1);
    d->text[sizeof(d->text) - 1] = '\0';
  }     
}

void LoadRegistry(const char* filename, void (*parser)(char*)){
  FILE* file = fopen(filename, "r");
  if (!file) {
    TraceLog(LOG_ERROR, "Failed to open %s", filename);
    return;
  }
  TraceLog(LOG_INFO,"File Loaded - %s", filename);
  char line[1024];
  fgets(line, sizeof(line),file);
  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = 0;
    parser(line);
  }

  fclose(file);
}
// reads item csv and is the keystore for all items
void LoadItemRegistry(){
  LoadRegistry("data/tables/item.csv",ParseItemRow);
}

void LoadDialogRegistry() {
  LoadRegistry("data/tables/dialog.csv", ParseDialogRow);
}

void LoadCharacterRegistry(){
  LoadRegistry("data/tables/characters.csv", ParseCharacterRow);
}