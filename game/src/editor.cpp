#include "raylib.h"

#include "application.h"
#include "editor.h"

static void draw_editor_ui(Game *game) {
	const Font *editor_font = get_font(FONT_CONSOLE);
	const s32 font_size = 24;
	const s32 spacing = 0;
	const s32 buffer_size = 64;

    Vector2	pos = { 0.0f, (float)(g_app->game_height - font_size) };

	String camera_str = string_format(&game->temp_arena, "Camera | X: %.2f, Y: %.2f", game->camera.target.x, game->camera.target.y);
	draw_text_ex_with_string(editor_font, camera_str, pos, font_size, spacing, WHITE);

	String mouse_str = string_format(&game->temp_arena, "Mouse in screen | X: %d, Y: %d", GetMouseX(), GetMouseY());
	pos.y -= font_size;
	draw_text_ex_with_string(editor_font, mouse_str, pos, font_size, spacing, WHITE);
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

