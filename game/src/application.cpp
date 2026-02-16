#include "application.h"

#include "raymath.h" // For Clamp()

Application *g_app = nullptr;

static void calculate_game_viewport(Application *app) {
	const float game_aspect_ratio   = (float)app->game_width / (float)app->game_height;
	const float screen_aspect_ratio = (float)app->width      / (float)app->height;

	if (screen_aspect_ratio > game_aspect_ratio) { // Add letterboxing to left / right.
		const float viewport_width = app->height * game_aspect_ratio;
		const float x_offset       = (app->width - viewport_width) / 2.0f;

		app->game_viewport.x 	  = x_offset;
		app->game_viewport.y 	  = 0;
		app->game_viewport.width  = viewport_width;
		app->game_viewport.height = app->height;

	} else { // Add letterboxing to bottom / top
		const float viewport_height = app->width / game_aspect_ratio;
		const float y_offset        = (app->height - viewport_height) / 2.0f;

		app->game_viewport.x 	  = 0;
		app->game_viewport.y 	  = y_offset;
		app->game_viewport.width  = app->width;
		app->game_viewport.height = viewport_height;
	}
}

void init_app(Application *app) {
	g_app = app;
	// Setting window size based on user preference (should be loaded from a config eventually).
	switch (app->res) {
	case _1280x720_: {
		app->width  = 1280;
		app->height = 720;
		break;
	}
	case _1366x768_: {
		app->width  = 1366;
		app->height = 768;
		break;
	}
	case _1920x1080_: {
		app->width  = 1920;
		app->height = 1080;
		break;
	}
	default: {
		app->width  = 1280;
		app->height = 720;
		break;
	}
	}
	
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
	InitWindow(app->width, app->height, app->title);
	app->monitor = GetCurrentMonitor();
	app->monitor_width  = GetMonitorWidth(app->monitor);
	app->monitor_height = GetMonitorHeight(app->monitor);

	switch (app->display_mode) {
	case WINDOWED_MODE: {
		// Already initialized...
		break;
	}
	case FULLSCREEN_MODE: {
		app->width  = app->monitor_width;
		app->height = app->monitor_height;
		SetWindowSize(app->width, app->height);
		ToggleFullscreen();
		break;
	}
	case BORDERLESS_WINDOWED_MODE: {
		app->width  = app->monitor_width;
		app->height = app->monitor_height;
		SetWindowSize(app->width, app->height);
		ToggleBorderlessWindowed();
		break;
	}
	default: {
		break;
	}
   	}

	SetTargetFPS(app->target_fps);
	SetExitKey(KEY_Q);
	app->should_close = false;

	app->game_render_target = LoadRenderTexture(app->game_width, app->game_height);
	SetTextureFilter(app->game_render_target.texture, TEXTURE_FILTER_BILINEAR);
	calculate_game_viewport(app);

	g_asset_manager = &app->asset_manager;
	load_all_assets(&app->asset_manager);
	
	init_game(&app->game);
	
	return;
}

void update_app(Application *app) {
	app->dt = GetFrameTime();
	app->dt = Clamp(app->dt, 0.0f, (1.0f / 30.0f));

	if (app->should_close) return;
	if (IsWindowResized()) {
		app->width  = GetRenderWidth();
		app->height = GetRenderHeight();
		calculate_game_viewport(app);
	}

	update_game(&app->game, app->dt);
}

void draw_app(Application *app) {
	BeginTextureMode(app->game_render_target);
	ClearBackground(RAYWHITE);
	// BeginBlendMode(BLEND_ALPHA);

	draw_game(&app->game);
	
	// EndBlendMode();
	EndTextureMode();

	BeginDrawing();
	ClearBackground(WHITE); // Letterboxing color (white for temp).
	Rectangle src;
	src.x = 0;
	src.y = 0;
	src.width  = (float)app->game_render_target.texture.width;
	src.height = (float)-app->game_render_target.texture.height; // Negative to flip the image (raylib stuff).

	const Vector2 origin = {0, 0};
	const float rotation = 0.0f;
	DrawTexturePro(app->game_render_target.texture, src, app->game_viewport, origin, rotation, WHITE);

	EndDrawing();
}

void shutdown_app(Application *app) {
	g_app = nullptr;
	g_asset_manager = nullptr;
	
	unload_all_assets(&app->asset_manager); // This also closes the raylib audio device.
	CloseWindow();
}
