#ifndef COMMANDS_H
#define COMMANDS_H

#include "general.h"

const s32 MAX_COMMAND_COUNT = 16;

struct Game;

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

	// Console-specific commands.
	CMD_CLEAR_LOGS,
	
	CMD_COUNT
};

struct GameCommand {
	CommandType type;
	union {
		GameState target_state;
	} data;
};

// Stuff relating to the application / game commands.
void push_command_simple(Game *game, CommandType type);
void push_command_change_state(Game *game, GameState target_state);
void process_command_list(Game *game);


#endif
