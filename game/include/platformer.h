#ifndef PLATFORMER_H
#define PLATFORMER_H

#include "raylib.h"

#include "menu.h"
#include "world.h"
#include "player.h"

const int MAX_COMMAND_COUNT = 16;

enum GameState {
	GAME_OPENING_MENU = 0,
	GAME_MENU,
	GAME_WORLD,

	GAME_EDITOR, // @Dev

	GAME_COUNT
};

enum CommandType {
	CMD_NONE = 0,
	CMD_CHANGE_STATE,
	CMD_RESET_GAME,
	CMD_QUIT_GAME,

	CMD_COUNT
};

struct GameCommand {
	CommandType type;
	union {
		GameState target_state;
	} data;
};

struct Game {
	GameState state = GAME_OPENING_MENU;
	GameCommand pending_commands[MAX_COMMAND_COUNT] = {};
	int command_count = 0;

	Menu menu;
	World world;
	Player player;
	Camera2D camera;
};

void init_game(Game *game);
void update_game(Game *game, float dt);
void draw_game(Game *game);

// Stuff relating to the application / game commands.
void push_command_simple(Game *game, CommandType type);
void push_command_change_state(Game *game, GameState target_state);
void process_command_list(Game *game);

// @Dev: This is just for resetting the game to the first state by re-initting states.
void reset_game_state(Game *game);

#endif
