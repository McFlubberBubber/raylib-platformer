#include "world.h"

// @Temp: just to put some tiles on the screen for testing.
static void init_some_tiles(World *world) {
	constexpr float tile_width  = 40.0f;
	constexpr float tile_height = tile_width;
	constexpr int   tile_count  = 50; 
	constexpr int   starting_x  = 0;  // Start from the left.
	const     int   starting_y  = static_cast<int>(GetScreenHeight() * 0.7f);
	
	// Pushing new tiles into the array.
	for (int i = 0; i < tile_count; ++i) {
		Tile t;
		t.pos.x		  = starting_x + (tile_width * i);
		t.pos.y 	  = starting_y; // All tiles will be on the same y-level.
		t.rect.width  = tile_width;
		t.rect.height = tile_height;
		t.rect.x      = t.pos.x;
		t.rect.y      = t.pos.y;
		world->tiles.push_back(t);
	}

	// @Temp: Putting another row of tiles on a level above the other tiles.
	const int new_y = starting_y - tile_height;
	for (int i = 0; i < 5; ++i) {
		Tile t;
		t.pos.x		  = starting_x + (tile_width * (i + 10));
		t.pos.y 	  = new_y;
		t.rect.width  = tile_width;
		t.rect.height = tile_height;
		t.rect.x      = t.pos.x;
		t.rect.y      = t.pos.y;
		world->tiles.push_back(t);
	}
	
	Tile t1;
	t1.pos.x	   = starting_x + (tile_width * 14);
	t1.pos.y 	   = new_y - tile_height;
	t1.rect.width  = tile_width;
	t1.rect.height = tile_height;
	t1.rect.x      = t1.pos.x;
	t1.rect.y      = t1.pos.y;
	world->tiles.push_back(t1);	

	Tile t2;
	t2.pos.x       = starting_x + (tile_width * 15);
	t2.pos.y 	   = new_y - tile_height;
	t2.rect.width  = tile_width;
	t2.rect.height = tile_height;
	t2.rect.x      = t2.pos.x;
	t2.rect.y      = t2.pos.y;
	world->tiles.push_back(t2);	
}

void init_world(World *world) {
	init_some_tiles(world);
}

void update_world(World *world) {
	// Nothing is happening here yet.
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
