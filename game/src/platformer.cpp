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

static void draw_game_environment(Game *game) {
	ClearBackground(BLACK);

	draw_world(&game->world);
	draw_player(&game->player);

	DrawText("This is the game world!", 200, 200, 20, RAYWHITE);
	DrawText("Press Q to quit the game since nothing is here yet.", 200, 240, 20, RAYWHITE);	
}

void init_game(Game *game) {
	init_world(&game->world);
	init_player(&game->player);
}

void update_game(Game *game) {
	if (game->state == GAME_WORLD) {
		if (IsKeyPressed(KEY_R)) reset_game_state(game);
		update_world(&game->world);
		update_player(&game->player, &game->world);
	}
};

void draw_game(Game *game) {
	DrawFPS(0, 0);
	
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
