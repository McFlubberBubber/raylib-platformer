#ifndef CONSOLE_H
#define CONSOLE_H

#include "raylib.h"

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

struct ConsoleInput {
	char data[1024] = { 0 };
	int length;

	float height; // This is the height used for the rect in the input area.
	
	// Cursor-related things.
	int cursor_pos;
	float cursor_blink_time;

	float key_timer;
};

struct Console {
	ConsoleState state = CONSOLE_CLOSED;
	int openness;
	Rectangle rect;
	
	ConsoleInput input;
	ConsoleLog *logs;

	char **command_history;
	int history_index = -1;

	// @TODO: We need to put our actual commands that the console can look up in this struct aswell.
	
};

void init_console(Console *console);
void draw_console(Console *console);

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
