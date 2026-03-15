#ifndef CHARACTER_H
#define CHARACTER_H
#include "raylib.h"

typedef struct Character
{
  /* data */
  char name[32];
  Texture2D sprite;
  int dialogId;
} Character;

void Close_Character(Character* character);
#endif