#include "platformer.h"
#include "raylib.h"

#include <stdio.h> // For printing stuff. 

static void process_opening_menu_inputs(Game* game) {
	auto menu = &game->main_menu;
	int current_item = static_cast<int>(menu->current_menu_item);
	int total_items  = static_cast<int>(MENU_COUNT);

	if (IsKeyPressed(KEY_DOWN)) {
		menu->current_menu_item = static_cast<MainMenuItems>((current_item + 1) % total_items);
	} else if (IsKeyPressed(KEY_UP)) {
		int prev_item = (current_item - 1 + total_items) % total_items;
		menu->current_menu_item = static_cast<MainMenuItems>(prev_item);
	}

	if (IsKeyPressed(KEY_ENTER)) {
		switch(game->main_menu.current_menu_item) {
		case MENU_START: {
			game->state = GAME_WORLD;
			break;
		}
		case MENU_SETTINGS: {
			printf("Settings page not implemented yet.\n");
			break;
		}
		case MENU_CONTROLS: {
			printf("Controls page not implemented yet.\n");
			break;
		}
		case MENU_EXIT: {
			CloseWindow();
			break;
		}
		default: {
			break;
		}
		}
	}
}

static void draw_opening_menu(Game *game) {
	ClearBackground(BLACK);

	process_opening_menu_inputs(game);

	// @TODO: Find a custom font that we can use so that we don't need to rely on raylib's
	// default font.

	// Drawing the main title of the game (still in placeholder).
	const char *title = "PLACEHOLDER NAME";
	const int title_font_size = 64;
	int text_width = MeasureText(title, title_font_size);
	int center_x = GetScreenWidth()  / 2;
	int center_y = GetScreenHeight() / 2;
	int x = center_x - (text_width / 2);
	int y = center_y * 0.3f; 
	DrawText(title, x, y, title_font_size, RAYWHITE);

	// Drawing main menu options.
	const char *button_text[] = { "START", "SETTINGS", "CONTROLS", "QUIT" };
	const int menu_font_size = 32;
	Rectangle button_rect;
	button_rect.width  = 400;
	button_rect.height = 50;
	button_rect.x = center_x - (button_rect.width / 2);

	for (int i = 0; i < 4; ++i) {
		int button_gap = (100 * (i + 1));
		button_rect.y = y + button_gap;

		int current_button_text_width = MeasureText(button_text[i], menu_font_size);
		int button_text_x = center_x - (current_button_text_width / 2);
		int button_text_y = button_rect.y + 10;

		DrawRectangleRec(button_rect, WHITE);
		if (game->main_menu.current_menu_item == i) {
			const int outline_rect_padding = 5;
			Rectangle outline_rect;
			outline_rect.x	    = button_rect.x - outline_rect_padding;
			outline_rect.y 		= button_rect.y - outline_rect_padding;
			outline_rect.width  = button_rect.width  + (outline_rect_padding * 2);
			outline_rect.height = button_rect.height + (outline_rect_padding * 2);

			DrawRectangleLinesEx(outline_rect, 3.0f, YELLOW);                            
		}
		DrawText(button_text[i], button_text_x, button_text_y, menu_font_size, BLACK);
	}

	const char *credits			 = "Created by McFlubberBubber";
	const int credits_text_width = MeasureText(credits, menu_font_size);
	const int credits_text_x 	 = center_x - (credits_text_width / 2);
	const int credits_text_y     = GetScreenHeight() - menu_font_size;
	DrawText(credits, credits_text_x, credits_text_y, menu_font_size, GRAY);
}

static void draw_debug_overlay(Game *game) {
	DrawFPS(0, 0);

	auto player = &game->player;
	constexpr int font_size = 18;
	constexpr int text_x    = 0;
	constexpr int text_y    = 20;
	const int center_x      = GetScreenWidth()  / 2;
	const int center_y      = GetScreenHeight() / 2;
	
	char state_text[64];
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

	// Drawing death screen.
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
	draw_debug_overlay(game);
}

static void init_camera(Camera2D *camera) {
	// Camera offset (displacement from target)
	camera->target.x = 0; 
	camera->target.y = 0;

	// Camera target (rotation and zoom origin)
    camera->offset.x = GetScreenWidth()  / 2;
    camera->offset.y = GetScreenHeight() / 2;

    camera->rotation = 0;
    camera->zoom     = 1.0f;
}

void init_game(Game *game) {
	init_camera(&game->camera);
	init_world(&game->world);
	init_player(&game->player);
}

void update_game(Game *game) {
	if (game->state == GAME_WORLD) {
		if (IsKeyPressed(KEY_R)) reset_game_state(game);
		update_world(&game->world, &game->player, &game->camera);
		update_player(&game->player, &game->world);
	}
};

void draw_game(Game *game) {
	switch(game->state) {
	case GAME_OPENING_MENU: {
		draw_opening_menu(game);
		break;
	}
	case GAME_MENU: {
		// draw_in_game_menu(game);
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
}

// @Dev
void reset_game_state(Game *game) {
	init_game(game);
}
