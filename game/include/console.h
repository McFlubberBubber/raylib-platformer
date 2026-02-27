#ifndef CONSOLE_H
#define CONSOLE_H

enum ConsoleState {
	CONSOLE_CLOSED,
	CONSOLE_OPEN_SMALL,
	CONSOLE_OPEN_BIG
};

struct Console {
	ConsoleState state = CONSOLE_CLOSED;
	
	// float openness;
	int openness;
};

void init_console(Console *console);
void draw_console(Console *console);

#endif
