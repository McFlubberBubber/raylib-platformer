#ifndef EDITOR_H
#define EDITOR_H

#include "raylib.h"

struct Game;

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

#endif
