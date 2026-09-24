#include "registry/tarot_register.h"
#include "defs/types_engine.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "environment/map.h"
#include "registry/register.h"
#include <stdlib.h>
static int tarot_card_count = 78;

TarotCard TAROT_REGISTRY[78] = {0};
TarotCard *GetTarotCardByItemId(int item_id) {
    for (int i = 0; i < tarot_card_count; i++) {
        if (TAROT_REGISTRY[i].item_id == item_id) {
            return &TAROT_REGISTRY[i];
        }
    }
    return NULL; // Not a tarot card item
}

void (*tarot_callbacks[100])(void *) = {
    [0] = ThreeOfWands,
    [1] = ThreeOfCups,
    [2] = TheTower,
};

Vector2 GetTargetPosition(Map *map) {
    if (PLAYER->targeting.target_id == -1) {
        return map->player.position;
    }
    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *entity = &map->entities[i];
        if (entity->instance_id == PLAYER->targeting.target_id) {
            return entity->position;
        }
    }
    return map->player.position;
}

void ExecuteTarotCommand(int id, void *context) {
    if (id < 0 || id >= 100) {
        printf("Error: Tarot callback ID %d out of bounds!\n", id);
        return;
    }

    if (tarot_callbacks[id] == NULL) {
        printf("Warning: Tarot callback ID %d is not implemented yet!\n", id);
        return;
    }

    tarot_callbacks[id](context);
}

void ThreeOfWands(void *context) { // simple fire spell
    Gamestate *gamestate = (Gamestate *)context;
    Map *map = &gamestate->map;
    MapEntity *fireball = AddEntity(map);
    if (!fireball)
        return;

    fireball->entity_id = 0;
    fireball->position = map->player.position;

    // Spawn at the player's current altitude + an offset for hand height
    fireball->altitude = map->player.altitude + 12.0f;

    fireball->type = ENTITY_PROJECTILE;
    fireball->target_position = GetTargetPosition(map);
    fireball->behavior = BEHAVIOR_AIM;
    fireball->speed = 200.0f;
    fireball->element_flags = ELEMENT_FIRE;
}

void ThreeOfCups(void *context) { // simple heal
    (void)context; // may implement rendering later
    int newHp = PLAYER->stats.current_hp + 20 > PLAYER->stats.max_hp
                    ? PLAYER->stats.max_hp
                    : PLAYER->stats.current_hp + 20;
    PLAYER->stats.current_hp = newHp;
}

void TheTower(void *context) { // Late game convenience spell cull all enemies no exp
    Gamestate *gamestate = (Gamestate *)context;
    int enemyFound =1;
    while(enemyFound){
        enemyFound =0;
        for(int i =0;i< gamestate->map.entity_count;i++){
            if(gamestate->map.entities[i].type == ENTITY_ENEMY){
                RemoveEntityAt(&gamestate->map,i);
                enemyFound = 1;
                break;
            }
        }
    }

}
