#include "menu.h"

#include "raylib.h"
#include "application.h"
#include "platformer.h"

static void handle_main_page_interactions(Menu *menu, Game *game) {
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

	// @NOTE: Currently, there aren't any options on the main page that will utilise the arrow
	// keys to change stuff, but these are here just in case we do add them later.
#if 0
	if (IsKeyPressed(KEY_LEFT)) {

	}
	if (IsKeyPressed(KEY_RIGHT)) {

	}
#endif
}

static void handle_settings_page_interactions(Menu *menu, Game *game) {
	if (IsKeyPressed(KEY_ENTER)) {
		switch(menu->current_settings_item) {
		case SETTINGS_FULLSCREEN: {
			// Toggle between 3 options, pressing enter increments.
			// - Windowed
			// - Fullscreen
			// - Borderless Windowed
			break;
		}
		case SETTINGS_RESOLUTION: {
			// Navigate through a list of 16:9 resolutions, pressing enter increments.
			// - 1280x720
			// - 1366x768
			// - 1920x1080
			break;
		}
		case SETTINGS_SAVE: {
			// Overwrite the new settings in a config file and reload it, causing changes to take
			// effect. 
			break;
		}
		case SETTINGS_RETURN: {
			// We should keep track if settings have changed beforehand so that we can ask
			// the user if they want to save their changes or not if they decide to exit early. So in
			// psuedocode...

			// 1. Check if settings state has been changed
			// 2. If the settings have not changed, then allow the user to return to main.
			menu->current_menu_page = PAGE_MAIN;
			// 3. Else, if the settings have changed and the user has NOT saved them, then display:
			// display_settings_warning();
			// 4. Based on the input within the warning popup which are: Save / Discard Changes...
			// 5. If discard, then go back to main page normally
			// 6. If save, follow the path of the SETTINGS_SAVE case.

			break;
		}

		default: {
			break;
		}
		}
	}

	// Handling left + right arrow inputs to navigate lists of stuff directly.
	if (IsKeyPressed(KEY_LEFT)) {
		switch(menu->current_settings_item) {
		case SETTINGS_FULLSCREEN: {
			// Move backwards through the list.
			// - Windowed
			// - Fullscreen
			// - Borderless Windowed
			break;
		}
		case SETTINGS_RESOLUTION: {
			// Navigate through a list of 16:9 resolutions, move backwards.
			// - 1280x720
			// - 1366x768
			// - 1920x1080
			break;
		}
		case SETTINGS_SAVE: {
			// This wouldn't do anything.
			break;
		}
		default: {
			break;
		}
		}
	}

	if (IsKeyPressed(KEY_RIGHT)) {
		switch(menu->current_settings_item) {
		case SETTINGS_FULLSCREEN: {
			// Move forwards through the list.
			// - Windowed
			// - Fullscreen
			// - Borderless Windowed
			break;
		}
		case SETTINGS_RESOLUTION: {
			// Navigate through a list of 16:9 resolutions, move forwards.
			// - 1280x720
			// - 1366x768
			// - 1920x1080
			break;
		}
		case SETTINGS_SAVE: {
			// This wouldn't do anything.
			break;
		}
		default: {
			break;
		}
		}
	}
}

static void handle_controls_page_interactions(Menu *menu, Game *game) {
	if (IsKeyPressed(KEY_ENTER)) {
		switch (menu->current_controls_item) {
		case CONTROLS_RETURN: {
			menu->current_menu_page = PAGE_MAIN;
			break;
		}
		default: {
			break;
		}
		}
	}
}

static void process_menu_inputs(Menu *menu, Game *game) {
	// First check if there are any page / state changes.
	if (IsKeyPressed(KEY_ESCAPE)) {
		if (menu->current_menu_page == PAGE_MAIN && game->state == GAME_MENU) {
			push_command_change_state(game, GAME_WORLD);
			return;
		}
		if (menu->current_menu_page == PAGE_SETTINGS) {
			menu->current_menu_page = PAGE_MAIN;
			return;
		}
		if (menu->current_menu_page == PAGE_CONTROLS) {
			menu->current_menu_page = PAGE_MAIN;
			return;
		}
	}

	// Then handle any interactions (activations with ENTER, inner navigations with left + right).
	switch (menu->current_menu_page) {
	case PAGE_MAIN: {
		handle_main_page_interactions(menu, game);
		break;
	}
	case PAGE_SETTINGS: {
		handle_settings_page_interactions(menu, game);
		break;
	}
	case PAGE_CONTROLS: { // @TODO: There are no controls items yet.
		handle_controls_page_interactions(menu, game);
		break;
	}
	default: {
		break;
	}
	}

	// Lastly, check for up + down navigation of menu items based on current_menu_page.
	if (IsKeyPressed(KEY_DOWN)) { // Handling menu item increments.
		int current_item, total_items;
		switch (menu->current_menu_page) {
		case PAGE_MAIN: {
			current_item = static_cast<int>(menu->current_main_item);
			total_items  = static_cast<int>(MAIN_COUNT);
			menu->current_main_item = static_cast<MainPageItems>((current_item + 1) % total_items);
			break;
		}
		case PAGE_SETTINGS: {
			current_item = static_cast<int>(menu->current_settings_item);
			total_items  = static_cast<int>(SETTINGS_COUNT);
			menu->current_settings_item = static_cast<SettingsPageItems>((current_item + 1) % total_items);
			break;
		}
		case PAGE_CONTROLS: {
			current_item = static_cast<int>(menu->current_controls_item);
			total_items  = static_cast<int>(CONTROLS_COUNT);
			menu->current_controls_item = static_cast<ControlsPageItems>((current_item + 1) % total_items);
			break;
		}
		default: {
			break;
		}
		}
	}

	if (IsKeyPressed(KEY_UP)) { // Handling menu item decrements.
		int current_item, total_items, prev_item;
		switch (menu->current_menu_page) {
		case PAGE_MAIN: {
			current_item = static_cast<int>(menu->current_main_item);
			total_items  = static_cast<int>(MAIN_COUNT);
			prev_item    = (current_item - 1 + total_items) % total_items;
			menu->current_main_item = static_cast<MainPageItems>(prev_item);
			break;
		}
		case PAGE_SETTINGS: {
			current_item = static_cast<int>(menu->current_settings_item);
			total_items  = static_cast<int>(MAIN_COUNT);
			prev_item    = (current_item - 1 + total_items) % total_items;
			menu->current_settings_item = static_cast<SettingsPageItems>(prev_item);
			break;
		}
		case PAGE_CONTROLS: {
			current_item = static_cast<int>(menu->current_controls_item);
			total_items  = static_cast<int>(CONTROLS_COUNT);
			prev_item    = (current_item - 1 + total_items) % total_items;
			menu->current_controls_item = static_cast<ControlsPageItems>(prev_item);
			break;
		}
		default: {
			break;
		}
		}
	}

}

static void draw_opening_main_page(Menu *menu) {
	// @TODO: Find a custom font that we can use so that we don't need to rely on raylib's
	// default font.
	// Application *app = Application::instance;
	const int game_width  = g_app->game_width;
	const int game_height = g_app->game_height;

	// Drawing the main title of the game (still in placeholder).
	const char *title = "PLACEHOLDER NAME";
	const int title_font_size = 64;
	int text_width = MeasureText(title, title_font_size);
	int center_x = game_width  / 2;
	int center_y = game_height / 2;
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
	const int credits_text_y     = game_height - menu_font_size;
	DrawText(credits, credits_text_x, credits_text_y, menu_font_size, GRAY);
}

static void draw_paused_menu(Menu *menu) {
	// Application *app = Application::instance;
	const int game_width  = g_app->game_width;
	const int game_height = g_app->game_height;
	
	const char *title   = "PAUSED";
	const int font_size = 40;
	int text_width      = MeasureText(title, font_size);
	int center_x        = (int)(game_width  / 2); 
	int starting_y      = (int)(game_height * 0.2f);
	DrawText(title, (center_x - (text_width / 2)), starting_y, font_size, WHITE);

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

// @NOTE: The settings + controls pages are drawn the exact same way whether the menu is in the
// opening menu or paused versions (handled by 'state' member in the game struct).
static void draw_settings_page(Menu *menu) {
	// Getting access to the application since we will be changing it's members and stuff.
	// Application *app = Application::instance;
	const int game_width  = g_app->game_width;
	const int game_height = g_app->game_height;

	const char *title   = "SETTINGS";
	const int font_size = 40;
	int text_width      = MeasureText(title, font_size);
	int center_x        = (int)(game_width  / 2); 
	int starting_y      = (int)(game_height * 0.2f);
	DrawText(title, (center_x - (text_width / 2)), starting_y, font_size, WHITE);	

	// Each of these have their own buffers since we want to modify the string contents based on the
	// status of that setting.
	char display[24];
	char resolution[24];
	char save[8];
	char exit[8];

	
}

static void draw_controls_page(Menu *menu) {
	// Application *app = Application::instance;
	const int game_width  = g_app->game_width;
	const int game_height = g_app->game_height;

	const char *title   = "CONTROLS";
	const int font_size = 40;
	int text_width      = MeasureText(title, font_size);
	int center_x        = (int)(game_width  / 2); 
	int starting_y      = (int)(game_height * 0.2f);
	DrawText(title, (center_x - (text_width / 2)), starting_y, font_size, WHITE);

	
}

void draw_opening_menu(Menu *menu) {
	ClearBackground(BLACK); // Drawing a black BG for opening menu
	switch (menu->current_menu_page) {
	case PAGE_MAIN: {
		draw_opening_main_page(menu);
		break;
	}
	case PAGE_SETTINGS: {
		draw_settings_page(menu);
		break;
	}
	case PAGE_CONTROLS: {
		draw_controls_page(menu);
		break;
	}
	default: {
		break;
	}
	}
}

void draw_in_game_menu(Menu *menu) {
	// @NOTE: We skip on clearing background since we also draw a frame of the game environment
	// as the background. But we do draw this low opacitiy rect.
	// Application *app = Application::instance;
	Rectangle bg  = { 0, 0, (float)g_app->game_width, (float)g_app->game_height };
	Color overlay = { 0, 0, 0, 190 };
	DrawRectangleRec(bg, overlay);

	Color vignette = { 0, 0, 0, 50 };
	DrawRectangleRec(bg, vignette);
	
	switch (menu->current_menu_page) {
	case PAGE_MAIN: {
		draw_paused_menu(menu);
		break;
	}
	case PAGE_SETTINGS: {
		draw_settings_page(menu);
		break;
	}
	case PAGE_CONTROLS: {
		draw_controls_page(menu);
		break;
	}
	default: {
		break;
	}
	}
}

void update_menu(Menu *menu, Game *game) {
	process_menu_inputs(menu, game);
}
