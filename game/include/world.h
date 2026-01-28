#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include <vector> // For dynamic array stuff.

struct Player;

struct Tile {
	Rectangle rect;
};


struct World {
	std::vector<Tile> tiles; // @Temp: Just want to test collisions, these aren't even gonna be
						     // a tile, just wanna draw some rectangles for early testing.
	int current_screen_index;
	int  target_screen_index;
	
	int screen_width;
	int screen_height;
	int total_screens;
};

void init_world(World *world);
void update_world(World *world, Player *player, Camera2D *camera);
void draw_world(World *world);
void cleanup_world(World *world);

#endif
