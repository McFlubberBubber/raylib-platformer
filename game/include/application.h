#ifndef APPLICATION_H
#define APPLICATION_H

#include "raylib.h"
#include "general.h"
#include "constants.h"

#include "platformer.h"
#include "input.h"
#include "assets.h"
#include "vars.h"

struct Application;
extern Application *g_app;

enum DisplayMode {
	WINDOWED_MODE = 0,
	FULLSCREEN_MODE,
	BORDERLESS_WINDOWED_MODE,

	DISPLAY_MODE_COUNT
};

struct ScratchArenas {
	Arena arenas[2];
	int   current = 0;
};

struct Application {
	const char *title = "Platformer";
	const char *data_path;
	DisplayMode display_mode = WINDOWED_MODE;

	// Related to the actual window and monitor specs.
	int monitor, monitor_width, monitor_height;
	s32 width  = GAME_WIDTH * DEFAULT_WINDOW_SCALE;
	s32 height = GAME_HEIGHT * DEFAULT_WINDOW_SCALE;

	RenderTexture2D game_render_target;
	Rectangle       game_viewport;
	s32 game_width  = GAME_WIDTH;
	s32 game_height = GAME_HEIGHT;

	int target_fps = DESIRED_FPS;
	bool should_close;

	Game game;
	Input input;
	AssetManager asset_manager;
	HotloadedVariables hotloaded_variables;

	ScratchArenas scratch;
	Arena permanent_arena;

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
	return &g_app->scratch.arenas[1 - g_app->scratch.current];
}
inline Arena *get_permanent_arena() {
	return &g_app->permanent_arena;
}

inline const char *get_data_path() {
	return g_app->data_path;
}

inline Vector2 get_screen_center() {
	Vector2 result = {};
	result = {(float)(g_app->game_width * 0.5f), (float)(g_app->game_height * 0.5f)};
	return result;
}

void flip_scratch_arenas(Application *app);
#endif
