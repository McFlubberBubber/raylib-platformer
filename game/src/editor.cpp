#include "raylib.h"

#include "platformer.h"
#include "editor.h"

static void draw_editor_ui(Game *game) {

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

