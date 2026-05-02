#include "world.h"

#include "raymath.h" // For Lerp
#include "application.h"

bool save_world(const World *world) {
	char full_path[128];
	const char *app_dir    = GetApplicationDirectory();
	const char *world_path = "data/world/level_01.wld";
	
	snprintf(full_path, sizeof(full_path), "%s%s", app_dir, world_path); 

	FILE *file = fopen(full_path, "wb");
	if (!file) {
		fprintf(stderr, "[SAVE_WORLD] Failed to open '%s' for writing.\n", full_path);
		return false;
	}

	WorldFileHeader header = {};
	header.magic   		 = WORLD_FILE_MAGIC;
	header.version 		 = WORLD_FILE_VERSION;
	header.screen_width  = (u16)world->screen_width_in_tiles;
	header.screen_height = (u16)world->screen_height_in_tiles;
	header.grid_width    = (u16)world->grid_width;
	header.grid_height   = (u16)world->grid_height;

	if (fwrite(&header, sizeof(header), 1, file) != 1) {
		fprintf(stderr, "[SAVE_WORLD] Failed to write to header.\n");
		fclose(file);
		return false;
	}

	u32 tile_count = world->grid_width * world->grid_height * world->tiles_per_screen;
	if (fwrite(world->tiles.data, sizeof(Tile), tile_count, file) != tile_count) {
		fprintf(stderr, "[SAVE_WORLD] Failed to write tile data.\n");
		fclose(file);
		return false;
	}

	fclose(file);
	return true;
}

bool load_world(World *world, Arena *arena) {
	char full_path[128];
	const char *app_dir    = GetApplicationDirectory();
	const char *world_path = "data/world/level_01.wld";
	
	printf("app_dir: %s\n", app_dir);
	snprintf(full_path, sizeof(full_path), "%s%s", app_dir, world_path); 

	FILE *file = fopen(full_path, "rb");
	if (!file) {
		fprintf(stderr, "[LOAD_WORLD] Failed to open '%s' for reading.\n", full_path);
		return false;
	}

	WorldFileHeader header = {};
	if (fread(&header, sizeof(header), 1, file) != 1) {
		fprintf(stderr, "[LOAD_WORLD] Failed to read header.\n");
		fclose(file);
		return false;
	}

	// Validation checks for magic value and world version. 
	if (header.magic != WORLD_FILE_MAGIC) {
		fprintf(stderr, "[LOAD_WORLD] Bad magic value: 0x%08X.\n", header.magic);
		fclose(file);
		return false;
	}
	
	if (header.version != WORLD_FILE_VERSION) {
		fprintf(stderr, "[LOAD_WORLD] Version mismatch: got %u, expected %u.\n", header.version, WORLD_FILE_VERSION);
		fclose(file);
		return false;
	}

	// Initialize the world structure with the arena.
	init_world(world, arena, header.grid_width, header.grid_height, header.screen_width, header.screen_height);

	// Reading the tiles directly into the arena-backed array.
	u32 tile_count = header.grid_width * header.grid_height * (header.screen_width * header.screen_height);
	if (fread(world->tiles.data, sizeof(Tile), tile_count, file) != tile_count) {
        fprintf(stderr, "[LOAD_WORLD] Failed to read tile data.\n");
        fclose(file);
        return false;
    }

    // tiles.count was set by init_world, data is now populated
    fclose(file);
    return true;
}

void load_placeholder_world(World *world, Arena *arena) {
	const u32 grid_width = 5;
	const u32 grid_height = 1;
	const u32 width_in_tiles  = (u32)(g_app->game_width / world->tile_size);
	const u32 height_in_tiles = (u32)(g_app->game_height / world->tile_size); 
	init_world(world, arena, grid_width, grid_height, width_in_tiles, height_in_tiles);
	
	for (u32 gy = 0; gy < world->grid_height; ++gy) {
		for (u32 gx = 0; gx < world->grid_width; ++gx) {
			Screen *screen = world_get_screen(world, (s32)gx, (s32)gy);
			if (!screen) continue;

			u32 bottom_row = screen->tile_offset + (world->screen_height_in_tiles - 1) * world->screen_width_in_tiles;
			for (u32 x = 0; x < world->screen_width_in_tiles; ++x) {
				world->tiles.data[bottom_row + x].type = TILE_SOLID; 
			}
		}
	}
}

void init_world(World *world, Arena *arena, u32 grid_width, u32 grid_height, u32 width_in_tiles, u32 height_in_tiles) {
	assert(arena != nullptr);
	assert(arena->base != nullptr);
	
	world->arena = arena;
	
	world->screen_width_in_tiles  = width_in_tiles;
	world->screen_height_in_tiles = height_in_tiles;
	world->tiles_per_screen = width_in_tiles * height_in_tiles;
	world->tile_size = 32.0f;

	world->grid_width  = grid_width;
	world->grid_height = grid_height;
	
	world->current_screen_x = 0;
	world->current_screen_y = 0;

	u32 screen_count = grid_width * grid_height;
	array_init(&world->screens, arena, screen_count);
	array_init(&world->tiles, arena, (screen_count * world->tiles_per_screen));

	assert((uintptr_t)world->tiles.data > 0x10000);

    // Initialize screens with 2D grid coords.
	for (u32 y = 0; y < grid_height; ++y) {
		for (u32 x = 0; x < grid_width; ++x) {
			Screen screen = {};
			screen.grid_x = (s32)x;
			screen.grid_y = (s32)y;
			screen.tile_offset = (y * grid_width + x) * world->tiles_per_screen;
			screen.is_valid = true;
			array_push(&world->screens, screen);
		}
	}

	world->tiles.count = world->tiles.capacity;
	for (u32 i = 0; i < world->tiles.count; ++i) {
		world->tiles.data[i] = {};
	}
}

void cleanup_world(World *world) {
	// Not needed?
}

void update_world(World *world, Vector2 player_center) {
	float screen_pixel_width  = world_screen_pixel_width(world);	
	float screen_pixel_height = world_screen_pixel_height(world);
	
	s32 new_screen_x = (s32)floorf(player_center.x / screen_pixel_width);
	s32 new_screen_y = (s32)floorf(player_center.y / screen_pixel_height);

	// Clamping to a valid grid range.
	new_screen_x = (s32)fmaxf(0.0f, fminf(new_screen_x, (float)world->grid_width - 1));
	new_screen_y = (s32)fmaxf(0.0f, fminf(new_screen_y, (float)world->grid_height - 1));

	world->current_screen_x = new_screen_x;
	world->current_screen_y = new_screen_y;
}

void draw_world(const World *world) {
    const float line_thickness = 2.0f;

    s32 screen_index = world->current_screen_y * world->grid_width + world->current_screen_x;
    Screen *screen   = &world->screens.data[screen_index];

    assert(screen->is_valid);

    u32 base  = screen->tile_offset;
    u32 count = world->tiles_per_screen;

    for (u32 i = 0; i < count; ++i) {
        Tile *tile = &world->tiles.data[base + i];
        if (tile->type == TILE_EMPTY) continue;

        Vector2 pos  = tile_index_to_world(world, screen_index, i);
        Rectangle rect = { pos.x, pos.y, world->tile_size, world->tile_size };
        Color color  = BLUE;

        switch (tile->type) {
        case TILE_SOLID: { color = DARKGRAY; break; }
        case TILE_SPIKE: { color = PURPLE;   break; }
        }

        DrawRectangleLinesEx(rect, line_thickness, color);
    }
}

Screen *world_get_screen_from_pos(const World *world, Vector2 world_pos) {
	float screen_pixel_width  = world_screen_pixel_width(world);
	float screen_pixel_height = world_screen_pixel_height(world);

	s32 gx = (s32)floorf(world_pos.x / screen_pixel_width);
	s32 gy = (s32)floorf(world_pos.y / screen_pixel_height);
	return world_get_screen(world, gx, gy);
}

Vector2 tile_index_to_world(const World *world, u32 screen_index, u32 tile_index) {
	u32 x = tile_index % world->screen_width_in_tiles;
	u32 y = tile_index / world->screen_width_in_tiles;

	u32 screen_gx = screen_index % world->grid_width;
	u32 screen_gy = screen_index / world->grid_width;
	
	Vector2 result = { 0 };	
	result.x = ((screen_gx * world->screen_width_in_tiles) + x) * world->tile_size;
    result.y = ((screen_gy * world->screen_height_in_tiles) + y) * world->tile_size;
	return result;
}

// @TODO.
void world_pos_to_tile(const World *world, Vector2 world_pos, u32 *out_screen, u32 *out_x, u32 *out_y) {

}

bool is_solid(const World *world, float world_x, float world_y) {
	Screen *screen = world_get_screen_from_pos(world, {world_x, world_y});
    if (!screen) return true; // null or out of bounds = solid boundary

    float screen_pixel_width  = world_screen_pixel_width(world);
    float screen_pixel_height = world_screen_pixel_height(world);

    float local_x = world_x - (screen->grid_x * screen_pixel_width);
    float local_y = world_y - (screen->grid_y * screen_pixel_height);

    s32 tile_x = (s32)(local_x / world->tile_size);
    s32 tile_y = (s32)(local_y / world->tile_size);

    if (tile_x < 0 || tile_x >= (s32)world->screen_width_in_tiles ||
        tile_y < 0 || tile_y >= (s32)world->screen_height_in_tiles) {
        return true; // out of screen bounds = solid
    }

    u32 screen_index = screen->grid_y * world->grid_width + screen->grid_x;
    u32 tile_index   = get_tile_index(world, screen_index, (u32)tile_x, (u32)tile_y);
    if (tile_index >= world->tiles.count) return true;

    Tile *tile = &world->tiles.data[tile_index];
    return (tile->type == TILE_SOLID || tile->type == TILE_SPIKE);
}
