#include "camera.h"

#include "raylib.h"
#include "application.h"

void init_camera(Camera2D *camera) {
	const Vector2 CAMERA_ORIGIN  = { 0, 0 };
	const Vector2 CAMERA_OFFSET = {
		(float)(g_app->game_width / 2), (float)(g_app->game_height / 2)
	};
	const float CAMERA_ROTATION = 0.0f;
	const float CAMERA_ZOOM     = 1.0f;

	camera->target.x = CAMERA_ORIGIN.x;
	camera->target.y = CAMERA_ORIGIN.y;
	
    camera->offset.x = CAMERA_OFFSET.x;
	camera->offset.y = CAMERA_OFFSET.y;
    
	camera->rotation = CAMERA_ROTATION;
    camera->zoom     = CAMERA_ZOOM;
}

// We only do things in this procedure when we are in editor mode, but we should probably be doing
// stuff here even when we are in game view.
void update_camera(Camera2D *camera, Input *input) {
	Game *game = &g_app->game;
	if (game->state != GAME_EDITOR) {
		return;
	}
	
	const float movement_speed = 5.0f;

	camera->offset.y += (input->camera_movement.y * movement_speed); 
	camera->offset.x += (input->camera_movement.x * movement_speed);
	camera->zoom = input->camera_zoom;    
}

// @TODO: This needs to get changed in the future since we just reset to 0,0.
void reset_camera(Camera2D *camera) {
	init_camera(camera);
}

// @Cleanup: We don't even use this so it's currently dead code.
void transition_camera(Camera2D *camera, Vector2 dest, float dt) {
	const float CAMERA_LERP_SPEED = 12.0f;
	Vector2 new_target = { dest.x * g_app->game_width, dest.y * g_app->game_height };

	if (new_target.x != camera->target.x) {
		camera->target.x += (new_target.x - camera->target.x) * CAMERA_LERP_SPEED * dt;
		if (fabs(camera->target.x - new_target.x) < 0.5f) {
			camera->target.x = new_target.x;
		}
	}

	if (new_target.y != camera->target.y) {
		camera->target.y += (new_target.y - camera->target.y) * CAMERA_LERP_SPEED * dt;
		if (fabs(camera->target.y - new_target.y) < 0.5f) {
			camera->target.y = new_target.y;
		}
	}
}
