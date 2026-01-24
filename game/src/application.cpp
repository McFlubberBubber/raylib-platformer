#include "application.h"

#include "raylib.h"

void Application::init() {
	InitWindow(width, height, title);
	SetTargetFPS(target_fps);
	return;
}

void Application::update() {
	BeginDrawing();
	ClearBackground(BLACK);
	DrawText("This is the new application struct showing this off!", 200, 200, 20, RAYWHITE);
	EndDrawing();
}

void Application::shutdown() {
	CloseWindow();
}
