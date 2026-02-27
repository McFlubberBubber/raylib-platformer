#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include <vector> // For dynamic array stuff.

struct Player;

struct Tile {
	Rectangle rect;
};

struct Screen {
	std::vector<Tile> tiles; 
};

struct World {
	std::vector<Tile> tiles; // Temp...
	int current_screen_index;
	int  target_screen_index;


	int screen_width;
	int screen_height;
	int total_screens;
	std::vector<Screen> screens;
};

void init_world(World *world, Camera2D *camera);
void update_world(World *world, Player *player, Camera2D *camera, float dt);
void draw_world(World *world);
void cleanup_world(World *world);

#endif
