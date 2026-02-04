#ifndef APPLICATION_H
#define APPLICATION_H

#include "platformer.h"

enum DisplayMode {
	WINDOWED_MODE = 0,
	FULLSCREEN_MODE,
	BORDERLESS_WINDOWED_MODE,

	DISPLAY_MODE_COUNT
};

enum ResolutionType {
	_1280x720_ = 0,
	_1366x768_,
	_1920x1080_,

	RESOLUTION_COUNT
};
	
struct Application {
	static Application *instance; // Singleton stuff. 
	
	const char *title = "Platformer";
	ResolutionType res = _1280x720_;
#if 1
	DisplayMode display_mode = WINDOWED_MODE;
#else
	DisplayMode display_mode = FULLSCREEN_MODE;
#endif

	int monitor, monitor_width, monitor_height;
	int width, height;
	int target_fps = 60;
	bool should_close;

	Game game;
};

void init_app(Application *app);
void update_app(Application *app);
void shutdown_app(Application *app);

#endif
