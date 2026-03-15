#include "systems/character.h"

void Close_Character(Character* character){
  UnloadTexture(character->sprite);
}