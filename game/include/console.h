#ifndef CONSOLE_H
#define CONSOLE_H

#include "raylib.h"
#include "commands.h"
#include "general.h"

const s32 CONSOLE_INPUT_SIZE  = 512;
const s32 CONSOLE_MAX_LOGS    = 256;
const s32 CONSOLE_MAX_HISTORY = 64;

const s32 CONSOLE_ARENA_SIZE = kilobytes(128);

// Forward declarations.
struct Console;

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
};

struct ConsoleInput {
	char data[CONSOLE_INPUT_SIZE] = { 0 };
	int length;

	float height; // This is the height used for the rect in the input area.
	
	// Cursor-related things.
	int cursor_pos;
	float cursor_blink_time;
};

struct Console {
	Arena arena;

	ConsoleState state;

	int openness;
	Rectangle rect;
	
	ConsoleInput input;
	ConsoleLogBuffer log_buffer;

	Array<String> history;
	s32 history_index; // We use -1 as "not browsing history" mode.

	// Command commands[CMD_COUNT] = { 0 };

	bool is_initialized;
};

struct ParseResult {
	String *tokens;
	s32     count;
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
