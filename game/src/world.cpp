#include "world.h"

#include "player.h"
#include "raymath.h" // For Lerp

// @Temp: just to put some tiles on the screen for testing.
static void init_some_tiles(World *world) {
	constexpr float tile_width  = 50.0f;
	constexpr float tile_height = tile_width;
	constexpr int   tile_count  = 100; 
	constexpr int   starting_x  = -650;  // Start from the left.
	const     int   starting_y  = 310;
	
	// Pushing new tiles into the array.
	for (int i = 0; i < tile_count; ++i) {
		Tile t;
		t.rect.x	  = starting_x + (tile_width * i);
		t.rect.y 	  = starting_y; // All tiles will be on the same y-level.
		t.rect.width  = tile_width;
		t.rect.height = tile_height;
		world->tiles.push_back(t);
	}

	// @Temp: Putting another row of tiles on a level above the other tiles.
	const int new_y = starting_y - tile_height;
	for (int i = 0; i < 6; ++i) {
		Tile t;
		t.rect.x	  = starting_x + (tile_width * (i + 10));
		t.rect.y 	  = new_y;
		t.rect.width  = tile_width;
		t.rect.height = tile_height;
		world->tiles.push_back(t);
	}
}

void init_world(World *world) {
	world->screen_width		    = GetScreenWidth();
	world->screen_height	    = GetScreenHeight();
	world->current_screen_index = 0;
	world->total_screens        = 10;

	init_some_tiles(world);
}

void update_world(World *world, Player *player, Camera2D *camera) {
	// Checking if the player has moved to the next screen.
	const float left_boundary  = camera->target.x - (world->screen_width / 2.0f);
	const float right_boundary = camera->target.x + (world->screen_width / 2.0f);
	const float player_center  = player->pos.x + (player->sprite.width / 2);
	constexpr float buffer     = 2.0f;
	int target = world->current_screen_index;
	
	if (player_center > right_boundary + buffer &&
		world->current_screen_index < world->total_screens - 1) {
		target = world->current_screen_index + 1;
	} else if (player_center < left_boundary - buffer && world->current_screen_index > -10) {
		target = world->current_screen_index - 1;
	}
	
	if (target != world->current_screen_index) {
		world->current_screen_index = target;
		world->target_screen_index  = target;
		// camera->target.x = target * world->screen_width; // Instant snap to position
	}

	// Transition for the camera.
	constexpr float lerp_speed = 12.0f;
	float target_camera_x = target * world->screen_width;
	float dt = GetFrameTime();

	camera->target.x += (target_camera_x - camera->target.x) * lerp_speed * dt;
	if (fabs(camera->target.x - target_camera_x) < 0.5f) camera->target.x = target_camera_x;
}

void draw_world(World *world) {
	constexpr float line_thickness = 2.0f; 
	for (auto &t : world->tiles) {
		DrawRectangleLinesEx(t.rect, line_thickness, BLUE);
	}
}

void cleanup_world(World *world) {
	// std::vector should cleanup after itself, but just incase.
	world->tiles.clear();
}
