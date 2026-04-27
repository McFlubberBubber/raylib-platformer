#include "raylib.h"

#include "application.h"
#include "editor.h"

static void draw_editor_ui(Game *game) {
	const Font *editor_font = get_font(FONT_CONSOLE);
	const s32 font_size = 24;
	const s32 spacing = 0;
	const s32 buffer_size = 64;

	Vector2 pos = {};
	s32 text_width = 0;

	// We could make our own sprintf() using the arena in the future.
	const char *placeholder = "This is an error!";
	char *camera_pos_text = (char *)arena_allocate(&game->temp_arena, buffer_size, alignof(char));
	pos = { 0.0f, (float)(g_app->game_height - font_size) };
	if (camera_pos_text) {
		snprintf(camera_pos_text, buffer_size, "Camera | X: %.2f, Y: %.2f",
				 game->camera.target.x, game->camera.target.y);
		DrawTextEx(*editor_font, camera_pos_text, pos, font_size, spacing, WHITE);
	} else {
		fprintf(stderr, "Arena returned a nullptr! Could not display info for camera_pos_text.\n");
		DrawTextEx(*editor_font, placeholder, pos, font_size, spacing, RED);
	}
	
	char *mouse_in_screen_pos_text = (char *)arena_allocate(&game->temp_arena, buffer_size,	alignof(char));
	pos.y -= font_size;
	if (mouse_in_screen_pos_text) {
		snprintf(mouse_in_screen_pos_text, buffer_size, "Mouse in screen | X: %d, Y: %d",
				 GetMouseX(), GetMouseY());
		DrawTextEx(*editor_font, mouse_in_screen_pos_text, pos, font_size, spacing, WHITE);
	} else {
		fprintf(stderr, "Arena returned a nullptr! Could not display info for mouse_in_screen_pos_text.\n");
		DrawTextEx(*editor_font, placeholder, pos, font_size, spacing, RED);
	}
}

void draw_editor_view(Game *game) {
	ClearBackground(BLACK);

	// Drawing actual world space stuff.
	BeginMode2D(game->camera);

	draw_world(&game->world);
	draw_player(&game->player);

	EndMode2D();
	
	draw_editor_ui(game);
}

