/*
	Welcome to the game source code to this platformer game that I had in mind, but this time,
	I will be using the services offered by raylib to create the game (unlike the LearningOpenGL
	project that required me to use the rendering API directly). This should streamline the process,
	and allow us to get to programming an actual "game" instead of worrying about the whole game
	engine / renderer aspect of it. So let's see how it goes!

	This project started on the 25th January, 2026 - Nathaniel Fernandes / McFlubberBubber
*/

#include "raylib.h"
#include "raymath.h" // For Clamp()
#include "application.h"

int main() {
	Application app = {};
	init_app(&app);

	while (!WindowShouldClose() && !app.should_close) {
		app.dt = GetFrameTime();
		app.dt = Clamp(app.dt, 0.0f, (1.0f / 30.0f));
		app.game.command_count = 0; // Clearing the command queue.
		
		poll_inputs(&app);
		update_app(&app);
		process_command_list(&app.game);
		draw_app(&app);
	}

	shutdown_app(&app);
	return 0;
}
