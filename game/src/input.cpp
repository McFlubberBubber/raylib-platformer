#include "input.h"

#include "application.h"

static void handle_global_inputs(Application *app) {
	Game *game = &app->game;

	if (IsKeyPressed(KEY_F1)) {
		push_command_simple(game, CMD_TOGGLE_DEBUG_MODE);
	}
	
	// Console stuff.
	if (IsKeyPressed(KEY_GRAVE)) { // Tilda key.
		if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
			push_command_simple(game, CMD_TOGGLE_BIG_CONSOLE);
		} else {
			push_command_simple(game, CMD_TOGGLE_SMALL_CONSOLE);
		}
	}

	// Editor stuff.
	if (IsKeyPressed(KEY_BACKSPACE)) {
		push_command_simple(game, CMD_TOGGLE_EDITOR_MODE);
	}
}

static void poll_console_inputs(Game *game) {
	Console *console = &game->console;
	bool ctrl_held = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

	int character;
	while ((character = GetCharPressed()) != 0)  {
		insert_character(&game->console, character);
	}
	
	if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        ctrl_held ? delete_word(console) : delete_character(console);
    }

	// These arrow key inputs are for cursor navigations, where false = backwards and
	// true = forwards.
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        ctrl_held ? move_cursor_by_word(console, false) : move_cursor_by_char(console, false);
        console->input.cursor_blink_time = 0.0f; // @TODO: Reset cursor blink time automatically.
    }

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        ctrl_held ? move_cursor_by_word(console, true) : move_cursor_by_char(console, true);
        console->input.cursor_blink_time = 0.0f; // @TODO: Same here.
    }

    if (IsKeyPressed(KEY_ENTER)) {
        submit_command(console);
    }
}

static void poll_menu_inputs(Game *game) {
	Menu *menu = &game->menu;

	int key;
	bool go_forward;
	while ((key = GetKeyPressed()) != 0) {
		switch (key) {
		// Arrow keys.
		case KEY_UP: {
			navigate_menu_item(menu, go_forward = false);
			break;
		}
		case KEY_DOWN: {
			navigate_menu_item(menu, go_forward = true);
			break;
		}
		case KEY_RIGHT: {
			cycle_menu_item(menu, go_forward = true);
			break;
		}
		case KEY_LEFT: {
			cycle_menu_item(menu, go_forward = false);
			break;
		}

		// Enter and escape interactions.
		case KEY_ENTER: {
			handle_menu_activations(menu);
			break;
		}
		case KEY_ESCAPE: {
			handle_menu_escape_press(menu);
			break;
		}

		default: {
			break;
		}
		}
	}
}

void poll_inputs(Application *app) {
	Input *input = &app->input;
	handle_global_inputs(app);

	if (app->game.console.state != CONSOLE_CLOSED) {
		poll_console_inputs(&app->game);
		return;
	}

	if (app->game.state == GAME_OPENING_MENU || app->game.state == GAME_MENU) {
		poll_menu_inputs(&app->game);
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

	// @TODO: Editor inputs?
}
