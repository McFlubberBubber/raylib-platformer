#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

enum PlayerState {
	PLAYER_IDLE = 0,
	
	PLAYER_MOVING,

	PLAYER_JUMPING,
	PLAYER_FALLING,

	PLAYER_STATE_COUNT
};

struct Player {
	Rectangle sprite;
	Vector2 pos;

	PlayerState state;
	bool is_grounded; // Temp?

	float current_velocity;
};

void init_player(Player *player);
void update_player(Player *player);
void draw_player(Player *player);

#endif
