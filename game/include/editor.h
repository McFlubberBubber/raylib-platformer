#ifndef EDITOR_H
#define EDITOR_H

#include "raylib.h"

struct Game;
struct GameSession;

enum EditorMode {
	EDITOR_VIEW = 0,
	EDITOR_TILES,

	EDITOR_MODE_COUNT,
};

struct Editor {
	EditorMode mode = EDITOR_VIEW;
	bool is_active = false;
};

void draw_editor_view(Game *game);
void cycle_editor_mode(Editor *editor, bool do_increment);
void handle_tile_editor_input(Game *game);

Vector2 get_snapped_mouse_pos_in_world(const GameSession* session);

#endif
