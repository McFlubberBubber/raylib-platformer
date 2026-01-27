#include "player.h"

#include <stdio.h>

void init_player(Player *player) {
	int center_x = GetScreenWidth() / 2;
	float ground_y = GetScreenHeight() * 0.7f; // @Temp
	player->pos.x = center_x;
	player->pos.y = ground_y;

	player->sprite.x = player->pos.x;
	player->sprite.y = player->pos.y;
	player->sprite.width  = 20;
	player->sprite.height = 20;

	player->state	    = PLAYER_IDLE;
	player->is_grounded = true; 

	player->current_velocity = 0.0f;
}

void update_player(Player *player) {
	float dt = GetFrameTime();
	constexpr float gravity        =  980.0f;  // Positive to move down
	constexpr float jump_force     = -400.0f;  // Negative to move upwards
	constexpr float movement_speed =  300.0f;  // Pixels per sec.
	float ground_y = GetScreenHeight() * 0.7f; // @Temp
	
	float horizontal = 0.0f;
	if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  horizontal = -1.0f;
	if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) horizontal =  1.0f;
	player->pos.x   += horizontal * movement_speed * dt;

	if (player->is_grounded && IsKeyPressed(KEY_SPACE)) {
		player->current_velocity = jump_force;
		player->is_grounded = false;
		player->state = PLAYER_JUMPING;
	}
	if (!player->is_grounded) {
		player->current_velocity += gravity * dt;
	}

	player->pos.y += player->current_velocity * dt;

	// @Temp: Collision check but it's not really there.
	if (player->pos.y >= ground_y) {
		player->pos.y		     = ground_y;
		player->current_velocity = 0.0f;
		player->is_grounded      = true;

		if (horizontal != 0.0f) player->state = PLAYER_MOVING;
		else				    player->state = PLAYER_IDLE;
		
	} else {
		// We are in the air in this branch, so determine the player state.
		if (player->current_velocity < 0.0f) player->state = PLAYER_JUMPING;
		else							     player->state = PLAYER_FALLING;
	}

	// @Temp: making sure the player's rect doesn't go outside the screen space.
	const int left_wall  = 0;
	const int right_wall = GetScreenWidth();
	if (player->pos.x <= left_wall) {
		player->pos.x = left_wall;
	}

	if ((player->pos.x + player->sprite.width) >= right_wall) {
		player->pos.x = right_wall - player->sprite.width;
	}
}

void draw_player(Player *player) {
	player->sprite.x = player->pos.x;
	player->sprite.y = player->pos.y;
	DrawRectangleRec(player->sprite, RED);

	// This is all just debug overlay stuff, so in release mode we can get this out.
	constexpr int font_size = 18;
	constexpr int text_x    = 0;
	constexpr int text_y    = 20;
	char state_text[64];
	switch(player->state) {
	case PLAYER_IDLE: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: IDLE");
		break;
	}
	case PLAYER_MOVING: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: MOVING");
		break;
	}
	case PLAYER_JUMPING: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: JUMPING");
		break;
	}
	case PLAYER_FALLING: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: FALLING");
		break;
	}

	default: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: ERROR");
		break;
	}
	}
	
	DrawText(state_text, text_x, text_y, font_size, WHITE); // (0, 0) is the top-left.

	// Drawing grounded state text.
	char grounded_text[32];
	if (player->is_grounded) snprintf(grounded_text, sizeof(grounded_text), "PLAYER: IS GROUNDED");
	else                     snprintf(grounded_text, sizeof(grounded_text), "PLAYER: NOT GROUNDED");
	DrawText(grounded_text, text_x, (text_y + font_size), font_size, WHITE);

	// Drawing player positions based on screen_space coords.
	char screen_space_x[16];
	snprintf(screen_space_x, sizeof(screen_space_x), "PLAYER_X: %2f", player->pos.x);
	DrawText(screen_space_x, text_x, (text_y + (font_size * 2)), font_size, WHITE);
	char screen_space_y[16];
	snprintf(screen_space_y, sizeof(screen_space_y), "PLAYER_Y: %2f", player->pos.y);
	DrawText(screen_space_y, text_x, (text_y + (font_size * 3)), font_size, WHITE);
}
