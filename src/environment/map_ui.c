#include "environment/map_ui.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "defs/types_env.h"
#include "engine/palette.h"
#include "systems/player.h"
#include "systems/script_manager.h"
#include "systems/input.h"
#include "registry/register.h"
#include "raymath.h"
#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "raylib.h"
#include "registry/mineral_register.h"

bool IsTileOccludingPlayer(Map* map, int x, int y) {
    if (map->player == NULL) return false;

    // Get player's tile coordinates
    int px = (int)(map->player->position.x / TILE_SIZE);
    int py = (int)(map->player->position.y / TILE_SIZE);

    int dx = x - px;
    int dy = y - py;

    // Use symmetric bounds (0 to 2 for both axes) to ensure a uniform 3x3 cutout shape
    if (dx < 0 || dx > 2 || dy < 0 || dy > 2) return false;

    float tileH = map->grid[y][x].height * 8.0f;
    float playerAlt = map->player->altitude;

    // Must be taller than the player + sprite height to trigger transparency
    Texture2D* sprite = GetSprite(ENTITY_PLAYER, 0);
    float spriteH = (sprite != NULL) ? (float)sprite->height : 48.0f;

    if (tileH <= playerAlt + spriteH) return false;

    return true;
}

void Draw_Tile(Map* map, int x, int y) {
    if (x < 0 || x >= map->columns || y < 0 || y >= map->rows) return;

    float h = map->grid[y][x].height * 8.0f;
    Color base = TILE_REGISTRY[map->grid[y][x].type].color;
    bool blocking = TILE_REGISTRY[map->grid[y][x].type].is_blocking;

    // Check if this tile is covering the player
    bool occluding = IsTileOccludingPlayer(map, x, y);
    if (occluding) {
        base.a = 70; // Fade the tile face
    }

    Vector2 g1 = map->grid[y][x].isoPos;
    Vector2 g2 = { g1.x + TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) };
    Vector2 g4 = { g1.x - TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) };
    Vector2 g3 = { g1.x,             g1.y + TILE_SIZE };

    Vector2 t1 = { g1.x, g1.y - h };
    Vector2 t2 = { g2.x, g2.y - h };
    Vector2 t3 = { g3.x, g3.y - h };
    Vector2 t4 = { g4.x, g4.y - h };

    // WALL DRAWING with strict neighbor checking
    if (h > 0 && !blocking) {
        Color sideL = { (unsigned char)(base.r * 0.8f), (unsigned char)(base.g * 0.8f), (unsigned char)(base.b * 0.8f), base.a };
        Color sideR = { (unsigned char)(base.r * 0.6f), (unsigned char)(base.g * 0.6f), (unsigned char)(base.b * 0.6f), base.a };
        int currentH = map->grid[y][x].height;

        // Check East Neighbor
        if (x + 1 < map->columns) {
            int targetH = map->grid[y][x+1].height;
            if (targetH < currentH) {
                float neighborH = map->grid[y][x+1].height * 8.0f;
                Vector2 n2 = { g2.x, g2.y - neighborH };
                Vector2 n3 = { g3.x, g3.y - neighborH };
                DrawTriangleFan((Vector2[]){ t3, n3, n2, t2 }, 4, sideL);
            }
        }

        // Check South Neighbor
        if (y + 1 < map->rows) {
            if (map->grid[y+1][x].height < map->grid[y][x].height) {
                float neighborH = map->grid[y+1][x].height * 8.0f;
                Vector2 n4 = { g4.x, g4.y - neighborH };
                Vector2 n3 = { g3.x, g3.y - neighborH };
                DrawTriangleFan((Vector2[]){ t4, n4, n3, t3 }, 4, sideR);
            }
        }
    }

    if (map->grid[y][x].type == TILE_WATER) {
        float pulse = sinf(GetTime()) * 20.0f;
        Color waterColor = COLOR_BEAVIS_SHIRT;
        waterColor.a = occluding ? 50 : (100 + (unsigned char)pulse);
        DrawTriangleFan((Vector2[]){ t1, t4, t3, t2 }, 4, waterColor);
    } else {
        Color gridColor = ColorBrightness(base, -0.2f);
        gridColor.a = base.a;
        DrawTriangleFan((Vector2[]){ t1, t4, t3, t2 }, 4, gridColor);

        // Draw the top face slightly inset
        float inset = 0.2f;
        Vector2 it1 = { t1.x, t1.y + inset };
        Vector2 it2 = { t2.x - inset, t2.y };
        Vector2 it3 = { t3.x, t3.y - inset };
        Vector2 it4 = { t4.x + inset, t4.y };
        DrawTriangleFan((Vector2[]){ it1, it4, it3, it2 }, 4, base);
    }
}

void Draw_Buildings(Map* map, int current_x, int current_y) {
    BuildingZone* b = map->buildings;
    while (b != NULL) {
        if (current_x >= b->x1 && current_x <= b->x2 && current_y >= b->y1 && current_y <= b->y2) {

            bool playerInside = false;
            if (map->player != NULL) {
                int ptx = (int)(map->player->position.x / TILE_SIZE);
                int pty = (int)(map->player->position.y / TILE_SIZE);
                if (ptx >= b->x1 && ptx <= b->x2 && pty >= b->y1 && pty <= b->y2) {
                    playerInside = true;
                }
            }

            float roofH = (b->min_height + b->max_height) * 8.0f;
            float floorH = b->min_height * 8.0f;

            Color roofColor = b->color;
            Color wallSouth = ColorBrightness(roofColor, -0.3f);
            Color wallEast = ColorBrightness(roofColor, -0.5f);

            if (playerInside) {
                roofColor.a = 70;
                wallSouth.a = 70;
                wallEast.a = 70;
            }

            // Get standard isometric corners for this specific 1x1 tile slice
            Vector2 g1 = map->grid[current_y][current_x].isoPos;

            // Floor coordinates (anchors)
            // Vector2 f1 = { g1.x, g1.y - floorH };
            Vector2 f2 = { g1.x + TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) - floorH };
            Vector2 f4 = { g1.x - TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) - floorH };
            Vector2 f3 = { g1.x, g1.y + TILE_SIZE - floorH };

            // Roof coordinates (elevated)
            Vector2 r1 = { g1.x, g1.y - roofH };
            Vector2 r2 = { g1.x + TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) - roofH };
            Vector2 r4 = { g1.x - TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) - roofH };
            Vector2 r3 = { g1.x, g1.y + TILE_SIZE - roofH };

            if (current_y == b->y2) {
                DrawTriangleFan((Vector2[]){ r4, f4, f3, r3 }, 4, wallSouth);
            }

            // 3. Draw East Wall segment ONLY if this tile is on the right edge
            if (current_x == b->x2) {
                DrawTriangleFan((Vector2[]){ r3, f3, f2, r2 }, 4, wallEast);
            }

            // 4. Draw Roof segment for this specific tile
            DrawTriangleFan((Vector2[]){ r1, r4, r3, r2 }, 4, roofColor);
        }
        b = b->next;
    }
}


void DrawSimpleSword(Vector2 position, float rotation) {
    float bladeLength = 34.0f;
    float tipLength = 12.0f;
    float bladeWidth = 4.5f;
    float guardWidth = 14.0f;
    float guardThick = 2.5f;
    float hiltLength = 14.0f;

    // METALLIC PALETTE - Using Sunken Ink as the base
    Color metalShadow = COLOR_SUNKEN_INK;
    // Light side is Sunken Ink lightened up significantly
    Color metalBase = ColorBrightness(COLOR_SUNKEN_INK, 0.4f);
    Color highlight = WHITE;
    Color gold = COLOR_PYRITE_BRASS;
    Color leather = COLOR_BURNT_SIENNA;

    float cosR = cosf(rotation);
    float sinR = sinf(rotation);
    float cosP = cosf(rotation + PI/2.0f);
    float sinP = sinf(rotation + PI/2.0f);

    // HILT & POMMEL
    Vector2 hiltEnd = { position.x - cosR * hiltLength, position.y - sinR * hiltLength };
    DrawLineEx(position, hiltEnd, 3.0f, leather);
    DrawCircleV(hiltEnd, 2.5f, gold);

    // THE GUARD
    Vector2 gL = { position.x - cosP * guardWidth/2, position.y - sinP * guardWidth/2 };
    Vector2 gR = { position.x + cosP * guardWidth/2, position.y + sinP * guardWidth/2 };
    DrawLineEx(gL, gR, guardThick, gold);

    // BLADE POINTS
    Vector2 bL = { position.x - cosP * bladeWidth/2, position.y - sinP * bladeWidth/2 };
    Vector2 bR = { position.x + cosP * bladeWidth/2, position.y + sinP * bladeWidth/2 };
    Vector2 sL = { bL.x + cosR * bladeLength, bL.y + sinR * bladeLength };
    Vector2 sR = { bR.x + cosR * bladeLength, bR.y + sinR * bladeLength };
    Vector2 tip = { position.x + cosR * (bladeLength + tipLength), position.y + sinR * (bladeLength + tipLength) };

    // BLADE GEOMETRY
    DrawTriangle(sL, bL, position, metalShadow);
    DrawTriangle(tip, sL, position, metalShadow);
    DrawTriangle(bR, sR, position, metalBase);
    DrawTriangle(sR, tip, position, metalBase);

    // FINISHING SHINE
    DrawLineV(position, tip, highlight);
}



void Draw_MapEntity(MapEntity* entity, Map* map) {
    Vector2 position = GetWorldToIso(entity->position);
    position.y -= entity->altitude;

    if(entity->type == ENTITY_MINERAL){
        DrawMineral(entity->id, position);
    }else{
        Texture2D* sprite = GetSprite(entity->type, entity->id);
        float renderHeight = (entity->type == ENTITY_ITEM) ? (sprite->height * 0.5f) : (float)sprite->height;
        float renderWidth = (entity->type == ENTITY_ITEM) ? (sprite->width * 0.5f) : (float)sprite->width;
        Vector2 drawPos = { position.x - (renderWidth / 2), position.y - renderHeight };

        if(entity->type == ENTITY_ITEM ) {
            DrawTextureEx(*sprite, drawPos, 0.0, 0.5, WHITE);
        } else {
            DrawTextureV(*sprite, drawPos, WHITE);
            DrawText(GetName(entity->type, entity->id), drawPos.x+20, drawPos.y-20.0, 2.0, COLOR_SUNKEN_INK);
            if(entity->type == ENTITY_ENEMY){
                char hpStr[10];
                sprintf(hpStr,"hp:%d", entity->hp);
                DrawText(hpStr, drawPos.x+20, drawPos.y-10.0, 1.0, COLOR_SUNKEN_INK);
            }
            // SHADOW LOGIC WITH BOUNDS CHECK
            int tx = (int)(entity->position.x / TILE_SIZE);
            int ty = (int)(entity->position.y / TILE_SIZE);

            float floorY = 0;
            // SAFETY: Prevent accessing grid[-1] or grid[MAX]
            if (tx >= 0 && tx < map->columns && ty >= 0 && ty < map->rows) {
                floorY = map->grid[ty][tx].height * 8.0f;
            }

            Vector2 shadowPos = GetWorldToIso(entity->position);
            shadowPos.y -= floorY;
            DrawCircleGradient(shadowPos.x, shadowPos.y, 8, Fade(BLACK, 0.3f), BLANK);
        }

            if(entity->type == ENTITY_PLAYER) {
                // Adjusted offsets to align with the character's right arm (viewer's left-ish)
                // Based on image_0f13da.png, his 'hand' area is about 32 pixels down and 8 pixels in.
                Vector2 handPos = { drawPos.x + 12, drawPos.y + 48 };

                DrawSimpleSword(handPos, map->player->combat.attackAngle);
            }
    }


}

void DrawWaterEffects(Map* map, int x, int y) {
    if (x < 0 || x >= map->columns || y < 0 || y >= map->rows) return;

    float h = map->grid[y][x].height * 8.0f;
    Vector2 g1 = map->grid[y][x].isoPos;
    Vector2 t1 = { g1.x, g1.y - h };

    // 1. The Sparkle Logic (ONLY the effects)
    float tileSeed = (float)(x * 12.9898f + y * 78.233f);
    float sparkleTime = sinf(GetTime() * 2.5f + tileSeed);

    if (sparkleTime > 0.97f) {
        float offsetX = fmodf(tileSeed * 43758.5453f, (float)TILE_SIZE);
        float offsetY = fmodf(tileSeed * 12345.6789f, (float)TILE_SIZE / 2.0f);

        // Adjusting sparklePos to stay within the diamond
        Vector2 sparklePos = { t1.x + offsetX - (int)(TILE_SIZE/2), t1.y + offsetY };
        float sizePulse = (sinf(GetTime() * 8.0f + tileSeed) + 1.0f) * 1.5f + 1.0f;

        // Instead of just blue, pick a "glitter
        DrawSimpleSparkle(sparklePos, COLOR_INDANTHRONE_BLUE, sizePulse);
        // DrawCircleV(sparklePos, sizePulse * 0.3f, WHITE);
    }
}

void DrawSimpleSparkle(Vector2 pos, Color color, float size) {
    // 1. THE HALO (Very faint, very wide)
    // This hides the 'edges' of the lines by pre-lighting the area
    DrawCircleV(pos, size * 5.0f, Fade(color, 0.03f));
    DrawCircleV(pos, size * 3.0f, Fade(color, 0.08f));

    // 2. THE FLARE (Softer lines)
    // Instead of one thick line, we draw two:
    // a thick faint one and a thin bright one.
    float thin = size * 0.15f;
    float thick = size * 0.4f;

    // Vertical
    DrawLineEx((Vector2){pos.x, pos.y - size}, (Vector2){pos.x, pos.y + size}, thick, Fade(color, 0.1f));
    DrawLineEx((Vector2){pos.x, pos.y - size}, (Vector2){pos.x, pos.y + size}, thin, Fade(color, 0.4f));

    // Horizontal (Wide)
    DrawLineEx((Vector2){pos.x - (size * 0.8f), pos.y}, (Vector2){pos.x + (size * 0.8f), pos.y}, thick, Fade(color, 0.1f));
    DrawLineEx((Vector2){pos.x - (size * 0.8f), pos.y}, (Vector2){pos.x + (size * 0.8f), pos.y}, thin, Fade(color, 0.4f));

    // 3. THE HOT CORE (Gaussian-style stack)
    // This makes the center feel like it's glowing from within
    DrawCircleV(pos, size * 0.8f, Fade(color, 0.5f));
    DrawCircleV(pos, size * 0.4f, ColorBrightness(color, 0.9f));
    // Force a pure white additive strike at the center
    DrawCircleV(pos, size * 0.3f, (Color){ 255, 255, 255, 255 });

}

void Draw_Map(Map* map, Camera2D* camera) {
    if (map == NULL || !map->is_ready) return;

    BeginMode2D(*camera);

    // 1. Get Screen Corners
    Vector2 tl = GetScreenToWorld2D((Vector2){0,0}, *camera);
    Vector2 tr = GetScreenToWorld2D((Vector2){SCREEN_WIDTH,0}, *camera);
    Vector2 bl = GetScreenToWorld2D((Vector2){0,SCREEN_HEIGHT}, *camera);
    Vector2 br = GetScreenToWorld2D((Vector2){SCREEN_WIDTH,SCREEN_HEIGHT}, *camera);

    // 2. Convert corners to Grid indices
    Vector2 g1 = GetIsoWorldToGrid(tl);
    Vector2 g2 = GetIsoWorldToGrid(tr);
    Vector2 g3 = GetIsoWorldToGrid(bl);
    Vector2 g4 = GetIsoWorldToGrid(br);

    // 3. THE "BRUTE FORCE" PADDING
    // We expand the bounds by 25 tiles in every direction.
    // This ensures that even very tall tiles from "off-screen" are drawn.
    int min_x = (int)fminf(fminf(g1.x, g2.x), fminf(g3.x, g4.x)) - 25;
    int max_x = (int)fmaxf(fmaxf(g1.x, g2.x), fmaxf(g3.x, g4.x)) + 25;
    int min_y = (int)fminf(fminf(g1.y, g2.y), fminf(g3.y, g4.y)) - 25;
    int max_y = (int)fmaxf(fmaxf(g1.y, g2.y), fmaxf(g3.y, g4.y)) + 25; // Huge bottom buffer

    // 4. HARD CLAMPING TO ACTUAL MAP LIMITS
    // This is the only way to ensure we don't crash but also don't stop at 48/50.
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= map->columns) max_x = map->columns - 1;
    if (max_y >= map->rows)    max_y = map->rows - 1;

    // 5. Safe Bucket Allocation
    // MapEntity** buckets = (MapEntity**)calloc(map->rows, sizeof(MapEntity*));
    memset(map->buckets, 0, sizeof(map->buckets));
    MapEntity* e = map->entities;
    while (e != NULL) {
        int ty = (int)(e->position.y / TILE_SIZE);
        if (ty >= 0 && ty < map->rows) {
            e->next_in_bucket = map->buckets[ty];
            map->buckets[ty] = e;
        }
        e = e->next;
    }

        // 6. THE LOOP: Use <= to ensure the last index (49) is hit.

for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            Draw_Tile(map, x, y);
            MapEntity* curr = map->buckets[y];
            while (curr != NULL) {
                int tx = (int)(curr->position.x / TILE_SIZE);
                if (tx == x) {
                    Draw_MapEntity(curr, map);
                }
                curr = curr->next_in_bucket;
            }
            Draw_Buildings(map, x, y);
        }
    }

    // Draw_Buildings(map, min_x, min_y,  max_x,  max_y);
    BeginBlendMode(BLEND_ADDITIVE);
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if(map->grid[y][x].type ==  TILE_WATER){
                DrawWaterEffects(map,x,y);
            }
        }
    }

    EndBlendMode();

    // --- PLAYER OCCLUSION SILHOUETTE PASS ---
        if (map->player != NULL) {
            int px = (int)(map->player->position.x / TILE_SIZE);
            int py = (int)(map->player->position.y / TILE_SIZE);

            bool isOccluded = false;
            // Search surrounding tiles in front of the player for high geometry
            for (int dy = 0; dy <= 4; dy++) {
                for (int dx = 0; dx <= 4; dx++) {
                    int checkX = px + dx;
                    int checkY = py + dy;
                    if (checkX < map->columns && checkY < map->rows) {
                        if (IsTileOccludingPlayer(map, checkX, checkY)) {
                            isOccluded = true;
                            break;
                        }
                    }
                }
                if (isOccluded) break;
            }

            // If covered, render a glowing x-ray silhouette
            if (isOccluded) {
                Vector2 pPos = GetWorldToIso(map->player->position);
                pPos.y -= map->player->altitude;
                Texture2D* sprite = GetSprite(map->player->type, map->player->id);
                Vector2 drawPos = { pPos.x - (sprite->width / 2.0f), pPos.y - sprite->height };

                // Draw bright cyan/white silhouette overlay
                DrawTextureV(*sprite, drawPos, Fade(COLOR_SHELL_PINK, 0.6f));
                DrawCircleLines(pPos.x, pPos.y - 12, 10, WHITE);
            }
        }
    EndMode2D();


    DrawFPS(10, 10);
}
