#include "console.h"

#include "application.h"
#include "commands.h"

inline static bool is_space(char c) {
	return c == ' ' || c == '\t';
}

inline static void clear_input_area(Console *console) {
	memset(console->input.data, 0, sizeof(console->input.data));

	console->input.length            = 0;
	console->input.cursor_pos 		 = 0;
	console->input.cursor_blink_time = 0.0f;

	// console->history_index = -1; 
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

// @TODO: So currently, this procedure does indeed wrap text, but there are some issues with it.
// For one, we constantly do this strbuild_append thing every frame, when we could much rather just
// do it once. In addition, there are a lot of string_copy() calls which could be reduced to none.
// Finally, during some of the appends, in the event that a log that gets pushed appears to end
// perfectly at the end of the screen (check toggle_editor() in commands.cpp to see the log itself),
// we end up hitting the assert(data) that is seen within the strbuild_append(), so there's
// a lot of stuff to fix here.	-nathan, 07/05/2026.
static Array<String> wrap_text(String message, const float max_width) {
	Arena *scratch = get_current_arena_frame();
	Array<String> lines = {};
	array_init(&lines, scratch, 64); // @Hardcode.
	if (string_is_empty(message)) return lines;

	const Font *font = get_font(FONT_CONSOLE);
	const float font_size = CONSOLE_FONT_SIZE;
	const float font_spacing = 0.0f;

//	StringBuilder sb = {};
//	String current_line = {};
//	String current_word = {};
	StringBuilder line_builder = {};
	StringBuilder word_builder = {};

	for (s32 i = 0; i < message.length; ++i) {
		char c = message.data[i];
		if (c == ' ' || c == '\n') {
			// Build a test string with the current_line + " " + current_word.
			StringBuilder test_builder = {};
			if (line_builder.buffer.length == 0) {
				strbuild_append_string(scratch, &test_builder, word_builder.buffer);
			} else {
				strbuild_append_string(scratch, &test_builder, line_builder.buffer);
				strbuild_append_cstring(scratch, &test_builder, " ");
				strbuild_append_string(scratch, &test_builder, word_builder.buffer);
			}
			
			String test = strbuild_terminate(scratch, &test_builder);

			Vector2 measured = measure_text_ex_with_string(font, test, font_size, font_spacing);
			if (measured.x > max_width && line_builder.buffer.length > 0) {
				array_add(&lines, string_copy(scratch, line_builder.buffer));
				
				// New line becomes the current word.
				strbuild_reset(&line_builder);
				strbuild_append_string(scratch, &line_builder, word_builder.buffer);
			} else {
				// Accept the combined line.
				strbuild_reset(&line_builder);
				strbuild_append_string(scratch, &line_builder, test);
			}

			// Reset current word, then handle the newline.
			strbuild_reset(&word_builder);
			if (c == '\n') {
				if (line_builder.buffer.length > 0) {
					array_add(&lines, string_copy(scratch, line_builder.buffer));
				}
				strbuild_reset(&line_builder);
			}
		} else {
			strbuild_append_char(scratch, &word_builder, c);
		}
	}
	
	// Handle the remaining word and line.
	if (word_builder.buffer.length > 0) {
		StringBuilder test_builder = {};
		if (line_builder.buffer.length == 0) {
			strbuild_append_string(scratch, &test_builder, word_builder.buffer);
		} else {
			strbuild_append_string(scratch, &test_builder, line_builder.buffer);
			strbuild_append_cstring(scratch, &test_builder, " ");
			strbuild_append_string(scratch, &test_builder, word_builder.buffer);
		}
						
		String test = strbuild_terminate(scratch, &test_builder);

		Vector2 measured = measure_text_ex_with_string(font, test, font_size, font_spacing);
		if (measured.x > max_width && line_builder.buffer.length > 0) {
			array_add(&lines, string_copy(scratch, line_builder.buffer));
			array_add(&lines, string_copy(scratch, word_builder.buffer));
		} else {
			array_add(&lines, string_copy(scratch, test));
		}
	} else if (line_builder.buffer.length > 0) {
		array_add(&lines, string_copy(scratch, line_builder.buffer));
	}
	return lines;
}

static void ensure_log_is_wrapped(Console *console, ConsoleLog *log, float max_width) {
	if (log->cached_width == max_width && log->wrapped_lines.count > 0) return;

	// We need to invalidate and re-wrap since we can't free individual arena allocations.
	// Therefore, we just reset the array count and re-use the existing capacity.
	log->wrapped_lines.count = 0;

	Arena *scratch = get_current_arena_frame();
	String message = string_create(scratch, log->message);

	// Wrap text into the console->arena.
	Array<String> temp = wrap_text(message, max_width);
    /*
	for (s32 i = 0; i < temp.count; ++i) {
		String *s = array_get_at_index(&temp, i);
		String persistent = string_copy(&console->arena, *s);
		array_add(&log->wrapped_lines, persistent);
	}
    */
    For (temp) {
        String persistent = string_copy(&console->arena, *it);
        array_add(&log->wrapped_lines, persistent);
    }

	log->cached_width = max_width;
}

static void draw_logs(Console* console) {
    // First, draw the rectangle that will contain the logs.
    const float log_x      = console->rect.x;
    const float log_y      = console->rect.y;
    const float log_width  = console->rect.width;
    const float log_height = console->rect.height - console->input.height;
    Rectangle log_rect = { log_x, log_y, log_width, log_height };
    Color log_bg_color = { 25, 25, 25, 255 };
    DrawRectangleRec(log_rect, log_bg_color);

    // Then, draw the actual logs.
    const int log_padding = 6;
    const int log_count = console->log_buffer.log_count;
    const int log_line_height = CONSOLE_FONT_SIZE + 2.0f;
	const float max_text_width = log_width - (log_padding * 2);

    const Font *font      = get_font(FONT_CONSOLE);
    const int   font_size = CONSOLE_FONT_SIZE;

	const int text_x    = log_x + log_padding;
    int       text_y    = log_y + log_height - font_size - log_padding;
    Color     text_color;

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
        default:				  { text_color = RAYWHITE; break; } 
        }
		 
//		Arena *scratch = get_current_arena_frame();
//		Array<String> wrapped_lines = wrap_text(string_create(scratch, text), max_text_width);
		ensure_log_is_wrapped(console, log, max_text_width);
		for (s32 j = log->wrapped_lines.count - 1; j >= 0; --j) {
			if (text_y + font_size < log_y + font_size) break;
			
			const float font_spacing = 0.0f;
			Vector2 pos = { (float)text_x, (float)text_y };
			String *line = array_get_at_index(&log->wrapped_lines, j);
			draw_text_ex_with_string(font, *line, pos, font_size, font_spacing, text_color);
			text_y -= log_line_height;
		}
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
//	const Font *font         = get_font(FONT_CONSOLE_INPUT);
//	const float font_size    = font->baseSize;
	const Font *font         = get_font(FONT_CONSOLE);
	const float font_size    = CONSOLE_INPUT_FONT_SIZE;
	const float font_spacing = 0.0f;

	const float padding = 2;
	
	const char   *text            = console->input.data;
	const Vector2 text_pos        = {input_x + padding, input_y + padding};
//	const Vector2 text_pos        = { (float)(input_x + 2), (float)(input_y + 6) };
	const Vector2 text_dimensions = MeasureTextEx(*font, text, font_size, font_spacing);
	const Color   text_color      = GREEN;

	char text_before_cursor[CONSOLE_INPUT_SIZE] = { 0 };
	memcpy(text_before_cursor, console->input.data, console->input.cursor_pos);
	text_before_cursor[console->input.cursor_pos] = '\0';
	const Vector2 text_before_cursor_dimensions = MeasureTextEx(*font, text_before_cursor, font_size, font_spacing);

	const float cursor_x        = text_pos.x + text_before_cursor_dimensions.x;
	const float cursor_y        = input_y + padding*0.5f;
	const float cursor_width    = 4.0f;
	const float cursor_height   = font_size;
	const Rectangle cursor_rect = { cursor_x, cursor_y, cursor_width, cursor_height };
	const Color cursor_color    = text_color;

	// Draw calls below.
	DrawRectangleRec(input_rect, input_bg_color);
	DrawTextEx(*font, text, text_pos, font_size, font_spacing, text_color);

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
//	console->input.height = CONSOLE_FONT_SIZE * 2;
	console->input.height = CONSOLE_INPUT_FONT_SIZE + 2;
	
	console->input.cursor_pos        = 0;
	console->input.cursor_blink_time = 0.0f;

	arena_init(&console->arena, CONSOLE_ARENA_SIZE);
	if (!console->arena.base) {
		console->is_initialized = false;
		fprintf(stderr, "Console has not been initialized correctly!\n");
		return;
	}

	console->log_buffer.log_count = 0;
	console->history.count = 0;

	array_init(&console->history, &console->arena, CONSOLE_MAX_HISTORY);
	init_commands();
	
	push_log("This is the console. Type 'help' for more commands.", CONSOLE_LOG_INFO);
}

void draw_console(Console *console) {
	float dt = g_app->dt;
	update_openness(console, dt);

	draw_logs(console);
	draw_input_area(console);
}

void cleanup_console(Console *console) {
	arena_free(&console->arena);
	// dynamic_array_free(&console->history);
	cleanup_commands();
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
	const u32 MAX_TOKEN_COUNT = 16;
	if (console->input.length == 0) return;

	// First, add the command to the history.
	char *command = console->input.data;	
	array_add(&console->history, string_create(&console->arena, command));

	// Turn the inputted command into our String so that we can log it with
	// formatting + parse the tokens.
	Arena *scratch = get_current_arena_frame();
	String *output = push_array_to_arena(scratch, String, MAX_TOKEN_COUNT);
	String command_as_string = string_create(scratch, command);
	
	StringBuilder sb = {};
	strbuild_append_cstring(scratch, &sb, "> ");
	strbuild_append_string(scratch, &sb, command_as_string);
	const char *log_text = string_to_cstr(strbuild_terminate(scratch, &sb));
	push_log(log_text, CONSOLE_LOG_COMMAND);

	s32 token_count = string_split_whitespace(command_as_string, output, MAX_TOKEN_COUNT);
	ParseResult result = { output, token_count };
	run_command(command_as_string, &result);
	clear_input_area(console);
	console->history_index = -1;
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

void push_log(const char *message, ConsoleLogType type) {
	Console *console = &g_app->game.console;
	ConsoleLogBuffer *buffer = &console->log_buffer;
	if (buffer->log_count >= CONSOLE_MAX_LOGS) {
		// @TODO: Handle full log array.
		return;
	}

	ConsoleLog *log   = &buffer->logs[buffer->log_count++];
	log->type    	  = type;
	log->message      = string_copy_cstr(&console->arena, message).data;
	log->cached_width = 0.0f;
	array_init(&log->wrapped_lines, &console->arena, 8);
	return;
}

void push_log(String message, ConsoleLogType type) {
	push_log(message.data, type);
}

void navigate_command_history(Console *console, bool move_forward) {
	s32 history_count = (s32)console->history.count;
	if (history_count < 1) return;
	
	clear_input_area(console);
	if (console->history_index == -1) {
		console->history_index = history_count; // Start with the most recent history item.
	}
	history_count -= 1; // For zero-indexxing.

	if (!move_forward) {
		console->history_index--;
		if (console->history_index <= 0) {
			console->history_index = 0; // Clamp value.
		}
	} else {
		console->history_index++;
		if (console->history_index >= history_count) {
			console->history_index = history_count; // Clamp value.
		}
	}
	
	// printf("history_index: %d\n", console->history_index);
	
	String *command = array_get_at_index(&console->history, console->history_index);
	for (u32 i = 0; i < command->length; ++i) {
		insert_character(console, command->data[i]);
	}
}
