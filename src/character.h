#ifndef CHARACTER_H
#define CHARACTER_H
#include "raylib.h"
typedef struct Character
{
  /* data */
  char* name;
  Texture2D sprite;
  Vector2 position;
  int dialogId;
} Character;

Character GetCharacterDefault();
void Draw_Character(Character* character);
void Close_Character(Character* character);
#endif