#include "commands.h"

#include "application.h"

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
		case CMD_TOGGLE_DEBUG_MODE: {
			game->debug_mode = !game->debug_mode;
			printf("Toggling DEBUG_MODE. \n");
			break;
		}
		case CMD_TOGGLE_BIG_CONSOLE: {
			if (game->console.state != CONSOLE_OPEN_BIG) {
				game->console.state = CONSOLE_OPEN_BIG;
				printf("Toggling console state: CONSOLE_OPEN_BIG\n");
			} else {
				game->console.state = CONSOLE_CLOSED;
				printf("Toggling console state: CONSOLE_CLOSED\n");
			}
			break;
		}
		case CMD_TOGGLE_SMALL_CONSOLE: {
			if (game->console.state != CONSOLE_OPEN_SMALL) {
				game->console.state = CONSOLE_OPEN_SMALL;
				printf("Toggling console state: CONSOLE_OPEN_SMALL\n");
			} else {
				game->console.state = CONSOLE_CLOSED;
				printf("Toggling console state: CONSOLE_CLOSED\n");
			}
			break;
		}
		case CMD_TOGGLE_EDITOR_MODE: {
			static GameState prev = game->state;
			if (game->state != GAME_EDITOR) {
				printf("Toggling game state: GAME_EDITOR\n");
				game->state = GAME_EDITOR;
				ShowCursor();
			} else if (game->state == GAME_EDITOR){
				printf("Reverting to previous game state.\n");
				game->state = prev;
				reset_camera(&session->camera, &session->world);
				HideCursor();
			}
			break;
		}

		}
	}
	game->command_count = 0;
}
