#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

struct World;

enum PlayerState {
	PLAYER_IDLE = 0,
	
	PLAYER_MOVING,

	PLAYER_JUMPING,
	PLAYER_FALLING,

	PLAYER_STATE_COUNT
};

// @TODO: There is a data dupe problem here, which the sprite.x and y obviously needs to sync up
// with the pos.x and y since they use rects (currently). But if the rect already has an x and y
// then we can just use that vector in the sprite rather than bundling up another vector within the
// player struct.
struct Player {
	Rectangle sprite;
	Vector2 pos;
	Vector2 vel;

	PlayerState state;
	PlayerState prev_state;
	
	bool is_grounded; // Temp?
	float coyote;
};

void init_player(Player *player);
void update_player(Player *player, World *world);
void draw_player(Player *player);

#endif
