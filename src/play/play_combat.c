#include "play/play_combat.h"
#include "raylib.h"
#include "raymath.h"
#include "environment/map.h"
#include <math.h>
#include <stdio.h>

void UpdateCombat(Map* map) {
    MapEntity* player = map->player;

    if (!player->combat.isAttacking) return;

    float t = player->combat.attackTimer / player->combat.attackDuration;

    // Only hit during active swing window
    if (t < 0.3f || t > 0.7f) return;

    float angle = player->combat.attackAngle;

    Vector2 swordTip = {
        player->position.x + cosf(angle) * 40.0f,
        player->position.y + sinf(angle) * 40.0f
    };

    float hitRadius = 12.0f;

    MapEntity* e = map->entities;
    while (e != NULL) {
        MapEntity* next = e->next;
        if (e != player) {

            float dist = Vector2Distance(swordTip, e->position);

            if (dist < hitRadius && !e->hitThisSwing) {
                // HIT LOGIC
                printf("Hit entity!\n");
                e->hitThisSwing = true;
                // simple knockback
                Vector2 dir = Vector2Normalize(
                    Vector2Subtract(e->position, player->position)
                );
                if(e->type == ENTITY_PLANT){
                    Remove_Entity(map, e);
                }else{
                e->position.x += dir.x * 20.0f;
                e->position.y += dir.y * 20.0f;}
            }
        }
        e = next;
    }
}

void UpdatePlayerCombatAnimation(MapEntity* player, float dt){
    if (player->combat.isAttacking) {
        player->combat.attackTimer += dt;

        float t = player->combat.attackTimer / player->combat.attackDuration;

        if (t >= 1.0f) {
            player->combat.isAttacking = false;
        } else {
            // swing from -1.5 → +1.0 radians
            player->combat.attackAngle = -1.5f + t * 2.5f;
        }
    }
}
