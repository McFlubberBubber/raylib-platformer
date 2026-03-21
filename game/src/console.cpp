#include "console.h"

#include "application.h"

// Read-only globals for the console specs
static void update_openness(Console *console, float dt) {
	// Openness relates to the actual pixels in y.
	const int SMALL_OPENNESS_TARGET = (int)(g_app->game_height * 0.2f);
	const int BIG_OPENNESS_TARGET   = (int)(g_app->game_height * 0.8f);
	const float OPENNESS_DT         = 0.5f;
	
	int target_openness = 0;
	
	switch (console->state) {
	case CONSOLE_CLOSED: {
		target_openness = 0;
		break;
	}
	case CONSOLE_OPEN_SMALL: {
		target_openness = SMALL_OPENNESS_TARGET;
		break;
	}
	case CONSOLE_OPEN_BIG: {
		target_openness = BIG_OPENNESS_TARGET;
		break;
	}
	}
	
	// Console animation.
	float diff = target_openness - console->openness;
	if (fabs(diff) > 0.001f) {
		console->openness += diff * OPENNESS_DT * (dt * 60.0f);
	} else {
		console->openness = target_openness;
	}

	console->rect.height = (float)console->openness;
}

static void draw_logs(Console* console) {
	const float LOG_X 	   = console->rect.x;
    const float LOG_Y	   = console->rect.y;
	const float LOG_WIDTH  = console->rect.width;
	const float LOG_HEIGHT = console->rect.height - console->input.height;
	
	Rectangle log_rect = { LOG_X, LOG_Y, LOG_WIDTH, LOG_HEIGHT };
	Color log_bg_color = { 25, 25, 25, 255 };
	
	DrawRectangleRec(log_rect, log_bg_color);	
}

static void draw_input_area(Console *console) {
	// Specifications for the input rect.
	const float INPUT_X      = console->rect.x;
	const float INPUT_Y      = console->rect.height - console->input.height;
	const float INPUT_WIDTH  = console->rect.width;
	const float INPUT_HEIGHT = console->input.height;
	const Rectangle input_rect = { INPUT_X, INPUT_Y, INPUT_WIDTH, INPUT_HEIGHT };
	const Color input_bg_color = { 18, 75, 75, 200 };

	// Specifications for the text and the cursor.
	const char *text    = console->input.data;
	const int x         = (int)INPUT_X + 6;
	const int y         = (int)INPUT_Y;
	const int font_size = (int)INPUT_HEIGHT - 6;
	const Color text_color = { 50, 200, 100, 255 };

	// Draw calls.
	DrawRectangleRec(input_rect, input_bg_color);
	DrawText(text, x, y, font_size, text_color);
}

void init_console(Console *console) {
	console->rect = { 0.0f, 0.0f, (float)g_app->game_width, 0.0f };

	// Input area initialization.
	console->input.length     = 0;
	console->input.height     = 50.0f;
	console->input.cursor_pos = 0;
	
	// This is where we will load console arguments...
}

void draw_console(Console *console) {
	float dt = g_app->dt;
	update_openness(console, dt);

	draw_logs(console);
	draw_input_area(console);
}

void insert_character(Console *console, int character) {
	if (character == '`' || character == '~') return;

	if (console->input.cursor_pos >= 1023) {
		// @TODO: Error handling
	} else {
		int pos = console->input.cursor_pos;
		
		// Make room for the new character
		memmove(&console->input.data[pos + 1], &console->input.data[pos],
				console->input.length - pos + 1); // Moving the null terminator.

		console->input.data[pos]         = (char)character;
		console->input.cursor_pos       += 1;
		console->input.length           += 1;
		console->input.cursor_blink_time = 0.0f;
	}

	return;
}

void delete_character(Console *console) {
	if (console->input.cursor_pos == 0) return; 
	
	int pos = console->input.cursor_pos;
	
	// Shift everything after the cursor to the left.
	memmove(&console->input.data[pos - 1], &console->input.data[pos],
			console->input.length - pos + 1); // We do the +1 to move the null terminator.
	
	console->input.cursor_pos       -= 1;
	console->input.length           -= 1;
	console->input.cursor_blink_time = 0.0f;

	return;
}
