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
	const float log_x 	   = console->rect.x;
    const float log_y	   = console->rect.y;
	const float log_width  = console->rect.width;
	const float log_height = console->rect.height - console->input.height;
	
	Rectangle log_rect = { log_x, log_y, log_width, log_height };
	Color log_bg_color = { 25, 25, 25, 255 };
	
	DrawRectangleRec(log_rect, log_bg_color);

	// Then, draw the actual logs.
	const int log_padding = 6;
	const int log_count = console->log_buffer.log_count;
	const int log_line_height = 36;

	const Font *font = get_font(FONT_CONSOLE);
	const int font_size = 32;
	const int text_x    = log_x + log_padding;
	int text_y          = log_y + log_height - font_size - log_padding;
	Color text_color;

	// @TODO: Does this make a difference?
	BeginScissorMode(log_x, log_y, log_width, log_height);

	for (int i = log_count - 1; i >= 0; --i) {
		if (text_y + font_size < log_y + font_size) break;

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

		Vector2 pos = { (float)text_x, (float)text_y };
		const float spacing = 0.0f;
		DrawTextEx(*font, text, pos, font_size, spacing, text_color);

		text_y -= log_line_height;
	}

	EndScissorMode();
}

static void draw_input_area(Console *console) {
	// Specifications for the input rect.
	const float input_x      = console->rect.x;
	const float input_y      = console->rect.height - console->input.height;
	const float input_width  = console->rect.width;
	const float input_height = console->input.height;
	const Rectangle input_rect = { input_x, input_y, input_width, input_height };
	const Color input_bg_color = { 18, 75, 75, 200 };

	// Specifications for the text and the cursor.	
	const Font *font         = get_font(FONT_CONSOLE_INPUT);
	const float font_spacing = 0.0f;

	const char   *text            = console->input.data;
	const Vector2 text_pos        = { (float)(input_x + 2), (float)(input_y + 6) };
	const Vector2 text_dimensions = MeasureTextEx(*font, text, font->baseSize, font_spacing);
	const Color   text_color      = GREEN;

	char text_before_cursor[CONSOLE_INPUT_SIZE] = { 0 };
	memcpy(text_before_cursor, console->input.data, console->input.cursor_pos);
	text_before_cursor[console->input.cursor_pos] = '\0';
	const Vector2 text_before_cursor_dimensions = MeasureTextEx(*font, text_before_cursor, font->baseSize, font_spacing);

	const float cursor_x        = text_pos.x + text_before_cursor_dimensions.x;
	const float cursor_y        = input_y + 4;
	const float cursor_width    = 4.0f;
	const float cursor_height   = font->baseSize;
	const Rectangle cursor_rect = { cursor_x, cursor_y, cursor_width, cursor_height };
	const Color cursor_color    = text_color;

	// Draw calls below.
	DrawRectangleRec(input_rect, input_bg_color);
	DrawTextEx(*font, text, text_pos, font->baseSize, font_spacing, text_color);

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
	console->input.height = 48.0f;
	
	console->input.cursor_pos        = 0;
	console->input.cursor_blink_time = 0.0f;

	// Console arena stuff...
#if 0
	console->log_buffer.arena = (char *)malloc(CONSOLE_ARENA_SIZE);
#else
	console->log_buffer.arena = (char *)MemAlloc(CONSOLE_ARENA_SIZE);
#endif

	if (!console->log_buffer.arena) {
		// @TODO: Error diagnostic for no memory allocated.
		console->is_initialized = false;
	}
	console->log_buffer.log_count = 0;
	console->log_buffer.offset    = 0;
	console->log_buffer.capacity  = CONSOLE_ARENA_SIZE;

#if 0
	console->history.arena = (char *)malloc(CONSOLE_HISTORY_SIZE);
#else
	console->history.arena = (char *)MemAlloc(CONSOLE_HISTORY_SIZE);
#endif
	
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
#if 0
	free(console->log_buffer.arena);
	free(console->history.arena);
#else
	MemFree(console->log_buffer.arena);
	MemFree(console->history.arena);
#endif
	
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

void move_cursor_by_char(Console* console, bool is_forward) {
	int old_pos = console->input.cursor_pos;
	if (is_forward) {
		if (console->input.cursor_pos < console->input.length) {
			console->input.cursor_pos++;
		}
	} else {
		if (console->input.cursor_pos > 0) {
			console->input.cursor_pos--;
		}
	}

	if (console->input.cursor_pos != old_pos) {
		console->input.cursor_blink_time = 0.0f;
	}
}

void move_cursor_by_word(Console* console, bool is_forward) {
	int old_pos = console->input.cursor_pos;
	int *current_pos = &console->input.cursor_pos; // Aliasing.
//	int& current_pos = console->input.cursor_pos;

	char* text = console->input.data;
	int length = console->input.length;

	if (is_forward) { // Parse in front of the cursor position.
		if (*current_pos >= length) {
			return;
		}

		// Skip the current word first, then the whitespaces.
		while (*current_pos < length && !is_space(text[*current_pos])) {
			(*current_pos)++;
		}
		while (*current_pos < length && is_space(text[*current_pos])) {
			(*current_pos)++;
		}
		
	} else { // Parse behind the cursor position.
		if (*current_pos <= 0) {
			return;
		}

		// Skip the whitespaces first, then the previous word.
		while (*current_pos > 0 && is_space(text[*current_pos - 1])) {
			(*current_pos)--;
		}
		while (*current_pos > 0 && !is_space(text[*current_pos - 1])) {
			(*current_pos)--;
		}
	}

	if (*current_pos != old_pos) { console->input.cursor_blink_time = 0.0f; }
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
