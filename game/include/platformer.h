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

	CMD_TOGGLE_DEBUG_MODE,
	CMD_TOGGLE_BIG_CONSOLE,
	CMD_TOGGLE_SMALL_CONSOLE,

	CMD_TOGGLE_EDITOR_MODE,
	
	CMD_COUNT
};

struct GameCommand {
	CommandType type;
	union {
		GameState target_state;
	} data;
};

struct GameSession {
	Arena world_arena;

	World world;
	Camera_2D camera;
	Player player;
	Editor editor;
};

struct Game {
	Arena temp_arena;

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

// Stuff relating to the application / game commands.
void push_command_simple(Game *game, CommandType type);
void push_command_change_state(Game *game, GameState target_state);
void process_command_list(Game *game);

// @Dev: This is just for resetting the game to the first state by re-initting states.
void reset_game_state(Game *game);

#endif
