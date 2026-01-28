#include "application.h"

#include "raylib.h"

#include "platformer.h"

Application *Application::instance = nullptr;

void init_app(Application *app) {
	Application::instance = app;

	InitWindow(app->width, app->height, app->title);
	SetTargetFPS(app->target_fps);
	SetExitKey(KEY_Q);

	init_game(&app->game);
	
	return;
}

void update_app(Application *app) {
	BeginDrawing(); // @TODO: Should this be moved after we have finished running our updates?
	
	update_game(&app->game);
	draw_game(&app->game);
	
	EndDrawing();
}

void shutdown_app(Application *app) {
	Application::instance = nullptr;
	CloseWindow();
}
