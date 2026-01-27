#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include <vector> // For dynamic array stuff.

struct Tile {
	Rectangle rect;
	Vector2   pos;
};

struct World {
	std::vector<Tile> tiles; // @Temp: Just want to test collisions, these aren't even gonna be
						     // a tile, just wanna draw some rectangles for early testing.
};

void init_world(World *world);
void update_world(World *world);
void cleanup_world(World *world);

#endif
