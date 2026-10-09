#ifndef TYPES_ENV
#define TYPES_ENV

#include "defs/types_entities.h"
#include "defs/types_tiles.h"
#include "raylib.h"
#include <stdint.h>
#define TILE_SIZE 32
#define MAP_WIDTH 256
#define MAP_HEIGHT 256
#define MAX_ENTITIES 1000
// Physical dimensions in pixels
#define WORLD_WIDTH_PX (MAP_WIDTH * TILE_SIZE)
#define WORLD_HEIGHT_PX (MAP_HEIGHT * TILE_SIZE)
#define INVALID_INDEX -1
typedef enum State { NORMAL_STATE, JUMPING_STATE } State;

typedef enum Behavior {
    BEHAVIOR_IMMOBILE,
    BEHAVIOR_WANDER,
    BEHAVIOR_CHASE,
    BEHAVIOR_WINDUP,
    BEHAVIOR_ATTACK,
    BEHAVIOR_RECOVERY,
    BEHAVIOR_STAGGERED,
    BEHAVIOR_AIM,
    BEHAVIOR_ORBIT
} Behavior;

typedef enum ComboState { COMBO_NONE, COMBO_1, COMBO_2, COMBO_3 } ComboState;

typedef struct {
    bool isAttacking;
    float attackTimer;
    float attackDuration;
    float attackAngle;
    ComboState combo_state;
    float combo_timer;
    float facing_direction; // Base angle (e.g. 0 for right, PI for left)
} CombatState;

typedef struct MovementComponent{
    Vector2 target_position;
    Behavior behavior;
    float behavior_timer;
    State state;
    Vector2 velocity;
    float speed;
    float jumpoffset;
    float vertical_velocity;
    int is_active;
}MovementComponent;

typedef struct MapEntity {
    //identity
    EntityType type;  // Is this a person or a flower?
    int entity_id; // Which member of its type is it
    int instance_id; // which entity on the board is it
    uint32_t trait_flags; // what traits does it have
    //location
    Vector2 position;
    float altitude;
    struct MapEntity *next_in_bucket; // render help
    int movement_component_id;
    // TODO implement combat_component
    int hp;
    CombatState combat;
    bool hitThisSwing;
    bool isCollecting;
    StatBlock *stats;
    uint32_t element_flags;

} MapEntity;

typedef struct Tile {
    int height;
    TileType type;
    Vector2 isoPos;
} Tile;

typedef struct BuildingZone {
    int id;
    int x1, y1;       // Northwest grid corner
    int x2, y2;       // Southeast grid corner
    int min_height;   // Ground floor voxel height
    int max_height;   // Roof/ceiling voxel height
    int total_floors; // Number of interior levels
    Vector2 door_pos; // Entrance position
    Color color;
    struct BuildingZone *next;
} BuildingZone;

typedef struct Map {
    Tile grid[MAP_WIDTH][MAP_HEIGHT];
    int pixel_width;
    int pixel_height;
    int rows;
    int columns;
    char name[32];
    MapEntity player;
    int lastTileHeight;
    int active_nodes[10];
    int node_count;
    bool is_ready;
    float hitstop_timer;
    MapEntity *buckets[MAP_HEIGHT];
    MapEntity entities[MAX_ENTITIES];
    MovementComponent movement_components[250];
    int movement_component_count;
    int entity_count;
    BuildingZone *buildings;
    int building_id; // used to guarantee any new building has a unique id
    int instance_id_count;
} Map;
#endif
