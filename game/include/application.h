#ifndef APPLICATION_H
#define APPLICATION_H

#include "raylib.h"
#include "general.h"

#include "platformer.h"
#include "input.h"
#include "assets.h"

const uint32_t DESIRED_FPS = 144;

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

struct ScratchArenas {
	Arena arenas[2];
	int   current = 0;
};

struct Application {
	const char *title = "Platformer";
	ResolutionType res = _1280x720_;

	DisplayMode display_mode = WINDOWED_MODE;

	// Related to the actual window and monitor specs.
	int monitor, monitor_width, monitor_height;
	int width, height;

	RenderTexture2D game_render_target;
	Rectangle       game_viewport;
	s32 game_width  = 1920;
	s32 game_height = 1080;

	int target_fps = DESIRED_FPS;
	bool should_close;

	Game game;
	Input input;
	AssetManager asset_manager;

	ScratchArenas scratch;

	float dt; // Delta time.
};

void init_app(Application *app);
void update_app(Application *app);
void draw_app(Application *app);
void shutdown_app(Application *app);

// Scratch arena stuff.
inline Arena *get_current_arena_frame() {
	return &g_app->scratch.arenas[g_app->scratch.current];
}
inline Arena *get_prev_arena_frame() {
	return &g_app->scratch.arenas[1-g_app->scratch.current];
}
void flip_scratch_arenas(Application *app);
#endif
