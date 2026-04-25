// core/camera_tools.h
#ifndef CAMERA_TOOLS_H
#define CAMERA_TOOLS_H

#include "raylib.h"

// Resets camera to defaults and centers it on a world position
void CenterCameraOn(Camera2D* camera, Vector2 target, float zoom);

#endif
