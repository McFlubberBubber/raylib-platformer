#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "general.h"

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
	SETTINGS_DISPLAY = 0,
	SETTINGS_RESOLUTION,

	SETTINGS_SAVE,
	SETTINGS_RETURN,

	SETTINGS_COUNT
};

enum SetingsDisplayOptions {
	SETTINGS_DISPLAY_WINDOWED = 0,
	SETTINGS_DISPLAY_FULLSCREEN,
	SETTINGS_DISPLAY_BORDERLESS_WINDOWED,

	SETTINGS_DISPLAY_COUNT
};

enum ControlsPageItems {
	CONTROLS_RETURN,

	CONTROLS_COUNT
};

struct SettingsItem {
	char heading[64];
	char value[64];
	
	// These get casted from the enum that relates to the item.
	s32 current_value;
	s32 previous_value;
	
	bool is_modified;
};

struct SettingsPage {
	const char *title = "SETTINGS";
	SettingsPageItems current_item;
	SettingsItem items[2]; // This excludes the save / return items.
};

struct Menu {
	Font *font; // @Placeholder, we don't have a good 'game' font yet.
	float font_size;

	MenuPages current_page;
	
	MainPageItems current_main_item;

	// Settings-related items.
	// SettingsPageItems current_settings_item;
	SettingsPage settings;

	// Controls-related items.
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
