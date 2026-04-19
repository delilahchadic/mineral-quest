#include "engine/register.h"

ItemDefinition ITEM_REGISTRY[100] = {0};
Message DIALOG_REGISTRY[500] = {0};
Character CHARACTER_REGISTRY[200] = {0};
Texture2D SPRITE_OVERRIDE[10] = {0};
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

void ParseSpriteOverrideRow(char* line){
  char* idToken = strtok(line,",");
  char* spriteToken = strtok(NULL,",");
  if(idToken && spriteToken){
    int id = atoi(idToken);
    Texture2D* d =&SPRITE_OVERRIDE[id];
    Image image = LoadImage(spriteToken);
    *d = LoadTextureFromImage(image);
    UnloadImage(image);
  } 
}


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
    d->default_trait_flags = TRAIT_NONE;
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
    d->default_trait_flags = TRAIT_TALK;
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
    Message* d =&DIALOG_REGISTRY[id];
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

void LoadSpriteOverrideRegistry(){
  LoadRegistry("data/tables/sprite_override.csv", ParseSpriteOverrideRow);
}


void CloseCharacterRegistry(){
  for(int i =0; i<200;i++){
    if(CHARACTER_REGISTRY[i].sprite.id > 0){
      UnloadTexture(CHARACTER_REGISTRY[i].sprite);
    }
  }
}

void InitRegistries(){
  LoadItemRegistry();
  LoadDialogRegistry();
  LoadCharacterRegistry();
  LoadPlantRegistry();
  LoadSpriteOverrideRegistry();
  PLAYER = &GLOBAL_PLAYER;
  GLOBAL_PLAYER = Get_Default_Player();}

char* GetName(EntityType type, int id){
  switch (type)
  {
  case ENTITY_PLANT:
    return PLANT_REGISTRY[id].species_name;
    break;
  case ENTITY_CHARACTER:
    return CHARACTER_REGISTRY[id].name;
  case ENTITY_ITEM:
    return ITEM_REGISTRY[id].name;
  default:
    return NULL;
  }
}

char* GetDescription(EntityType type, int id){
  switch (type)
  {
  case ENTITY_ITEM:
    return ITEM_REGISTRY[id].description;
  default:
    return NULL;
  }
}

Texture2D* GetSpriteOverride(int id){
  return &SPRITE_OVERRIDE[id];
}

Texture2D* GetSprite(EntityType type, int id){
  switch (type){
  case ENTITY_PLANT:
    return &PLANT_REGISTRY[id].sprite;
    break;
  case ENTITY_CHARACTER:
    return &CHARACTER_REGISTRY[id].sprite;
  case ENTITY_ITEM:
    return GetSpriteOverride(0);
  case ENTITY_PLAYER:
    return &GLOBAL_PLAYER.sprite;
  default:
    return NULL;
  }
}

int GetDialogID(EntityType type, int id){
  switch (type){
  case ENTITY_CHARACTER:
    return CHARACTER_REGISTRY[id].dialogId;
  default:
    return -1;
  }
}

uint32_t GetDefaultTraitFlags(EntityType type, int id){
  switch (type){
    case ENTITY_PLANT:
      return PLANT_REGISTRY[id].default_trait_flags;
      break;
    case ENTITY_CHARACTER:
      return CHARACTER_REGISTRY[id].default_trait_flags;
    case ENTITY_ITEM:
      return TRAIT_GATHER;
    default:
      return TRAIT_NONE;
  }
}