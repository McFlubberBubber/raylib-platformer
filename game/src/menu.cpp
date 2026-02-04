#include "menu.h"

#include "raylib.h"
#include "platformer.h"

static void handle_main_page_inputs(Menu *menu, Game *game) {
	int current_item = static_cast<int>(menu->current_main_item);
	int total_items  = static_cast<int>(MAIN_COUNT);
	int prev_item    = (current_item - 1 + total_items) % total_items;
	
	// @TODO: Do we want to add more inputs for navigating the menu? Also, do we want to add
	// infinite increments / decrements when the button is held for a period of time?
	if (IsKeyPressed(KEY_DOWN)) {
		menu->current_main_item = static_cast<MainPageItems>((current_item + 1) % total_items);
	} else if (IsKeyPressed(KEY_UP)) {
		menu->current_main_item = static_cast<MainPageItems>(prev_item);
	}

	// Handling main page activations with the ENTER key.
	if (IsKeyPressed(KEY_ENTER)) {
		switch(menu->current_main_item) {
		case MAIN_START: {
			push_command_change_state(game, GAME_WORLD);
			break;
		}
		case MAIN_SETTINGS: {
			menu->current_menu_page = PAGE_SETTINGS;
			break;
		}
		case MAIN_CONTROLS: {
			menu->current_menu_page = PAGE_CONTROLS;
			break;
		}
		case MAIN_EXIT: {
			if (game->state == GAME_MENU) {
				push_command_change_state(game, GAME_OPENING_MENU);
				game->menu.current_main_item = MAIN_START;
			} else if (game->state == GAME_OPENING_MENU) {
				push_command_simple(game, CMD_QUIT_GAME);
			}
			
			break;
		}
		default: {
			break;
		}
		}
	}
}

static void handle_settings_page_inputs(Menu *menu) {
	int current_item = static_cast<int>(menu->current_settings_item);
	int total_items  = static_cast<int>(SETTINGS_COUNT);
	int prev_item	 = (current_item - 1 + total_items) % total_items;
	
	if (IsKeyPressed(KEY_DOWN)) {
		menu->current_settings_item = static_cast<SettingsPageItems>((current_item + 1)
																	 % total_items);
	} else if (IsKeyPressed(KEY_UP)) {
		menu->current_settings_item = static_cast<SettingsPageItems>(prev_item);
	}

	if (IsKeyPressed(KEY_ESCAPE)) {
		menu->current_menu_page = PAGE_MAIN;
	}
}

static void handle_controls_page_inputs(Menu *menu) {
	// @TODO: We don't have any controls page to show off yet, but when we do, this needs to get
	// updated.
	if (IsKeyPressed(KEY_ESCAPE)) {
		menu->current_menu_page = PAGE_MAIN;
	}
}

static void draw_main_page(Menu *menu) {
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
		if (menu->current_main_item == i) {
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


void draw_opening_menu(Menu *menu) {
	ClearBackground(BLACK);

	switch (menu->current_menu_page) {
	case PAGE_MAIN: {
		draw_main_page(menu);
		break;
	}
	case PAGE_SETTINGS: {
		break;
	}
	case PAGE_CONTROLS: {
		break;
	}
	default: {
		break;
	}
	}
}

void draw_in_game_menu(Menu *menu) {
	Rectangle bg  = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
	Color overlay = { 0, 0, 0, 190 };
	DrawRectangleRec(bg, overlay);

	Color vignette = { 0, 0, 0, 50 };
	DrawRectangleRec(bg, vignette);
	
	const char *title   = "PAUSED";
	const int font_size = 40;
	int text_width      = MeasureText(title, font_size);
	int center_x        = (int)(GetScreenWidth()  / 2); 
	int starting_y      = (int)(GetScreenHeight() * 0.2f);
	
	DrawText("PAUSED", (center_x - (text_width / 2)), starting_y, font_size, WHITE);

	// Drawing the menu options
	const char *button_text[] = { "RESUME", "SETTINGS", "CONTROLS", "BACK TO MAIN MENU" };
	const int button_font_size = 20;
	Rectangle button_rect;
	button_rect.width  = 300;
	button_rect.height = button_font_size * 2;
	button_rect.x = center_x - (button_rect.width / 2);
	starting_y += 100;

	for (int i = 0; i < 4; ++i) {
		int button_gap = (60 * (i + 1));
		button_rect.y = starting_y + button_gap;

		int current_button_text_width = MeasureText(button_text[i], button_font_size);
		int button_text_x = center_x - (current_button_text_width / 2);
		int button_text_y = button_rect.y + 10;

		DrawRectangleRec(button_rect, WHITE);
		if (menu->current_main_item == i) {
			const int outline_rect_padding = 5;
			Rectangle outline_rect;
			outline_rect.x	    = button_rect.x - outline_rect_padding;
			outline_rect.y 		= button_rect.y - outline_rect_padding;
			outline_rect.width  = button_rect.width  + (outline_rect_padding * 2);
			outline_rect.height = button_rect.height + (outline_rect_padding * 2);

			DrawRectangleLinesEx(outline_rect, 3.0f, YELLOW);                            
		}
		DrawText(button_text[i], button_text_x, button_text_y, button_font_size, BLACK);
	}
}

void update_menu(Menu *menu, Game *game) {
	// Processing menu inputs.
	switch (menu->current_menu_page) {
	case PAGE_MAIN: {
		handle_main_page_inputs(menu, game);
		break;
	}
	case PAGE_SETTINGS: {
		handle_settings_page_inputs(menu);
		break;
	}
	case PAGE_CONTROLS: {
		handle_controls_page_inputs(menu);
		break;
	}

	default : {
		printf("ERROR: Unknown page.\n");
		break;
	}
	}
}
