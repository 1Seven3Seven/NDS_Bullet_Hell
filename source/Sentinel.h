#ifndef BULLET_HELL_SENTINEL_H
#define BULLET_HELL_SENTINEL_H

#include <nds.h>

#include "Constants.h"
#include "Bullet.h"
#include "Entity.h"

/// Sets a sentinel at the given position.
void SentinelSetup(Entity *self, int x, int y);

/// Sets up a sentinel in the entity array.
/// Same returns as EntitySetupInEntityArray.
int SentinelSetupInEntityArray(Entity entity_array[], int entity_array_len, int x, int y);

/// Moves the sentinel with respect to the player's position.
/// The move_direction dictates which axis the sentinel moves along.
///      X-axis = 0.
///      Y-axis = 1.
///      Any other values will cause problems.
void SentinelMove(
    Entity *self,
    int move_direction,
    const int player_center[2],
    int hitbox_array[][4],
    int hitbox_array_len);

/// Animates the sentinel.
void SentinelAnimate(
    Entity *self,
    int priority,
    int frame_number,
    int id_offset,
    int move_direction,
    u16 *sentinel_gfx_mem[2][SENTINEL_ANIMATION_FRAMES],
    u16 *enemy_explosion_gfx_mem[]);

/// Attempts to fire a bullet at the player.
void SentinelFireBullet(
    Entity *self,
    int move_direction,
    const int player_center[2],
    Bullet bullet_array[],
    int bullet_array_len);

#endif
