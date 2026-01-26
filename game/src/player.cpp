#include "player.h"

#include "raylib.h"

void init_player(Player *player) {
	int center_x = GetScreenWidth()  / 2;
	int center_y = GetScreenHeight() / 2;
	
	player->pos.x = center_x;
	player->pos.y = center_y;

	player->sprite.x = player->pos.x;
	player->sprite.y = player->pos.y;
	player->sprite.width  = 20;
	player->sprite.height = 20;
}

// @TODO: Make this code better.
void update_player(Player *player) {
	const int velocity = 5.0f;
	
	if (IsKeyDown(KEY_W)) {
		player->pos.y   -= velocity;
		player->sprite.y = player->pos.y;
	}

	if (IsKeyDown(KEY_S)) {
		player->pos.y   += velocity;
		player->sprite.y = player->pos.y;
	}

	if (IsKeyDown(KEY_A)) {
		player->pos.x   -= velocity;
		player->sprite.x = player->pos.x;
	}

	if (IsKeyDown(KEY_D)) {
		player->pos.x   += velocity;
		player->sprite.x = player->pos.x;
	}
}

void draw_player(Player *player) {
	DrawRectangleRec(player->sprite, RED);
}
