#include "world.h"

#include "raymath.h" // For Lerp
#include "application.h"

void init_world(World *world, Arena *arena, u32 screen_count, u32 width, u32 height) {
	assert(arena != nullptr);
	assert(arena->base != nullptr);
	
	world->arena = arena;

	world->screen_width  = width;
	world->screen_height = height;

	world->tiles_per_screen = width * height;

	array_init(&world->screens, arena, screen_count);
	array_init(&world->tiles, arena, (screen_count * world->tiles_per_screen));
	
	printf("tiles.data: %p\n", world->tiles.data);
	assert((uintptr_t)world->tiles.data > 0x10000);

    // Initialize screens with correct tile offsets
    for (u32 i = 0; i < screen_count; ++i) {
        Screen s = {};
        s.tile_offset = i * world->tiles_per_screen;
        array_push(&world->screens, s);
    }

    // Zero out all tiles
    world->tiles.count = world->tiles.capacity;
    for (u32 i = 0; i < world->tiles.capacity; ++i) {
        world->tiles.data[i] = {};
    }

	world->tile_size = 32.0f; // @Hardcode.
	
	world->current_screen_index = 0;
	world->target_screen_index  = 0;
}

void update_world(World *world, Player *player, Camera2D *camera, float dt) {
	const float half_screen = world->screen_width * 0.5f;

	const float left_boundary  = camera->target.x - half_screen;
	const float right_boundary = camera->target.x + half_screen;

	const float player_center = player->pos.x + (player->sprite.width *  0.5f);
	const float buffer = 2.0f;

	s32 target = world->current_screen_index;

	if (player_center > (right_boundary + buffer) &&
		world->current_screen_index < (s32)world->screens.count - 1) {
		target = world->current_screen_index + 1;
	} else if (player_center < (left_boundary - buffer) && world->current_screen_index > 0) {
		target = world->current_screen_index - 1;
	}

	if (target != world->current_screen_index) {
		world->current_screen_index = target;
		world->target_screen_index  = target;
	}

	// Camera smoothing towards the next screen center.
	const float lerp_speed = 12.0f;
	float target_camera_x = target * world->screen_width;

	camera->target.x += (target_camera_x - camera->target.x) * lerp_speed * dt;

	if (fabsf(camera->target.x - target_camera_x) < 0.5f) {
		camera->target.x = target_camera_x; // Instant snap when at certain threshold.
	}
}

void draw_world(World *world) {
	const float line_thickness = 2.0f;
//	const float tile_size = 32.0f;

	s32 screen_index = world->current_screen_index;
	Screen *screen   = &world->screens.data[screen_index];

	assert(screen_index >= 0 && screen_index < world->screens.count);

	u32 base  = screen->tile_offset;
	u32 count = world->tiles_per_screen;

	for (u32 i = 0; i < count; ++i) {
		Tile *tile = &world->tiles.data[base + i];
		if (tile->type == TILE_EMPTY) continue;

		Vector2 pos = tile_index_to_world(world, screen_index, i);

		// Tile visual specifications.
		Rectangle rect = { pos.x, pos.y, world->tile_size, world->tile_size };
		Color color = BLUE;
	
		switch (tile->type) {
		case TILE_SOLID: { color = DARKGRAY; break; }
		case TILE_SPIKE: { color = PURPLE;   break; }
		}
		
		DrawRectangleLinesEx(rect, line_thickness, color);
	}
}

void cleanup_world(World *world) {
	// Not needed?
}

Vector2 tile_index_to_world(const World *world, u32 screen_index, u32 tile_index) {
	u32 x = tile_index % world->screen_width;
	u32 y = tile_index / world->screen_width;

	Vector2 result = { 0 };
	result.x = ((screen_index * world->screen_width) + x) * world->tile_size;
	result.y = y * world->tile_size;
	
	return result;
}

void world_to_tile(World *world, float world_x, float world_y, u32 *screen,
						  u32 *out_x, u32 *out_y) {
	float screen_world_width = world->screen_width * world->tile_size;
	*screen = (u32)floorf(world_x / screen_world_width);
	float local_x = world_x - (*screen * screen_world_width);

	*out_x = (u32)(local_x / world->tile_size);
	*out_y = (u32)(world_y / world->tile_size);
}

bool is_solid(const World *world, float world_x, float world_y) {
	float screen_world_width = world->screen_width * world->tile_size;

	s32 screen = (s32)(world_x / screen_world_width); // Find which screen we are in.
	if (screen < 0 || screen >= (s32)world->screens.count) {
		return false; // Treat as out of bounds.
	}

	// Converting to local screen space.
	float local_x = world_x - (screen * screen_world_width);
	float local_y = world_y;

	// Convert to tile coordinates.
	s32 tile_x = (s32)(local_x / world->tile_size);
	s32 tile_y = (s32)(local_y / world->tile_size);

	// Bounds checking.
	if (tile_x < 0 || tile_x >= (s32)world->screen_width ||
		tile_y < 0 || tile_y >= (s32)world->screen_height) {
		return false;
	}

	// Fetch tile index.
	u32 tile_index = get_tile_index(world, screen, (u32)tile_x, (u32)tile_y);
	if (tile_index >= world->tiles.count) return false;
		
	Tile *tile = &world->tiles.data[tile_index];
	return (tile->type == TILE_SOLID || tile->type == TILE_SPIKE);
}
