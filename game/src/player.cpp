#include "player.h"

#include "application.h"
#include "world.h"

static void check_horizontal_collisions(Player *player, World *world) {
    const float tile_size = world->tile_size;

    float left         = player->sprite.x;
    float right        = player->sprite.x + player->sprite.width;
    float mid_y        = player->sprite.y + player->sprite.height * 0.5f;
    float top_inset    = player->sprite.y + 0.1f;
    float bottom_inset = player->sprite.y + player->sprite.height - 0.1f;

    if (player->vel.x > 0) {
        if (is_solid(world, right, top_inset) || is_solid(world, right, mid_y) ||
            is_solid(world, right, bottom_inset)) {
            player->sprite.x = floorf(right / tile_size) * tile_size - player->sprite.width;
            player->vel.x    = 0;
        }
    } else if (player->vel.x < 0) {
        if (is_solid(world, left, top_inset) || is_solid(world, left, mid_y) ||
            is_solid(world, left, bottom_inset)) {
            player->sprite.x = (floorf(left / tile_size) + 1.0f) * tile_size;
            player->vel.x    = 0;
        }
    }
}

static void check_vertical_collisions(Player *player, World *world) {
    player->is_grounded = false;

    const float tile_size = world->tile_size;
    float left   = player->sprite.x;
    float right  = player->sprite.x + player->sprite.width;
    float top    = player->sprite.y;
    float bottom = player->sprite.y + player->sprite.height;
    float mid_x  = player->sprite.x + player->sprite.width * 0.5f;

    float inset       = tile_size * 0.1f;
    float left_inset  = player->sprite.x + inset;
    float right_inset = player->sprite.x + player->sprite.width - inset;

    if (player->vel.y > 0) {
        float check_bottom = bottom + 0.1f;
        if (is_solid(world, left_inset, check_bottom) || is_solid(world, mid_x, check_bottom) ||
            is_solid(world, right_inset, check_bottom)) {
            player->sprite.y    = floorf(bottom / tile_size) * tile_size - player->sprite.height;
            player->vel.y       = 0;
            player->is_grounded = true;
        }
    } else if (player->vel.y < 0) {
        if (is_solid(world, left_inset, top) || is_solid(world, mid_x, top) ||
            is_solid(world, right_inset, top)) {
            player->sprite.y = (floorf(top / tile_size) + 1.0f) * tile_size;
            player->vel.y    = 0;
        }
    }

    if (!player->is_grounded) {
        float probe       = bottom + 1.0f;
        float left_probe  = player->sprite.x + inset;
        float right_probe = player->sprite.x + player->sprite.width - inset;
        float mid_probe   = player->sprite.x + player->sprite.width * 0.5f;

        bool left_hit  = is_solid(world, left_probe,  probe);
        bool mid_hit   = is_solid(world, mid_probe,   probe);
        bool right_hit = is_solid(world, right_probe, probe);

        if (left_hit || mid_hit || right_hit) player->is_grounded = true;
    }
}


void init_player(Player *player, const World *world) {
	float screen_pixel_width  = world_screen_pixel_width(world);
	float screen_pixel_height = world_screen_pixel_height(world);
	
	player->sprite.x = screen_pixel_width * 0.5f - (20 - 0.5f);
	player->sprite.y = 0;
	player->sprite.width  = 20;
	player->sprite.height = 20;

	player->vel.x = 0.0f;
	player->vel.y = 0.0f;

	player->state	    = PLAYER_IDLE;
	player->prev_state  = player->state;
	
	player->is_grounded = false;
	player->coyote = 0.0f;
	player->health = 3;
}

void update_player(Player *player, World *world, Input* input, float dt) {
	constexpr float gravity    =  1200.0f;
	constexpr float jump_force = -550.0f;
	constexpr float movement_speed  = 300.0f;  // Pixels per sec.
	constexpr float coyote_duration = 0.1f;    // 100ms grace.

	if (player->health == 0) return;

	// Updating velocity from input state.
	player->vel.x = input->player_move_x * movement_speed;

	// Gravity check by resetting velocity based on grounded flag.
	if (player->is_grounded)  player->vel.y = 0.0f;
	if (!player->is_grounded) player->vel.y += gravity * dt;

	// Apply movement to the sprite and resolve collisions.
	player->sprite.x += player->vel.x *dt;
	check_horizontal_collisions(player, world);
	player->sprite.y += player->vel.y *dt;
	check_vertical_collisions(player, world);

	// Check for jump input after the collision.
	if (player->is_grounded) player->coyote = coyote_duration;
	else                     player->coyote -= dt;

	if (player->coyote >= 0.0f && input->player_jump) {
		player->vel.y 		= jump_force;
		player->is_grounded = false;
		player->coyote      = 0.0f;
		
		// Playing jump sfx.
		float pitch = (float)(GetRandomValue(70, 90) / 100.0f);
		play_sound_with_pitch(SOUND_PLAYER_JUMP, pitch);
	}

	// Death by falling.
	float world_pixel_height = world_screen_pixel_height(world) * world->grid_height;
	if ((player->sprite.y + player->sprite.height) >= world_pixel_height + world->tile_size) {
        player->health = 0;
    }

	// State machine.
	player->prev_state = player->state;
	if (player->health <= 0) {
		player->state = PLAYER_DEAD;
	} else if (player->is_grounded) {
		player->state = (input->player_move_x != 0.0f) ? PLAYER_MOVING : PLAYER_IDLE;
	} else {
		player->state = (player->vel.y < 0.0f) ? PLAYER_JUMPING : PLAYER_FALLING;
	}
}

void draw_player(const Player *player) {
	DrawRectangleRec(player->sprite, RED);
}
