#include "raylib.h"

#include "application.h"
#include "editor.h"

static void draw_editor_ui(Game *game) {
	const GameSession *session = &game->session;
	const Font *editor_font = get_font(FONT_CONSOLE);
	const s32 font_size = 24;
	const s32 spacing = 0;
	const s32 buffer_size = 64;

    Vector2	pos = { 0.0f, (float)(g_app->game_height - font_size) };
	
	String camera_str = string_format(&game->temp_arena, "Camera | X: %.2f, Y: %.2f", session->camera.raylib_cam.target.x, session->camera.raylib_cam.target.y);
	draw_text_ex_with_string(editor_font, camera_str, pos, font_size, spacing, WHITE);

	String mouse_str = string_format(&game->temp_arena, "Mouse in screen | X: %d, Y: %d", GetMouseX(), GetMouseY());
	pos.y -= font_size;
	draw_text_ex_with_string(editor_font, mouse_str, pos, font_size, spacing, WHITE);

	StringBuilder sb = {};
	strbuild_append_cstring(&game->temp_arena, &sb, "Editor Mode: ");
	switch (session->editor.mode) {
	case EDITOR_VIEW:  { strbuild_append_cstring(&game->temp_arena, &sb, "VIEW");  break; }
	case EDITOR_TILES: { strbuild_append_cstring(&game->temp_arena, &sb, "TILES"); break; }
	default:	       { strbuild_append_cstring(&game->temp_arena, &sb, "ERROR");        break; }
	}
	strbuild_terminate(&game->temp_arena, &sb);
	pos.y -= font_size;
	draw_text_ex_with_string(editor_font, sb.buffer, pos, font_size, spacing, WHITE);
}

static void draw_tile_editor_view(Game *game) {
	// @TODO: We don't store the tile_size in our World struct yet...
	const u32 tile_size = 32;
	const Vector2 mouse_pos = GetMousePosition();
	const Vector2 mouse_pos_in_world = GetScreenToWorld2D(mouse_pos, game->session.camera.raylib_cam);

#if 1
	printf("mouse_pos_in_world: X: %.2f | Y: %.2f\n", mouse_pos_in_world.x, mouse_pos_in_world.y);
#endif
	
	Rectangle tile_preview_rect = {mouse_pos_in_world.x, mouse_pos_in_world.y, (float)tile_size, (float)tile_size};
	Color tile_preview_color = {255, 255, 255, 255};
	DrawRectangleRec(tile_preview_rect, tile_preview_color);
}

void draw_editor_view(Game *game) {
	const GameSession *session = &game->session;

	ClearBackground(BLACK);

	// Drawing actual world space stuff.
	BeginMode2D(session->camera.raylib_cam);

	draw_world(&session->world);
	draw_player(&session->player);

	if (session->editor.mode == EDITOR_TILES) {
		draw_tile_editor_view(game);
	}

	EndMode2D();
	
	draw_editor_ui(game);
}

void cycle_editor_mode(Editor *editor, bool go_forward) {
	s32 current_item, total_items, prev_item;	
	current_item = static_cast<s32>(editor->mode);
	total_items  = static_cast<s32>(EDITOR_MODE_COUNT);

	if (go_forward) {
		editor->mode = static_cast<EditorMode>((current_item + 1) % total_items);
	} else {
		prev_item    = (current_item - 1 + total_items) % total_items;
		editor->mode = static_cast<EditorMode>(prev_item);
	}
}
