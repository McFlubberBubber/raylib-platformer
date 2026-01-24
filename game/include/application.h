#pragma once

struct Application {
	const char *title = "Platformer";
	int target_fps = 60;

	int width  = 1280;
	int height = 720;

	void init();
	void update();
	void shutdown();
};

