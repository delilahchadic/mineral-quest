#include "systems/minerals/mineral_draw_functions.h"
#include <math.h>
#include "engine/palette.h"

void DrawRuby(Vector2 center){
    DrawAdvancedGem(center, 7.5f, 7.0f, Fade(COLOR_NEON_CRIMSON, 0.9f), GetTime() * 2.0f, 0.55f);
}

void DrawSaphire(Vector2 center){
    DrawAdvancedGem(center, 7.5f, 7.0f, Fade(COLOR_COBALT_BLUE, 0.9f), GetTime() * 2.0f, 0.55f);
}

void DrawAdvancedGem(Vector2 center, float width, float height, Color baseColor, float rotation, float tableSize) {
    int sides = 6;
    Vector2 waistPoints[6];
    Vector2 topPoints[6];

    // 1. CALCULATE POINTS
    for (int i = 0; i < sides; i++) {
        float angle = (i * (360.0f / sides) * DEG2RAD) + rotation;

        // Waist Ring
        waistPoints[i] = (Vector2){
            center.x + cosf(angle) * width,
            center.y + sinf(angle) * (width / 2.0f)
        };

        // Top Ring (The "Table" cut)
        topPoints[i] = (Vector2){
            center.x + (cosf(angle) * width * tableSize),
            center.y - height + (sinf(angle) * (width / 2.0f) * tableSize)
        };
    }

    // 2. DRAW BOTTOM (The Culet)
    Vector2 bottomPoint = { center.x, center.y + height };
    for (int i = 0; i < sides; i++) {
        Color fColor = ColorBrightness(baseColor, (i % 2 == 0) ? -0.3f : -0.4f);
        // Note: Swapped order of points to ensure correct winding
        DrawTriangle(bottomPoint, waistPoints[(i + 1) % sides], waistPoints[i], fColor);
    }

    // 3. DRAW SIDE WALLS (Connecting waist to top)
    for (int i = 0; i < sides; i++) {
        Color fColor = ColorBrightness(baseColor, (i % 2 == 0) ? -0.1f : 0.1f);
        Vector2 v1 = waistPoints[i];
        Vector2 v2 = topPoints[i];
        Vector2 v3 = waistPoints[(i + 1) % sides];
        Vector2 v4 = topPoints[(i + 1) % sides];
        DrawTriangleStrip((Vector2[]){ v1, v2, v3, v4 }, 4, fColor);
    }

    // 4. DRAW THE TOP CAP (The Table)
    if (tableSize > 0.0f) {
        // Make the top noticeably brighter so it doesn't blend with walls
        Color topColor = ColorBrightness(baseColor, 0.4f);
        Vector2 topCenter = { center.x, center.y - height };

        Vector2 fanPoints[8];
        fanPoints[0] = topCenter;

        // Reverse the loop to fix winding order issues
        for (int i = 0; i < sides; i++) {
            fanPoints[i + 1] = topPoints[sides - 1 - i];
        }
        fanPoints[7] = topPoints[sides - 1]; // Close the loop

        DrawTriangleFan(fanPoints, 8, topColor);

        // Optional: Add a tiny "glint" line on the top edge
        // DrawPolyLinesEx(topCenter, 6, width * tableSize, rotation * RAD2DEG, 2.0f, Fade(WHITE, 0.5f));
    }
}
