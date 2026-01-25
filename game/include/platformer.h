#ifndef PLATFORMER_H
#define PLATFORMER_H

enum GameState {
	GAME_OPENING_MENU = 0,
	GAME_MENU,
	GAME_WORLD,

	GAME_COUNT
};

enum MainMenuItems {
	MENU_START = 0,
	MENU_SETTINGS,
	MENU_EXIT,

	MENU_COUNT
};


struct GameMainMenu {
	MainMenuItems current_menu_item = MENU_START; 
};

struct Game {
	GameState state = GAME_OPENING_MENU;
	GameMainMenu main_menu;
};

// void init_game(Game *game); // Will probably be necessary when we wanna start loading in other
                               // game-related stuff...
void draw_game(Game *game);

#endif
