#include "register.h"

ItemDefinition ITEM_REGISTRY[100] = {0};
DialogDef DIALOG_REGISTRY[500] = {0};

// reads item csv and is the keystore for all items
void LoadItemRegistry(){
  FILE* file = fopen("item.csv", "r");
  if (!file) {
    TraceLog(LOG_ERROR, "Items not loaded");
    return;
  }
    TraceLog(LOG_INFO,"File Loaded - item.csv");
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
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
    fclose(file);

}