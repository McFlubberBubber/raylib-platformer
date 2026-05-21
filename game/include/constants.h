#ifndef CONSTANTS_H
#define CONSTANTS_H

// This file includes a bunch of compile-time known variables that several files
// may want to read from as a reference. I decided to stash all of them here so
// I don't need to run around for variables. It also helps maintain this file as
// a single source of truth for plenty of these types. We may plan to refactor
// some of these into being hotloaded.

// In addition, the reason these are not being included in the general.h is because
// these variables are more related to the game itself, and not general utilities
// that I could carry over to other projects, so I dumped them here!

#include "general.h"
#include "raylib.h"

constexpr u32 DESIRED_FPS = 144;
constexpr u32 SCRATCH_ARENA_SIZE = megabytes(1);

// @TODO: Make this permanent arena include the console + world arena in the future. This is
// currently here only for hotloaded_vars bindings.
constexpr u32 PERMANENT_ARENA_SIZE = megabytes(2);
constexpr u32 MAX_VAR_BINDINGS     = 64;

constexpr u32 VARS_LINE_BUFFER_SIZE = 512;

constexpr u32 TILE_SIZE = 32;
constexpr u32 TILE_HALF = static_cast<u32>(TILE_SIZE * 0.5f);
constexpr u32 TILE_QUARTER = static_cast<u32>(TILE_SIZE * 0.25f);

constexpr u32 TILES_PER_COL = 40;
constexpr u32 TILES_PER_ROW = 23;

constexpr float WORLD_TILE_SIZE = static_cast<float>(TILE_SIZE);

constexpr u32 GAME_WIDTH  = TILE_SIZE * TILES_PER_COL;
constexpr u32 GAME_HEIGHT = TILE_SIZE * TILES_PER_ROW;

constexpr u32 DEFAULT_WINDOW_SCALE = 1;

constexpr s32 DEFAULT_FONT_SIZE = 16.0f;
// constexpr s32 CONSOLE_FONT_SIZE = 24.0f; // Moved to vars.
constexpr s32 CONSOLE_INPUT_FONT_SIZE = 32.0f; 

constexpr float EDITOR_PAN_SPEED = 5.0f;
constexpr float EDITOR_FAST_PAN_SPEED = 7.5f;

#endif
