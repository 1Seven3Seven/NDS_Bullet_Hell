#include "Game.h"

#include <nds.h>

#include <stdio.h>

#include "Constants.h"
#include "Other.h"

#include "UI.h"

#include "GFX.h"

#include "Bullet.h"

#include "Entity.h"
#include "Player.h"
#include "Sentinel.h"
#include "Shredder.h"
#include "Miner.h"

#include "Enemies.h"

const int GameEnemySpawnData[3][8][4] = {
        { // Sentinels [x, y, move_direction, NONE]
                {31,  15, 0,  -1},
                {15,  31,  1,  -1},
                {209, 161, 0,  -1},
                {225, 145, 1,  -1},
                {47, 31, 0,  -1},
                {31,  47,  1,  -1},
                {193, 145, 0,  -1},
                {209, 129, 1,  -1}
        },
        { // Shredders [x, y, NONE, NONE]
                {15,  15, -1, -1},
                {15,  162, -1, -1},
                {226, 15,  -1, -1},
                {226, 162, -1, -1},
                {15, 89, -1, -1},
                {226, 89,  -1, -1},
                {121, 15,  -1, -1},
                {121, 162, -1, -1}
        },
        { // Miners [x, y, vx, vy] vx and vy are to be divided by 10
                {120, 30, -5, -5},
                {120, 148, 5,  5},
                {30,  88,  -5, 5},
                {210, 88,  5,  -5},
                {31, 31, -5, 5},
                {31,  145, 5,  5},
                {209, 31,  -5, -5},
                {209, 145, 5,  -5}
        }
};

int GameEnemySpawnIndexes[3][8] = {
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 1, 2, 3, 4, 5, 6, 7}
};

int GameEnemiesToSpawn[10] = {
        SENTINEL_TYPE, SENTINEL_TYPE, SENTINEL_TYPE, SENTINEL_TYPE,
        SHREDDER_TYPE, SHREDDER_TYPE, SHREDDER_TYPE,
        MINER_TYPE, MINER_TYPE, MINER_TYPE
};

void GameShuffleEnemySpawnIndexes() {
    ShuffleIntArray(GameEnemySpawnIndexes[0], 8);
    ShuffleIntArray(GameEnemySpawnIndexes[1], 8);
    ShuffleIntArray(GameEnemySpawnIndexes[2], 8);
}

void GameShuffleEnemiesToSpawn() {
    ShuffleIntArray(GameEnemiesToSpawn, 10);
}

void GameRandomiseEnemySpawns() {
    GameShuffleEnemySpawnIndexes();
    GameShuffleEnemiesToSpawn();
}

void GameLoadSentinel(Entity enemy_array[], int enemy_array_len, int sentinel_move_directions[],
                      const int sentinel_spawn_data[4]) {
    int index = SentinelSetupInEntityArray(
            enemy_array, enemy_array_len,
            sentinel_spawn_data[0], sentinel_spawn_data[1]
    );

    sentinel_move_directions[index] = sentinel_spawn_data[2];
    enemy_array[index].current_bullet_delay = ENEMY_START_DELAY;
}

void GameLoadShredder(Entity enemy_array[], int enemy_array_len, float shredder_move_vectors[8][2],
                      const int shredder_spawn_data[4]) {
    int index = ShredderSetupInEntityArray(
            enemy_array, enemy_array_len,
            shredder_spawn_data[0], shredder_spawn_data[1]
    );

    shredder_move_vectors[index][0] = 0.f;
    shredder_move_vectors[index][1] = 0.f;
    enemy_array[index].current_bullet_delay = ENEMY_START_DELAY;
}

void GameLoadMiner(Entity enemy_array[], int enemy_array_len, float miner_move_vectors[8][2], int miner_mine_delay[8],
                   const int miner_spawn_data[4]) {
    int index = MinerSetupInEntityArray(
            enemy_array, enemy_array_len,
            miner_spawn_data[0], miner_spawn_data[1]
    );

    miner_move_vectors[index][0] = (float) miner_spawn_data[2] / 10;
    miner_move_vectors[index][1] = (float) miner_spawn_data[3] / 10;
    miner_mine_delay[index] = ENEMY_START_DELAY;
}

void GameLoadNumberOfRandomEnemySets(Entity enemy_array[], int enemy_array_len, EnemiesEnemyDataStruct *all_enemy_data,
                                     int num_enemy_sets) {
    EntityInitEntityArray(
            enemy_array, enemy_array_len
    );

    for (int i = 0; i < num_enemy_sets * 2; i += 2) {
        for (int j = 0; j < 2; j++) {
            switch (GameEnemiesToSpawn[i]) {
                case SENTINEL_TYPE:
                    GameLoadSentinel(
                            enemy_array, enemy_array_len,
                            all_enemy_data->SentinelMoveDirections,
                            GameEnemySpawnData[0][GameEnemySpawnIndexes[0][i + j]]
                    );
                    break;

                case SHREDDER_TYPE:
                    GameLoadShredder(
                            enemy_array, enemy_array_len,
                            all_enemy_data->ShredderMoveVectors,
                            GameEnemySpawnData[1][GameEnemySpawnIndexes[1][i + j]]
                    );
                    break;

                case MINER_TYPE:
                    GameLoadMiner(
                            enemy_array, enemy_array_len,
                            all_enemy_data->MinerMoveVectors,
                            all_enemy_data->MinerMineDelays,
                            GameEnemySpawnData[2][GameEnemySpawnIndexes[2][i + j]]
                    );
                    break;
            }
        }
    }
}

void GameSectorSetup(Entity *player, Entity enemy_array[], int enemy_array_len, Bullet bullet_array[],
                     int bullet_array_len, int *frame_number, EnemiesEnemyDataStruct *all_enemy_data,
                     GFXSpritesStruct *all_sprite_gfx, int num_enemy_sets) {
    // Setup
    PlayerSetup(player);
    BulletInitBulletArray(bullet_array, bullet_array_len);
    EntityInitEntityArray(enemy_array, enemy_array_len);

    // Loading the enemies
    GameLoadNumberOfRandomEnemySets(
            enemy_array, enemy_array_len,
            all_enemy_data,
            num_enemy_sets
    );

    // Hide everything
    HideEverySprite();

    // Timers
    int player_spawn_counter = 32, enemy_spawn_counter = 32;

    // Loop
    while (player_spawn_counter || enemy_spawn_counter) {
        // Clear the text
        consoleClear();
        // Frame increment
        (*frame_number)++;

        // Player timer
        if (player_spawn_counter) {
            player_spawn_counter--;
        }

        // Animating the player portal
        oamSet(
                &oamMain,
                BULLET_ID_START, // Use the bullet sprite IDs as there is no bullets in the setup
                player->x - 1, player->y - 1,
                0,
                0,
                SpriteSize_16x16,
                SpriteColorFormat_256Color,
                all_sprite_gfx->PortalGFXMem[15 - (int) (player_spawn_counter / 2)],
                -1,
                0,
                !player_spawn_counter,
                0,
                0,
                0
        );

        // Drawing the player when the portal is halfway done
        if (player_spawn_counter < 16) {
            PlayerAnimate(player, 1, *frame_number, all_sprite_gfx->PlayerGFXMem, all_sprite_gfx->PortalGFXMem);
        }

        // Spawning in the enemies when the player is finished
        if (!player_spawn_counter) {
            // Enemy timer
            if (enemy_spawn_counter) {
                enemy_spawn_counter--;
            }

            // Animating the enemies portals
            for (int i = 0; i < enemy_array_len; i++) {
                // If the enemy exists
                if (!enemy_array[i].dead) {
                    oamSet(
                            &oamMain,
                            BULLET_ID_START + 1 + i,
                            enemy_array[i].x, enemy_array[i].y,
                            0,
                            0,
                            SpriteSize_16x16,
                            SpriteColorFormat_256Color,
                            all_sprite_gfx->PortalGFXMem[15 - (int) (enemy_spawn_counter / 2)],
                            -1,
                            0,
                            !enemy_spawn_counter,
                            0,
                            0,
                            0
                    );
                }
            }

            // Drawing the enemies when the portals are halfway done
            if (enemy_spawn_counter < 16) {
                EnemiesDrawAll(
                        enemy_array, enemy_array_len,
                        1,
                        *frame_number,
                        all_enemy_data,
                        all_sprite_gfx
                );
            }
        }

        // Lore, kinda
        UIResetDisplayBuffer();

        UIWriteTextAtOffset(
                "Warping in",
                1,
                1
        );

        UIPrintDisplayBuffer();

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
    }
}

int GameRunGameLoop(Entity *player, Entity enemy_array[], int enemy_array_len, Bullet bullet_array[],
                    int bullet_array_len, int *frame_number, EnemiesEnemyDataStruct *all_enemy_data,
                    GFXSpritesStruct *all_sprite_gfx, int playable_area[4], int hitbox_array[][4], int hitbox_array_len,
                    int difficulty) {
    // Keys
    int keys;

    // Data
    int player_center[2];

    // Exit delays
    int player_death_exit_delay = EXIT_DELAY;
    int win_condition_exit_delay = EXIT_DELAY;

    // True when the player has won
    int win_condition = 0;

    while (1) {
        // Clear the text
        consoleClear();
        // Get key presses
        scanKeys();
        keys = keysHeld();
        // Frame number
        (*frame_number)++;

        // Player Movement
        PlayerMovement(player, keys, hitbox_array, hitbox_array_len);

        // Player bullets
        PlayerFireBullet(
                player,
                keys,
                bullet_array, bullet_array_len,
                hitbox_array, hitbox_array_len
        );

        // Get the player center
        EntityGetCenterArray(player, player_center);

        // Handling enemies
        EnemiesHandleAll(
                enemy_array, enemy_array_len,
                bullet_array, bullet_array_len,
                player_center,
                &EnemiesAllEnemyData,
                hitbox_array, hitbox_array_len
        );

        // Death bullets
        if (difficulty) {
            BulletSpawnDeathBullets(
                    bullet_array, bullet_array_len,
                    enemy_array, enemy_array_len,
                    player
            );
        }

        // Handling bullets
        BulletHandleBulletArray(
                bullet_array, bullet_array_len,
                playable_area
        );

        // Miner mine explosions
        MinerSpawnMineExplosion(bullet_array, bullet_array_len);

        // Handling bullet collisions
        BulletArrayCollisionWithPlayerAndEnemies(
                bullet_array, bullet_array_len,
                enemy_array, enemy_array_len,
                player
        );

        // Collisions between player and enemies
        EnemiesCheckCollisionAgainstPlayer(
                enemy_array, enemy_array_len,
                player
        );

        // DRAWING and ANIMATION
        PlayerAnimate(player, 0, *frame_number, GFXAllSpriteGFX.PlayerGFXMem, GFXAllSpriteGFX.PlayerExplosionGFXMem);

        EnemiesDrawAll(
                enemy_array, enemy_array_len,
                0,
                *frame_number,
                &EnemiesAllEnemyData,
                &GFXAllSpriteGFX
        );

        BulletDrawArray(bullet_array, bullet_array_len, GFXAllSpriteGFX.BulletGFXMem, GFXAllSpriteGFX.WildBulletGFXMem);

        // region - Checking for end condition
        // If end condition is met, wait 60 frames before exiting
        // Player death is checked first

        // If player is dead
        if (player->dead) {
            player_death_exit_delay--;
            if (!player_death_exit_delay) {
                return 0;
            }
        } else if (!win_condition) { // If all enemies are dead and no bullets are alive
            // Assume true
            win_condition = 1;

            // Check if each enemy is dead
            for (int i = 0; i < enemy_array_len; i++) {
                // If there is an alive enemy the not won yet
                if (!enemy_array[i].dead) {
                    win_condition = 0;
                    break;
                }
            }
            // If each enemy is dead, check for alive bullets
            if (win_condition) {
                for (int i = 0; i < bullet_array_len; i++) {
                    // If not a player bullet and alive then the win condition is false
                    if ((bullet_array[i].type != PLAYER_BULLET) && bullet_array[i].alive) {
                        win_condition = 0;
                        break;
                    }
                }
            }
        }

        if (win_condition) {
            if (win_condition_exit_delay) {
                win_condition_exit_delay--;
            } else {
                break;
            }
        }
        // endregion

        // region - Showing information to the user

        UIResetDisplayBuffer();

        if (!player->dead)
            UIWriteTextAtOffset(
                    "Engaging in Combat",
                    1,
                    1
            );
        else
            UIWriteTextAtOffset(
                    "Initiating Temporal Reset",
                    1,
                    1
            );

        UIPrintDisplayBuffer();

        // endregion

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
        // To exit
        if (keys & KEY_SELECT)
            return -1;
    }

    return 1;
}