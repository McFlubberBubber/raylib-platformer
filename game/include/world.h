#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include "general.h"

struct Player;

enum TileType : u8 {
	TILE_EMPTY = 0,
	TILE_SOLID,
	TILE_SPIKE
};

struct Tile {
	TileType type;
	u8 flags;
	u16 variant;
};

struct Screen {
	u32 tile_offset; // @NOTE: This assumes every screen size has the same offset...
	s32 grid_x;
	s32 grid_y;
	bool is_valid;
	// 3 bytes of padding here...
};

struct World {
	Arena *arena;

	// The following dimensions are in tiles, not in pixels.
	u32 screen_width_in_tiles;
	u32 screen_height_in_tiles;
	u32 tiles_per_screen; 	 // Width * Height.
	float tile_size = 32.0f; 

	u32 grid_width;			 // In screens.
	u32 grid_height;		 // In screens.
	Array<Screen> screens;	 // Count == grid_width * grid_height.
	Array<Tile> tiles;		 // Count == grid_width * grid_height * tiles_per_screen.

	// Grid coordinates, not pixel or tile.
	s32 current_screen_x;
	s32 current_screen_y;
};

#pragma pack(push, 1)
struct WorldFileHeader {
	u32 magic;			// 4 bytes.
	u16 version;		// 2 bytes.
	
	u16 screen_width;   // 2 bytes - in tiles.
	u16 screen_height;  // 2 bytes - in tiles.
	
	u16 grid_width;     // 2 bytes - how many screens wide.
	u16 grid_height;	// 2 bytes - how many screens tall.
	u16 reserved;		// 2 bytes of padding.
}; 						// Should be a total of 16 bytes.
#pragma pack(pop)

#define WORLD_FILE_MAGIC 0x57524C44 // Hex value reads "WRLD".
#define WORLD_FILE_VERSION 2

static_assert(sizeof(Tile) == 4,  "Tile size changed! serialization error.");
static_assert(sizeof(WorldFileHeader) == 16, "Header layout changed! Serialization error.");

bool save_world(const World *world);
bool load_world(World *world, Arena *arena); 
void load_placeholder_world(World* world, Arena *arena);
void init_world(World *world, Arena *arena, u32 grid_width, u32 grid_height, u32 width_in_tiles, u32 height_in_tiles);
void cleanup_world(World *world);

void update_world(World *world, Vector2 player_center);

void draw_world(const World *world, bool draw_all_screens);

// Coordinate utils.
inline u32 get_tile_index(const World *world, u32 screen_index, u32 x, u32 y) {
	return world->screens.data[screen_index].tile_offset + (y * world->screen_width_in_tiles + x);
}

// Takes grid coordinates.
inline Screen *world_get_screen(const World *world, s32 grid_x, s32 grid_y) {
    if (grid_x < 0 || grid_x >= (s32)world->grid_width ||
        grid_y < 0 || grid_y >= (s32)world->grid_height) {
        return nullptr; // Out of bounds.
    }
    Screen *s = &world->screens.data[grid_y * world->grid_width + grid_x];
    return s->is_valid ? s : nullptr; // Null screen.
}
Screen *world_get_screen_from_pos(const World *world, Vector2 world_pos);

// Pixel-space helpers, derived from tile dimensions
inline float world_screen_pixel_width(const World *world) {
    return world->screen_width_in_tiles * world->tile_size;
}
inline float world_screen_pixel_height(const World *world) {
    return world->screen_height_in_tiles * world->tile_size;
}

// Returns pixel-space pos of a tile.
Vector2 tile_index_to_world(const World *world, u32 screen_index, u32 tile_index);
// Converts pixel space to world coords.
void world_pos_to_tile(const World *world, Vector2 world_pos, u32 *out_screen, u32 *out_x, u32 *out_y);

bool is_solid(const World *world, float world_x, float world_y);

#endif
