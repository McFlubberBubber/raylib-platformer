#include "camera.h"

#include "raylib.h"
#include "raymath.h"
#include "application.h"

void init_camera(Camera_2D *camera, const World *world) {
	float screen_pixel_width = world_screen_pixel_width(world);
	float screen_pixel_height = world_screen_pixel_height(world);

	// Start centered at 0, 0 on the grid.
	camera->raylib_cam.target.x = screen_pixel_width * 0.5f;
	camera->raylib_cam.target.y = screen_pixel_height * 0.5f;
	camera->raylib_cam.offset   = {g_app->game_width * 0.5f, g_app->game_height * 0.5f};
	camera->raylib_cam.rotation = 0;
	camera->raylib_cam.zoom     = 1.0f;

	camera->is_transitioning = false;
	camera->target_world_pos = camera->raylib_cam.target;
	camera->lerp_speed       = 12.0f;
}

// @NOTE: player_center may be used in the future for player trackig purposes.
void update_camera(Camera_2D *camera, const World *world, Vector2 player_center, Input *input) {
	float screen_pixel_width = world_screen_pixel_width(world);
	float screen_pixel_height = world_screen_pixel_height(world);

	float desired_x = (world->current_screen_x * screen_pixel_width) + screen_pixel_width * 0.5f;
	float desired_y = (world->current_screen_y * screen_pixel_height) + screen_pixel_height * 0.5f;
	
	if (desired_x != camera->target_world_pos.x || desired_y != camera->target_world_pos.y) {
		camera->target_world_pos = {desired_x, desired_y};
		camera->is_transitioning = true;
	}

	// Lerp towards the target.
	float dt = g_app->dt;
	if (camera->is_transitioning) {
		camera->raylib_cam.target.x += (camera->target_world_pos.x - camera->raylib_cam.target.x) * camera->lerp_speed * dt;
		camera->raylib_cam.target.y += (camera->target_world_pos.y - camera->raylib_cam.target.y) * camera->lerp_speed * dt;

		float dx = fabsf(camera->raylib_cam.target.x - camera->target_world_pos.x);
		float dy = fabsf(camera->raylib_cam.target.y - camera->target_world_pos.y);
		if (dx < 0.5f && dy < 0.5f) {
			camera->raylib_cam.target = camera->target_world_pos;
			camera->is_transitioning = false;
		}
	}

	//
	// Editor Camera Movements
	//
	if (input->camera_movement.x != 0.0f || input->camera_movement.y != 0.0f) {
		const float pan_speed = 5.0f;
		camera->raylib_cam.target.x += input->camera_movement.x * pan_speed;
		camera->raylib_cam.target.y += input->camera_movement.y * pan_speed;
	}

	// 
	// Editor Zoom
	//
	if (input->camera_zoom != 0.0f) {
		camera->raylib_cam.zoom += input->camera_zoom * 0.25f;
		if (camera->raylib_cam.zoom > 10.0f) camera->raylib_cam.zoom = 10.0f;
		if (camera->raylib_cam.zoom < 0.1f)  camera->raylib_cam.zoom = 0.1f;
	}
}

void reset_camera(Camera_2D *camera, const World *world) {
	init_camera(camera, world);
}

