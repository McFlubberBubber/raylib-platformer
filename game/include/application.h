#ifndef APPLICATION_H
#define APPLICATION_H

#include "raylib.h"
#include "platformer.h"
#include "assets.h"

struct Application;
extern Application *g_app;

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
	const char *title = "Platformer";
	ResolutionType res = _1280x720_;
#if 1
	DisplayMode display_mode = WINDOWED_MODE;
#else
	DisplayMode display_mode = FULLSCREEN_MODE;
#endif

	// Related to the actual window and monitor specs.
	int monitor, monitor_width, monitor_height;
	int width, height;

	RenderTexture2D game_render_target;
	Rectangle       game_viewport;
	int game_width  = 1920;
	int game_height = 1080;

	int target_fps = 60;
	bool should_close;

	Game game;
	float dt; // Delta time.

	AssetManager asset_manager;
};

void init_app(Application *app);
void update_app(Application *app);
void draw_app(Application *app);
void shutdown_app(Application *app);

#endif
