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
	Arena *arena = get_current_arena_frame();

	const Font *font = get_font(FONT_CONSOLE);
	const s32 font_size = 24;
	const s32 spacing   = 0;
	const s32 text_x    = 0;
	const s32 text_y    = 20;
	const s32 center_x  = (s32)g_app->game_width  * 0.5f;
	const s32 center_y  = (s32)g_app->game_height * 0.5f;

	StringBuilder builder = {0};
	String final_str;
	strbuild_append_cstring(arena, &builder, "PLAYER_STATE: ");
	switch(player->state) {
	case PLAYER_IDLE: {
		strbuild_append_cstring(arena, &builder, "IDLE");
		break;
	}
	case PLAYER_MOVING: {
		strbuild_append_cstring(arena, &builder, "MOVING");
		break;
	}
	case PLAYER_JUMPING: {
		strbuild_append_cstring(arena, &builder, "JUMPING");
		break;
	}
	case PLAYER_FALLING: {
		strbuild_append_cstring(arena, &builder, "FALLING");
		break;
	}
	case PLAYER_HURT: {
		strbuild_append_cstring(arena, &builder, "HURT");
		break;
	}
	case PLAYER_DEAD: {
		strbuild_append_cstring(arena, &builder, "DEAD");
		break;
	}

	default: {
		strbuild_append_cstring(arena, &builder, "ERROR");
		break;
	}
	}

	Vector2 pos = { text_x, text_y };
	final_str = strbuild_terminate(arena, &builder);
	draw_text_ex_with_string(font, final_str, pos, font_size, spacing, WHITE);
	strbuild_reset(&builder);
	
	// Drawing if grounded / not grounded on the player.
	strbuild_append_cstring(arena, &builder, player->is_grounded ? "PLAYER: IS_GROUNDED" : "PLAYER: NOT_GROUNDED");
	final_str = strbuild_terminate(arena, &builder);
	pos.y = text_y + font_size;		
	draw_text_ex_with_string(font, final_str, pos, font_size, spacing, WHITE);
	strbuild_reset(&builder);
	
	// Drawing player positions.
	String player_pos_x = string_format(arena, "PLAYER_X: %2f", player->sprite.x); 
	pos.y = text_y + (font_size * 2);
	draw_text_ex_with_string(font, player_pos_x, pos, font_size, spacing, WHITE);
	String player_pos_y = string_format(arena, "PLAYER_Y: %2f", player->sprite.y); 
	pos.y = text_y + (font_size * 3);
	draw_text_ex_with_string(font, player_pos_y, pos, font_size, spacing, WHITE);

	// Drawing game state.
	strbuild_append_cstring(arena, &builder, "GAME_STATE: ");
	switch(game->state) {
	case GAME_OPENING_MENU: {
		strbuild_append_cstring(arena, &builder, "OPENING_MENU");
		break;
	}
	case GAME_MENU: {
		strbuild_append_cstring(arena, &builder, "MENU");
		break;
	}
	case GAME_WORLD: {
		strbuild_append_cstring(arena, &builder, "WORLD");
		break;
	}
	case GAME_EDITOR: {
		strbuild_append_cstring(arena, &builder, "EDITOR");
		break;
	}
	default: {
		strbuild_append_cstring(arena, &builder, "ERROR");
		break;
	}
	}
	pos.y = text_y + (font_size * 4);
	final_str = strbuild_terminate(arena, &builder);
	draw_text_ex_with_string(font, final_str, pos, font_size, spacing, WHITE);
	strbuild_reset(&builder);
	
	strbuild_append_cstring(arena, &builder, "MENU_PAGE: ");
	switch(game->menu.current_page) {
	case PAGE_MAIN: {
		strbuild_append_cstring(arena, &builder, "MAIN");
		break;
	}
	case PAGE_SETTINGS: {
		strbuild_append_cstring(arena, &builder, "SETTINGS");
		break;
	}
	case PAGE_CONTROLS: {
		strbuild_append_cstring(arena, &builder, "CONTROLS");
		break;
	}
	default: {
		strbuild_append_cstring(arena, &builder, "ERROR");
		break;
	}
	}
	pos.y = text_y + (font_size * 5);
	final_str = strbuild_terminate(arena, &builder);
	draw_text_ex_with_string(font, final_str, pos, font_size, spacing, WHITE);
	strbuild_reset(&builder);
}

static void draw_ui(Game *game) {
	const s32 font_size = 18;
	const s32 center_x = g_app->game_width  / 2;
	const s32 center_y = g_app->game_height / 2;
	
	Player *player = &game->session.player;
	if (player->state == PLAYER_DEAD) {
		const char *death = "YOU ARE DEAD.";
		s32 width = MeasureText(death, font_size * 2);
		s32 x = center_x - (width / 2);
		s32 y = center_y * 0.4f;
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
	// arena_free(&game->temp_arena);
	
	cleanup_console(&game->console);
}


void reset_game_state(Game *game) {
	init_player(&game->session.player, &game->session.world);
	reset_camera(&game->session.camera, &game->session.world);
}
