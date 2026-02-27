#include "console.h"

#include "application.h"

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

	// console->openness = target_openness;
	
	// Console animation.
	float diff = target_openness - console->openness;
	if (fabs(diff) > 0.001f) {
		console->openness += diff * OPENNESS_DT * (dt * 60.0f);
	} else {
		console->openness = target_openness;
	}
}

void init_console(Console *console) {
	// This is where we will load console arguments...
}

void draw_console(Console *console) {
	// printf("Drawing console!!!\n");
	float dt = g_app->dt;
	update_openness(console, dt);

	// We compute the full console rectangle that includes the log and input field
	// areas to ensure areas of the console don't clip into the viewport. This is only
	// an issue now since we keep drawing the console, so maybe once we have a member in the
	// console struct that toggles when the console is done animating, we can base it off whether
	// we make the draw call or not, but for now, this needs to get done.
	const float CONSOLE_X 	   = 0.0f;
	const float CONSOLE_Y      = 0.0f;
	const float CONSOLE_WIDTH  = (float)g_app->game_width;
	const float CONSOLE_HEIGHT = (float)console->openness;

	const float INPUT_X      = 0.0f;
	const float INPUT_HEIGHT = 50.0f;
	const float INPUT_Y      = CONSOLE_HEIGHT - INPUT_HEIGHT;
	const float INPUT_WIDTH  = CONSOLE_WIDTH;

	const float LOG_X 	   = CONSOLE_X;
    const float LOG_Y	   = CONSOLE_Y;	
	const float LOG_WIDTH  = CONSOLE_WIDTH;
	const float LOG_HEIGHT = CONSOLE_HEIGHT - INPUT_HEIGHT;

	Rectangle log_rect = { LOG_X, LOG_Y, LOG_WIDTH, LOG_HEIGHT };
	Color log_bg_color = { 25, 25, 25, 255 };

	Rectangle input_rect = { INPUT_X, INPUT_Y, INPUT_WIDTH, INPUT_HEIGHT };
	Color input_bg_color = { 18, 75, 75, 200 };

	// Drawing both the report log and the input field area.
	DrawRectangleRec(log_rect, log_bg_color);
	DrawRectangleRec(input_rect, input_bg_color);
}

