#ifndef CONSOLE_H
#define CONSOLE_H

#include "raylib.h"

const int CONSOLE_INPUT_SIZE = 512;
const int CONSOLE_ARENA_SIZE = (1024 * 64); // 64KB.
const int CONSOLE_MAX_LOGS   = 256;

const int CONSOLE_MAX_HISTORY  = 64;
const int CONSOLE_HISTORY_SIZE = (1024 * 64); // 64KB.

enum ConsoleLogType {
	CONSOLE_LOG_COMMAND = 0,
	CONSOLE_LOG_OUTPUT,
	CONSOLE_LOG_ERROR,
	CONSOLE_LOG_WARNING,
	CONSOLE_LOG_INFO,
};

enum ConsoleState {
	CONSOLE_CLOSED = 0,
	CONSOLE_OPEN_SMALL,
	CONSOLE_OPEN_BIG
};

struct ConsoleLog {
	ConsoleLogType type = CONSOLE_LOG_INFO;
	char *message;
};

struct ConsoleLogBuffer {
	ConsoleLog logs[CONSOLE_MAX_LOGS];
	int log_count;

	char  *arena; // Heap-allocated.
	int    offset;
	size_t capacity;
};

struct ConsoleHistory {
	char *entries[CONSOLE_MAX_HISTORY];
	int   count;
	
	char  *arena; // Heap-allocated.
	int    offset;
	size_t capacity;
};

struct ConsoleInput {
	char data[CONSOLE_INPUT_SIZE] = { 0 };
	int length;

	float height; // This is the height used for the rect in the input area.
	
	// Cursor-related things.
	int cursor_pos;
	float cursor_blink_time;

	float key_timer;
};

struct Console {
	ConsoleState state;
	int openness;
	Rectangle rect;
	
	ConsoleInput input;
	ConsoleLogBuffer log_buffer;
	ConsoleHistory history;

	int history_index;

	// @TODO: We need to put our actual commands that the console can look up in this struct aswell.

	bool is_initialized;
};

void init_console(Console *console);
void draw_console(Console *console);
void cleanup_console(Console *console);

// Utilities.
void insert_character(Console *console, int character);
void delete_character(Console *console);
void delete_word(Console *console);

void submit_command(Console *console);
void autocomplete_command(Console *console);

void move_cursor_by_char(Console *console, bool move_forward);
void move_cursor_by_word(Console *console, bool move_forward);
void navigate_command_history(Console *console, bool move_forward);

void push_log(Console *console, const char *message, ConsoleLogType type);

#endif
