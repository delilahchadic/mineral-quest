#include "registry/register_loader.h"
#include "defs/types_entities.h"
#include "registry/register.h"


void InitRegistries(){
  SetEntityTypeCount(ENTITY_ITEM,LoadItemRegistry());
  SetEntityTypeCount(ENTITY_CHARACTER,LoadCharacterRegistry());
  SetEntityTypeCount(ENTITY_PLANT,LoadPlantRegistry());
  SetEntityTypeCount(ENTITY_PORTAL, LoadPortalRegistry());
  LoadDialogRegistry();
  LoadSpriteOverrideRegistry();

  LoadCommandRegistry();
  PLAYER = &GLOBAL_PLAYER;
  GLOBAL_PLAYER = Get_Default_Player();
}

void CloseRegistries(){
  CloseCharacterRegistry();
  ClosePlantRegistry();
  CloseSpriteOverrideRegistry();
}

void CloseCharacterRegistry(){
  for(int i =0; i<200;i++){
    if(CHARACTER_REGISTRY[i].sprite.id > 0){
      UnloadTexture(CHARACTER_REGISTRY[i].sprite);
    }
  }
}

void ClosePlantRegistry(){
  for(int i =0; i<100;i++){
    if(PLANT_REGISTRY[i].sprite.id > 0){
      UnloadTexture(PLANT_REGISTRY[i].sprite);
    }
  }
}

void CloseSpriteOverrideRegistry(){
  for(int i =0; i<10;i++){
    UnloadTexture(SPRITE_OVERRIDE[i]);
  }
}

void ParseCommandRegistryRow(char* line){
  char* idToken = strtok(line,",");
  char* labelToken = strtok(NULL,",");
  char* descriptionToken = strtok(NULL,",");
  if(idToken && labelToken && descriptionToken){
    int id = atoi(idToken);
    Command* d =&COMMAND_REGISTRY[id];
    strncpy(d->label, labelToken, sizeof(d->label) - 1);
    d->label[sizeof(d->label) - 1] = '\0';
    strncpy(d->description, descriptionToken, sizeof(d->description) - 1);
    d->description[sizeof(d->description) - 1] = '\0';
  }
}

void ParsePortalRow(char* line){
  char* idToken = strtok(line,",");
  char* nameToken = strtok(NULL,",");
  if(idToken && nameToken){
    int id = atoi(idToken);
    Portal* p = &PORTAL_REGISTRY[id];
    strncpy(p->level_name, nameToken, sizeof(p->level_name) - 1);
    p->level_name[sizeof(p->level_name) - 1] = '\0';
  }
}

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

int LoadRegistry(const char* filename, void (*parser)(char*)){
    int count =0;
  FILE* file = fopen(filename, "r");
  if (!file) {
    TraceLog(LOG_ERROR, "Failed to open %s", filename);
    return -1;
  }
  TraceLog(LOG_INFO,"File Loaded - %s", filename);
  char line[1024];
  fgets(line, sizeof(line),file);
  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = 0;
    parser(line);
    count++;
  }

  fclose(file);
  return count;
}
// reads item csv and is the keystore for all items
int LoadItemRegistry(){
  return LoadRegistry("data/tables/item.csv",ParseItemRow);
}

void LoadDialogRegistry() {
  LoadRegistry("data/tables/dialog.csv", ParseDialogRow);
}

int LoadCharacterRegistry(){
  return LoadRegistry("data/tables/characters.csv", ParseCharacterRow);
}
int LoadPlantRegistry(){
  return LoadRegistry("data/tables/plants.csv", ParsePlantRow);
}

void LoadSpriteOverrideRegistry(){
  LoadRegistry("data/tables/sprite_override.csv", ParseSpriteOverrideRow);
}

void LoadCommandRegistry(){
  LoadRegistry("data/tables/commands.csv", ParseCommandRegistryRow);
}

int LoadPortalRegistry(){
    return LoadRegistry("data/tables/portals.csv", ParsePortalRow);
}
