#include "systems/behavior.h"
#include "environment/map.h"
#include "raylib.h"
#include "raymath.h"
#include "registry/register.h"
#include <math.h>
#include <stdlib.h>

bool PickNewWanderTarget(Map *map, MapEntity *entity) {
    int cur_tx = (int)(entity->position.x / TILE_SIZE);
    int cur_ty = (int)(entity->position.y / TILE_SIZE);

    if (cur_tx < 0 || cur_tx >= map->columns || cur_ty < 0 ||
        cur_ty >= map->rows)
        return false;
    int cur_h = map->grid[cur_ty][cur_tx].height;

    for (int i = 0; i < 10; i++) {
        // Generate candidate around current position
        float cx = entity->position.x + ((rand() % 128) - 64);
        float cy = entity->position.y + ((rand() % 128) - 64);

        // Clamp to map boundaries so it never goes into negative space or
        // off-screen
        if (cx < 0.0f)
            cx = 0.0f;
        if (cx > (float)map->pixel_width)
            cx = (float)map->pixel_width;
        if (cy < 0.0f)
            cy = 0.0f;
        if (cy > (float)map->pixel_height)
            cy = (float)map->pixel_height;

        int t_tx = (int)(cx / TILE_SIZE);
        int t_ty = (int)(cy / TILE_SIZE);

        if (t_tx >= map->columns)
            t_tx = map->columns - 1;
        if (t_ty >= map->rows)
            t_ty = map->rows - 1;

        if (t_tx >= 0 && t_tx < map->columns && t_ty >= 0 && t_ty < map->rows) {
            if (abs(map->grid[t_ty][t_tx].height - cur_h) <= 2) {
                entity->target_position = (Vector2){cx, cy};
                return true;
            }
        }
    }
    return false;
}

void UpdateWanderBehavior(Map *map, MapEntity *entity, float dt) {
    Vector2 diff = Vector2Subtract(entity->target_position, entity->position);
    float distance = Vector2Length(diff);

    if (distance > 1.0f) {
        entity->position =
            Vector2Add(entity->position, Vector2Scale(Vector2Normalize(diff),
                                                      entity->speed * dt));

        // Hard clamp position to stay within map pixel boundaries
        if (entity->position.x < 0.0f)
            entity->position.x = 0.0f;
        if (entity->position.x > (float)map->pixel_width)
            entity->position.x = (float)map->pixel_width;
        if (entity->position.y < 0.0f)
            entity->position.y = 0.0f;
        if (entity->position.y > (float)map->pixel_height)
            entity->position.y = (float)map->pixel_height;

        int tx = (int)(entity->position.x / TILE_SIZE);
        int ty = (int)(entity->position.y / TILE_SIZE);

        // Prevent array out-of-bounds if exactly on the right/bottom edge
        if (tx >= map->columns)
            tx = map->columns - 1;
        if (ty >= map->rows)
            ty = map->rows - 1;

        if (tx >= 0 && tx < map->columns && ty >= 0 && ty < map->rows) {
            float targetAlt = map->grid[ty][tx].height * 8.0f;
            entity->altitude += (targetAlt - entity->altitude) * 10.0f * dt;
        }
    } else if (!PickNewWanderTarget(map, entity)) {
        entity->target_position = entity->position;
    }
}

void UpdateEnemyCombat(Map *map, MapEntity *e, float dt) {
    if (e->type != ENTITY_ENEMY)
        return;

    MapEntity *player = &map->player;
    Texture2D *playerTex = GetSprite(ENTITY_PLAYER, player->entity_id);
    Vector2 playerCenter = {player->position.x + (playerTex->width / 2.0f),
                            player->position.y + (playerTex->height / 2.0f)};

    Texture2D *enemyTex = GetSprite(e->type, e->entity_id);
    Vector2 enemyCenter = {e->position.x + (enemyTex->width / 2.0f),
                           e->position.y + (enemyTex->height / 2.0f)};

    float distToPlayer = Vector2Distance(enemyCenter, playerCenter);

    switch (e->behavior) {
    case BEHAVIOR_WANDER:
        // Existing wander behavior, but poll for player proximity
        if (distToPlayer < 140.0f) {
            e->behavior = BEHAVIOR_CHASE;
        } else {
            // Run normal wander movement
            UpdateWanderBehavior(map, e, dt);
        }
        break;

    case BEHAVIOR_CHASE:
        if (distToPlayer > 220.0f) {
            e->behavior = BEHAVIOR_WANDER;
        } else if (distToPlayer < 80.0f) {
            // Close enough to strike! Enter telegraph/windup
            e->behavior = BEHAVIOR_WINDUP;
            e->behavior_timer = 0.0f;
        } else {
            // Move smoothly toward player
            Vector2 dir = Vector2Normalize(
                Vector2Subtract(player->position, e->position));
            e->position = Vector2Add(e->position,
                                     Vector2Scale(dir, e->speed * 0.9f * dt));
        }
        break;

    case BEHAVIOR_WINDUP:
        e->behavior_timer += dt;
        // 300ms visual telegraph window
        if (e->behavior_timer >= 0.3f) {
            e->behavior = BEHAVIOR_ATTACK;
            e->behavior_timer = 0.0f;
        }
        break;

    case BEHAVIOR_ATTACK: {
        // Give enemies a generous hit footprint matching their 64x64
        // transparent sprite bounds
        float enemyRadius = 26.0f;
        float playerRadius = 12.0f; // Adjust to your player core size
        Vector2 dir =
            Vector2Normalize(Vector2Subtract(player->position, e->position));
        e->position =
            Vector2Add(e->position, Vector2Scale(dir, e->speed * 0.9f * dt));
        // Check edge-to-edge distance instead of strict center-to-center
        if (distToPlayer < (enemyRadius + playerRadius + 10.0f)) {
            // Hurt player logic
            printf("Player hit by enemy!\n");
            map->hitstop_timer = 0.06f; // Impact hitstop
            DamagePlayer(35);

            // Apply knockback to player
            Vector2 kbDir = Vector2Normalize(
                Vector2Subtract(player->position, e->position));
            player->position =
                Vector2Add(player->position, Vector2Scale(kbDir, 25.0f));
            e->behavior = BEHAVIOR_RECOVERY;
            e->behavior_timer = 0.0f;
        }

        break;
    }

    case BEHAVIOR_RECOVERY:
        e->behavior_timer += dt;
        // 400ms recovery window where enemy is vulnerable
        if (e->behavior_timer >= 0.4f) {
            e->behavior = BEHAVIOR_CHASE;
        }
        break;

    default:
        e->behavior = BEHAVIOR_WANDER;
        break;
    }
}

void UpdateEntityMovement(Map *map, float dt) {
    if (map == NULL)
        return;

    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *entity = &map->entities[i];

        if (entity->isCollecting) {
            Vector2 targetPos = map->player.position;
            entity->position =
                Vector2Lerp(entity->position, targetPos, 40.0f * dt);

            if (Vector2DistanceSqr(entity->position, targetPos) < 10.0f) {
                GLOBAL_PLAYER.mineral_inventory[entity->entity_id]++;
                float pitch =
                    0.95f + ((float)(entity->entity_id % 10) / 100.0f);
                SetSoundPitch(MINERAL_SOUND, pitch);
                PlaySound(MINERAL_SOUND);

                // Remove it using your flat-array helper, then step i back
                // so the newly shifted element at this index isn't skipped!
                RemoveEntityAt(map, i);
                i--;
                continue;
            }
        }

        if (entity->type == ENTITY_ENEMY) {
            UpdateEnemyCombat(map, entity, dt);
        } else if (entity->behavior == BEHAVIOR_WANDER) {
            UpdateWanderBehavior(map, entity, dt);
        }
    }
}
