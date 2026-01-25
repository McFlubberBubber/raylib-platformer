#include "application.h"

#include "raylib.h"

#include "platformer.h"

void init_app(Application *app) {
	InitWindow(app->width, app->height, app->title);
	SetTargetFPS(app->target_fps);
	SetExitKey(KEY_Q);
	return;
}

void update_app(Application *app) {
	BeginDrawing();
	
	draw_game(&app->game);
	
	EndDrawing();
}

void shutdown_app(Application *app) {
	CloseWindow();
}
