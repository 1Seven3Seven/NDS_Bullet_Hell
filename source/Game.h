#ifndef BULLET_HELL_GAME_H
#define BULLET_HELL_GAME_H

#include "GFX.h"
#include "Bullet.h"
#include "Entity.h"
#include "Enemies.h"

/// Data for spawning enemies.
/// Do not touch.
extern const int GameEnemySpawnData[3][8][4];
//                                  ^  ^  ^-- [x, y, move_direction or vx, vy]
//                                  |  |----- Number of enemies to chose from
//                                  |-------- Enemy index, 0 = sentinel, 1 = shredder, 2 = miner

/// The indexes of enemies to be selected from the GameEnemySpawnData.
/// Do not touch.
extern int GameEnemySpawnIndexes[3][8];

/// As there can be four sets of two enemies.
/// Enemies are loaded in as pairs.
/// Four sets of shredders and miners seems impossible, so there are only three sets of them.
/// Do not touch.
extern EntityType GameEnemiesToSpawn[10];

/// Shuffles `GameEnemySpawnIndexes` to randomise the enemy locations.
/// Uses `GameEnemySpawnIndexes` as global.
void GameShuffleEnemySpawnIndexes();

/// Shuffles `GameEnemiesToSpawn` to randomise the enemies to fight against.
void GameShuffleEnemiesToSpawn();

/// Randomises the enemy spawn data.
/// Should only be called once before a level, to keep the enemy loading consistent.
void GameRandomiseEnemySpawns();

//
// Loading enemies
//

/// Load a sentinel into the enemy array from the given spawn data.
void GameLoadSentinel(
    Entity enemy_array[],
    int enemy_array_len,
    int sentinel_move_directions[],
    const int sentinel_spawn_data[4]);

/// Load a shredder into the enemy array from the given spawn data.
void GameLoadShredder(
    Entity enemy_array[],
    int enemy_array_len,
    float shredder_move_vectors[8][2],
    const int shredder_spawn_data[4]);

/// Load a miner into the enemy array from the given spawn data.
void GameLoadMiner(
    Entity enemy_array[],
    int enemy_array_len,
    float miner_move_vectors[8][2],
    int miner_mine_delay[8],
    const int miner_spawn_data[4]);

//
// Random loading of enemies
//

/// Loads enemies using the data from GameEnemySpawnIndexes and GameEnemiesToSpawn to index GameEnemySpawnData.
///      A set of enemies is exactly 2 of them, that is there is always a multiple of 2 enemies.
///
/// Initialises the enemy_array in the process.
void GameLoadNumberOfRandomEnemySets(
    Entity enemy_array[],
    int enemy_array_len,
    EnemiesEnemyDataStruct *all_enemy_data,
    int num_enemy_sets);

//
// The fun part, sector setup
// PORTALS, WOO
//

/// Sets up the sector.
/// Initialises the player, bullet array and enemy array.
/// Loads the random enemies.
///      This will produce the same result till GameRandomiseEnemySpawns is called again
///
/// This is its own game loop that lasts for 64 frames (I think, maybe 65), it does not handle any input and is purely
///  aesthetic.
///
/// `GameRunGameLoop` should be run after.
void GameSectorSetup(
    Entity *player,
    Entity enemy_array[],
    int enemy_array_len,
    Bullet bullet_array[],
    int bullet_array_len,
    int *frame_number,
    EnemiesEnemyDataStruct *all_enemy_data,
    _GFXAllSpriteGFX *all_sprite_gfx,
    int num_enemy_sets);

/// Runs the game loop till either:
///
///     1. All the enemies are dead and there are no enemy bullets.
///          Returns 1.
///     2. The player dies.
///          Returns 0.
///     3. Select is pressed.
///          Returns -1.
///          Considered pause, re-run `GameRunGameLoop` to resume.
///
/// Assumes `GameSectorSetup` has already been run.
/// The difficulty determines if death bullets are spawned when enemies die, 1 = yes, 0 = no.
int GameRunGameLoop(
    Entity *player,
    Entity enemy_array[],
    int enemy_array_len,
    Bullet bullet_array[],
    int bullet_array_len,
    int *frame_number, EnemiesEnemyDataStruct *all_enemy_data,
    _GFXAllSpriteGFX *all_sprite_gfx,
    int playable_area[4],
    int hitbox_array[][4],
    int hitbox_array_len,
    int difficulty);

#endif // BULLET_HELL_GAME_H
