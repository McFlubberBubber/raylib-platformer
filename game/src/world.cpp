#include "world.h"

//#include "player.h"
#include "raymath.h" // For Lerp
#include "application.h"

// Global tilemap for now...
const int SCREEN_HEIGHT = 9;
const int SCREEN_WIDTH  = 16;

const int SCREEN_HEIGHT2 = 25;
const int SCREEN_WIDTH2  = 20;

const int screen1[SCREEN_HEIGHT2][SCREEN_WIDTH2] = { // [row][column]
	{ 0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },

	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },

	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },

	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },

	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 0, 1,  0, 0, 0, 0, 0,  0, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 0, 0, 0, 0,  0, 0, 0, 1, 1,  1, 0, 0, 0, 1,  1, 0, 0, 0, 0 }, // 0, 0, 0, 0, 0 },
	{ 1, 1, 1, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1, 1 }, // 1, 0, 0, 1, 1 }
};

const int screen2[SCREEN_HEIGHT][SCREEN_WIDTH] = {
	{ 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0 },
	{ 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0 },
	{ 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0 },

	{ 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0 },
	{ 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0 },
	{ 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1 },

	{ 0, 0, 0, 0,  0, 0, 1, 1,  0, 0, 0, 0,  0, 0, 1, 1 },
	{ 0, 1, 1, 0,  0, 1, 1, 0,  0, 0, 0, 0,  0, 1, 1, 1 },
	{ 1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1 }
};

void init_world(World *world, Camera2D *camera) {
	// Application *app = Application::instance;
	world->screen_width		    = g_app->game_width;
	world->screen_height	    = g_app->game_height;
	world->current_screen_index = 0;
	world->total_screens        = 10;

#if 0
	// Doing 2 screens for now, since those are all the tile data stuff we have.
	for (int i = 0; i < 2; ++i) {
		Screen screen;
		const float tile_width  = 50.0f;
		const float tile_height = tile_width;

		const float left_boundary  = camera->target.x - (world->screen_width / 2.0f);
		const float right_boundary = camera->target.x + (world->screen_width / 2.0f);

		Vector2 top_left = { 0 };
		top_left.x = camera->target.x - (world->screen_width  / 2.0f);
		top_left.y = camera->target.y - (world->screen_height / 2.0f);
		
		for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			for (int x = 0; x < SCREEN_WIDTH; ++x) {
				// @TODO: Super janky temp things to differentiate between the 2 screens
				// because I can't be asked, we should be having a proper data structure between
				// the screens and its data.
				if (i == 0) {
					if (screen1[y][x] == 1) {
						Tile t;
						t.rect.x      = (x * tile_width)  - 960;
						t.rect.y      = (y * tile_height) - 650;
						t.rect.width  = tile_width;
						t.rect.height = tile_height;
						screen.tiles.push_back(t);
					}
				} else {
					if (screen2[y][x] == 1) {
						Tile t;
						t.rect.x      = top_left.x - (x * tile_width);
						t.rect.y      = top_left.y - (y * tile_height);
						t.rect.width  = tile_width;
						t.rect.height = tile_height;
						screen.tiles.push_back(t);
					}
				}
			} 
		} 
		world->screens.push_back(screen);
	}
#else
	Screen screen;
	const float tile_width  = 50.0f;
	const float tile_height = tile_width;

	for (int y = 0; y < SCREEN_HEIGHT2; ++y) {
		for (int x = 0; x < SCREEN_WIDTH2; ++x) {
			// @TODO: Super janky temp things to differentiate between the 2 screens
			// because I can't be asked, we should be having a proper data structure between
			// the screens and its data.
			if (screen1[y][x] == 1) {
				Tile t;
				t.rect.x      = (x * tile_width)  - 960;
				t.rect.y      = (y * tile_height) - 710;
				t.rect.width  = tile_width;
				t.rect.height = tile_height;
				screen.tiles.push_back(t);
			}
		}
	}
	world->screens.push_back(screen);
#endif

}

void update_world(World *world, Player *player, Camera2D *camera, float dt) {
	// Checking if the player has moved to the next screen.
	const float left_boundary  = camera->target.x - (world->screen_width / 2.0f);
	const float right_boundary = camera->target.x + (world->screen_width / 2.0f);
	const float player_center  = player->pos.x + (player->sprite.width / 2);
	const float buffer     = 2.0f;
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
	const float lerp_speed = 12.0f;
	float target_camera_x = target * world->screen_width;
	// float dt = GetFrameTime();

	camera->target.x += (target_camera_x - camera->target.x) * lerp_speed * dt;
	if (fabs(camera->target.x - target_camera_x) < 0.5f) camera->target.x = target_camera_x;
}

void draw_world(World *world) {
	const float line_thickness = 2.0f; 
#if 0
	for (auto &tile : world->screens[world->current_screen_index].tiles) {
		DrawRectangleLinesEx(tile.rect, line_thickness, BLUE);
	}
#else
	for (auto &tile : world->screens[0].tiles) {
		DrawRectangleLinesEx(tile.rect, line_thickness, BLUE);
	}
#endif
	
}

void cleanup_world(World *world) {
	// std::vector should cleanup after itself, but just incase.
	world->tiles.clear();
}
