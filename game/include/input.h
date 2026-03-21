#ifndef INPUT_H
#define INPUT_H

struct Application;

struct Input {
	// General game inputs.
	bool pause_game = false;
	bool reset_game = false;
	
	// Player-specific inputs.
	float player_move_x = 0.0f;
	bool player_jump    = false;
};

void poll_inputs(Application *app);

#endif
