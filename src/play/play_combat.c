#include "play/play_combat.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "defs/types_systems.h"
#include "environment/map.h"
#include "raylib.h"
#include "raymath.h"
#include "registry/register.h"
#include "systems/targeting.h"
#include "systems/weapon_grid.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

float EaseOutCubic(float x) {
    float f = 1.0f - x;
    return 1.0f - (f * f * f);
}

void ExecuteDirectionalAttack(Map *map,CombatState *combat, Input *input) {
    if (!map) {
        // Prevent immediate crash if map is null
        return;
    }
    if (!input)
        return;
    // 1. Update facing direction immediately to match the IJKL input
    combat->facing_direction =
        atan2f(input->attack_dir.y, input->attack_dir.x);

    // 2. Trigger the attack combo (InitCombat handles combo states 1, 2,
    // and 3)
    InitCombat(map,combat);
}

void ExecuteTargetedAttack(Map *map, Player *player, CombatState* combat) {
    if (player->targeting.target_id != -1) {
        MapEntity *target = GetTargetEntity(map, player->targeting.target_id);
        if (target) {
            // Point player straight at the target
            Vector2 diff = Vector2Subtract(GetEntityCenter(target),
                                           GetEntityCenter(&map->player));
            combat->facing_direction = atan2f(diff.y, diff.x);

            // Trigger a combat swing/lunge immediately
            InitCombat(map,combat);
        }
    } else {
        InitCombat(map,combat);
    }
    return;
}

void InitCombat(Map *map,CombatState* combat) {
    if (!combat->isAttacking) {
        if (combat->combo_state == COMBO_NONE ||
            combat->combo_timer <= 0) {
            combat->combo_state = COMBO_1;
            combat->attackDuration =
                0.20f; // Slipped down from 0.25f for speed
        } else if (combat->combo_state == COMBO_1) {
            combat->combo_state = COMBO_2;
            combat->attackDuration =
                0.20f; // Slipped down from 0.25f for speed
        } else if (combat->combo_state == COMBO_2) {
            combat->combo_state = COMBO_3;
            combat->attackDuration =
                0.35f; // Slipped down from 0.45f for speed
        } else {
            combat->combo_state = COMBO_1;
            combat->attackDuration = 0.20f;
        }

        combat->isAttacking = true;
        combat->attackTimer = 0;
        combat->combo_timer = 0.5f;
        ResetAllHitFlags(map);
    }
}

// Internal helper function (not exposed in header)
static bool ProcessHit(Map *map,CombatState *combat_state, MapEntity *player, int index) {
    MapEntity *target = &map->entities[index];

    // Hitstop freeze frame
    map->hitstop_timer =
        (combat_state->combo_state == COMBO_3) ? 0.12f : 0.05f;

    // Instant destruction (Plants)
    if (target->type == ENTITY_PLANT) {
        RemoveEntityAt(map, index);
        return true;
    }

    // Damage dealing (Enemies)
    if (target->type == ENTITY_ENEMY) {
        CombatComponent *enemy_combat_component = &map->combat_components[target->combat_component_id];
        enemy_combat_component->hitThisSwing = true;
        Enemy* enemy = &ENEMY_REGISTRY[target->entity_id];
        ApplyDamageToEnemy(map,index, PLAYER->stats.current[STAT_STR] - (enemy->stats[STAT_DEF] / 10));

    }

    // Unified Knockback for all remaining entities
    Vector2 dir =
        Vector2Normalize(Vector2Subtract(target->position, player->position));
    float knockbackForce =
        (combat_state->combo_state == COMBO_3) ? 40.0f : 20.0f;

    target->position.x += dir.x * knockbackForce;
    target->position.y += dir.y * knockbackForce;
    return false;
}

void UpdateCombat(Map *map, CombatState* player_combat_state) {
    if (PLAYER->weapon_grid.activeIndex == -1)
        return;
    MapEntity *player = &map->player;
    WeaponSlot *slot = GetActiveWeaponsSlot(&PLAYER->weapon_grid);
    if (!player_combat_state->isAttacking)
        return;

    float t = player_combat_state->attackTimer / player_combat_state->attackDuration;

    // Only hit during active swing window
    if (t < 0.3f || t > 0.7f)
        return;

    float baseAngle = player_combat_state->facing_direction;
    bool useSword = slot->weapon_id == 5;

    // 1. DYNAMIC HITBOX SETTINGS (Increased reach distance)
    float hitRadius = useSword ? 24.0f : 32.0f;
    float reachDistance =
        useSword ? 64.0f : 74.0f; // Bumped up from 50.0f / 60.0f
    float arcStartDeg = useSword ? -90.0f : -110.0f;
    float arcEndDeg = useSword ? 90.0f : 110.0f;

    if (player_combat_state->combo_state == COMBO_2) {
        arcStartDeg = useSword ? -75.0f : -105.0f;
        arcEndDeg = useSword ? 75.0f : 105.0f;
    } else if (player_combat_state->combo_state == COMBO_3) {
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
        if (e->combat_component_id != INVALID_INDEX) {
            bool hit = false;
            CombatComponent *entity_combat_component = &map->combat_components[e->combat_component_id];
            float entityRadius = 16.0f;
            Vector2 eCenter = GetEntityCenter(e);

            for (float offset = arcStartRad; offset <= arcEndRad;
                 offset += arcStep) {
                Vector2 checkPos = {
                    playerCenter.x + cosf(baseAngle + offset) * reachDistance,
                    playerCenter.y + sinf(baseAngle + offset) * reachDistance};

                if (Vector2Distance(checkPos, eCenter) <
                    (hitRadius + entityRadius)) {
                    hit = true;
                    break;
                }
            }

            if (hit && !entity_combat_component->hitThisSwing) {
                if (ProcessHit(map,player_combat_state, &map->player, i)) {
                    i--;
                }
            }
        }
    }
}

void UpdatePlayerCombatAnimation(MapEntity *player, CombatState* combat,float dt) {

    if (PLAYER->weapon_grid.activeIndex == -1)
        return;
    float baseAngle = combat->facing_direction;
    if (baseAngle == -999.0f && combat->isAttacking) {
            baseAngle = -1.2f;
        }

    if (combat->isAttacking) {
        combat->attackTimer += dt;

        float t = combat->attackTimer / combat->attackDuration;
        float easeT = EaseOutCubic(t);

        // LUNGE / STEP-IN MOMENTUM (slightly scaled up to match the extra
        // reach) LUNGE / STEP-IN MOMENTUM (Rusty-style smooth dash)
        if (t < 0.5f) {
            float maxLungeSpeed = 0.0f;
            switch (combat->combo_state) {
            case COMBO_1:
                maxLungeSpeed = 500.0f;
                break;
            case COMBO_2:
                maxLungeSpeed = 700.0f;
                break;
            case COMBO_3:
                maxLungeSpeed = 900.0f;
                break; // Heavy finisher lunge!
            default:
                break;
            }

            // Decaying speed curve: starts blazing fast, smoothly slows down as
            // t approaches 0.5f
            float dashProgress = t / 0.5f;
            float currentSpeed =
                maxLungeSpeed * (1.0f - (dashProgress * dashProgress));

            player->position.x += cosf(baseAngle) * currentSpeed * dt;
            player->position.y += sinf(baseAngle) * currentSpeed * dt;
        }

        if (t >= 1.0f) {
            combat->isAttacking = false;
            combat->attackTimer = 0;
            combat->attackAngle = baseAngle;
        } else {
            float startOffsetDeg = 0.0f;
            float endOffsetDeg = 0.0f;
            WeaponSlot *slot = GetActiveWeaponsSlot(&PLAYER->weapon_grid);
            bool useSword = slot->weapon_id == 5;

            switch (combat->combo_state) {
            case COMBO_1:
                startOffsetDeg = useSword ? -90.0f : -110.0f;
                endOffsetDeg = useSword ? 90.0f : 110.0f;
                break;
            case COMBO_2:
                startOffsetDeg = useSword ? -75.0f : -105.0f;
                endOffsetDeg = useSword ? 75.0f : 105.0f;
                break;
            case COMBO_3:
                startOffsetDeg = -120.0f;
                endOffsetDeg = 120.0f;
                break;
            default:
                startOffsetDeg = useSword ? -75.0f : -105.0f;
                endOffsetDeg = useSword ? 75.0f : 105.0f;
                break;
            }

            float startRad = startOffsetDeg * DEG2RAD;
            float endRad = endOffsetDeg * DEG2RAD;

            combat->attackAngle =
                baseAngle + startRad + ((endRad - startRad) * easeT);
        }
    } else {
        combat->attackAngle = baseAngle;

        if (combat->combo_timer > 0) {
            combat->combo_timer -= dt;
            if (combat->combo_timer <= 0) {
                combat->combo_state = COMBO_NONE;
            }
        }
    }
}
