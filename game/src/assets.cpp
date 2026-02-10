#include "assets.h"

#include "iostream"

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

void load_all_assets(AssetManager *asset_manager) {
	// @TODO: We have some temporary sounds that we want to test first, but we
	// will be loading textures and fonts + other stuff later...

	InitAudioDevice();

	const char *base = GetApplicationDirectory();
	// load_all_textures(asset_manager, base);
	load_all_sounds(asset_manager, base);
	// load_all_fonts(asset_manager, base);
}

void unload_all_assets(AssetManager *asset_manager) {
	CloseAudioDevice();
}

Texture2D *get_texture_from_id(AssetManager *asset_manager, TextureID id) {
	assert(id >= 0 && id < TEXTURE_COUNT);
	return &asset_manager->textures[id];
}

Sound *get_sound_from_id(AssetManager *asset_manager, SoundID id) {
	assert(id >= 0 && id < SOUND_COUNT);
	return &asset_manager->sounds[id];
}

Font *get_font_from_id(AssetManager *asset_manager, FontID id) {
	assert(id >= 0 && id < FONT_COUNT);
	return &asset_manager->fonts[id];
}
