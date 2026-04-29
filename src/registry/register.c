#include "registry/register.h"
#include "defs/types_engine.h"
#include "defs/types_entities.h"

ItemDefinition ITEM_REGISTRY[100] = {0};
Message DIALOG_REGISTRY[500] = {0};
Character CHARACTER_REGISTRY[200] = {0};
Texture2D SPRITE_OVERRIDE[10] = {0};
Plant PLANT_REGISTRY[100] = {0};
Player GLOBAL_PLAYER;
Player* PLAYER;
static int item_count = 0;
TileDefinition TILE_REGISTRY[8] = {
  { TILE_WATER, true,  0.5f, 101, COLOR_CERULEAN_DUSTY,"Water"},
  { TILE_GRASS, false,  1.0f, 102, COLOR_DUSTY_SAP,"Grass"},
  { TILE_SAND,  false, 0.4f, 103, COLOR_PALE_SAND,"Sand"},
  { TILE_DIRT,  false, 0.8f, 104, COLOR_DUSTY_SALMON,"Dirt"},
  { TILE_STONE, false, 0.9f, 105, COLOR_TEXAS_HAZE,"Stone"},
  { TILE_ROAD,  false, 1.0f, 106, COLOR_ASPHALT ,"Road"},
  {TILE_TERRA_PALE,false,1.0f,107,COLOR_TERRA_PALE, "Terra Pale"},
  {TILE_SILT, false, 1.0f, 108, COLOR_OCHRE_SILT,"Ochre Silt"}
};

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

void SetItemCount(int count){
    item_count = count;
}

int GetItemCount(){
    return item_count;
}
