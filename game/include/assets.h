#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h" // For definitions of asset structs
#include <assert.h>

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
Texture2D *get_texture_from_id(AssetManager *asset_manager, TextureID id);
Sound     *get_sound_from_id(AssetManager *asset_manager, SoundID   id);
Font      *get_font_from_id(AssetManager *asset_manager, FontID    id);

#endif
