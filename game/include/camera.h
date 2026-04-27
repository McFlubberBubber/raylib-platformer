#ifndef CAMERA_H
#define CAMERA_H

// #include "raylib.h"
struct Camera2D;
struct Input;

void init_camera(Camera2D *camera);
void update_camera(Camera2D *camera, Input *input);
void reset_camera(Camera2D *camera);

#endif
