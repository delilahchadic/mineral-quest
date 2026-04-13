#include "engine/register.h"

ItemDefinition ITEM_REGISTRY[100] = {0};
Dialog DIALOG_REGISTRY[500] = {0};
Character CHARACTER_REGISTRY[200] = {0};
Plant PLANT_REGISTRY[100] = {0};
Player GLOBAL_PLAYER;
Player* PLAYER;


// registry types temporary till we move into a dedicated register
TileDefinition TILE_REGISTRY[6] = {
  { TILE_WATER, true,  0.5f, 101, COLOR_CERULEAN_DUSTY},
  { TILE_GRASS, false,  1.0f, 102, COLOR_DUSTY_SAP    },
  { TILE_SAND,  false, 0.4f, 103, COLOR_CERULEAN_BERYL    },
  { TILE_DIRT,  false, 0.8f, 104, COLOR_DUSTY_SALMON  },
  { TILE_STONE, false, 0.9f, 105, COLOR_DUSTY_SAP          },
  { TILE_ROAD,  false, 1.0f, 106, COLOR_ASPHALT       }
};
/// @brief parses line of the plant csv
/// @param line 
void ParsePlantRow(char* line) {
  // Plant ID
  char* idToken = strtok(line,",");
  //Plant Species Name
  char* nameToken = strtok(NULL,",");
  //Plant sprite
  char* spriteToken = strtok(NULL,",");
  //Plant sprite height
  char* heightToken = strtok(NULL,",");
  //Plant sprite width
  char* widthToken = strtok(NULL,",");
  //Plant hitbox height
  char*  hitheightToken = strtok(NULL,",");
  //Plant hitbox width
  char* hitwidthToken = strtok(NULL,",");

  // if all are valid
  if(idToken && nameToken && spriteToken && heightToken && widthToken && hitheightToken && hitwidthToken){
    // grab the appropriate plant
    int id = atoi(idToken);
    Plant* d =&PLANT_REGISTRY[id];
    //Load the sprite
    d->sprite = LoadTexture(spriteToken);
    // get the species name
    strncpy(d->species_name, nameToken, sizeof(d->species_name) - 1);
    d->species_name[sizeof(d->species_name) - 1] = '\0'; 

    //grab our size info
    d->frameheight = atoi(heightToken);
    d->framewidth = atoi(widthToken);
    d->hitboxheight = atoi(hitheightToken);
    d->hitboxwidth = atoi(hitwidthToken);
  }     
}

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
void LoadPlantRegistry(){
  LoadRegistry("data/tables/plants.csv", ParsePlantRow);
}

void CloseCharacterRegistry(){
  for(int i =0; i<200;i++){
    if(CHARACTER_REGISTRY[i].sprite.id >0){
      Close_Character(&CHARACTER_REGISTRY[i]);
    }
  }
}

void InitRegistries(){
  LoadItemRegistry();
  LoadDialogRegistry();
  LoadCharacterRegistry();
  LoadPlantRegistry();
  
  PLAYER = &GLOBAL_PLAYER;
  GLOBAL_PLAYER = Get_Default_Player();
  

}