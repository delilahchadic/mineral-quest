#include "play/play_combat.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "raylib.h"
#include "raymath.h"
#include "environment/map.h"
#include <math.h>
#include <stdio.h>


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

    float baseAngle = player->combat.attackAngle;

    // Forgiving Hit Sweep: Check a small arc instead of a single point
    float hitRadius = 24.0f; // Increased for forgiveness

    MapEntity* e = map->entities;
    while (e != NULL) {
        MapEntity* next = e->next;
        if (e != player) {
            // Check points along the arc to see if any hit the entity's general area
            bool hit = false;
            for (float offset = -0.5f; offset <= 0.5f; offset += 0.25f) {
                Vector2 checkPos = {
                    player->position.x + cosf(baseAngle + offset) * 35.0f,
                    player->position.y + sinf(baseAngle + offset) * 35.0f
                };

                if (Vector2Distance(checkPos, e->position) < hitRadius) {
                    hit = true;
                    break;
                }
            }

            if (hit && !e->hitThisSwing) {
                // HIT LOGIC
                printf("Hit entity!\n");
                e->hitThisSwing = true;
                // simple knockback
                Vector2 dir = Vector2Normalize(
                    Vector2Subtract(e->position, player->position)
                );
                if(e->type == ENTITY_PLANT){
                    Remove_Entity(map, e);
                }else if(e->type == ENTITY_ENEMY){
                    if(e->hp == 1){
                        Remove_Entity(map, e);
                    }else{e->hp = e->hp - 1;}
                }else{
                e->position.x += dir.x * 20.0f;
                e->position.y += dir.y * 20.0f;}
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
        float t = player->combat.attackTimer / player->combat.attackDuration;

        if (t >= 1.0f) {
            player->combat.isAttacking = false;
            player->combat.attackTimer = 0;
            player->combat.attackAngle = baseAngle;
        } else {
            // Determine inversion based on facing (Left vs Right)
            // Right is -1.2, Left is -1.94
            bool isLeft = (baseAngle < -1.5f);
            float directionMultiplier = isLeft ? -1.0f : 1.0f;
            float swingRange = 2.5f * directionMultiplier;

            // Adjust swing direction based on combo state and facing direction
            switch (player->combat.combo_state) {
                case COMBO_2:
                    // Backhand swing: Reverse direction
                    player->combat.attackAngle = (baseAngle + (1.25f * directionMultiplier)) - t * swingRange;
                    break;
                case COMBO_3:
                    // Heavy strike: Faster or more dramatic arc
                    player->combat.attackAngle = (baseAngle - (1.5f * directionMultiplier)) + t * (4.0f * directionMultiplier);
                    break;
                case COMBO_1:
                default:
                    // Standard swing
                    player->combat.attackAngle = (baseAngle - (1.25f * directionMultiplier)) + t * swingRange;
                    break;
            }
        }
    } else {
        // IDLE STATE: Keep sword at facing direction
        player->combat.attackAngle = baseAngle;

        // Decrease combo window timer when not attacking
        if (player->combat.combo_timer > 0) {
            player->combat.combo_timer -= dt;
            if (player->combat.combo_timer <= 0) {
                player->combat.combo_state = COMBO_NONE;
            }
        }
    }
}
