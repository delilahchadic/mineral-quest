#include "environment/map_ui.h"

#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "defs/types_systems.h"
#include "engine/palette.h"
#include "raylib.h"
#include "raymath.h"
#include "registry/mineral_register.h"
#include "registry/register.h"
#include "registry/weapon_register.h"
#include "systems/input.h"
#include "systems/player.h"
#include "systems/script_manager.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool IsTileOccludingPlayer(Map *map, int x, int y) {
    // Get player's tile coordinates
    int px = (int)(map->player.position.x / TILE_SIZE);
    int py = (int)(map->player.position.y / TILE_SIZE);

    int dx = x - px;
    int dy = y - py;

    // Use symmetric bounds (0 to 2 for both axes) to ensure a uniform 3x3
    // cutout shape
    if (dx < 0 || dx > 2 || dy < 0 || dy > 2)
        return false;

    float tileH = map->grid[y][x].height * 8.0f;
    float playerAlt = map->player.altitude;

    // Must be taller than the player + sprite height to trigger transparency
    Texture2D *sprite = GetSprite(ENTITY_PLAYER, 0);
    float spriteH = (sprite != NULL) ? (float)sprite->height : 48.0f;

    if (tileH <= playerAlt + spriteH)
        return false;

    return true;
}

void Draw_Tile(Map *map, int x, int y) {
    if (x < 0 || x >= map->columns || y < 0 || y >= map->rows)
        return;

    float h = map->grid[y][x].height * 8.0f;
    Color base = TILE_REGISTRY[map->grid[y][x].type].color;
    bool blocking = TILE_REGISTRY[map->grid[y][x].type].is_blocking;

    // Check if this tile is covering the player
    bool occluding = IsTileOccludingPlayer(map, x, y);
    if (occluding) {
        base.a = 70; // Fade the tile face
    }

    Vector2 g1 = map->grid[y][x].isoPos;
    Vector2 g2 = {g1.x + TILE_SIZE, g1.y + (TILE_SIZE / 2.0f)};
    Vector2 g4 = {g1.x - TILE_SIZE, g1.y + (TILE_SIZE / 2.0f)};
    Vector2 g3 = {g1.x, g1.y + TILE_SIZE};

    Vector2 t1 = {g1.x, g1.y - h};
    Vector2 t2 = {g2.x, g2.y - h};
    Vector2 t3 = {g3.x, g3.y - h};
    Vector2 t4 = {g4.x, g4.y - h};

    // WALL DRAWING - Only render wall faces exposed toward the camera viewer
    if (h > 0 && !blocking) {
        Color sideL = {(unsigned char)(base.r * 0.8f),
                       (unsigned char)(base.g * 0.8f),
                       (unsigned char)(base.b * 0.8f), base.a};
        Color sideR = {(unsigned char)(base.r * 0.6f),
                       (unsigned char)(base.g * 0.6f),
                       (unsigned char)(base.b * 0.6f), base.a};
        int currentH = map->grid[y][x].height;

        // SOUTH-WEST FACE (Facing camera left/down):
        // Only draw if the neighbor to the South (y+1) is lower than this tile!
        if (y + 1 < map->rows) {
            int southH = map->grid[y + 1][x].height;
            if (southH < currentH) {
                float neighborH = southH * 8.0f;
                Vector2 n4 = {g4.x, g4.y - neighborH};
                Vector2 n3 = {g3.x, g3.y - neighborH};
                DrawTriangleFan((Vector2[]){t4, n4, n3, t3}, 4, sideR);
            }
        } else if (currentH > 0) {
            // Map boundary edge
            Vector2 n4 = {g4.x, g4.y};
            Vector2 n3 = {g3.x, g3.y};
            DrawTriangleFan((Vector2[]){t4, n4, n3, t3}, 4, sideR);
        }

        // SOUTH-EAST FACE (Facing camera right/down):
        // Only draw if the neighbor to the East (x+1) is lower than this tile!
        if (x + 1 < map->columns) {
            int eastH = map->grid[y][x + 1].height;
            if (eastH < currentH) {
                float neighborH = eastH * 8.0f;
                Vector2 n2 = {g2.x, g2.y - neighborH};
                Vector2 n3 = {g3.x, g3.y - neighborH};
                DrawTriangleFan((Vector2[]){t3, n3, n2, t2}, 4, sideL);
            }
        } else if (currentH > 0) {
            // Map boundary edge
            Vector2 n2 = {g2.x, g2.y};
            Vector2 n3 = {g3.x, g3.y};
            DrawTriangleFan((Vector2[]){t3, n3, n2, t2}, 4, sideL);
        }
    }

    // TOP FACE DRAWING
    if (map->grid[y][x].type == TILE_WATER) {
        // Layer two sine waves for a smooth, organic breath effect
        double t = GetTime();
        float primaryWave =
            sinf((float)t * 1.5f) * 35.0f; // Slow overall swell (+/- 35 alpha)
        float secondaryWave =
            sinf((float)t * 3.8f) * 10.0f;         // Faster subtle micro-ripple
        float pulse = primaryWave + secondaryWave; // Range: approx +/- 45
        Color waterColor = COLOR_BEAVIS_SHIRT;

        if (occluding) {
            waterColor.a = 50;
        } else {
            // Start from base shirt alpha, apply signed pulse float, then clamp
            // bounds
            float calculatedAlpha = (float)waterColor.a + pulse;

            if (calculatedAlpha < 0.0f)
                calculatedAlpha = 0.0f;
            if (calculatedAlpha > 255.0f)
                calculatedAlpha = 255.0f;

            waterColor.a = (unsigned char)calculatedAlpha;
        }

        DrawTriangleFan((Vector2[]){t1, t4, t3, t2}, 4, waterColor);
    } else {
        Color gridColor = ColorBrightness(base, -0.2f);
        gridColor.a = base.a;
        DrawTriangleFan((Vector2[]){t1, t4, t3, t2}, 4, gridColor);

        // Inset face
        float inset = 0.2f;
        Vector2 it1 = {t1.x, t1.y + inset};
        Vector2 it2 = {t2.x - inset, t2.y};
        Vector2 it3 = {t3.x, t3.y - inset};
        Vector2 it4 = {t4.x + inset, t4.y};
        DrawTriangleFan((Vector2[]){it1, it4, it3, it2}, 4, base);
    }
}

void Draw_Buildings(Map *map, int current_x, int current_y) {
    BuildingZone *b = map->buildings;
    while (b != NULL) {
        if (current_x >= b->x1 && current_x <= b->x2 && current_y >= b->y1 &&
            current_y <= b->y2) {

            bool playerInside = false;

            int ptx = (int)(map->player.position.x / TILE_SIZE);
            int pty = (int)(map->player.position.y / TILE_SIZE);
            if (ptx >= b->x1 && ptx <= b->x2 && pty >= b->y1 && pty <= b->y2) {
                playerInside = true;
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
            Vector2 f2 = {g1.x + TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) - floorH};
            Vector2 f4 = {g1.x - TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) - floorH};
            Vector2 f3 = {g1.x, g1.y + TILE_SIZE - floorH};

            // Roof coordinates (elevated)
            Vector2 r1 = {g1.x, g1.y - roofH};
            Vector2 r2 = {g1.x + TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) - roofH};
            Vector2 r4 = {g1.x - TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) - roofH};
            Vector2 r3 = {g1.x, g1.y + TILE_SIZE - roofH};

            if (current_y == b->y2) {
                DrawTriangleFan((Vector2[]){r4, f4, f3, r3}, 4, wallSouth);
            }

            // Draw East Wall segment ONLY if this tile is on the right edge
            if (current_x == b->x2) {
                DrawTriangleFan((Vector2[]){r3, f3, f2, r2}, 4, wallEast);
            }

            // Draw Roof segment for this specific tile
            DrawTriangleFan((Vector2[]){r1, r4, r3, r2}, 4, roofColor);
        }
        b = b->next;
    }
}
void DrawTargetReticle(Vector2 position, EntityType type) {
    // if (!position) return;

    // 1. Create a pulsing glow effect based on real-time
    float pulse = sinf(GetTime() * 8.0f) * 3.0f;
    float radius = 24.0f + pulse;

    // 2. Color-code by entity type (replace these with your custom color names)
    Color reticleColor = WHITE;
    if(PLAYER->targeting.locked){
        switch (type) {
            case ENTITY_ENEMY:
                reticleColor = COLOR_RED_OCHRE;       // Swap with your custom enemy color (e.g., COLOR_ENEMY_RED)
                break;
            case ENTITY_PLANT:
                reticleColor = COLOR_SAP_GREEN;     // Swap with your custom harvest color
                break;
            case ENTITY_PORTAL:
                reticleColor = COLOR_SNOOT_PINK;
                break;
            default:
                reticleColor = COLOR_CERULEAN_WISTFUL;    // Swap with your default/interactive color
                break;
        }
    }


    // 3. Draw an outer glowing, semi-transparent ring
    DrawRing(position, radius - 3.0f, radius + 1.0f, 0.0f, 360.0f, 16, Fade(reticleColor, 0.35f));

    // 4. Draw the crisp main reticle ring
    DrawCircleLines(position.x, position.y, radius, reticleColor);
}

void Draw_MapEntity(MapEntity *entity, Map *map) {

    Vector2 position = GetWorldToIso(entity->position);
    position.y -= entity->altitude;

    if (entity->type == ENTITY_MINERAL) {
        DrawMineral(entity->entity_id, position);
    } else {
        Texture2D *sprite = GetSprite(entity->type, entity->entity_id);
        float renderHeight = (entity->type == ENTITY_ITEM)
                                 ? (sprite->height * 0.5f)
                                 : (float)sprite->height;
        float renderWidth = (entity->type == ENTITY_ITEM)
                                ? (sprite->width * 0.5f)
                                : (float)sprite->width;
        Vector2 drawPos = {position.x - (renderWidth / 2),
                           position.y - renderHeight};

        if (entity->type == ENTITY_ITEM) {
            DrawTextureEx(*sprite, drawPos, 0.0, 0.5, WHITE);
        } else {
            DrawTextureV(*sprite, drawPos, WHITE);
            DrawText(GetName(entity->type, entity->entity_id), drawPos.x + 20,
                     drawPos.y - 20.0, 2.0, COLOR_SUNKEN_INK);
            if (entity->type == ENTITY_ENEMY) {
                char hpStr[16];
                snprintf(hpStr, sizeof(hpStr), "hp:%d", entity->hp);
                DrawText(hpStr, drawPos.x + 20, drawPos.y - 10.0, 12.0,
                         COLOR_SUNKEN_INK);
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
            DrawCircleGradient(shadowPos.x, shadowPos.y, 8, Fade(BLACK, 0.3f),
                               BLANK);
        }

        if(entity->instance_id==PLAYER->targeting.target_id){
            Texture2D *sprite = GetSprite(entity->type, entity->entity_id);
            DrawTargetReticle((Vector2){drawPos.x + (sprite->width/2.0),drawPos.y + (sprite->height/2.0),} , entity->type);
        }
        if (entity == &map->player) {
            Vector2 handPos = {drawPos.x + 12, drawPos.y + 48};
            DrawWeapon(GLOBAL_PLAYER.gear.weapon_id, handPos,
                       map->player.combat.attackAngle);

        }
    }
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

void Draw_Map(Map *map, Camera2D *camera, bool drawPlayer) {
    if (map == NULL || !map->is_ready)
        return;

    BeginMode2D(*camera);

    // 1. Get Screen Corners
    Vector2 tl = GetScreenToWorld2D((Vector2){0, 0}, *camera);
    Vector2 tr = GetScreenToWorld2D((Vector2){SCREEN_WIDTH, 0}, *camera);
    Vector2 bl = GetScreenToWorld2D((Vector2){0, SCREEN_HEIGHT}, *camera);
    Vector2 br =
        GetScreenToWorld2D((Vector2){SCREEN_WIDTH, SCREEN_HEIGHT}, *camera);

    // 2. Convert corners to Grid indices
    Vector2 g1 = GetIsoWorldToGrid(tl);
    Vector2 g2 = GetIsoWorldToGrid(tr);
    Vector2 g3 = GetIsoWorldToGrid(bl);
    Vector2 g4 = GetIsoWorldToGrid(br);
    // Dynamically calculate top padding based on altitude to save performance
    int altitudePadding = (int)(map->player.altitude / 8.0f) + 25;
    if (altitudePadding > 100) altitudePadding = 100;

    int min_y = (int)fminf(fminf(g1.y, g2.y), fminf(g3.y, g4.y)) - altitudePadding;
    int max_y = (int)fmaxf(fmaxf(g1.y, g2.y), fmaxf(g3.y, g4.y)) + altitudePadding;
    int min_x = (int)fminf(fminf(g1.x, g2.x), fminf(g3.x, g4.x)) - altitudePadding;
    int max_x = (int)fmaxf(fmaxf(g1.x, g2.x), fmaxf(g3.x, g4.x)) + altitudePadding;

    if (min_x < 0)
        min_x = 0;
    if (min_y < 0)
        min_y = 0;
    if (max_x >= map->columns)
        max_x = map->columns - 1;
    if (max_y >= map->rows)
        max_y = map->rows - 1;

    // 4. REPOPULATE BUCKETS
    memset(map->buckets, 0, sizeof(map->buckets));

    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *e = &map->entities[i];
        int ty = (int)(e->position.y / TILE_SIZE);
        if (ty >= 0 && ty < map->rows) {
            e->next_in_bucket = map->buckets[ty];
            map->buckets[ty] = e;
        }
    }

    // Explicitly bucket the player so they render on their correct tile row
    if (drawPlayer) {
        int pty = (int)(map->player.position.y / TILE_SIZE);
        if (pty >= 0 && pty < map->rows) {
            map->player.next_in_bucket = map->buckets[pty];
            map->buckets[pty] = &map->player;
        }
    }

    // 5. THE LOOP: Row-by-Row rendering
    for (int y = min_y; y <= max_y; y++) {
        // Step 1: Render ALL tile ground/walls for the entire row first
        for (int x = min_x; x <= max_x; x++) {
            Draw_Tile(map, x, y);
        }

        // Step 2: Render all static buildings on row y
        for (int x = min_x; x <= max_x; x++) {
            Draw_Buildings(map, x, y);
        }

        // Step 3: Draw all entities belonging to row y ON TOP of row y's
        // tiles/buildings
        MapEntity *curr = map->buckets[y];
        while (curr != NULL) {
            int tx = (int)(curr->position.x / TILE_SIZE);
            // Only draw entity if it falls within visible horizontal bounds
            if (tx >= min_x && tx <= max_x) {
                Draw_MapEntity(curr, map);
            }
            curr = curr->next_in_bucket;
        }
    }

    // 6. WATER FX
    BeginBlendMode(BLEND_ADDITIVE);
    for (int y = min_y; y <= max_y; y++) {
        // Render all tile ground/walls for the row
        for (int x = min_x; x <= max_x; x++) {
            if (map->grid[y][x].type == TILE_WATER) {
                DrawWaterEffects(map, x, y);
            }
        }
    }
    EndBlendMode();

    // 7. PLAYER OCCLUSION SILHOUETTE PASS

    int px = (int)(map->player.position.x / TILE_SIZE);
    int py = (int)(map->player.position.y / TILE_SIZE);

    bool isOccluded = false;
    for (int dy = 0; dy <= 2; dy++) {
        for (int dx = 0; dx <= 2; dx++) {
            int checkX = px + dx;
            int checkY = py + dy;
            if (checkX < map->columns && checkY < map->rows) {
                if (IsTileOccludingPlayer(map, checkX, checkY)) {
                    isOccluded = true;
                    break;
                }
            }
        }
        if (isOccluded)
            break;
    }

    if (isOccluded) {
        Vector2 pPos = GetWorldToIso(map->player.position);
        pPos.y -= map->player.altitude;
        Texture2D *sprite = GetSprite(map->player.type, 0);
        Vector2 drawPos = {pPos.x - (sprite->width / 2.0f),
                           pPos.y - sprite->height};

        DrawTextureV(*sprite, drawPos, Fade(COLOR_SHELL_PINK, 0.6f));
        DrawCircleLines(pPos.x, pPos.y - 12, 10, WHITE);
    }
    // }

    EndMode2D();
    DrawFPS(10, 10);
}
