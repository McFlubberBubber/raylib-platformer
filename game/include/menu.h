#ifndef MENU_H
#define MENU_H

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

	SETTINGS_COUNT
};

struct Menu {
	MenuPages current_menu_page = PAGE_MAIN;
	
	MainPageItems 	  current_main_item     = MAIN_START;
	SettingsPageItems current_settings_item = SETTINGS_FULLSCREEN;
};

void draw_opening_menu(Menu *menu);
void draw_in_game_menu(Menu *menu);

void update_menu(Menu *menu, Game *game); // This is meant for processing inputs and changing states.

#endif
