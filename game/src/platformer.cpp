#include "platformer.h"
#include "raylib.h"

#include <stdio.h> 		 // For printing stuff. 

#include "application.h" // For accessing the app state.
#include "input.h"       // Input-handling...
#include "general.h"
#include "editor.h"

static void draw_debug_overlay(Game *game) {
	DrawFPS(0, 0);

	Player *player = &game->session.player;
	const Font *font = get_font(FONT_CONSOLE);

	const int font_size = 30;
	const int spacing   = 0;
	const int text_x    = 0;
	const int text_y    = 20;
	const int center_x  = (int)g_app->game_width  * 0.5f;
	const int center_y  = (int)g_app->game_height * 0.5f;

	StringBuilder builder = {0};
	String final_str;
	strbuild_append_cstring(&game->temp_arena, &builder, "PLAYER_STATE: ");
	switch(player->state) {
	case PLAYER_IDLE: {
		strbuild_append_cstring(&game->temp_arena, &builder, "IDLE");
		break;
	}
	case PLAYER_MOVING: {
		strbuild_append_cstring(&game->temp_arena, &builder, "MOVING");
		break;
	}
	case PLAYER_JUMPING: {
		strbuild_append_cstring(&game->temp_arena, &builder, "JUMPING");
		break;
	}
	case PLAYER_FALLING: {
		strbuild_append_cstring(&game->temp_arena, &builder, "FALLING");
		break;
	}
	case PLAYER_HURT: {
		strbuild_append_cstring(&game->temp_arena, &builder, "HURT");
		break;
	}
	case PLAYER_DEAD: {
		strbuild_append_cstring(&game->temp_arena, &builder, "DEAD");
		break;
	}

	default: {
		strbuild_append_cstring(&game->temp_arena, &builder, "ERROR");
		break;
	}
	}

	Vector2 pos = { text_x, text_y };
	final_str = strbuild_terminate(&game->temp_arena, &builder);
	draw_text_ex_with_string(font, final_str, pos, font_size, spacing, WHITE);
	strbuild_reset(&builder);
	
	// Drawing if grounded / not grounded on the player.
	strbuild_append_cstring(&game->temp_arena, &builder, player->is_grounded ? "PLAYER: IS_GROUNDED" : "PLAYER: NOT_GROUNDED");
	final_str = strbuild_terminate(&game->temp_arena, &builder);
	pos.y = text_y + font_size;		
	draw_text_ex_with_string(font, final_str, pos, font_size, spacing, WHITE);
	strbuild_reset(&builder);
	
	// Drawing player positions.
	String player_pos_x = string_format(&game->temp_arena, "PLAYER_X: %2f", player->sprite.x); 
	pos.y = text_y + (font_size * 2);
	draw_text_ex_with_string(font, player_pos_x, pos, font_size, spacing, WHITE);
	String player_pos_y = string_format(&game->temp_arena, "PLAYER_Y: %2f", player->sprite.y); 
	pos.y = text_y + (font_size * 3);
	draw_text_ex_with_string(font, player_pos_y, pos, font_size, spacing, WHITE);

	// Drawing game state.
	strbuild_append_cstring(&game->temp_arena, &builder, "GAME_STATE: ");
	switch(game->state) {
	case GAME_OPENING_MENU: {
		strbuild_append_cstring(&game->temp_arena, &builder, "OPENING_MENU");
		break;
	}
	case GAME_MENU: {
		strbuild_append_cstring(&game->temp_arena, &builder, "MENU");
		break;
	}
	case GAME_WORLD: {
		strbuild_append_cstring(&game->temp_arena, &builder, "WORLD");
		break;
	}
	case GAME_EDITOR: {
		strbuild_append_cstring(&game->temp_arena, &builder, "EDITOR");
		break;
	}
	default: {
		strbuild_append_cstring(&game->temp_arena, &builder, "ERROR");
		break;
	}
	}
	pos.y = text_y + (font_size * 4);
	final_str = strbuild_terminate(&game->temp_arena, &builder);
	draw_text_ex_with_string(font, final_str, pos, font_size, spacing, WHITE);
	strbuild_reset(&builder);
	
	strbuild_append_cstring(&game->temp_arena, &builder, "MENU_PAGE: ");
	switch(game->menu.current_page) {
	case PAGE_MAIN: {
		strbuild_append_cstring(&game->temp_arena, &builder, "MAIN");
		break;
	}
	case PAGE_SETTINGS: {
		strbuild_append_cstring(&game->temp_arena, &builder, "SETTINGS");
		break;
	}
	case PAGE_CONTROLS: {
		strbuild_append_cstring(&game->temp_arena, &builder, "CONTROLS");
		break;
	}
	default: {
		strbuild_append_cstring(&game->temp_arena, &builder, "ERROR");
		break;
	}
	}
	pos.y = text_y + (font_size * 5);
	final_str = strbuild_terminate(&game->temp_arena, &builder);
	draw_text_ex_with_string(font, final_str, pos, font_size, spacing, WHITE);
	strbuild_reset(&builder);
}

static void draw_ui(Game *game) {
	const int font_size = 18;
	const int center_x = g_app->game_width  / 2;
	const int center_y = g_app->game_height / 2;
	
	Player *player = &game->session.player;
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
	GameSession *session = &game->session;

	ClearBackground(BLACK);

	// Drawing actual world space stuff.
	BeginMode2D(session->camera.raylib_cam);
	draw_world(&session->world, false); // Don't draw all screens.
	draw_player(&session->player);
	
	EndMode2D();

	// Drawing UI things using screen space.
	draw_ui(game);
}

void init_game_session(GameSession *session) {
	arena_init(&session->world_arena, megabytes(128));

	bool loaded = load_world(&session->world, &session->world_arena);
	if (!loaded) {
		fprintf(stderr, "[INIT_GAME] No world data loaded. Starting placeholder world instead.\n");
		load_placeholder_world(&session->world, &session->world_arena);
	}

	init_player(&session->player, &session->world);
	init_camera(&session->camera, &session->world);

	session->editor.mode = EDITOR_VIEW;
}

void init_game(Game *game) {
	arena_init(&game->temp_arena, megabytes(16));

	init_console(&game->console);
	init_game_session(&game->session);
}

void update_game(Game *game, Input *input, float dt) {
	GameSession *session = &game->session;
	const Vector2 player_center = {session->player.sprite.x + session->player.sprite.width * 0.5f,session->player.sprite.y + session->player.sprite.height * 0.5f};

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
		
		update_world(&session->world, player_center);
		update_camera(&session->camera, &session->world, player_center, input);
		update_player(&session->player, &session->world, input, dt);
		break;
	}
	case GAME_EDITOR: {
		update_camera(&session->camera, &session->world, player_center, input);
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
	arena_free(&game->session.world_arena);
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
	GameSession *session = &game->session;

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

void reset_game_state(Game *game) {
	init_player(&game->session.player, &game->session.world);
	reset_camera(&game->session.camera, &game->session.world);
}
