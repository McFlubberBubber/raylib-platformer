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
};

struct World {
	Arena *arena;

	u32 screen_width;
	u32 screen_height;
	u32 tiles_per_screen; // Width * Height.

	Array<Screen> screens;
	Array<Tile> tiles;
	
	float tile_size; // @Temporary? 

	s32 current_screen_index;
	s32  target_screen_index;
};

#pragma pack(push, 1)
struct WorldFileHeader {
	u32 magic;
	u16 version;
	
	u16 screen_width;
	u16 screen_height;
	u16 screen_count;
	
	u32 reserved;
};
#pragma pack(pop)

#define WORLD_FILE_MAGIC 0x57524C44 // Hex value reads "WRLD".
#define WORLD_FILE_VERSION 1

static_assert(sizeof(Tile)            == 4,  "Tile size changed! serialization error.");
static_assert(sizeof(WorldFileHeader) == 16, "Header layout changed! Serialization error.");

bool save_world(const World *world);
bool load_world(World *world, Arena *arena); 

void init_world(World *world, Arena *arena, u32 screen_count, u32 width, u32 height);
void update_world(World *world, Player *player, Camera2D *camera, float dt);
void draw_world(World *world);
void cleanup_world(World *world);

// Lookup functions.
inline u32 get_tile_index(const World *world, u32 screen_index, u32 x, u32 y) {
	return world->screens.data[screen_index].tile_offset + (y * world->screen_width + x);
}

Vector2 tile_index_to_world(const World *world, u32 screen_index, u32 tile_index);
void world_to_tile(const World *world, Vector2 world_pos, u32 *screen, u32 *out_x, u32 *out_y);
bool is_solid(const World *world, float world_x, float world_y);

#endif
