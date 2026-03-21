#include "input.h"

#include "application.h"

static void handle_global_inputs(Application *app) {
	Game *game = &app->game;

	if (IsKeyPressed(KEY_F1)) {
		push_command_simple(game, CMD_TOGGLE_DEBUG_MODE);
	}
	
	// Console stuff
	if (IsKeyPressed(KEY_GRAVE)) { // Tilda key.
		if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
			push_command_simple(game, CMD_TOGGLE_BIG_CONSOLE);
		} else {
			push_command_simple(game, CMD_TOGGLE_SMALL_CONSOLE);
		}
	}
}

static void poll_console_inputs(Game *game) {
	int character;
	while ((character = GetCharPressed()) != 0) {
		insert_character(&game->console, character);
	}

	if (IsKeyPressed(KEY_BACKSPACE)) {
		delete_character(&game->console);
	}

	if (IsKeyPressed(KEY_ENTER)) {
		// submit_command(&game->console);
	}
}

void poll_inputs(Application *app) {
	Input *input = &app->input;
	handle_global_inputs(app);

	if (app->game.console.state != CONSOLE_CLOSED) {
		poll_console_inputs(&app->game);
		return;
	}

	// General game inputs.
	input->pause_game = IsKeyPressed(KEY_ESCAPE);
	input->reset_game = IsKeyPressed(KEY_R);

	// Player-specific inputs.
	input->player_move_x = 0.0f;
	
	if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  input->player_move_x = -1.0f;
	if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) input->player_move_x =  1.0f;
	
	input->player_jump = IsKeyPressed(KEY_SPACE);

	// @TODO: Console + Editor inputs?
}
