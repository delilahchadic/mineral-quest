#include "systems/targeting.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include "registry/register.h"

MapEntity *GetTargetEntity(Map *map, int target_id) {
    if (target_id == -1)
        return NULL;

    for (int i = 0; i < map->entity_count; i++) {
        if (map->entities[i].instance_id == target_id) {
            return &map->entities[i]; // Found it, even if its index changed!
        }
    }
    return NULL; // Target no longer exists (it died)
}

void UpdatePlayerTargets(Map *map, PlayerTargeting *targeting) {
    targeting->potential_count = 0;
    float maxTargetRange = 300.0f;

    typedef struct {
        int instance_id;
        float dist;
    } TempCandidate;

    TempCandidate candidates[8];
    int candidate_count = 0;

    int closest_id = -1;
    float min_dist = 99999.0f;
    Vector2 player_center = GetEntityCenter(&map->player);
    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *e = &map->entities[i];

        // Skip player, minerals, or items only
        if (e == &map->player || e->type == ENTITY_MINERAL || e->type ==ENTITY_VHS)
            continue;

        Vector2 enemy_center = GetEntityCenter(e);
        float dist = Vector2Distance(player_center, enemy_center);

        if (dist <= maxTargetRange) {
            if (candidate_count < 8) {
                candidates[candidate_count++] =
                    (TempCandidate){e->instance_id, dist};
            }

            if (dist < min_dist) {
                min_dist = dist;
                closest_id = e->instance_id;
            }
        }
    }

    // Sort ascending: closest to furthest using Insertion Sort
    for (int i = 1; i < candidate_count; i++) {
        TempCandidate key = candidates[i];
        int j = i - 1;

        while (j >= 0 && candidates[j].dist > key.dist) {
            candidates[j + 1] = candidates[j];
            j--;
        }
        candidates[j + 1] = key;
    }

    targeting->potential_count = candidate_count;
    for (int i = 0; i < candidate_count; i++) {
        targeting->potential_targets[i] = candidates[i].instance_id;
    }

    // If we have no targets in range at all, clear everything
    if (candidate_count == 0) {
        targeting->target_id = -1;
        targeting->locked = false;
        return;
    }

    // If locked, ONLY auto-unlock if the target completely leaves range (+
    // buffer)
    if (targeting->locked) {
        MapEntity *currentTarget = GetTargetEntity(map, targeting->target_id);
        if (!currentTarget ||
            Vector2Distance(player_center, GetEntityCenter(currentTarget)) >
                maxTargetRange + 50.0f) {
            targeting->locked = false;
            targeting->target_id = closest_id;
        }
    } else {
        // Unlocked: passively stick to the closest target
        targeting->target_id = closest_id;
    }
}

void CycleTarget(PlayerTargeting *targeting) {
    if (targeting->potential_count == 0) {
        targeting->target_id = -1;
        return;
    }

    // Find where our current target is in the potential list
    int currentIndex = -1;
    for (int i = 0; i < targeting->potential_count; i++) {
        if (targeting->potential_targets[i] == targeting->target_id) {
            currentIndex = i;
            break;
        }
    }

    // Move to the next target in the list (loops back to 0 at the end)
    int nextIndex = (currentIndex + 1) % targeting->potential_count;
    targeting->target_id = targeting->potential_targets[nextIndex];
}

void DrawTargetReticle(Vector2 position, EntityType type) {
    // if (!position) return;

    // 1. Create a pulsing glow effect based on real-time
    float pulse = sinf(GetTime() * 8.0f) * 3.0f;
    float radius = 24.0f + pulse;

    // 2. Color-code by entity type (replace these with your custom color names)
    Color reticleColor = WHITE;
    if (PLAYER->targeting.locked) {
        switch (type) {
        case ENTITY_ENEMY:
            reticleColor = COLOR_RED_OCHRE; // Swap with your custom enemy color
                                            // (e.g., COLOR_ENEMY_RED)
            break;
        case ENTITY_PLANT:
            reticleColor =
                COLOR_SAP_GREEN; // Swap with your custom harvest color
            break;
        case ENTITY_PORTAL:
            reticleColor = COLOR_MUTED_FUCHSIA;
            break;
        case ENTITY_CHARACTER:
            reticleColor = COLOR_COBALT_TEAL_PALE;
        default:
            reticleColor = COLOR_CERULEAN_WISTFUL; // Swap with your
                                                   // default/interactive color
            break;
        }
    }

    // 3. Draw an outer glowing, semi-transparent ring
    DrawRing(position, radius - 3.0f, radius + 1.0f, 0.0f, 360.0f, 16,
             Fade(reticleColor, 0.35f));

    // 4. Draw the crisp main reticle ring
    DrawCircleLines(position.x, position.y, radius, reticleColor);
}
