#include "menu.h"

#include "raylib.h"
#include "application.h"
#include "platformer.h"

static void do_main_page_activations(Menu *menu) {
	Game *game = &g_app->game;
	switch (menu->current_main_item) {
	case MAIN_START: {
		push_command_change_state(game, GAME_WORLD);
		break;
	}
	case MAIN_SETTINGS: {
		menu->current_page = PAGE_SETTINGS;
		break;
	}
	case MAIN_CONTROLS: {
		menu->current_page = PAGE_CONTROLS;
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
	return;
}

static void do_settings_page_activations(Menu *menu) {
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
		menu->current_page = PAGE_MAIN;
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
	return;
}

static void do_controls_page_activations(Menu *menu) {
	switch (menu->current_controls_item) {
	case CONTROLS_RETURN: {
		menu->current_page = PAGE_MAIN;
		break;
	}
	default: {
		break;
	}
	}
	return;
}

static void draw_opening_main_page(Menu *menu) {
//	const s32 game_width  = g_app->game_width;
//	const s32 game_height = g_app->game_height;
//	const s32 center_x = (s32)(game_width  * 0.5f);
//	const s32 center_y = (s32)(game_height * 0.5f);

	const Vector2 screen_center = get_screen_center();
	const float spacing = 0;

	// Drawing the main title of the game (still in placeholder).
	const char   *title      = "PLACEHOLDER NAME";
	const s32     title_size = menu->font_size * 2;
	const Vector2 title_dim  = MeasureTextEx(*menu->font, title, title_size, spacing);

	Vector2 title_pos = {screen_center.x - (title_dim.x * 0.5f), screen_center.y * 0.3f};
	Color color = RAYWHITE;
	DrawTextEx(*menu->font, title, title_pos, title_size, spacing, color);

	// Drawing main menu options.
	const char *button_text[] = { "START", "SETTINGS", "CONTROLS", "QUIT" };
	const Color button_color  = WHITE;
	Rectangle button_rect;
	button_rect.width  = 400;
	button_rect.height = 50;
	button_rect.x = screen_center.x - (button_rect.width * 0.5f);

	const Color text_color = BLACK;

	for (int i = 0; i < 4; ++i) {
		s32 button_gap = (100 * (i + 1));
		button_rect.y = title_pos.y + button_gap;

/*
		int current_button_text_width = MeasureText(button_text[i], menu_font_size);
		s32 button_text_x = center_x - (current_button_text_width / 2);
		s32 button_text_y = button_rect.y + 10;
*/

		Vector2 current_button_text_dim = MeasureTextEx(*menu->font, button_text[i], menu->font_size, spacing);
		Vector2 button_text_pos = {screen_center.x - (current_button_text_dim.x * 0.5f), button_rect.y + 10};

		DrawRectangleRec(button_rect, button_color);
		if (menu->current_main_item == i) {
			const s32 outline_rect_padding = 5;
			Rectangle outline_rect;
			outline_rect.x	    = button_rect.x - outline_rect_padding;
			outline_rect.y 		= button_rect.y - outline_rect_padding;
			outline_rect.width  = button_rect.width  + (outline_rect_padding * 2);
			outline_rect.height = button_rect.height + (outline_rect_padding * 2);
			DrawRectangleLinesEx(outline_rect, 3.0f, YELLOW);                            
		}

		DrawTextEx(*menu->font, button_text[i], button_text_pos, menu->font_size, spacing, text_color);
		// DrawText(button_text[i], button_text_x, button_text_y, menu_font_size, BLACK);
	}

	const char   *credits = "Created by McFlubberBubber";
	const Vector2 credits_text_dim = MeasureTextEx(*menu->font, credits, menu->font_size, spacing);
	const Vector2 credits_pos = {screen_center.x - (credits_text_dim.x * 0.5f), g_app->game_height - menu->font_size};
	const Color   credits_color = GRAY;

	DrawTextEx(*menu->font, credits, credits_pos, menu->font_size, spacing, credits_color);
}

static void draw_paused_menu(Menu *menu) {
	// Application *app = Application::instance;
	// const int game_width  = g_app->game_width;
	// const int game_height = g_app->game_height;
	const Vector2 screen_center = get_screen_center();
	const float spacing = 0;

	const char   *title      = "PAUSED";
	const s32     title_size = (s32)(menu->font_size * 1.5f);
	const Vector2 title_dim  = MeasureTextEx(*menu->font, title, title_size, spacing);

	Vector2 pos  = {screen_center.x - (title_dim.x * 0.5f), g_app->game_height * 0.2f};
	DrawTextEx(*menu->font, title, pos, title_size, spacing, WHITE);

	// Drawing the menu options
	const char *button_text[] = { "RESUME", "SETTINGS", "CONTROLS", "BACK TO MAIN MENU" };
	const float button_font_size = menu->font_size * 0.7f;
//	const int button_font_size = 20;
	Rectangle button_rect;
	button_rect.width  = 300;
	button_rect.height = button_font_size * 2;
	button_rect.x = screen_center.x - (button_rect.width / 2);
	
	pos.y += 100;

	for (int i = 0; i < 4; ++i) {
		int button_gap = (60 * (i + 1));
		button_rect.y = pos.y + button_gap;

		// int button_text_x = screen_center.x - (current_button_text_width / 2);
		// int button_text_y = button_rect.y + 10;
		Vector2 current_button_text_dim = MeasureTextEx(*menu->font, button_text[i], button_font_size, spacing);
		Vector2 button_text_pos = {screen_center.x - (current_button_text_dim.x * 0.5f), button_rect.y + 10};

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
		
		DrawTextEx(*menu->font, button_text[i], button_text_pos, button_font_size, spacing, BLACK);
		// DrawText(button_text[i], button_text_x, button_text_y, button_font_size, BLACK);
	}
}

// @NOTE: The settings + controls pages are drawn the exact same way whether the menu is in the
// opening menu or paused versions (handled by 'state' member in the game struct).
static void draw_settings_page(Menu *menu) {
	// Getting access to the application since we will be changing it's members and stuff.
	const int game_width  = g_app->game_width;
	const int game_height = g_app->game_height;

	const Vector2 screen_center = get_screen_center();
	const float spacing   = 0;

	const char *title       = "SETTINGS";
	const s32   title_size  = menu->font_size * 1.5f;
	const Vector2 title_dim = MeasureTextEx(*menu->font, title, title_size, spacing);
	Vector2 title_pos = {screen_center.x - (title_dim.x * 0.5f), g_app->game_height * 0.2f};
	DrawTextEx(*menu->font, title, title_pos, title_size, spacing, WHITE);	
}

static void draw_controls_page(Menu *menu) {
	const float spacing = 0;
	const Vector2 screen_center = get_screen_center();
	
	const char   *title      = "CONTROLS";
	const s32     title_size = menu->font_size * 1.5f;
	const Vector2 title_dim  = MeasureTextEx(*menu->font, title, title_size, spacing);
	Vector2 title_pos = {screen_center.x - (title_dim.x * 0.5f), g_app->game_height * 0.2f};
	DrawTextEx(*menu->font, title, title_pos, title_size, spacing, WHITE);	
}

void init_menu(Menu *menu) {
	Font *the_font = get_font(FONT_MENU);
	menu->font = the_font;
	menu->font_size = 32;
	
	menu->current_page = PAGE_MAIN;
	
	menu->current_main_item 	= MAIN_START;
	menu->current_settings_item = SETTINGS_FULLSCREEN;
	menu->current_controls_item = CONTROLS_RETURN;
}

void draw_opening_menu(Menu *menu) {
	ClearBackground(BLACK); // Drawing a black BG for opening menu
	switch (menu->current_page) {
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
	// as the background. But we do draw this low opacity rect.
	BeginBlendMode(BLEND_MULTIPLIED);

	Rectangle bg  = { 0, 0, (float)g_app->game_width, (float)g_app->game_height };
	Color overlay = { 0, 0, 0, 200 };

	DrawRectangleRec(bg, overlay);	
	EndBlendMode();

	switch (menu->current_page) {
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
	// We used to put input handling in here, but we refactored it into our input system now.
	// -nathan, 22/03/2025
}

void navigate_menu_item(Menu *menu, bool go_forward) {
	int current_item, total_items, prev_item;	
	if (go_forward) {
		switch (menu->current_page) {
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

	} else {
		switch (menu->current_page) {
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
	return;
}

// @TODO: We don't have any things to cycle, yet.
void cycle_menu_item(Menu *menu, bool go_forward) {
	if (go_forward) {

	} else {

	}
	
	return;
}

void handle_menu_activations(Menu *menu) {
	// First check what page we are on, then handle each menu item seperately.
	switch (menu->current_page) {
	case PAGE_MAIN: {
		do_main_page_activations(menu);
		break;
	}
	case PAGE_SETTINGS: {
		do_settings_page_activations(menu);
		break;
	}
	case PAGE_CONTROLS: {
		do_controls_page_activations(menu);
		break;
	}
	}
}

void handle_menu_escape_press(Menu *menu) {
	Game *game = &g_app->game;

	if (menu->current_page == PAGE_MAIN && game->state == GAME_OPENING_MENU) {
		return; // Escape press won't do anything on the opening menu (for now).
	}

	switch (menu->current_page) {
	case PAGE_MAIN: {
		push_command_change_state(game, GAME_WORLD);
		break;
	}
	case PAGE_SETTINGS: {
		menu->current_page = PAGE_MAIN;
		break;
	}
	case PAGE_CONTROLS: {
		menu->current_page = PAGE_MAIN;
		break;
	}
	}
	return;
}
