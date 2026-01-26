#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

struct Player {
	Rectangle sprite;
	Vector2 pos;
};

void init_player(Player *player);
void update_player(Player *player);
void draw_player(Player *player);

#endif
