#include "player.h"

#include <stdio.h>
#include "world.h"

// @Speed: Currently, we iterate through EVERY TILE that exists within the world, we could change
// this where we only check collisions that are within range of the player, or if the tiles are
// even on the same screen, etc.
static void check_vertical_collisions(Player *player, World *world) {
	player->is_grounded = false;
	for (auto &tile : world->tiles) {
		if (CheckCollisionRecs(player->sprite, tile.rect)) {
			if (player->vel.y > 0) { // Hitting the ground.
				player->pos.y = tile.rect.y - player->sprite.height;
				player->vel.y = 0.0f;
				player->is_grounded = true;
			} else { // Hitting the ceiling.
				player->pos.y = tile.rect.y + tile.rect.height;
				player->vel.y = 0.0f;
			}
			player->sprite.y = player->pos.y;
		}
	}
}

static void check_horizontal_collisions(Player *player, World *world) {
	for (auto &tile : world->tiles) {
		if (CheckCollisionRecs(player->sprite, tile.rect)) {
			if (player->vel.x > 0) { // Moving right.
				player->pos.x = tile.rect.x - player->sprite.width;
			} else { // Moving left.
				player->pos.x = tile.rect.x + tile.rect.width;
			}
			player->sprite.x = player->pos.x;
		}
	}
}

void init_player(Player *player) {
	int center_x = GetScreenWidth() / 2;
	player->pos.x = center_x;
	player->pos.y = GetScreenHeight() * 0.2f; // Just drop them from the top-ish
	player->vel.x = 0.0f;
	player->vel.y = 0.0f;

	player->sprite.x = player->pos.x;
	player->sprite.y = player->pos.y;
	player->sprite.width  = 20;
	player->sprite.height = 20;

	player->state	    = PLAYER_IDLE;
	player->prev_state  = player->state;
	
	player->is_grounded = false; 
}

// @TODO: We bundle up the player input in here, so if we want to, we can pull that out into a
// seperate function and put some of these vars into the player struct. But we also may need a
// seperate file that is dedicated to responsing to inputs since we want to take inputs from a
// lot of places (opening menu, main menu, the different pages within them, then within the game
// world, also in the editor, so on).
void update_player(Player *player, World *world) {
	float dt = GetFrameTime();

	// These value are usually the inverse, but because of OpenGL / raylib rendering, the y
	// starts at the top, meaning if we want to jump to go up, we need to minus the screen space
	// coords.
#if 1
	constexpr float gravity    =  1500.0f;
	constexpr float jump_force = -550.0f;
#else
	constexpr float gravity    =  980.0f; 
	constexpr float jump_force = -450.0f;
#endif
	constexpr float movement_speed  = 300.0f;  // Pixels per sec.
	constexpr float coyote_duration = 0.1f;    // 100ms grace.
	
	float horizontal = 0.0f;
	if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  horizontal = -1.0f;
	if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) horizontal =  1.0f;
	player->vel.x = horizontal * movement_speed;

	if (player->is_grounded) player->coyote = coyote_duration;
	else                     player->coyote -= dt;
	bool can_jump = player->coyote > 0.0f;

	if (can_jump && IsKeyPressed(KEY_SPACE)) {
		player->vel.y 		= jump_force;
		player->is_grounded = false;
		player->coyote      = 0.0f;
	}
	
	if (!player->is_grounded) player->vel.y += gravity * dt;

	// Handling horizontal movement first, then checking its collisions.
	player->pos.x   += player->vel.x * dt;
	player->sprite.x = player->pos.x;
	player->sprite.y = player->pos.y;
	check_horizontal_collisions(player, world);

	// Then, handle the verticle stuff.
	player->pos.y += player->vel.y * dt;
	player->sprite.y = player->pos.y;
	check_vertical_collisions(player, world);

	// Handling state changes.
	player->prev_state = player->state;
	PlayerState new_state;
    if (player->is_grounded) {
        player->vel.y = 0.0f;
		new_state = (horizontal != 0.0f) ? PLAYER_MOVING : PLAYER_IDLE;
	} else {
		new_state = (player->vel.y < 0.0f) ? PLAYER_JUMPING : PLAYER_FALLING;
	}
	
	// if (new_state != player->state)
	player->state = new_state;
}

void draw_player(Player *player) {
	DrawRectangleRec(player->sprite, RED);
}
