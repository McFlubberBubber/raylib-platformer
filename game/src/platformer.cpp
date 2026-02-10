#include "platformer.h"
#include "raylib.h"

#include <stdio.h> 		 // For printing stuff. 
#include "application.h" // For accessing the app state.

static void draw_debug_overlay(Game *game) {
	// Application *app = Application::instance;

	DrawFPS(0, 0);

	auto player = &game->player;
	constexpr int font_size = 18;
	constexpr int text_x    = 0;
	constexpr int text_y    = 20;
	const int center_x      = g_app->game_width  / 2;
	const int center_y      = g_app->game_height / 2;
	
	char state_text[32];
	switch(player->state) {
	case PLAYER_IDLE: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: IDLE");
		break;
	}
	case PLAYER_MOVING: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: MOVING");
		break;
	}
	case PLAYER_JUMPING: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: JUMPING");
		break;
	}
	case PLAYER_FALLING: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: FALLING");
		break;
	}
	case PLAYER_HURT: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: HURT");
		break;
	}
	case PLAYER_DEAD: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: DEAD");
		break;
	}

	default: {
		snprintf(state_text, sizeof(state_text), "PLAYER_STATE: ERROR");
		break;
	}
	}
	DrawText(state_text, text_x, text_y, font_size, WHITE); // (0, 0) is the top-left.

	// Drawing grounded state text.
	char grounded_text[32];
	if (player->is_grounded) snprintf(grounded_text, sizeof(grounded_text), "PLAYER: IS GROUNDED");
	else                     snprintf(grounded_text, sizeof(grounded_text), "PLAYER: NOT GROUNDED");
	DrawText(grounded_text, text_x, (text_y + font_size), font_size, WHITE);

	// Drawing player positions based on screen_space coords.
	char screen_space_x[16];
	snprintf(screen_space_x, sizeof(screen_space_x), "PLAYER_X: %2f", player->pos.x);
	DrawText(screen_space_x, text_x, (text_y + (font_size * 2)), font_size, WHITE);
	char screen_space_y[16];
	snprintf(screen_space_y, sizeof(screen_space_y), "PLAYER_Y: %2f", player->pos.y);
	DrawText(screen_space_y, text_x, (text_y + (font_size * 3)), font_size, WHITE);

	char game_state[32];
	switch(game->state) {
	case GAME_OPENING_MENU: {
		snprintf(game_state, sizeof(game_state), "GAME_STATE: OPENING_MENU");
		break;
	}
	case GAME_MENU: {
		snprintf(game_state, sizeof(game_state), "GAME_STATE: MENU");
		break;
	}
	case GAME_WORLD: {
		snprintf(game_state, sizeof(game_state), "GAME_STATE: WORLD");
		break;
	}
	case GAME_EDITOR: {
		snprintf(game_state, sizeof(game_state), "GAME_STATE: EDITOR");
		break;
	}
	default: {
		snprintf(game_state, sizeof(game_state), "GAME_STATE: ERROR");
		break;
	}
	}
	DrawText(game_state, text_x, (text_y + (font_size * 4)), font_size, WHITE);

	char menu_page[24];
	switch(game->menu.current_menu_page) {
	case PAGE_MAIN: {
		snprintf(menu_page, sizeof(menu_page), "MENU_PAGE: MAIN");
		break;
	}
	case PAGE_SETTINGS: {
		snprintf(menu_page, sizeof(menu_page), "MENU_PAGE: SETTINGS");
		break;
	}
	case PAGE_CONTROLS: {
		snprintf(menu_page, sizeof(menu_page), "MENU_PAGE: CONTROLS");
		break;
	}
	default: {
		snprintf(menu_page, sizeof(menu_page), "MENU_PAGE: ERROR");
		break;
	}
	}
	DrawText(menu_page, text_x, (text_y + (font_size * 5)), font_size, WHITE);


	// Drawing death screen. @TODO: This is NOT debug overlay, this is genuine UI.
	if (player->health == 0) {
		const char *death = "YOU ARE DEAD.";
		int width = MeasureText(death, font_size * 2);
		int x = center_x - (width / 2);
		int y = center_y * 0.4f;
		DrawText(death, x, y, (font_size * 2), RED);
		
		const char *reset = "Press R to respawn.";
		width = MeasureText(reset, font_size);
		x = center_x - (width / 2);
		DrawText(reset, x, y + (font_size * 2), font_size, WHITE);
	}
}

static void draw_game_environment(Game *game) {
	ClearBackground(BLACK);

	// update_camera(game);

	// Drawing actual world space stuff.
	BeginMode2D(game->camera);
	draw_world(&game->world);
	draw_player(&game->player);
	EndMode2D();

	// Drawing UI things using screen space.
	// draw_ui(game);
}

static void init_camera(Camera2D *camera) {
	// Application *app = Application::instance;
	// Camera offset (displacement from target)
	camera->target.x = 0; 
	camera->target.y = 0;

	// Camera target (rotation and zoom origin)
    camera->offset.x = g_app->game_width  / 2;
    camera->offset.y = g_app->game_height / 2;

    camera->rotation = 0;
    camera->zoom     = 1.0f;
}

void init_game(Game *game) {
	init_camera(&game->camera);
	init_world(&game->world);
	init_player(&game->player);
}

void update_game(Game *game) {
	game->command_count = 0;

	// Handle global inputs (toggling debug overlay, console, special dev hotkeys?).
	// handle_global_inputs(game);
	
	switch (game->state) {
	case GAME_OPENING_MENU: {
		update_menu(&game->menu, game);
		break;
	}
	case GAME_MENU: {
		update_menu(&game->menu, game);
		break;
	}
	case GAME_WORLD: {
		// @TODO: Should these inputs get moved to a process_game_input()? We already track inputs
		// for player movement through the update_player(), but other inputs like maybe
		// inventory, pausing the game, and so on, could be moved here...
		if (IsKeyPressed(KEY_R)) push_command_simple(game, CMD_RESET_GAME);

		if (IsKeyPressed(KEY_ESCAPE)) {
			game->menu.current_main_item = MAIN_START;
			push_command_change_state(game, GAME_MENU);
		}
		
		update_world(&game->world, &game->player, &game->camera);
		update_player(&game->player, &game->world);
		break;
	}
	case GAME_EDITOR: {
		break;
	}
	default: {
		break;
	}
	}

	process_command_list(game);
};

void draw_game(Game *game) {
	switch(game->state) {
	case GAME_OPENING_MENU: {
		draw_opening_menu(&game->menu);
		break;
	}
	case GAME_MENU: {
		draw_game_environment(game);
		draw_in_game_menu(&game->menu);
		break;
	}
	case GAME_WORLD: {
		draw_game_environment(game);
		break;
	}
	case GAME_EDITOR: {
		// @TODO: Handle editor stuff...
		break;
	}
	default: {
		break;
	}
		
	}

	draw_debug_overlay(game);
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
	for (int i = 0; i < game->command_count; ++i) {
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
		}
	}
	game->command_count = 0;
}


// @Dev
void reset_game_state(Game *game) {
#if 1
	init_player(&game->player);
#else
	auto player = &game->player;
	player->pos.x = 0;
	player->pos.y = 0;
	
	player->vel.x = 0.0f;
	player->vel.y = 0.0f;
	
	player->sprite.x = player->pos.x;
	player->sprite.y = player->pos.y;
	player->sprite.width  = 20;
	player->sprite.height = 20;

	player->state	    = PLAYER_IDLE;
	player->prev_state  = player->state;
	player->is_grounded = false;
	player->health = 3;
#endif
}
