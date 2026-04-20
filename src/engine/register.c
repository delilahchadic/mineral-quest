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