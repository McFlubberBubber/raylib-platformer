#ifndef APPLICATION_H
#define APPLICATION_H

#include "platformer.h"

struct Application {
	static Application *instance; // Singleton stuff. 
	
	const char *title = "Platformer";
	int target_fps = 60;

	int width  = 1280;
	int height = 720;
	
	bool should_close;

	// Game-specific stuff.
	Game game;
};

void init_app(Application *app);
void update_app(Application *app);
void shutdown_app(Application *app);

#endif
