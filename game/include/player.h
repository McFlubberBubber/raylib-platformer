#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "general.h"

struct World;
struct Input;

enum PlayerState {
	PLAYER_IDLE = 0,
	
	PLAYER_MOVING,

	PLAYER_JUMPING,
	PLAYER_FALLING,

	PLAYER_HURT,
	PLAYER_DEAD,

	PLAYER_STATE_COUNT
};

struct Player {
	Rectangle sprite;
	Vector2 vel;

	PlayerState state;
	PlayerState prev_state;
	
	bool is_grounded;
	float coyote;
	s32 health;
};

void init_player(Player *player, const World *world);
void update_player(Player *player, World *world, Input *input, float dt);
void draw_player(const Player *player);

#endif
