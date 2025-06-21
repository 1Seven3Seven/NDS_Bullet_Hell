#ifndef BULLET_HELL_MINER_H
#define BULLET_HELL_MINER_H

#include <nds.h>

#include "Bullet.h"
#include "Entity.h"

/// Sets up a miner at the given position.
void MinerSetup(Entity *self, int x, int y);

/// Sets up a miner in the entity array.
/// Same returns as EntitySetupInEntityArray.
int MinerSetupInEntityArray(Entity entity_array[], int entity_array_len, int x, int y);

/// Moves the miner with respect to its movement vector.
/// Changes the values within the movement vector when necessary.
void MinerMove(Entity *self, float vector[2], int hitbox_array[][4], int hitbox_array_len);

/// Animates the miner
void MinerAnimate(
    Entity *self,
    int priority,
    int frame_number,
    int id_offset,
    u16 *miner_gfx_mem[],
    u16 *enemy_explosion_gfx_mem[]);

/// Attempts to place a mine.
/// Uses the value within miner_place_mine_delay to delay placing the mine to produce, what I think is, a behaviour that
///  looks nicer.
void MinerPlaceMine(Entity *self, int *miner_place_mine_delay, Bullet bullet_array[], int bullet_array_len);

/// Checks for mines that are to explode and spawns the explosion bullets.
/// Changes the type of the previous mine to prevent this from running multiple times on the same mine.
void MinerSpawnMineExplosion(Bullet bullet_array[], int bullet_array_len);

#endif //BULLET_HELL_MINER_H
