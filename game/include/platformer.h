#ifndef PLATFORMER_H
#define PLATFORMER_H

#include "raylib.h"

#include "general.h"
#include "menu.h"
#include "world.h"
#include "player.h"
#include "camera.h"
#include "console.h"
#include "editor.h"

struct Input;

struct GameSession {
	Arena world_arena;

	World world;
	Camera_2D camera;
	Player player;
	Editor editor;
};

struct Game {
	// Arena temp_arena;

	GameState state = GAME_OPENING_MENU;
	GameCommand pending_commands[MAX_COMMAND_COUNT] = {};
	s32 command_count = 0;
	
	GameSession session;

	Menu menu;
	Console console = {};

	bool debug_mode = false;
};

void init_game_session(GameSession* session);
void cleanup_game_session(GameSession *session);

void init_game(Game *game);
void update_game(Game *game, Input *input, float dt);
void draw_game(Game *game);
void cleanup_game(Game *game);

// @Dev: This is just for resetting the game to the first state by re-initting states.
void reset_game_state(Game *game);

#endif
