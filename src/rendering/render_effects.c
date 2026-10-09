#include "rendering/render_effects.h"
#include "engine/palette.h"
#include <math.h>

void DrawSimpleSparkle(Vector2 pos, Color color, float size) {
    // Halo
    DrawCircleV(pos, size * 5.0f, Fade(color, 0.03f));
    DrawCircleV(pos, size * 3.0f, Fade(color, 0.08f));

    // Flare
    float thin = size * 0.15f;
    float thick = size * 0.4f;

    // Vertical
    DrawLineEx((Vector2){pos.x, pos.y - size}, (Vector2){pos.x, pos.y + size},
               thick, Fade(color, 0.1f));
    DrawLineEx((Vector2){pos.x, pos.y - size}, (Vector2){pos.x, pos.y + size},
               thin, Fade(color, 0.4f));

    // Horizontal
    DrawLineEx((Vector2){pos.x - (size * 0.8f), pos.y},
               (Vector2){pos.x + (size * 0.8f), pos.y}, thick,
               Fade(color, 0.1f));
    DrawLineEx((Vector2){pos.x - (size * 0.8f), pos.y},
               (Vector2){pos.x + (size * 0.8f), pos.y}, thin,
               Fade(color, 0.4f));

    // Core
    DrawCircleV(pos, size * 0.8f, Fade(color, 0.5f));
    DrawCircleV(pos, size * 0.4f, ColorBrightness(color, 0.9f));
    DrawCircleV(pos, size * 0.3f, (Color){255, 255, 255, 255});
}

void DrawWaterEffects(Map *map, int x, int y) {
    if (x < 0 || x >= map->columns || y < 0 || y >= map->rows)
        return;

    float h = map->grid[y][x].height * 8.0f;
    Vector2 g1 = map->grid[y][x].isoPos;
    Vector2 t1 = {g1.x, g1.y - h};

    // Sparkle Logic
    float tileSeed = (float)(x * 12.9898f + y * 78.233f);
    float sparkleTime = sinf(GetTime() * 2.5f + tileSeed);

    if (sparkleTime > 0.97f) {
        float offsetX = fmodf(tileSeed * 43758.5453f, (float)TILE_SIZE);
        float offsetY = fmodf(tileSeed * 12345.6789f, (float)TILE_SIZE / 2.0f);

        Vector2 sparklePos = {t1.x + offsetX - (int)(TILE_SIZE / 2),
                              t1.y + offsetY};
        float sizePulse =
            (sinf(GetTime() * 8.0f + tileSeed) + 1.0f) * 1.5f + 1.0f;

        DrawSimpleSparkle(sparklePos, COLOR_INDANTHRONE_BLUE, sizePulse);
    }
}
