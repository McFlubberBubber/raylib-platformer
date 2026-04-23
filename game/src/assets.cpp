#include "assets.h"

#include "iostream"

AssetManager *g_asset_manager = nullptr;

static void load_all_sounds(AssetManager *asset_manager, const char *base) {
	char full_path[256];
	const char *paths[] = {
		"data/audio/player_jump.wav",
	};

	for (int i = 0; i < SOUND_COUNT; ++i) {
		snprintf(full_path, sizeof(full_path), "%s%s", base, paths[i]);
		asset_manager->sounds[i] = LoadSound(full_path);

		if (!IsSoundValid(asset_manager->sounds[i])) {
			TraceLog(LOG_ERROR, "Failed to load sound: %s", full_path);
		}
	}
}

static void load_all_fonts(AssetManager *asset_manager, const char *base) {
	char full_path[256];

#if 0 
	const char *paths[] = {
		"data/fonts/Consolas-Regular.ttf",
	};

	for (int i = 0; i < FONT_COUNT; ++i) {
		snprintf(full_path, sizeof(full_path), "%s%s", base, paths[i]);
		asset_manager->fonts[i] = LoadFont(full_path);

		if (!IsFontValid(asset_manager->fonts[i])) {
			TraceLog(LOG_ERROR, "Failed to load font: %s", full_path);
		}
	}
#else
	const char *consolas = "data/fonts/Consolas-Regular.ttf";
	snprintf(full_path, sizeof(full_path), "%s%s", base, consolas);

//	asset_manager->fonts[FONT_CONSOLE]       = LoadFontEx(full_path, 32, NULL, 0);
	asset_manager->fonts[FONT_CONSOLE]       = LoadFontEx(full_path, 100, NULL, 0);
//  asset_manager->fonts[FONT_CONSOLE_INPUT] = LoadFontEx(full_path, 40, NULL, 0);
#endif

}

void load_all_assets(AssetManager *asset_manager) {
	InitAudioDevice();

	const char *base = GetApplicationDirectory();
	// load_all_textures(asset_manager, base);
	load_all_sounds(asset_manager, base);
	load_all_fonts(asset_manager, base);
}

void unload_all_assets(AssetManager *asset_manager) { CloseAudioDevice(); }

Texture2D *get_texture(TextureID id) {
	assert(g_asset_manager != nullptr);
	assert(id >= 0 && id < TEXTURE_COUNT);
	return &g_asset_manager->textures[id];
}

Sound *get_sound(SoundID id) {
	assert(g_asset_manager != nullptr);
	assert(id >= 0 && id < SOUND_COUNT);
	return &g_asset_manager->sounds[id];
}

Font *get_font(FontID id) {
	assert(g_asset_manager != nullptr);
	assert(id >= 0 && id < FONT_COUNT);
	return &g_asset_manager->fonts[id];
}

// This is a wrapper function around raylib's PlaySound().
void play_sound(SoundID id) {
	Sound *sound = get_sound(id);
	PlaySound(*sound);
}

void play_sound_with_pitch(SoundID id, float pitch) {
	Sound *sound = get_sound(id);
	SetSoundPitch(*sound, pitch);
	PlaySound(*sound);
}
