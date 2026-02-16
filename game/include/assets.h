#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h" // For definitions of asset structs
#include <assert.h>

struct AssetManager;
extern AssetManager *g_asset_manager;

enum TextureID {
	TEXTURE_TEMP = 0,
	
	TEXTURE_COUNT,
};

enum SoundID {
	SOUND_PLAYER_JUMP = 0,
	
	SOUND_COUNT,
};

enum FontID {
	FONT_TEMP = 0,
	
	FONT_COUNT,
};

struct AssetManager {
	Texture2D textures[TEXTURE_COUNT];
	Sound     sounds[SOUND_COUNT];
	Font      fonts[FONT_COUNT];
};

void load_all_assets(AssetManager *asset_manager);
void unload_all_assets(AssetManager *asset_manager);

// Utility functions.
Texture2D *get_texture(TextureID id);
Sound     *get_sound(SoundID id);
Font      *get_font(FontID id);

void play_sound(SoundID id);
void play_sound_with_pitch(SoundID id, float pitch);

#endif
