#pragma once

#include "platformer.h"

struct Application {
	const char *title = "Platformer";
	int target_fps = 60;

	int width  = 1280;
	int height = 720;

	// Game-specific stuff.
	Game game;
};

void init_app(Application *app);
void update_app(Application *app);
void shutdown_app(Application *app);
