#include "raylib.h"

int main() {
	InitWindow(1280, 720, "Platformer");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		DrawText("Hello from RAYLIB!", 200, 200, 20, RAYWHITE);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
