#include "platformer.h"
#include "raylib.h"

#include <stdio.h> 		 // For printing stuff. 

#include "application.h" // For accessing the app state.
#include "input.h"       // Input-handling...
#include "general.h"
#include "editor.h"

static void draw_debug_overlay(Game *game) {
	DrawFPS(0, 0);

	Player *player = &game->player;
	const Font *font = get_font(FONT_CONSOLE);

	const int font_size = 20;
	const int spacing   = 0;
	const int text_x    = 0;
	const int text_y    = 20;
	const int center_x  = (int)g_app->game_width  * 0.5f;
	const int center_y  = (int)g_app->game_height * 0.5f;
	
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
	Vector2 pos = { text_x, text_y };
	DrawTextEx(*font, state_text, pos, font_size, spacing, WHITE);
	
// Drawing grounded state text.
	char grounded_text[32];
	if (player->is_grounded) snprintf(grounded_text, sizeof(grounded_text), "PLAYER: IS GROUNDED");
	else                     snprintf(grounded_text, sizeof(grounded_text), "PLAYER: NOT GROUNDED");
	pos.y = text_y + font_size;
	DrawTextEx(*font, grounded_text, pos, font_size, spacing, WHITE);

	// Drawing player positions based on screen_space coords.
	char screen_space_x[16];
	snprintf(screen_space_x, sizeof(screen_space_x), "PLAYER_X: %2f", player->pos.x);
	pos.y = text_y + (font_size * 2);
	DrawTextEx(*font, screen_space_x, pos, font_size, spacing, WHITE);
	char screen_space_y[16];
	snprintf(screen_space_y, sizeof(screen_space_y), "PLAYER_Y: %2f", player->pos.y);
	pos.y = text_y + (font_size * 3);
	DrawTextEx(*font, screen_space_y, pos, font_size, spacing, WHITE);

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
	pos.y = text_y + (font_size * 4);
	DrawTextEx(*font, game_state, pos, font_size, spacing, WHITE);
	
	char menu_page[24];
	switch(game->menu.current_page) {
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
	pos.y = text_y + (font_size * 5);
	DrawTextEx(*font, menu_page, pos, font_size, spacing, WHITE);
}

static void draw_ui(Game *game) {
	const int font_size = 18;
	const int center_x = g_app->game_width  / 2;
	const int center_y = g_app->game_height / 2;
	
	Player *player = &game->player;
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

	// Drawing actual world space stuff.
	BeginMode2D(game->camera);
	draw_world(&game->world);
	draw_player(&game->player);
	EndMode2D();

	// Drawing UI things using screen space.
	draw_ui(game);
}


void init_game(Game *game) {
	init_camera(&game->camera);

	const u32 screen_count = 10;
	const u32 tiles_wide = g_app->game_width  / 32;
	const u32 tiles_tall = g_app->game_height / 32;

	// The world_arena is sized for tile data + future entity metadata overhead.
	arena_init(&game->world_arena, megabytes(64));
	arena_init(&game->temp_arena, megabytes(16));

#if 0
	init_world(&game->world, &game->world_arena, screen_count, tiles_wide, tiles_tall);
#else
	bool loaded = load_world(&game->world, &game->world_arena);
	if (!loaded) {
		init_world(&game->world, &game->world_arena, screen_count, tiles_wide, tiles_tall);
		save_world(&game->world);
		fprintf(stderr, "[INIT_GAME] No world data loaded. Starting blank world instead.\n");
	}
#endif

	init_player(&game->player);
	init_console(&game->console);
}

void update_game(Game *game, Input *input, float dt) {
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
		if (input->reset_game) push_command_simple(game, CMD_RESET_GAME);

		if (input->pause_game) {
			game->menu.current_main_item = MAIN_START;
			push_command_change_state(game, GAME_MENU);
		}
		
		update_world(&game->world, &game->player, &game->camera, dt);
		update_camera(&game->camera, input);
		update_player(&game->player, &game->world, input, dt);
		break;
	}
	case GAME_EDITOR: {
		// @TODO: Should we still be updating stuff while in editor mode? update_world() has it's
		// own camera system, and we want to be able to use our WASD keys to navigate the world.
		// update_world(&game->world, &game->player, &game->camera, dt);
		// update_player(&game->player, &game->world, input, dt);
		update_camera(&game->camera, input);
		break;
	}
	default: {
		break;
	}
	}
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
		draw_editor_view(game);
		break;
	}
	default: {
		break;
	}
	}

	if (game->debug_mode) {	// @Dev 
		draw_debug_overlay(game);
	}

	draw_console(&game->console);
}

void cleanup_game(Game *game) {
	arena_free(&game->world_arena);
	arena_free(&game->temp_arena);
	
	cleanup_console(&game->console);
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
		case CMD_TOGGLE_DEBUG_MODE: {
			game->debug_mode = !game->debug_mode;
			printf("Toggling debug mode\n");
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
				reset_camera(&game->camera);
				HideCursor();
			}
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
	reset_camera(&game->camera);
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
