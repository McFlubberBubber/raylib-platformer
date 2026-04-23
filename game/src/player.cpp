#include "player.h"

#include "application.h"
#include "world.h"

static void check_horizontal_collisions(Player *player, World *world) {
	const float tile_size = world->tile_size;

	float top    = player->pos.y;
    float bottom = player->pos.y + player->sprite.height;
    float left   = player->pos.x;
    float right  = player->pos.x + player->sprite.width;

	float mid_y = player->pos.y + player->sprite.height * 0.5f;

    if (player->vel.x > 0) {
        // Moving right -> check right.
        if (is_solid(world, right, top) || is_solid(world, right, mid_y) ||
			is_solid(world, right, bottom)) {
            player->pos.x = floorf(right / tile_size) * tile_size - player->sprite.width;
            player->vel.x = 0;
        }
    } else if (player->vel.x < 0) {
        // Moving left -> check left.
        if (is_solid(world, left, top) || is_solid(world, left, mid_y) ||
			is_solid(world, left, bottom)) {
            player->pos.x = (floorf(left / tile_size) + 1.0f) * tile_size;
            player->vel.x = 0;
        }
    }

//    player->sprite.x = player->pos.x;
}


static void check_vertical_collisions(Player *player, World *world) {
	player->is_grounded = false;
	
	const float tile_size = world->tile_size;
	float left   = player->pos.x;
	float right  = player->pos.x + player->sprite.width;
	float top    = player->pos.y;
	float bottom = player->pos.y + player->sprite.height;

	float mid_x  = player->pos.x + player->sprite.width * 0.5f; 

	if (player->vel.y > 0) {
		// Falling -> check bottom.
		if (is_solid(world, left, bottom) || is_solid(world, mid_x, bottom) ||
			is_solid(world, right, bottom)) {
			player->pos.y = floorf(bottom / tile_size) * tile_size - player->sprite.height;
			player->vel.y = 0;
			player->is_grounded = true;
		}
	} else if (player->vel.y < 0) {
		// Jumping -> check top.
		if (is_solid(world, left, top) || is_solid(world, mid_x, top) ||
			is_solid(world, right, top)) {
			player->pos.y = (floorf(top / tile_size) + 1.0f) * tile_size;
			player->vel.y = 0;
		}
	}

//	player->sprite.y = player->pos.y;
}

void init_player(Player *player) {
	int center_x = g_app->game_width / 2;

	player->pos.x = 0;
	player->pos.y = 0;
	player->vel.x = 0.0f;
	player->vel.y = 0.0f;

	player->sprite.x = player->pos.x;
	player->sprite.y = player->pos.y;
	player->sprite.width  = 20;
	player->sprite.height = 20;

	player->state	    = PLAYER_IDLE;
	player->prev_state  = player->state;
	
	player->is_grounded = false;

	player->health = 3;
}

void update_player(Player *player, World *world, Input* input, float dt) {
	// These value are usually the inverse, but because of OpenGL / raylib rendering, the y
	// starts at the top, meaning if we want to jump to go up, we need to minus the screen space
	// coords.
#if 1
	constexpr float gravity    =  1200.0f;
	constexpr float jump_force = -550.0f;
#else
	constexpr float gravity    =  980.0f; 
	constexpr float jump_force = -450.0f;
#endif
	constexpr float movement_speed  = 300.0f;  // Pixels per sec.
	constexpr float coyote_duration = 0.1f;    // 100ms grace.

	if (player->health == 0) return;

	// Reading from the input state.
	player->vel.x = input->player_move_x * movement_speed;

	if (player->is_grounded) player->coyote = coyote_duration;
	else                     player->coyote -= dt;
	bool can_jump = player->coyote > 0.0f;
	
	if (can_jump && input->player_jump) {
		player->vel.y 		= jump_force;
		player->is_grounded = false;
		player->coyote      = 0.0f;

		float pitch = (float)(GetRandomValue(70, 90) / 100.0f);
		play_sound_with_pitch(SOUND_PLAYER_JUMP, pitch);
	}

	if (!player->is_grounded) player->vel.y += gravity * dt;

	// Handling horizontal movement first, then checking its collisions.
	float dx = player->vel.x * dt;
	player->pos.x   += dx;
	check_horizontal_collisions(player, world);

	// Then, handle the verticle stuff.
	float dy = player->vel.y * dt;
	player->pos.y += dy;
	check_vertical_collisions(player, world);

	if (player->is_grounded) player->vel.y = 0.0f;
	if ((player->sprite.y + player->sprite.height) >= 1000) player->health = 0;

	// Handling state changes.
	player->prev_state = player->state;
	PlayerState new_state;
    
	if (player->is_grounded) {
		new_state = (input->player_move_x != 0.0f) ? PLAYER_MOVING  : PLAYER_IDLE;
	} else {
		new_state = (player->vel.y < 0.0f) ? PLAYER_JUMPING : PLAYER_FALLING;
	}

	if (player->health <= 0) new_state = PLAYER_DEAD;
	
	player->state = new_state;

	player->sprite.x = player->pos.x;
	player->sprite.y = player->pos.y;
}

void draw_player(Player *player) {
	DrawRectangleRec(player->sprite, RED);
}
