#include "console.h"

#include "application.h"

inline static bool is_space(char c) {
	return c == ' ' || c == '\t';
}

inline static void clear_input_area(Console *console) {
	memset(console->input.data, 0, sizeof(console->input.data));

	console->input.length            = 0;
	console->input.cursor_pos 		 = 0;
	console->input.cursor_blink_time = 0.0f;
}

static void update_openness(Console *console, float dt) {
	// Openness relates to the actual pixels in y.
	const int SMALL_OPENNESS_TARGET = (int)(g_app->game_height * 0.4f);
	const int BIG_OPENNESS_TARGET   = (int)(g_app->game_height * 0.95f);
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
	// First, draw the rectangle that will contain the logs.
	const float LOG_X 	   = console->rect.x;
    const float LOG_Y	   = console->rect.y;
	const float LOG_WIDTH  = console->rect.width;
	const float LOG_HEIGHT = console->rect.height - console->input.height;
	
	Rectangle log_rect = { LOG_X, LOG_Y, LOG_WIDTH, LOG_HEIGHT };
	Color log_bg_color = { 25, 25, 25, 255 };
	
	DrawRectangleRec(log_rect, log_bg_color);

	// Then, draw the actual logs.
	const int log_padding = 6;
	const int log_count = console->log_buffer.log_count;
	const int log_line_height = 36;

	const int font_size = 32;
	const int text_x    = LOG_X + log_padding;
	int text_y          = LOG_Y + LOG_HEIGHT - font_size - log_padding;
	Color text_color;

	BeginScissorMode(LOG_X, LOG_Y, LOG_WIDTH, LOG_HEIGHT);

	for (int i = log_count - 1; i >= 0; --i) {
		if (text_y + font_size < LOG_Y + font_size) break;

		ConsoleLog *log = &console->log_buffer.logs[i];
		char *text = log->message; 

		switch (log->type) {
		case CONSOLE_LOG_COMMAND: { text_color = RAYWHITE; break; }
		case CONSOLE_LOG_OUTPUT:  { text_color = GREEN;    break; }
		case CONSOLE_LOG_ERROR:   { text_color = RED;      break; }
		case CONSOLE_LOG_WARNING: { text_color = ORANGE;   break; }
		case CONSOLE_LOG_INFO:    { text_color = RAYWHITE; break; }
			
		default: { text_color = RAYWHITE; break; } 
		}
		
		DrawText(text, text_x, text_y, font_size, text_color);
		text_y -= log_line_height;
	}

	EndScissorMode();
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
	const char *text       = console->input.data;
	const int TEXT_X       = (int)INPUT_X + 6;
	const int TEXT_Y       = (int)INPUT_Y;
	const int FONT_SIZE    = (int)INPUT_HEIGHT - 6;
	const int text_width   = MeasureText(text, FONT_SIZE);
	const Color text_color = { 50, 200, 100, 255 };

	const float CURSOR_X      = text_width + 10;
	const float CURSOR_Y      = INPUT_Y + 8;
	const float CURSOR_WIDTH  = 8.0f;
	const float CURSOR_HEIGHT = FONT_SIZE - 10;
	const Rectangle cursor_rect  = { CURSOR_X, CURSOR_Y, CURSOR_WIDTH, CURSOR_HEIGHT };
	const Color cursor_color     = text_color;

	// Draw calls below.
	DrawRectangleRec(input_rect, input_bg_color);
	DrawText(text, TEXT_X, TEXT_Y, FONT_SIZE, text_color);

	// Cursor blink.
	float dt = g_app->dt;
	console->input.cursor_blink_time += dt;
	if (console->input.cursor_blink_time <= 1.0f) {
		DrawRectangleRec(cursor_rect, cursor_color);
	} else if (console->input.cursor_blink_time > 2.0f) {
		console->input.cursor_blink_time = 0.0f;
	}
}

void init_console(Console *console) {
	// Member initialization.
	console->is_initialized = true;
	console->rect = { 0.0f, 0.0f, (float)g_app->game_width, 0.0f };
	console->state = CONSOLE_CLOSED;
	console->history_index = -1;
																
	// Input area initialization.
	console->input.length = 0;
	console->input.height = 50.0f;
	
	console->input.cursor_pos        = 0;
	console->input.cursor_blink_time = 0.0f;

	// Console arena stuff...
	console->log_buffer.arena = (char *)malloc(CONSOLE_ARENA_SIZE);
	if (!console->log_buffer.arena) {
		// @TODO: Error diagnostic for no memory allocated.
		console->is_initialized = false;
	}
	console->log_buffer.log_count = 0;
	console->log_buffer.offset    = 0;
	console->log_buffer.capacity  = CONSOLE_ARENA_SIZE;

	console->history.arena = (char *)malloc(CONSOLE_HISTORY_SIZE);
	if (!console->history.arena) {
		// @TODO: Error diagnostic for no memory allocated.
		console->is_initialized = false;
	}
	console->log_buffer.log_count = 0;
	console->log_buffer.offset    = 0;
	console->log_buffer.capacity  = CONSOLE_ARENA_SIZE;
	
	// @TODO: Load up console arguments here.

	if (console->is_initialized) {
		push_log(console, "This is the console. Type 'help' for more commands.", CONSOLE_LOG_INFO);
	} else {
		fprintf(stderr, "Console has not been initialized correctly!\n");
	}
}

void draw_console(Console *console) {
	float dt = g_app->dt;
	update_openness(console, dt);

	draw_logs(console);
	draw_input_area(console);
}

void cleanup_console(Console *console) {
	free(console->log_buffer.arena);
	free(console->history.arena);
	
	console->log_buffer.arena = nullptr;
	console->history.arena    = nullptr;
}

void insert_character(Console *console, int character) {
	if (character == '`' || character == '~') return;

	if (console->input.cursor_pos >= (CONSOLE_INPUT_SIZE - 1)) {
		// @TODO: Error handling.
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
}

void delete_word(Console* console) {
	if (console->input.cursor_pos <= 0) return;
	int old_pos = console->input.cursor_pos;
	char *text  = console->input.data;

	// First, eat all the whitespaces, then the word
	while (console->input.cursor_pos > 0 && is_space(text[console->input.cursor_pos - 1])) {
		console->input.cursor_pos--;
	}
	while (console->input.cursor_pos > 0 && !is_space(text[console->input.cursor_pos - 1])) {
		console->input.cursor_pos--;
	}

	int num_of_chars_to_delete = old_pos - console->input.cursor_pos;
	if (num_of_chars_to_delete > 0) {
		// Move everything from the old_pos to the current_pos.
		// Src: where the text originally starts (old_pos).
		// Dest: where the new text should go to (current_pos).
		// Size: remaining text with its null terminator.
		memmove(&text[console->input.cursor_pos], &text[old_pos],
				console->input.length - old_pos + 1);
		
		console->input.length -= num_of_chars_to_delete;
		console->input.cursor_blink_time = 0.0f;
	}
}

void submit_command(Console *console) {
	if (console->input.length == 0) return; 
	char *command = console->input.data;
	ConsoleLogType type = CONSOLE_LOG_COMMAND;

	push_log(console, command, type);
	// add_command_to_history(console, command);
	// parse_and_tokenize(console, command);
	clear_input_area(console);
}

void push_log(Console *console, const char *message, ConsoleLogType type) {

	ConsoleLogBuffer *buffer = &console->log_buffer;
	int message_length = strlen(message) + 1; // Accounting for the null terminator.

	if ((buffer->offset + message_length) > buffer->capacity) {
		// @TODO: Handle full arena.
		return;
	}
	if (buffer->log_count >= CONSOLE_MAX_LOGS) {
		// @TODO: Handle full log array.
		return;
	}

	char *dest = &buffer->arena[buffer->offset];
	memcpy(dest, message, message_length);
	buffer->offset += message_length;

	ConsoleLog *log = &buffer->logs[buffer->log_count++];
	log->message = dest;
	log->type    = type;

#if 1
	printf("Console received: %s | log_count: %d\n", log->message, console->log_buffer.log_count);
#endif

	return;
}
