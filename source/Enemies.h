#ifndef BULLET_HELL_ENEMIES_H
#define BULLET_HELL_ENEMIES_H

#include <nds.h>

#include "GFX.h"

#include "Bullet.h"
#include "Entity.h"

/// Please don't create one of these.
/// There is an initialised one already floating around called `EnemiesAllEnemyData`.
typedef struct
{
    /// The current direction of movement, 0 = horizontal, 1 = vertical.
    int SentinelMoveDirections[8];
    /// The vector the shredder at the corresponding index is moving.
    float ShredderMoveVectors[8][2];
    /// The vector the miner at the corresponding index is moving.
    float MinerMoveVectors[8][2];
    /// The current delay for dropping a mine, to make the miners pause, then drop.
    int MinerMineDelays[8];
} EnemiesEnemyDataStruct;

/// A struct containing information about the entities.
/// Mainly used to just keep everything under one name.
/// Do not edit.
extern EnemiesEnemyDataStruct EnemiesAllEnemyData;

///
void EnemiesDrawAll(
    Entity enemy_array[],
    int enemy_array_len,
    int priority,
    int frame_number,
    const EnemiesEnemyDataStruct *all_enemy_data,
    _GFXAllSpriteGFX *all_sprite_gfx);

///
void EnemiesHandleAll(
    Entity enemy_array[],
    int enemy_array_len, Bullet bullet_array[],
    int bullet_array_len,
    int player_center[2],
    EnemiesEnemyDataStruct *all_enemy_data,
    int hitbox_array[][4],
    int hitbox_array_len);

/// Checks for collisions between the enemies and the player.
/// If there is a collision, then the type of the enemy is returned, otherwise the type of the player is returned.
int EnemiesCheckCollisionAgainstPlayer(Entity enemy_array[], int enemy_array_len, Entity *player);

#endif // BULLET_HELL_ENEMIES_H
