#include "core/camera_tools.h"

#include "defs/constants.h"

void CenterCameraOn(Camera2D* camera, Vector2 worldTarget, float zoom, Map* map) {
    // 1. Convert the flat world coordinates (e.g., 30, 30) to Isometric pixels
    Vector2 isoTarget = GetWorldToIso(worldTarget);

    // 2. Adjust for height (otherwise the camera looks at the feet/shadow)
    int tx = (int)(worldTarget.x / TILE_SIZE);
    int ty = (int)(worldTarget.y / TILE_SIZE);

    // Safety check for map bounds
    if (tx >= 0 && tx < map->columns && ty >= 0 && ty < map->rows) {
        float hOffset = map->grid[ty][tx].height * 8.0f;
        isoTarget.y -= hOffset; // Lift the camera eye to match the sprite lift
    }

    camera->target = isoTarget;
    camera->offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    camera->rotation = 0.0f;
    camera->zoom = zoom;
}
