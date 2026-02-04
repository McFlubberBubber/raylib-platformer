#include "application.h"

#include "raylib.h"

#include "platformer.h"

Application *Application::instance = nullptr;

void init_app(Application *app) {
	Application::instance = app;
	app->monitor = GetCurrentMonitor();
	app->monitor_width  = GetMonitorWidth(app->monitor);
	app->monitor_height = GetMonitorHeight(app->monitor);

	// Applying resolutions and display mode (should probably be loaded from a config file).
	switch (app->res) {
	case _1280x720_: {
		app->width  = 1280;
		app->height = 720;
		break;
	}
	case _1366x768_: {
		app->width  = 1366;
		app->height = 768;
		break;
	}
	case _1920x1080_: {
		app->width  = 1920;
		app->height = 1080;
		break;
	}
	default: {
		app->width  = 1280;
		app->height = 720;
		break;
	}
	}

	switch (app->display_mode) {
	case WINDOWED_MODE: {
		InitWindow(app->width, app->height, app->title);
		break;
	}
	case FULLSCREEN_MODE: {
		app->width  = app->monitor_width;
		app->height = app->monitor_height;
		InitWindow(app->width, app->height, app->title);
		ToggleFullscreen();
		break;
	}
	case BORDERLESS_WINDOWED_MODE: {
		app->width  = app->monitor_width;
		app->height = app->monitor_height;
		InitWindow(app->width, app->height, app->title);
		ToggleBorderlessWindowed();
		break;
	}
	default: {
		break;
	}
   	}

	SetTargetFPS(app->target_fps);
	SetExitKey(KEY_Q);
	app->should_close = false;
	init_game(&app->game);
	
	return;
}

void update_app(Application *app) {
	BeginDrawing(); // @TODO: Should this be moved after we have finished running our updates?
	// BeginBlendMode(BLEND_ALPHA);
	
	update_game(&app->game);
	draw_game(&app->game);
	
	EndDrawing();
}

void shutdown_app(Application *app) {
	Application::instance = nullptr;
	CloseWindow();
}
