#ifndef PLATFORMER_H
#define PLATFORMER_H

#include "player.h"

enum GameState {
	GAME_OPENING_MENU = 0,
	GAME_MENU,
	GAME_WORLD,

	GAME_EDITOR, // Developer mode only.

	GAME_COUNT
};

enum MenuPages {
	PAGE_MAIN = 0,
	PAGE_SETTINGS,
	PAGE_CONTROLS,

	PAGE_COUNT
};

enum MainMenuItems {
	MENU_START = 0,
	MENU_SETTINGS,
	MENU_CONTROLS,
	MENU_EXIT,

	MENU_COUNT
};


struct GameMainMenu {
	MainMenuItems current_menu_item = MENU_START; 
	MenuPages current_page = PAGE_MAIN;
};

struct Game {
	GameState state = GAME_OPENING_MENU;
	GameMainMenu main_menu;

	Player player;
};

void init_game(Game *game);
void update_game(Game *game);
void draw_game(Game *game);

#endif
