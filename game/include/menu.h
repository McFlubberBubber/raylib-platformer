#ifndef MENU_H
#define MENU_H

#include "raylib.h"

struct Game;

enum MenuPages {
	PAGE_MAIN = 0,
	PAGE_SETTINGS,
	PAGE_CONTROLS,

	PAGE_COUNT
};

enum MainPageItems {
	MAIN_START = 0,
	MAIN_SETTINGS,
	MAIN_CONTROLS,
	MAIN_EXIT, // @NOTE: This value depends on which state the menu is on. If we are in the in-game
	           // menu, then this would send us back to the opening main menu, but otherwise, this
	           // would exit the application.
	
	MAIN_COUNT
};

enum SettingsPageItems {
	SETTINGS_FULLSCREEN = 0,
	SETTINGS_RESOLUTION,

	SETTINGS_SAVE,
	SETTINGS_RETURN,

	SETTINGS_COUNT
};

enum ControlsPageItems {
	CONTROLS_RETURN,

	CONTROLS_COUNT
};

struct Menu {
	Font *font; // @Placeholder, we don't have a good 'game' font yet.
	float font_size;

	MenuPages current_page;
	
	MainPageItems 	  current_main_item;
	SettingsPageItems current_settings_item;
	ControlsPageItems current_controls_item;
};

void init_menu(Menu *menu);
void draw_opening_menu(Menu *menu);
void draw_in_game_menu(Menu *menu);

void update_menu(Menu *menu, Game *game); // This is meant for processing inputs and changing states.

// Utility functions.
void navigate_menu_item(Menu *menu, bool go_forward);
void    cycle_menu_item(Menu *menu, bool go_forward);

void handle_menu_activations(Menu *menu);
void handle_menu_escape_press(Menu *menu);

#endif
