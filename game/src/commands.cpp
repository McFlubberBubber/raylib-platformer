#include "commands.h"

#include "application.h"

DynamicArray<CommandInfo> commands;

// ===== START OF COMMAND LIST. =====

//
// Game state maninpulation. 
//
static void change_game_state(ParseResult *result) {
	String target_state = string_view(result->tokens[1].data, result->tokens[1].length);
	if (string_comp(target_state, string_literal_create("opening_menu"))) {
		push_log("GameState set to: OPENING_MENU", CONSOLE_LOG_OUTPUT);
		push_command_change_state(&g_app->game, GAME_OPENING_MENU);		
	} else if (string_comp(target_state, string_literal_create("menu"))) {
		push_log("GameState set to: MENU", CONSOLE_LOG_OUTPUT);
		push_command_change_state(&g_app->game, GAME_MENU);		
	} else if (string_comp(target_state, string_literal_create("world"))) {
		push_log("GameState set to: WORLD", CONSOLE_LOG_OUTPUT);
		push_command_change_state(&g_app->game, GAME_WORLD);		
	} else {
		push_log("ERROR :: Expected usage: game_state <state>", CONSOLE_LOG_ERROR);
	}
}
static void reset_game(ParseResult *result) {
	if (result->count != 1) {
		push_log("ERROR ::  Received additional arguments for a command that does not have any parameters! Expected usage: reset_game", CONSOLE_LOG_ERROR);
		return;
	}
	
	push_log("Resetting game...", CONSOLE_LOG_OUTPUT);
	push_command_simple(&g_app->game, CMD_RESET_GAME);
}
static void quit_game(ParseResult *result) {
	if (result->count != 1) {
		push_log("ERROR ::  Received additional arguments for a command that does not have any parameters! Expected usage: quit", CONSOLE_LOG_ERROR);
		return;
	}
	
	push_log("Quitting game...", CONSOLE_LOG_OUTPUT);
	push_command_simple(&g_app->game, CMD_QUIT_GAME);
}

//
// Toggling developer utilities.  
// 
static void toggle_debug(ParseResult *result) {
	if (result->count != 1) {
		push_log("ERROR ::  Received additional arguments for a command that does not have any parameters! Expected usage: debug", CONSOLE_LOG_ERROR);
		return;
	}

}

static void toggle_console(ParseResult *result) {
	if (result->count != 2) {
		push_log("ERROR :: Passed invalid number of arguments to toggle_console. Expected usage: console <state>. Type 'help console' to see more info.", CONSOLE_LOG_ERROR);
		return;
	}

	String target_state = string_view(result->tokens[1].data, result->tokens[1].length);
	if (string_comp(target_state, string_literal_create("small"))) {
		push_command_simple(&g_app->game, CMD_TOGGLE_SMALL_CONSOLE);
	} else if (string_comp(target_state, string_literal_create("big"))) {
		push_command_simple(&g_app->game, CMD_TOGGLE_BIG_CONSOLE);
	} else {
		push_log("ERROR :: Passed invalid arguments to toggle_console. Expected usage: console <state>. Type 'help console' to see more info.", CONSOLE_LOG_ERROR);
	}
}

static void toggle_editor(ParseResult *result) {
	if (result->count != 1) {
		push_log("ERROR ::  Received additional arguments for a command that does not have any parameters! Expected usage: editor", CONSOLE_LOG_ERROR);
		return;
	}

	push_log("Toggling editor mode", CONSOLE_LOG_OUTPUT);
	push_command_simple(&g_app->game, CMD_TOGGLE_EDITOR_MODE);
}

// 
// Console-related commands.
//
static void clear_console_logs(ParseResult *result) {
	if (result->count != 1) {
		push_log("ERROR :: Received additional arguments for a command that does not have any parameters! Expected usage: cls", CONSOLE_LOG_ERROR);
		return;
	}
	push_command_simple(&g_app->game, CMD_CLEAR_CONSOLE_LOGS);
}

static void clear_console_history(ParseResult *result) {
	if (result->count != 1) {
		push_log("ERROR :: Received additional arguments for a command that does not have any parameters! Expected usage: clear_history", CONSOLE_LOG_ERROR);
		return;
	}
	push_log("Cleared command history.", CONSOLE_LOG_OUTPUT);
	push_command_simple(&g_app->game, CMD_CLEAR_CONSOLE_HISTORY);
}

static void request_help(ParseResult *result) {
	if (result->count == 1) {
		push_log("===== 'help' =====", CONSOLE_LOG_INFO);
		push_log("This command will provide the expected usage of each command. You can simply input the following to find out more details of each command:", CONSOLE_LOG_INFO);
		push_log("Usage: help <command>", CONSOLE_LOG_INFO);
		push_log("To see a list of valid commands, type 'help commands'", CONSOLE_LOG_INFO);
		push_log("=====================", CONSOLE_LOG_INFO);
		return;
	}

	String second = result->tokens[1];
	if (string_comp(second, string_literal_create("console"))) {
		push_log("===== 'console' =====", CONSOLE_LOG_INFO);
		push_log("This command allows toggling the different states of the console within the console itself, which is kinda useless, but it is a command you can dispatch to!", CONSOLE_LOG_INFO);
		push_log("Usage: console <state>", CONSOLE_LOG_INFO);
		push_log("List of states: small, big", CONSOLE_LOG_INFO);
		push_log("=====================", CONSOLE_LOG_INFO);
		return;
	}
}

// ===== END OF COMMAND LIST. =====

void init_commands() {
	dynamic_array_init(&commands);
	
	add_command("game_state", change_game_state);
	add_command("reset_game", reset_game);
	add_command("quit", quit_game);

	add_command("debug", toggle_debug);
	add_command("console", toggle_console);
	add_command("editor", toggle_editor);

	add_command("cls", clear_console_logs);
	add_command("clear_history", clear_console_history);
	add_command("help", request_help);
	
}
void cleanup_commands() {
	dynamic_array_free(&commands);
}

void add_command(const char *name, void (*proc)(ParseResult *)) {
	Arena *arena = &g_app->game.console.arena;
	
	CommandInfo cmd = {};
	cmd.name = string_create(arena, name); // We just load commands into the console arena for now.
	cmd.proc = proc;
	dynamic_array_add(&commands, cmd);
}

// cmd = full string of the inputted command, *result are the tokens.
void run_command(String cmd, ParseResult *result) {
	bool found = false;
	for (u32 i = 0; i < commands.count; ++i) {
		auto command = dynamic_array_get_at_index(&commands, i);
		if (string_comp(command->name, result->tokens[0])) {
			command->proc(result);
			found = true;
		}
	}

	if (!found) {
		#if 0
		
		StringBuilder sb = {};
		Arena *scratch = get_current_arena_frame();
		if (result->count == 1) {
			strbuild_append_string(scratch, &sb, result->tokens[0]);
		} else {
			for (s32 i = 0; i < result->count; ++i) {
				strbuild_append_char(scratch, &sb, ' ');
				strbuild_append_string(scratch, &sb, result->tokens[i]);
			}
		}
		strbuild_append_cstring(scratch, &sb, ": Unknown command.");
		String message = strbuild_terminate(scratch, &sb);
		push_log(string_to_cstr(message), CONSOLE_LOG_ERROR);
		
		#else
		
		StringBuilder sb = {};
		Arena *scratch = get_current_arena_frame();
		strbuild_append_string(scratch, &sb, cmd);
		strbuild_append_cstring(scratch, &sb, ": Unknown command.");
		const char *message = string_to_cstr(strbuild_terminate(scratch, &sb));
		push_log(message, CONSOLE_LOG_ERROR);
		
		#endif
	}
}

void push_command_simple(Game *game, CommandType type) {
	if (game->command_count >= MAX_COMMAND_COUNT) {
#ifdef DEBUG
		fprintf(stderr, "CMD_ERROR: Command queue is full.\n");
		assert(false);
#endif
		return;
	}

	game->pending_commands[game->command_count].type = type;
	game->command_count++;
}

void push_command_change_state(Game *game, GameState target_state) {
	if (game->command_count >= MAX_COMMAND_COUNT) {
#ifdef DEBUG
		fprintf(stderr, "CMD_ERROR: Command queue is full.\n");
		assert(false);
#endif
		return;
	}

	game->pending_commands[game->command_count].type = CMD_CHANGE_STATE;
	game->pending_commands[game->command_count].data.target_state = target_state;
	game->command_count++;
}

void process_command_list(Game *game) {
	GameSession *session = &game->session;

	for (s32 i = 0; i < game->command_count; ++i) {
		auto &cmd = game->pending_commands[i];
		switch (cmd.type) {
		case CMD_NONE: {
			break;
		}
		
		//
		// Game state manipulation.
		//
		case CMD_CHANGE_STATE: {
			game->state = cmd.data.target_state;
			break;
		}
		case CMD_RESET_GAME: {
			reset_game_state(game);
			break;
		}
		case CMD_QUIT_GAME: {
			g_app->should_close = true;
			break;
		}

		//
		// Developer utilities.
		//
		case CMD_TOGGLE_DEBUG_MODE: {
			game->debug_mode = !game->debug_mode;
			break;
		}
		case CMD_TOGGLE_BIG_CONSOLE: {
			if (game->console.state != CONSOLE_OPEN_BIG) {
				game->console.state = CONSOLE_OPEN_BIG;
			} else {
				game->console.state = CONSOLE_CLOSED;
			}
			break;
		}
		case CMD_TOGGLE_SMALL_CONSOLE: {
			if (game->console.state != CONSOLE_OPEN_SMALL) {
				game->console.state = CONSOLE_OPEN_SMALL;
			} else {
				game->console.state = CONSOLE_CLOSED;
			}
			break;
		}
		case CMD_TOGGLE_EDITOR_MODE: {
			static GameState prev = game->state;
			if (game->state != GAME_EDITOR) {
				game->state = GAME_EDITOR;
				ShowCursor();
			} else if (game->state == GAME_EDITOR){
				game->state = prev;
				reset_camera(&session->camera, &session->world);
				HideCursor();
			}
			break;
		}

		//
		// Console-related commands.
		//
		case CMD_CLEAR_CONSOLE_LOGS: {
			Console *console = &g_app->game.console;
			memset(&console->log_buffer, 0, sizeof(ConsoleLogBuffer));
			break;
		}
		case CMD_CLEAR_CONSOLE_HISTORY: {
			array_reset(&game->console.history);
			break;
		}
			
		}
	}
	game->command_count = 0;
}
