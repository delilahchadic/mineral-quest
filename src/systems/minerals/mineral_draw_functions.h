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
void DrawAmethyst(Vector2 center);
void DrawQuartz(Vector2 center);
void DrawMagnetite(Vector2 center);
void DrawPlagiocase(Vector2 center);
void DrawFeldspar(Vector2 center, Color color_1, Color color_2);
void DrawBiotite(Vector2 center);
void DrawMuscovite(Vector2 center);
void DrawMicaSheet(Vector2 center, Color baseColor, Color highlightColor);
void DrawOlivine(Vector2 center);
void DrawRawChunk(Vector2 center, Color baseColor);
void DrawBeryl(Vector2 center);
void DrawHalite(Vector2 center);
void DrawIsometricCube(Vector2 center, float width, float height, Color baseColor, float rotation, float transparency, bool isMetallic);
#endif
