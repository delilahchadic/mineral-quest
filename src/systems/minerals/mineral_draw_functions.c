#include "systems/minerals/mineral_draw_functions.h"
#include <math.h>
#include "raymath.h"
#include "engine/palette.h"

void DrawRuby(Vector2 center){
    DrawAdvancedGem(center, 7.5f, 7.0f, Fade(COLOR_NEON_CRIMSON, 0.9f), GetTime() * 2.0f, 0.55f);
}

void DrawSaphire(Vector2 center){
    DrawAdvancedGem(center, 7.5f, 7.0f, Fade(COLOR_COBALT_BLUE, 0.9f), GetTime() * 2.0f, 0.55f);
}

void DrawPyrite(Vector2 center) {
    float size = 8.0f;
    float rotation = GetTime() * 2.0f; // Matches your Ruby/Sapphire speed
    DrawIsometricCube(center, size, size * 1.2f, COLOR_PYRITE_BRASS, rotation,1.0,true);
}

void DrawMagnetite(Vector2 center) {
    float size = 8.0f;
    float rotation = GetTime() * 2.0f; // Matches your Ruby/Sapphire speed
    DrawIsometricCube(center, size, size * 1.2f, COLOR_SUNKEN_INK, rotation,1.0,true);
}

void DrawIsometricCube(Vector2 center, float width, float height, Color baseColor, float rotation, float transparency, bool isMetallic) {
    Vector2 top[4], bot[4];
    float time = GetTime();

    // 1. Calculate points
    for (int i = 0; i < 4; i++) {
        float angle = (i * 90.0f * DEG2RAD) + rotation;
        bot[i] = (Vector2){
            center.x + cosf(angle) * width,
            center.y + sinf(angle) * (width * 0.5f) // Isometric 2:1 ratio
        };
        top[i] = (Vector2){ bot[i].x, bot[i].y - height };
    }

    // 3. Draw Faces
    for (int i = 0; i < 4; i++) {
        int next = (i + 1) % 4;

        if (bot[next].x < bot[i].x) { // Visibility check
            float faceAngle = (i * 90.0f * DEG2RAD) + rotation;
            float shade = cosf(faceAngle) * 0.4f;
            Color faceCol = ColorBrightness(baseColor, shade);
            faceCol.a = (unsigned char)(transparency * 255);

            // Triangle 1 & 2 (Body)
            DrawTriangle(bot[i], top[i], bot[next], faceCol);
            DrawTriangle(top[i], top[next], bot[next], faceCol);

            // 4. Luster Logic
            if (isMetallic) {
                // Sharp Metallic Shimmer
                float shimmer = sinf(time * 5.0f + (bot[i].x * 0.05f));
                if (shimmer > 0.8f) DrawLineV(top[i], bot[i], Fade(WHITE, 0.5f));
            }
        }
    }

    // 5. Top Cap
    Color topCol = ColorBrightness(baseColor, 0.2f);
    topCol.a = (unsigned char)(transparency * 255);
    DrawTriangle(top[0], top[3], top[2], topCol);
    DrawTriangle(top[0], top[2], top[1], topCol);
}

void DrawHalite(Vector2 center) {
    float size = 8.0f;
    float time = GetTime();

    // Use a slightly more "paper" white so the highlights actually show up
    Color saltBase = COLOR_PULP_PAPER;

    // 1. THE CORE GLOW (Keep this, it makes it look translucent)
    DrawCircleGradient(center.x, center.y - 4, size * 0.9f, Fade(WHITE, 0.3f), BLANK);

    // 2. THE MAIN CUBE
    // Transparency at 0.8f makes it look like solid rock salt
    DrawIsometricCube(center, size, size * 1.1f, saltBase, time * 0.5f, 0.8f, false);
}

void DrawMetallicCube(Vector2 center, float width, float height, Color baseColor, float rotation) {
    Vector2 top[4], bot[4];

    // 1. Calculate points
    for (int i = 0; i < 4; i++) {
        float angle = (i * 90.0f * DEG2RAD) + rotation;
        bot[i] = (Vector2){
            center.x + cosf(angle) * width,
            center.y + sinf(angle) * (width / 2.0f)
        };
        top[i] = (Vector2){ bot[i].x, bot[i].y - height };
    }

    // 2. Draw Side Faces (Corrected Winding)
    for (int i = 0; i < 4; i++) {
        int next = (i + 1) % 4;

        // Visibility check: only draw faces where 'next' is to the left of 'i' (the front faces)
        if (bot[next].x < bot[i].x) {
            float faceAngle = (i * 90.0f * DEG2RAD) + rotation;
            // High contrast: force the range from very dark to very bright
            float shade = cosf(faceAngle) * 0.6f;
            Color faceCol = ColorBrightness(baseColor, shade);

            // Add a "Specular Ping" (Light hitting the corner)
            // This doesn't care about rotation, it cares about screen space
            if (cosf(faceAngle + rotation) > 0.8f) {
                DrawLineV(top[i], bot[i], Fade(WHITE, 0.6f)); // Sharp vertical highlight
            }

            // WINDING FIX: Standard CCW order (Bot -> Top -> Next Bot)
            // Triangle 1
            DrawTriangle(bot[i], top[i], bot[next], faceCol);
            // Triangle 2
            DrawTriangle(top[i], top[next], bot[next], faceCol);

            // METALLIC SHIMMER
            float shimmer = sinf(GetTime() * 5.0f + (bot[i].x * 0.05f));
            if (shimmer > 0.8f) {
                // Line should also follow the corrected points
                DrawLineV(top[i], bot[i], Fade(WHITE, 0.5f));
            }
        }
    }

    // 3. Draw Top Cap (Corrected Winding)
    Color topCol = ColorBrightness(baseColor, 0.4f);
    // Order: Front -> Left -> Back, then Front -> Back -> Right
    DrawTriangle(top[0], top[3], top[2], topCol);
    DrawTriangle(top[0], top[2], top[1], topCol);
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

void DrawPearl(Vector2 center) {
    float radius = 8.0f;
    float time = GetTime();

    // 1. THE LUSTROUS BASE
    // Pure white base, but we use a very slight blue-tinted white to keep it "cool"
    DrawCircleV(center, radius, (Color){ 250, 252, 255, 255 });

    // 2. PRISMATIC IRIDESCENCE
    // We calculate a shifting hue based on time for that "rainbow" effect
    float hue1 = fmodf(time * 40.0f, 360.0f);
    float hue2 = fmodf(time * 40.0f + 180.0f, 360.0f); // Opposite side of the color wheel

    Color iris1 = ColorFromHSV(hue1, 0.6f, 1.0f);
    Color iris2 = ColorFromHSV(hue2, 0.5f, 1.0f);

    // Dynamic orbits for the color patches
    Vector2 p1 = { center.x + cosf(time * 1.2f) * 2.5f, center.y + sinf(time * 1.2f) * 2.5f };
    Vector2 p2 = { center.x + cosf(time * -0.8f) * 3.0f, center.y + sinf(time * -0.8f) * 3.0f };

    // Draw additive prismatic gradients
    DrawCircleGradient(p1.x, p1.y, radius * 1.1f, Fade(iris1, 0.4f), BLANK);
    DrawCircleGradient(p2.x, p2.y, radius * 1.1f, Fade(iris2, 0.35f), BLANK);

    // 3. CORE LUSTER (The "Glow")
    // This removes the grayness by washing the center with a soft white glow
    DrawCircleGradient(center.x, center.y, radius * 0.7f, Fade(WHITE, 0.6f), BLANK);

    // 4. SPECULAR BLOOM (The Sharp Glint)
    Vector2 glintPos = { center.x - 2.5f, center.y - 2.5f };

    // The "Halo" around the glint
    DrawCircleGradient(glintPos.x, glintPos.y, 4.5f, Fade(WHITE, 0.5f), BLANK);
    // The sharp specular point
    DrawCircleV(glintPos, 1.5f, WHITE);

    // 5. PRISMATIC RIM
    // We replace the sector lines with a ring segment to remove the "pie slice" lines.
    Color rimColor = ColorFromHSV(fmodf(time * 20.0f, 360.0f), 0.3f, 1.0f);

    // Draw the full faint halo
    DrawCircleLines(center.x, center.y, radius, Fade(rimColor, 0.4f));

    // The Sharp Prismatic Arc (The "Tangent" highlight)
    // Parameters: Center, InnerRadius, OuterRadius, StartAngle, EndAngle, Segments, Color
    // Using InnerRadius == OuterRadius (radius) ensures only the outer arc is drawn.
    DrawRingLines(center, radius, radius, 200, 290, 20, Fade(WHITE, 0.7f));
}



void DrawBasicQuartz(Vector2 center, Color color) {
    float width = 7.0f;
    float bodyHeight = 12.0f;
    float capHeight = 6.0f;
    float time = GetTime();
    int sides = 6;
    float rotation = time * 2.0f;

    Vector2 topRing[6], botRing[6];

    // 1. CALCULATE RINGS (Standardized 2:1 Isometric)
    for (int i = 0; i < sides; i++) {
        float angle = (i * (360.0f / sides) * DEG2RAD) + rotation;
        topRing[i] = (Vector2){ center.x + cosf(angle) * width, center.y - (bodyHeight / 2.0f) + sinf(angle) * (width * 0.5f) };
        botRing[i] = (Vector2){ center.x + cosf(angle) * width, center.y + (bodyHeight / 2.0f) + sinf(angle) * (width * 0.5f) };
    }

    Vector2 tipTop = { center.x, center.y - (bodyHeight / 2.0f) - capHeight };
    Vector2 tipBot = { center.x, center.y + (bodyHeight / 2.0f) + capHeight };

    // 2. SOFT SUBSURFACE GLOW
    DrawCircleGradient(center.x, center.y, width * 1.6f, Fade(color, 0.25f), BLANK);

    // 3. DRAW FACETS
    // 3. DRAW FACETS
    for (int i = 0; i < sides; i++) {
        int next = (i + 1) % sides;

        if (topRing[next].x < topRing[i].x) {
            float faceAngle = (i * (360.0f / sides) * DEG2RAD) + rotation + (PI / sides);

            // Shift range to be mostly positive (0.0 to 0.4)
            float lightIntensity = (cosf(faceAngle) * 0.2f) + 0.2f;

            // Use a higher alpha (0.8f - 0.9f) to keep the color solid and vibrant
            Color fColor = ColorBrightness(color, lightIntensity);
            fColor.a = 230; // Solid but slightly glassy

            // Instead of darkening the body, keep it near base color
            Color dColor = ColorBrightness(fColor, -0.05f);

            // TOP CAP (Make this the brightest part)
            DrawTriangle(topRing[i], tipTop, topRing[next], ColorBrightness(fColor, 0.1f));

            // PRISM BODY
            DrawTriangle(botRing[i], topRing[i], botRing[next], dColor);
            DrawTriangle(topRing[i], topRing[next], botRing[next], dColor);

            // BOTTOM CAP (The only truly dark part)
            DrawTriangle(botRing[i], botRing[next], tipBot, ColorBrightness(dColor, -0.2f));

            // 4. THE SHIMMER (Cleaned up threshold)
            float shimmerPos = topRing[i].x - center.x;
            float shimmer = sinf(time * 4.0f + (shimmerPos * 0.15f));
            if (shimmer > 0.92f) {
                DrawLineV(tipTop, topRing[i], Fade(WHITE, 0.4f));
                DrawLineV(topRing[i], botRing[i], Fade(WHITE, 0.25f));
            }
        }
    }
}

void DrawRoseQuartz(Vector2 center) {
    DrawBasicQuartz(center, COLOR_POTTERS_PINK);
}

void DrawAmethyst(Vector2 center) {
    DrawBasicQuartz(center, COLOR_WITHERED_LILAC);
}

void DrawQuartz(Vector2 center) {
    DrawBasicQuartz(center, COLOR_TEXAS_HAZE);
}

void DrawOrthoclase(Vector2 center) {
    DrawFeldspar(center, COLOR_DUSTY_SALMON, COLOR_BRILLIANT_JAUNE);
}
void DrawPlagiocase(Vector2 center){
    DrawFeldspar(center, COLOR_TEXAS_HAZE, COLOR_BLUE_OCHRE);
}

void DrawFeldspar(Vector2 center, Color color_1, Color color_2) {
    float width = 10.0f;
    float height = 14.0f;
    float lean = 4.0f;
    float time = GetTime();
    int sides = 4;
    float rotation = time * 1.2f;

    Vector2 bot[4];
    Vector2 top[4];

    // 1. STABILIZE CORNERS: Pre-calculate jittered vertices
    for (int i = 0; i < sides; i++) {
        float angle = (i * 90.0f * DEG2RAD) + rotation;
        // float jitter = sinf(time * 20.0f + i) * 0.3f;

        float cosA = cosf(angle);
        float sinA = sinf(angle) * 0.5f;

        bot[i] = (Vector2){
            center.x + cosA * width,
            center.y + (height / 2.0f) + sinA * width
        };
        top[i] = (Vector2){
            center.x + cosA * width + lean,
            center.y - (height / 2.0f) + sinA * width
        };
    }

    // 2. MATTE UNDER-GLOW
    DrawCircleGradient(center.x, center.y, width * 1.2f, Fade(color_1, 0.2f), BLANK);

    // 3. DRAW SIDE FACETS
    for (int i = 0; i < sides; i++) {
        int next = (i + 1) % sides;

        if (top[next].x < top[i].x) {
            float faceAngle = (i * 90.0f * DEG2RAD) + rotation;
            float light = (cosf(faceAngle) * 0.2f) + 0.1f;
            Color faceColor = ColorLerp(color_1, color_2, light);

            // Using the pre-calculated points ensures hinges are connected
            DrawTriangle(bot[i], top[i], bot[next], faceColor);
            DrawTriangle(top[i], top[next], bot[next], faceColor);

            // Earthy Cleavage Texture
            for (int j = 1; j < 3; j++) {
                float offset = j * 0.3f;
                Vector2 start = Vector2Lerp(top[i], top[next], offset);
                Vector2 end = Vector2Lerp(bot[i], bot[next], offset + 0.1f);
                DrawLineV(start, end, Fade(color_2, 0.15f));
            }
        }
    }

    // 4. DRAW TOP CAP (The missing face)
    Color topColor = ColorLerp(color_1, color_2, 0.4f);
    // Standard triangle fan for a 4-sided cap
    DrawTriangle(top[0], top[3], top[2], topColor);
    DrawTriangle(top[0], top[2], top[1], topColor);
}



void DrawProceduralGem(Vector2 center, float width, float height, Color baseColor) {
    int sides = 6; // Hexagonal
    Vector2 points[6];

    // 1. Calculate the 'waist' points
    for (int i = 0; i < sides; i++) {
        float angle = i * (360.0f / sides) * DEG2RAD;
        points[i] = (Vector2){
            center.x + cosf(angle) * width,
            center.y + sinf(angle) * (width / 2.0f) // Isometric squash
        };
    }

    // 2. Draw Top Faces (Pointed up)
    Vector2 topPoint = { center.x, center.y - height };
    for (int i = 0; i < sides; i++) {
        // Vary the brightness per face to simulate 'facets'
        Color facetColor = ColorBrightness(baseColor, (i % 2 == 0) ? -0.1f : 0.1f);
        DrawTriangle(topPoint, points[(i + 1) % sides], points[i], facetColor);
    }

    // 3. Draw Bottom Faces (Pointed down)
    Vector2 bottomPoint = { center.x, center.y + height };
    for (int i = 0; i < sides; i++) {
        Color facetColor = ColorBrightness(baseColor, (i % 2 == 0) ? -0.2f : -0.3f);
        DrawTriangle(bottomPoint, points[i], points[(i + 1) % sides], facetColor);
    }
}

void DrawEmerald(Vector2 center) {
    DrawWindowGem(center, 7.0f, 11.0f, 6.0f,  COLOR_JADE);
}

void DrawBeryl(Vector2 center) {
    DrawWindowGem(center, 7.0f, 11.0f, 6.0f,  COLOR_CERULEAN_BERYL);
}

void DrawTopaz(Vector2 center) {
    DrawWindowGem(center, 9.0f, 12.0f, 6.0f, COLOR_NAPLES_YELLOW);
}

void DrawWindowGem(Vector2 center, float width, float height, float thickness, Color color) {
    float time = GetTime();
    float rotation = time * 2.0f;
    float tableScale = 0.65f;
    Vector2 frontTable[8], girdle[8];
    float cosRot = cosf(rotation);
    float sinRot = sinf(rotation);


    for (int i = 0; i < 8; i++) {
        float angle = (i * 45.0f + 22.5f) * DEG2RAD;
        float nx = cosf(angle);
        float ny = sinf(angle);

        // 1. Center Ring (Girdle)
        // Swapped: cosRot applied to X instead of Y
        girdle[i] = (Vector2){
            center.x + (nx * width * cosRot),
            center.y + (ny * height)
        };

        // 2. Front Ring (Top Window)
        // Swapped: cosRot to X, and thickness shift to X
        frontTable[i] = (Vector2){
            center.x + (nx * width * tableScale * cosRot) + (thickness * sinRot),
            center.y + (ny * height * tableScale)
        };

    }

    // Update the centers for the triangle fans - moved shift to X
    Vector2 frontCenter = { center.x + (thickness * sinRot), center.y };
    Vector2 backCenter  = { center.x - (thickness * sinRot), center.y };

    // 2. DRAW THE BACK TABLE
    for (int i = 0; i < 8; i++) {
        float faceAngle = (i * 45.0f * DEG2RAD) + rotation;
        // Back intensity: 0.1 to 0.4 (darker)
        float intensity = (cosf(faceAngle) * 0.15f) + 0.25f;
        Color dColor = ColorBrightness(color, intensity - 0.1f); // Use negative to darken base
        int next = (i + 1) % 8;
        // Using the backCenter defined above to keep it consistent
        DrawTriangle(backCenter, girdle[i], girdle[next], dColor);
    }

    // 4. DRAW CONNECTING FACETS (Girdle to Front Table)
    for (int i = 0; i < 8; i++) {

        int next = (i + 1) % 8;
        float faceAngle = (i * 45.0f * DEG2RAD) + rotation;
        // Side intensity: -0.3 to +0.3 (high contrast)
        float shade = cosf(faceAngle) * 0.1f;
        Color dColor = ColorBrightness(color, shade);
        DrawTriangle(girdle[i], frontTable[i], frontTable[next], dColor);
        DrawTriangle(girdle[i], frontTable[next], girdle[next], dColor);
    }

    // 5. DRAW THE FRONT TABLE (The Top Window)
    for (int i = 0; i < 8; i++) {
        int next = (i + 1) % 8;
        DrawTriangle(frontCenter, frontTable[next], frontTable[i], color);
    }

    // 6. WIREFRAME HIGHLIGHTS
    for (int i = 0; i < 8; i++) {
        int next = (i + 1) % 8;
        // Inside the wireframe loop
        float edgeShade = cosf((i * 45.0f * DEG2RAD) + rotation);
        float alpha = (edgeShade > 0.7f) ? 0.6f : 0.2f;
        DrawLineV(frontTable[i], frontTable[next], Fade(WHITE, alpha));
    }
}

// Biotite: The Dark, Mafic Mica
void DrawBiotite(Vector2 center) {
    // Deep black-brown with a resinous/sub-metallic luster
    DrawMicaSheet(center, COLOR_SUNKEN_INK, (Color){ 80, 60, 45, 255 });
}

// Muscovite: The Clear, Rivers-region Mica
void DrawMuscovite(Vector2 center) {
    // Silvery-white/clear with a pearly luster
    DrawMicaSheet(center, COLOR_TEXAS_HAZE, Fade(WHITE, 0.8f));
}

void DrawMicaSheet(Vector2 center, Color baseColor, Color highlightColor) {
    float width = 9.0f;
    float thickness = 2.5f; // Very thin to show cleavage planes
    float time = GetTime();
    float rotation = time * 1.5f;
    int sides = 6; // Hexagonal "books"

    Vector2 top[6], bot[6];

    // 1. Calculate Hexagonal Layers
    for (int i = 0; i < sides; i++) {
        float angle = (i * 60.0f * DEG2RAD) + rotation;
        float cosA = cosf(angle);
        float sinA = sinf(angle) * 0.5f; // Isometric squash

        bot[i] = (Vector2){ center.x + cosA * width, center.y + (thickness / 2.0f) + sinA * width };
        top[i] = (Vector2){ bot[i].x, bot[i].y - thickness };
    }

    // 2. Draw Bottom Cap (Darkest)
    DrawTriangleFan((Vector2[]){ center, bot[0], bot[1], bot[2], bot[3], bot[4], bot[5], bot[0] }, 8, ColorBrightness(baseColor, -0.4f));

    // 3. Draw Side Edges (Showing "Layers")
    for (int i = 0; i < sides; i++) {
        int next = (i + 1) % sides;
        if (top[next].x < top[i].x) {
            // Alternate brightness to show the jagged "book" edges
            Color edgeCol = ColorBrightness(baseColor, (i % 2 == 0) ? -0.1f : -0.2f);
            DrawTriangle(bot[i], top[i], bot[next], edgeCol);
            DrawTriangle(top[i], top[next], bot[next], edgeCol);

            // CLEAVAGE LINES: Draw a faint horizontal line to simulate stacked sheets
            DrawLineV((Vector2){top[i].x, top[i].y + thickness/2}, (Vector2){top[next].x, top[next].y + thickness/2}, Fade(highlightColor, 0.3f));
        }
    }

    // 4. Draw Top Face (The "Window")
    // For Mica, the top face is often very reflective
    Color topCol = ColorBrightness(baseColor, 0.2f);
    Vector2 topCenter = { center.x, center.y - (thickness / 2.0f) };

    // Triangle fan for the top hexagonal face
    for (int i = 0; i < sides; i++) {
        int next = (i + 1) % sides;
        DrawTriangle(topCenter, top[next], top[i], topCol);
    }

    // 5. THE PEARLY/METALLIC FLASH
    // Mica has a distinct "sheen" when it catches the light
    float sheen = sinf(time * 3.0f + center.x);
    if (sheen > 0.7f) {
        DrawTriangle(topCenter, top[0], top[1], Fade(highlightColor, 0.4f));
        // Add a sharp "glint" on one edge
        DrawLineV(top[1], top[2], Fade(WHITE, 0.6f));
    }
}

void DrawOlivine(Vector2 center) {
    // A bright, translucent olive green
    Color olivineBase = (Color){ 154, 185, 45, 255 };
    DrawRawChunk(center, olivineBase);
}

void DrawRawChunk(Vector2 center, Color baseColor) {
    float size = 8.0f;
    float verticalScale = 6.0f; // This gives it "height"
    float time = GetTime();
    float rotation = time * 1.5f;
    int points = 5;
    Vector2 verts[5];

    // 1. Calculate the "Waist" Points (The middle ring)
    for (int i = 0; i < points; i++) {
        float angle = (i * (360.0f / points) * DEG2RAD) + rotation;
        // Keep your jitter logic to make it look unpolished
        float jitter = (i % 2 == 0) ? 1.2f : 0.7f;
        verts[i] = (Vector2){
            center.x + cosf(angle) * size * jitter,
            center.y + sinf(angle) * (size / 2.0f) * jitter // Isometric squash
        };
    }

    // 2. Define Top and Bottom Peaks
    Vector2 topPeak = { center.x, center.y - verticalScale };
    Vector2 botPeak = { center.x, center.y + verticalScale };

    // 3. Draw Bottom Half (The part touching the sand)
    for (int i = 0; i < points; i++) {
        int next = (i + 1) % points;
        Color fCol = ColorBrightness(baseColor, -0.3f); // Darker on bottom
        DrawTriangle(botPeak, verts[i], verts[next], fCol);
    }

    // 4. Draw Top Half (The part facing the sun)
    for (int i = 0; i < points; i++) {
        int next = (i + 1) % points;

        // Visibility check: only draw faces in front
        if (verts[next].x < verts[i].x || (verts[i].y > center.y)) {
            float faceAngle = (i * (360.0f / points) * DEG2RAD) + rotation;
            float light = cosf(faceAngle) * 0.2f;
            Color fCol = ColorBrightness(baseColor, light);

            DrawTriangle(topPeak, verts[next], verts[i], fCol);

            // 5. Glassy Specular Glints
            if (sinf(time * 3.0f + i) > 0.85f) {
                DrawLineV(topPeak, verts[i], Fade(WHITE, 0.6f));
            }
        }
    }

    // 6. Subsurface Scattering Glow
    DrawCircleGradient(center.x, center.y, size * 1.2f, Fade(baseColor, 0.2f), BLANK);
}
