#ifndef UI_HELPERS_H
#define UI_HELPERS_H
#include "defs/types_core.h"
#include "defs/types_env.h"
#include "raylib.h"
#include "engine/palette.h"
void DrawSelectionOverlay(SelectionBuffer* buffer, Map* map, Camera2D camera);
void ProcessTextInput(char* buffer, int* count, int maxLen, bool numbersOnly);
void DrawButton(Rectangle button, char* label, Color buttonColor, Color textColor);
void DrawCoolHighlight(Vector2 top, Color baseColor);
void DrawTileHighlight(Vector2 top, Color color);
#endif
