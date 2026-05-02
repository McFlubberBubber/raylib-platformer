#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"

struct World;
struct Input;

struct Camera_2D {
	Camera2D raylib_cam;

	bool is_transitioning;
	Vector2 target_world_pos;
	float lerp_speed;
};

void init_camera(Camera_2D *camera, const World* world);
void update_camera(Camera_2D *camera, const World *world, Vector2 player_center, Input *input);
void reset_camera(Camera_2D *camera, const World *world);

inline Camera2D get_raylib_camera2D(const Camera_2D* camera) {
	return camera->raylib_cam;
}

#endif
