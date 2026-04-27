#ifndef INPUT_H
#define INPUT_H

#include "raylib.h"

struct Application;

struct Input {
	// General game inputs.
	bool pause_game = false;
	bool reset_game = false;
	
	// Player-specific inputs.
	float player_move_x = 0.0f;
	bool player_jump    = false;

	// Camera inputs.
	Vector2 camera_movement = {};
	float   camera_zoom = 0.0f;
};

void poll_inputs(Application *app);

#endif
