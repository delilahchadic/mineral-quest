#ifndef MINERAL_DRAW_FUNCTIONS_H
#define MINERAL_DRAW_FUNCTIONS_H
#include "raylib.h"

void DrawRuby(Vector2 center);
void DrawSaphire(Vector2 center);
void DrawPyrite(Vector2 center);
void DrawMetallicCube(Vector2 center, float width, float height, Color baseColor, float rotation);
void DrawAdvancedGem(Vector2 center, float width, float height, Color baseColor, float rotation, float tableSize);
void DrawProceduralGem(Vector2 center, float width, float height, Color baseColor);
void DrawPearl(Vector2 center);
void DrawRoseQuartz(Vector2 center);
void DrawOrthoclase(Vector2 center);
void DrawEmerald(Vector2 center);
void DrawTopaz(Vector2 center);
void DrawWindowGem(Vector2 center, float width, float length, float height, Color baseColor);
#endif
