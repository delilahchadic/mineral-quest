#include "core/camera_tools.h"

#include "defs/constants.h"

void CenterCameraOn(Camera2D* camera, Vector2 target, float zoom){
    camera->target = target;
    camera->offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };// Center of the 800x450 screen
    camera->rotation = 0.0f;
    camera->zoom = zoom;
}
