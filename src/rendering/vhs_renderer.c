#include "vhs_renderer.h"
#include <math.h>

// Helper to map 2D coordinates (0.0 to 1.0) onto an isometric 3D quad face
Vector2 MapVHSDecalUV(Vector2 bl, Vector2 br, Vector2 tl, Vector2 tr, float u, float v) {
    Vector2 top_p = (Vector2){ tl.x + (tr.x - tl.x) * u, tl.y + (tr.y - tl.y) * u };
    Vector2 bot_p = (Vector2){ bl.x + (br.x - bl.x) * u, bl.y + (br.y - bl.y) * u };
    return (Vector2){ top_p.x + (bot_p.x - top_p.x) * v, top_p.y + (bot_p.y - top_p.y) * v };
}

// Helper to draw a mapped rectangle on the VHS face
void DrawVHSDecalQuad(Vector2 bl, Vector2 br, Vector2 tl, Vector2 tr, float u_min, float u_max, float v_min, float v_max, Color color) {
    Vector2 p1 = MapVHSDecalUV(bl, br, tl, tr, u_min, v_min);
    Vector2 p2 = MapVHSDecalUV(bl, br, tl, tr, u_max, v_min);
    Vector2 p3 = MapVHSDecalUV(bl, br, tl, tr, u_max, v_max);
    Vector2 p4 = MapVHSDecalUV(bl, br, tl, tr, u_min, v_max);

    // Draw double windings so it's always visible regardless of face orientation
    DrawTriangle(p1, p4, p3, color);
    DrawTriangle(p1, p3, p2, color);
    DrawTriangle(p1, p3, p4, color);
    DrawTriangle(p1, p2, p3, color);
}

// Helper to draw a mapped circle (tape spool) on the VHS face
void DrawVHSDecalCircle(Vector2 bl, Vector2 br, Vector2 tl, Vector2 tr, float u_center, float v_center, float rad_u, float rad_v, Color color) {
    const int segments = 16;
    Vector2 center = MapVHSDecalUV(bl, br, tl, tr, u_center, v_center);
    Vector2 prev = MapVHSDecalUV(bl, br, tl, tr, u_center + rad_u, v_center);

    for (int i = 1; i <= segments; i++) {
        float angle = (i * PI * 2.0f) / segments;
        Vector2 next = MapVHSDecalUV(bl, br, tl, tr, u_center + cosf(angle) * rad_u, v_center + sinf(angle) * rad_v);
        DrawTriangle(center, prev, next, color);
        DrawTriangle(center, next, prev, color); // Double winding
        prev = next;
    }
}
void DrawVHSTape(Vector2 center) {
    float time = GetTime();
    float rotation = time * 1.5f;

    float width = 18.0f;
    float depth = 3.0f;
    float height = 12.0f;

    Vector2 top[4], bot[4];

    Vector2 corners[4] = {
        { width * 0.5f,  depth * 0.5f},
        {-width * 0.5f,  depth * 0.5f},
        {-width * 0.5f, -depth * 0.5f},
        { width * 0.5f, -depth * 0.5f}
    };

    for (int i = 0; i < 4; i++) {
        float rx = corners[i].x * cosf(rotation) - corners[i].y * sinf(rotation);
        float ry = corners[i].x * sinf(rotation) + corners[i].y * cosf(rotation);

        bot[i] = (Vector2){ center.x + rx, center.y + (ry * 0.5f) };
        top[i] = (Vector2){ bot[i].x, bot[i].y - height };
    }

    for (int i = 0; i < 4; i++) {
        int next = (i + 1) % 4;

        if (bot[next].x < bot[i].x) {
            Color sideColor = (i % 2 == 0) ? (Color){30, 30, 30, 255} : (Color){15, 15, 15, 255};
            DrawTriangle(bot[i], top[i], bot[next], sideColor);
            DrawTriangle(top[i], top[next], bot[next], sideColor);

            if (i == 0) {
                Vector2 bl = bot[i];
                Vector2 br = bot[next];
                Vector2 tl = top[i];
                Vector2 tr = top[next];

                // 1. Grey Plastic Window (Spanning the middle horizontally)
                DrawVHSDecalQuad(bl, br, tl, tr, 0.1f, 0.9f, 0.35f, 0.75f, (Color){80, 85, 90, 255});

                // 2. Left and Right Black Tape Spools
                DrawVHSDecalCircle(bl, br, tl, tr, 0.22f, 0.55f, 0.08f, 0.12f, (Color){10, 10, 10, 255});
                DrawVHSDecalCircle(bl, br, tl, tr, 0.78f, 0.55f, 0.08f, 0.12f, (Color){10, 10, 10, 255});

                // 3. White Label (Centered exactly between the spools, matching window height)
                DrawVHSDecalQuad(bl, br, tl, tr, 0.35f, 0.65f, 0.35f, 0.75f, (Color){240, 240, 240, 255});

                // 4. (Optional) Small red stripe at the bottom of the white label just like the image
                DrawVHSDecalQuad(bl, br, tl, tr, 0.35f, 0.65f, 0.68f, 0.75f, (Color){210, 50, 50, 255});
            }
        }
    }

    Color topColor = (Color){45, 45, 45, 255};
    DrawTriangle(top[0], top[2], top[1], topColor);
    DrawTriangle(top[0], top[3], top[2], topColor);
}
