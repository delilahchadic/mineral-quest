#include "registry/register.h"
#include "defs/types_engine.h"
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "engine/palette.h"

ItemDefinition ITEM_REGISTRY[100] = {0};
Message DIALOG_REGISTRY[500] = {0};
Character CHARACTER_REGISTRY[200] = {0};
Texture2D SPRITE_OVERRIDE[10] = {0};
Plant PLANT_REGISTRY[100] = {0};
Player GLOBAL_PLAYER;
Player* PLAYER;
static int item_count = 0;
static int plant_count = 0;
static int charcter_count = 0;

TileDefinition TILE_REGISTRY[28] = {
    { TILE_WATER, true,  0.5f, 101, COLOR_CERULEAN_DUSTY,"Water"},
    { TILE_BUFF_TITANIUM,  false, 0.8f, 104, COLOR_BUFF_TITANIUM,"Buff Titanium"},
    { TILE_STONE, false, 0.9f, 105, COLOR_TEXAS_HAZE,"Stone"},
    { TILE_ROAD,  false, 1.0f, 106, COLOR_ASPHALT ,"Road"},
    { TILE_GRASS, false,  1.0f, 102, COLOR_DUSTY_SAP,"Grass"},
    { TILE_MAY_GREEN, false, 1.0f, 108, COLOR_GREEN_GOLD,"Green Gold"},
    { TILE_DEEP_MOSS,false,1.0f,107,COLOR_DEEP_MOSS, "Deep Moss"},
    { TILE_SAGE,  false, 0.8f, 104, COLOR_SAGE_BRUSH,"Sage"},
    { TILE_CELADON,false,1.0f,107,COLOR_CELADON, "Celadon"},
    { TILE_BERYL,false,1.0f,107,COLOR_CERULEAN_BERYL, "Beryl"},
    { TILE_AMAZONITE,false,1.0f,107,COLOR_AMAZONITE, "Amazonite"},
    { TILE_BASALT,  false, 1.0f, 106, COLOR_BASALT ,"Basalt"},
    { TILE_MOON_STONE, false,  1.0f, 102, COLOR_MOONGLOW,"Moon Glow"},
    { TILE_WITHERED_LILAC,false,1.0f,107,COLOR_WITHERED_LILAC, "Withered Lilac"},
    { TILE_SHELL_PINK,false,1.0f,107,COLOR_SHELL_PINK, "Shell Pink"},
    { TILE_BRILLIANT_JAUNE,false,1.0f,107,COLOR_BRILLIANT_JAUNE, "Brilliant Jaune"},
    { TILE_NAPLES_YELLOW,false,1.0f,107,COLOR_NAPLES_YELLOW, "Naples Yellow"},
    { TILE_NICKEL_TITANITE,  false, 0.8f, 104, COLOR_NICKEL_TITANITE,"Nickel Titanite"},
    { TILE_SALMON,  false, 1.0f, 106, COLOR_DUSTY_SALMON ,"Salmon"},
    { TILE_POTTERS_PINK, false, 0.9f, 105, COLOR_POTTERS_PINK,"Potters Pink"},
    {TILE_TERRA_PALE,false,1.0f,107,COLOR_TERRA_PALE, "Terra Pale"},
    {TILE_SILT, false, 1.0f, 108, COLOR_OCHRE_SILT,"Ochre Silt"},
    { TILE_FIRED_GOLD_OCHRE, false, 0.9f, 105, COLOR_FIRED_GOLD_OCHRE,"Fired Gold Ochre"},
    { TILE_YELLOW_OCHRE,  false, 0.4f, 103, COLOR_YELLOW_OCHRE,"Yellow Ochre"},
    { TILE_RED_OCHRE, false, 1.0f, 108, COLOR_RED_OCHRE,"Red Ochre"},
    { TILE_PURPLE_OCHRE,  false, 0.4f, 103, COLOR_PURPLE_OCHRE,"Purple Ochre"},
    { TILE_BLUE_OCHRE,false,1.0f,107,COLOR_BLUE_OCHRE, "Blue Ochre"},
    { TILE_BURNT_SIENNA,false,1.0f,107,COLOR_BURNT_SIENNA, "Burnt Sienna"}
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

void SetPlantCount(int count){
    item_count = count;
}

void SetCharacterCount(int count){
    item_count = count;
}

int GetEntityTypeCount(EntityType type){
    switch (type) {
        case ENTITY_PLANT: return plant_count;
        case ENTITY_ITEM: return item_count;
        case ENTITY_CHARACTER: return charcter_count;
        case ENTITY_MINERAL: return MINERAL_COUNT;
        case ENTITY_PLAYER: return 1;
        default: return -1;
    }
}

void SetEntityTypeCount(EntityType type, int count){
    switch (type) {
        case ENTITY_PLANT:
            plant_count = count;
            return;
        case ENTITY_ITEM:
            item_count = count;
            return;
        case ENTITY_CHARACTER:
            charcter_count = count;
            return;
        default: return;
    }
}
