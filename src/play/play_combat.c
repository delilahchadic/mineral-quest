#include "play/play_combat.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "raylib.h"
#include "raymath.h"
#include "environment/map.h"
#include <math.h>
#include <stdio.h>

float EaseOutCubic(float x) {
    float f = 1.0f - x;
    return 1.0f - (f * f * f);
}

void InitCombat(Map* map){
    if (!map->player->combat.isAttacking) {
        if (map->player->combat.combo_state == COMBO_NONE || map->player->combat.combo_timer <= 0) {
            map->player->combat.combo_state = COMBO_1;
            map->player->combat.attackDuration = 0.25f;
        } else if (map->player->combat.combo_state == COMBO_1) {
            map->player->combat.combo_state = COMBO_2;
            map->player->combat.attackDuration = 0.25f;
        } else if (map->player->combat.combo_state == COMBO_2) {
            map->player->combat.combo_state = COMBO_3;
            map->player->combat.attackDuration = 0.45f;
        } else {
            map->player->combat.combo_state = COMBO_1;
            map->player->combat.attackDuration = 0.25f;
        }

        map->player->combat.isAttacking = true;
        map->player->combat.attackTimer = 0;
        map->player->combat.combo_timer = 0.5f; // Window to hit next
        ResetAllHitFlags(map);
    }
}

void UpdateCombat(Map* map) {
    MapEntity* player = map->player;

    if (!player->combat.isAttacking) return;

    float t = player->combat.attackTimer / player->combat.attackDuration;

    // Only hit during active swing window
    if (t < 0.3f || t > 0.7f) return;

    float baseAngle = player->combat.facing_direction;

    // 1. DYNAMIC HITBOX SETTINGS based on Combo State
    float hitRadius = 24.0f;
    float reachDistance = 50.0f; // Distance from player center
    float arcStartDeg = -75.0f;
    float arcEndDeg = 75.0f;

    if (player->combat.combo_state == COMBO_3) {
        reachDistance = 65.0f;
        arcStartDeg = -120.0f;
        arcEndDeg = 120.0f;
        hitRadius = 30.0f;
    }

    // Convert the sweep degrees to radians for math
    float arcStartRad = arcStartDeg * DEG2RAD;
    float arcEndRad = arcEndDeg * DEG2RAD;

    // We step through the arc. A smaller step means more precise checking.
    float arcStep = 0.25f; // radians

    MapEntity* e = map->entities;
    while (e != NULL) {
        MapEntity* next = e->next;
        if (e != player) {

            bool hit = false;

                        // Give entities a fallback radius if they don't have one stored
                        // (e.g., 16.0f pixels, or replace `e->radius` with whatever property your entity uses)
                        float entityRadius = e->type == ENTITY_ENEMY ?16.0f : 5.0f;
                        // if (e->radius > 0) entityRadius = e->radius; // Uncomment if your struct has e->radius

                        // 2. CHECK THE DYNAMIC ARC
                        for (float offset = arcStartRad; offset <= arcEndRad; offset += arcStep) {
                            Vector2 checkPos = {
                                player->position.x + cosf(baseAngle + offset) * reachDistance,
                                player->position.y + sinf(baseAngle + offset) * reachDistance
                            };

                            // Combine the sword's thickness (hitRadius) with the entity's physical size
                            if (Vector2Distance(checkPos, e->position) < (hitRadius + entityRadius)) {
                                hit = true;
                                break;
                            }
                        }

            if (hit && !e->hitThisSwing) {
                            // HIT LOGIC
                            printf("Hit entity!\n");
                            e->hitThisSwing = true;

                            // --- NEW: TRIGGER HITSTOP ---
                            // Freeze longer for the heavy combo finisher
                            if (player->combat.combo_state == COMBO_3) {
                                map->hitstop_timer = 0.12f; // 120ms (massive impact)
                            } else {
                                map->hitstop_timer = 0.05f; // 50ms (standard impact)
                            }
                            // ----------------------------

                            // Knockback calculation
                            Vector2 dir = Vector2Normalize(
                                Vector2Subtract(e->position, player->position)
                            );

                if (e->type == ENTITY_PLANT) {
                    Remove_Entity(map, e);
                } else if (e->type == ENTITY_ENEMY) {
                    if (e->hp == 1) {
                        Remove_Entity(map, e);
                    } else {
                        e->hp = e->hp - 1;
                    }
                } else {
                    // Push the entity back
                    float knockbackForce = (player->combat.combo_state == COMBO_3) ? 40.0f : 20.0f;
                    e->position.x += dir.x * knockbackForce;
                    e->position.y += dir.y * knockbackForce;
                }
            }
        }
        e = next;
    }
}

void UpdatePlayerCombatAnimation(MapEntity* player, float dt){
    float baseAngle = player->combat.facing_direction;
    if (baseAngle == 0 && !player->combat.isAttacking) {
        baseAngle = -1.2f; // Default to braced Right
    }

    if (player->combat.isAttacking) {
        player->combat.attackTimer += dt;

        // 1. Calculate our linear time, then Ease it
        float t = player->combat.attackTimer / player->combat.attackDuration;
        float easeT = EaseOutCubic(t);

        // --- ADD LUNGE / STEP-IN MOMENTUM ---
                // We only push the player forward during the first 40% of the animation
                if (t < 0.4f) {
                    float lungeSpeed = 0.0f;

                    switch (player->combat.combo_state) {
                        case COMBO_1: lungeSpeed = 60.0f; break;  // Standard step in
                        case COMBO_2: lungeSpeed = 20.0f; break;  // Very little movement on the backhand
                        case COMBO_3: lungeSpeed = 140.0f; break; // Massive leap forward for the heavy slam!
                        default: break;
                    }

                    // Move the player in the direction they are facing
                    player->position.x += cosf(baseAngle) * lungeSpeed * dt;
                    player->position.y += sinf(baseAngle) * lungeSpeed * dt;
                }
                // ------------------------------------
        if (t >= 1.0f) {
            player->combat.isAttacking = false;
            player->combat.attackTimer = 0;
            player->combat.attackAngle = baseAngle;
        } else {
            bool isLeft = (baseAngle < -1.5f);
            float directionMultiplier = isLeft ? -1.0f : 1.0f;

            // 2. Define our swings in readable DEGREES
            float startOffsetDeg = 0.0f;
            float endOffsetDeg = 0.0f;

            switch (player->combat.combo_state) {
                case COMBO_1:
                    // Standard swing: start pulled back 75 degrees, sweep forward 75 degrees
                    startOffsetDeg = -75.0f;
                    endOffsetDeg   =  75.0f;
                    break;
                case COMBO_2:
                    // Backhand: start where Combo 1 ended, sweep back
                    startOffsetDeg =  75.0f;
                    endOffsetDeg   = -75.0f;
                    break;
                case COMBO_3:
                    // Heavy slam: massive 120 degree windup, massive sweep
                    startOffsetDeg = -120.0f;
                    endOffsetDeg   =  120.0f;
                    break;
                default:
                    startOffsetDeg = -75.0f;
                    endOffsetDeg   =  75.0f;
                    break;
            }

            // 3. Mirror the swing if facing left
            startOffsetDeg *= directionMultiplier;
            endOffsetDeg   *= directionMultiplier;

            // 4. Convert to radians for Raylib
            float startRad = startOffsetDeg * DEG2RAD;
            float endRad   = endOffsetDeg * DEG2RAD;

            // 5. Interpolate using our eased time!
            player->combat.attackAngle = baseAngle + startRad + ((endRad - startRad) * easeT);
        }
    } else {
        // IDLE STATE
        player->combat.attackAngle = baseAngle;

        if (player->combat.combo_timer > 0) {
            player->combat.combo_timer -= dt;
            if (player->combat.combo_timer <= 0) {
                player->combat.combo_state = COMBO_NONE;
            }
        }
    }
}
