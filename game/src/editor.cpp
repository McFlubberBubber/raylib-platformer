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

	String camera_zoom_str = string_format(&game->temp_arena, "Current Zoom | %.4f", session->camera.raylib_cam.zoom);
	pos.y -= font_size;
	draw_text_ex_with_string(editor_font, camera_zoom_str, pos, font_size, spacing, WHITE);

	
	Vector2 snapped_mouse_pos = get_snapped_mouse_pos_in_world(&game->session);
	String snapped_str = string_format(&game->temp_arena, "Snapped mouse pos | X: %.2f, Y: %.2f", snapped_mouse_pos.x, snapped_mouse_pos.y);
	pos.y -= font_size;
	draw_text_ex_with_string(editor_font, snapped_str, pos, font_size, spacing, WHITE);

	
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
	const GameSession *session = &game->session;
	const float tile_size = session->world.tile_size;
	const Vector2 snapped_pos = get_snapped_mouse_pos_in_world(session);

	Rectangle tile_preview_rect = {snapped_pos.x, snapped_pos.y, (float)tile_size, (float)tile_size};
	Color tile_preview_color = { 255, 255, 255, 100 };
	DrawRectangleRec(tile_preview_rect, tile_preview_color);
	DrawRectangleLinesEx(tile_preview_rect, 1.0f, WHITE);
}

void draw_editor_view(Game *game) {
	const GameSession *session = &game->session;

	ClearBackground(BLACK);

	// Drawing actual world space stuff.
	BeginMode2D(session->camera.raylib_cam);

	draw_world(&session->world, true); // Draw the entire world.
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

// @TODO: Handle error diagnostics within this function.
void handle_tile_editor_input(Game *game) {
	GameSession *session = &game->session;
	World *world = &session->world;
	const Vector2 snapped_pos = get_snapped_mouse_pos_in_world(session); 

	// Fetch tile coordinates.
	u32 out_screen, out_x, out_y;
	world_pos_to_tile(world, snapped_pos, &out_screen, &out_x, &out_y);

	// Ensure the current screen is valid.
	Screen *screen = world_get_screen_from_pos(world, snapped_pos);
	if (!screen) {
		fprintf(stderr, "[EDITOR]: world_get_screen_from_pos() returned NULL!\n");
		return;
	}

	// Fetch the tile_index and ensure it is also valid.
	u32 screen_index = screen->grid_y * world->grid_width + screen->grid_x;
	u32 tile_index   = get_tile_index(world, screen_index, out_x, out_y);
	if (tile_index >= world->tiles.count) {
		fprintf(stderr, "[EDITOR]: get_tile_index() returned an index outside the world tile count!\n");
		return;
	}

	// Finally, place / erase the tile.
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		world->tiles.data[tile_index].type = TILE_SOLID;
		printf("Placed tile at index: %u.\n", tile_index);
	}
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
		world->tiles.data[tile_index].type = TILE_EMPTY;
		printf("Erased tile at index: %u.\n", tile_index);
	}
		
	// Saving the world to disk.
	if (IsKeyPressed(KEY_S) && IsKeyDown(KEY_LEFT_CONTROL)) {
		bool saved = save_world(world);
		if (saved) {
			printf("[EDITOR]: World successfully saved.\n");
		} else {
			fprintf(stderr, "[EDITOR]: Failed to save world.\n");
		}
	}
}

Vector2 get_snapped_mouse_pos_in_world(const GameSession* session) {
	const World *world = &session->world;
	const float tile_size = world->tile_size;

	const Vector2 mouse_pos = GetMousePosition();
	const float scaled_x = (float)g_app->game_width  / (float)g_app->width;
	const float scaled_y = (float)g_app->game_height / (float)g_app->height;
	const Vector2 scaled_mouse_pos = { mouse_pos.x * scaled_x, mouse_pos.y * scaled_y }; 
	const Vector2 mouse_pos_in_world = GetScreenToWorld2D(scaled_mouse_pos, session->camera.raylib_cam);
	
	Vector2 result;
	result.x = floorf(mouse_pos_in_world.x / tile_size) * tile_size;
	result.y = floorf(mouse_pos_in_world.y / tile_size) * tile_size;
	return result;
}
