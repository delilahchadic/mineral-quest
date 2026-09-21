#include "play/play_combat.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "raylib.h"
#include "raymath.h"
#include "environment/map.h"
#include "registry/register.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

float EaseOutCubic(float x) {
    float f = 1.0f - x;
    return 1.0f - (f * f * f);
}

void InitCombat(Map* map){
    if (!map->player.combat.isAttacking) {
        if (map->player.combat.combo_state == COMBO_NONE || map->player.combat.combo_timer <= 0) {
            map->player.combat.combo_state = COMBO_1;
            map->player.combat.attackDuration = 0.20f; // Slipped down from 0.25f for speed
        } else if (map->player.combat.combo_state == COMBO_1) {
            map->player.combat.combo_state = COMBO_2;
            map->player.combat.attackDuration = 0.20f; // Slipped down from 0.25f for speed
        } else if (map->player.combat.combo_state == COMBO_2) {
            map->player.combat.combo_state = COMBO_3;
            map->player.combat.attackDuration = 0.35f; // Slipped down from 0.45f for speed
        } else {
            map->player.combat.combo_state = COMBO_1;
            map->player.combat.attackDuration = 0.20f;
        }

        map->player.combat.isAttacking = true;
        map->player.combat.attackTimer = 0;
        map->player.combat.combo_timer = 0.5f;
        ResetAllHitFlags(map);
    }
}

// Internal helper function (not exposed in header)
static bool ProcessHit(Map* map, MapEntity* player, int index) {
    MapEntity* target = &map->entities[index];
    target->hitThisSwing = true;

    // Hitstop freeze frame
    map->hitstop_timer = (player->combat.combo_state == COMBO_3) ? 0.12f : 0.05f;

    // Instant destruction (Plants)
    if (target->type == ENTITY_PLANT) {
        RemoveEntityAt(map, index);
        return true;
    }

    // Damage dealing (Enemies)
    if (target->type == ENTITY_ENEMY) {
        target->hp -= PLAYER->stats.current[STAT_STR];
        if (target->hp <= 0) {
            if(PLAYER->targeting.locked && PLAYER->targeting.target_id ==target->instance_id){
                PLAYER->targeting.locked=false;
            }
            RemoveEntityAt(map, index);

            return true;
        }
    }

    // Unified Knockback for all remaining entities
    Vector2 dir = Vector2Normalize(Vector2Subtract(target->position, player->position));
    float knockbackForce = (player->combat.combo_state == COMBO_3) ? 40.0f : 20.0f;

    target->position.x += dir.x * knockbackForce;
    target->position.y += dir.y * knockbackForce;
    return false;
}

void UpdateCombat(Map* map) {
    if(PLAYER->gear.weapon_id == -1) return;
    MapEntity* player = &map->player;

    if (!player->combat.isAttacking) return;

    float t = player->combat.attackTimer / player->combat.attackDuration;

    // Only hit during active swing window
    if (t < 0.3f || t > 0.7f) return;

    float baseAngle = player->combat.facing_direction;
    bool useSword = PLAYER->gear.weapon_id == 5;

    // 1. DYNAMIC HITBOX SETTINGS (Increased reach distance)
    float hitRadius = useSword ? 24.0f : 32.0f;
    float reachDistance = useSword ? 64.0f : 74.0f; // Bumped up from 50.0f / 60.0f
    float arcStartDeg = useSword ? -90.0f : -110.0f;
    float arcEndDeg   = useSword ?  90.0f :  110.0f;

    if (player->combat.combo_state == COMBO_2) {
        arcStartDeg = useSword ? -75.0f : -105.0f;
        arcEndDeg   = useSword ?  75.0f :  105.0f;
    } else if (player->combat.combo_state == COMBO_3) {
        reachDistance = 80.0f; // Bumped up from 65.0f for the heavy finisher
        arcStartDeg = -120.0f;
        arcEndDeg = 120.0f;
        hitRadius = 30.0f;
    }

    float arcStartRad = arcStartDeg * DEG2RAD;
    float arcEndRad = arcEndDeg * DEG2RAD;
    float arcStep = 0.1f;

    // Calculate player center from sprite dimensions
    Vector2 playerCenter = GetEntityCenter(player);

    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *e = &map->entities[i];
        if (e != player && e->type != ENTITY_MINERAL) {
            bool hit = false;
            float entityRadius = e->type == ENTITY_ENEMY ? 16.0f : 5.0f;
            Vector2 eCenter = GetEntityCenter(e);

            for (float offset = arcStartRad; offset <= arcEndRad; offset += arcStep) {
                Vector2 checkPos = {
                    playerCenter.x + cosf(baseAngle + offset) * reachDistance,
                    playerCenter.y + sinf(baseAngle + offset) * reachDistance
                };

                if (Vector2Distance(checkPos, eCenter) < (hitRadius + entityRadius)) {
                    hit = true;
                    break;
                }
            }

            if (hit && !e->hitThisSwing) {
                if(ProcessHit(map, &map->player, i)){
                    i--;
                }
            }
        }
    }
}

void UpdatePlayerCombatAnimation(MapEntity* player, float dt){
    if(PLAYER->gear.weapon_id == -1) return;
    float baseAngle = player->combat.facing_direction;
    if (baseAngle == 0 && !player->combat.isAttacking) {
        baseAngle = -1.2f;
    }

    if (player->combat.isAttacking) {
        player->combat.attackTimer += dt;

        float t = player->combat.attackTimer / player->combat.attackDuration;
        float easeT = EaseOutCubic(t);

        // LUNGE / STEP-IN MOMENTUM (slightly scaled up to match the extra reach)
        // LUNGE / STEP-IN MOMENTUM (Rusty-style smooth dash)
                if (t < 0.5f) {
                    float maxLungeSpeed = 0.0f;
                    switch (player->combat.combo_state) {
                        case COMBO_1: maxLungeSpeed = 500.0f; break;
                        case COMBO_2: maxLungeSpeed = 700.0f; break;
                        case COMBO_3: maxLungeSpeed = 900.0f; break; // Heavy finisher lunge!
                        default: break;
                    }

                    // Decaying speed curve: starts blazing fast, smoothly slows down as t approaches 0.5f
                    float dashProgress = t / 0.5f;
                    float currentSpeed = maxLungeSpeed * (1.0f - (dashProgress * dashProgress));

                    player->position.x += cosf(baseAngle) * currentSpeed * dt;
                    player->position.y += sinf(baseAngle) * currentSpeed * dt;
                }

        if (t >= 1.0f) {
            player->combat.isAttacking = false;
            player->combat.attackTimer = 0;
            player->combat.attackAngle = baseAngle;
        } else {
            float startOffsetDeg = 0.0f;
            float endOffsetDeg = 0.0f;
            bool useSword = PLAYER->gear.weapon_id == 5;

            switch (player->combat.combo_state) {
                case COMBO_1:
                    startOffsetDeg = useSword ? -90.0f : -110.0f;
                    endOffsetDeg   = useSword ?  90.0f :  110.0f;
                    break;
                case COMBO_2:
                    startOffsetDeg = useSword ? -75.0f : -105.0f;
                    endOffsetDeg   = useSword ?  75.0f :  105.0f;
                    break;
                case COMBO_3:
                    startOffsetDeg = -120.0f;
                    endOffsetDeg   =  120.0f;
                    break;
                default:
                    startOffsetDeg = useSword ? -75.0f : -105.0f;
                    endOffsetDeg   = useSword ?  75.0f :  105.0f;
                    break;
            }

            float startRad = startOffsetDeg * DEG2RAD;
            float endRad   = endOffsetDeg * DEG2RAD;

            player->combat.attackAngle = baseAngle + startRad + ((endRad - startRad) * easeT);
        }
    } else {
        player->combat.attackAngle = baseAngle;

        if (player->combat.combo_timer > 0) {
            player->combat.combo_timer -= dt;
            if (player->combat.combo_timer <= 0) {
                player->combat.combo_state = COMBO_NONE;
            }
        }
    }
}
