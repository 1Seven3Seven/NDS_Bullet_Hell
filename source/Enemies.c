#include "Enemies.h"

#include "Constants.h"
#include "Other.h"

#include "Entity.h"
#include "Sentinel.h"
#include "Shredder.h"
#include "Miner.h"

EnemiesEnemyDataStruct EnemiesAllEnemyData;

void EnemiesDrawAll(Entity enemy_array[], int enemy_array_len, int priority, int frame_number,
                    EnemiesEnemyDataStruct *all_enemy_data, GFXSpritesStruct *all_sprite_gfx) {
    for (int i = 0; i < enemy_array_len; i++) {
        switch (enemy_array[i].type) {
            case SENTINEL_TYPE:
                SentinelAnimate(
                        &enemy_array[i],
                        priority,
                        frame_number,
                        i,
                        !all_enemy_data->SentinelMoveDirections[i],
                        all_sprite_gfx->SentinelGFXMem,
                        all_sprite_gfx->EnemyExplosionGFXMem
                );
                break;

            case SHREDDER_TYPE:
                ShredderAnimate(
                        &enemy_array[i],
                        priority,
                        frame_number,
                        i,
                        all_sprite_gfx->ShredderGFXMem,
                        all_sprite_gfx->EnemyExplosionGFXMem
                );
                break;

            case MINER_TYPE:
                MinerAnimate(
                        &enemy_array[i],
                        priority,
                        frame_number,
                        i,
                        all_sprite_gfx->MinerGFXMem,
                        all_sprite_gfx->EnemyExplosionGFXMem
                );
                break;
        }
    }
}

void EnemiesHandleAll(Entity enemy_array[], int enemy_array_len, Bullet bullet_array[], int bullet_array_len,
                      int player_center[2], EnemiesEnemyDataStruct *all_enemy_data, int hitbox_array[][4],
                      int hitbox_array_len) {

    for (int i = 0; i < enemy_array_len; i++) {
        if (!enemy_array[i].dead) {
            switch (enemy_array[i].type) {
                case SENTINEL_TYPE:
                    SentinelMove(
                            &enemy_array[i],
                            all_enemy_data->SentinelMoveDirections[i],
                            player_center,
                            hitbox_array, hitbox_array_len
                    );
                    SentinelFireBullet(
                            &enemy_array[i],
                            all_enemy_data->SentinelMoveDirections[i],
                            player_center,
                            bullet_array, bullet_array_len
                    );
                    break;

                case SHREDDER_TYPE:
                    ShredderMove(
                            &enemy_array[i],
                            all_enemy_data->ShredderMoveVectors[i],
                            player_center,
                            hitbox_array, hitbox_array_len
                    );
                    break;

                case MINER_TYPE:
                    MinerMove(
                            &enemy_array[i],
                            all_enemy_data->MinerMoveVectors[i],
                            hitbox_array, hitbox_array_len
                    );
                    MinerPlaceMine(
                            &enemy_array[i],
                            &all_enemy_data->MinerMineDelays[i],
                            bullet_array, bullet_array_len
                    );
                    break;
            }
        }
    }
}

void EnemiesCheckCollisionAgainstPlayer(Entity enemy_array[], int enemy_array_len, Entity *player) {
    // Hitboxes
    int player_hitbox[4], enemy_hitbox[4];
    EntityGetHitBox(player, player_hitbox);
    // For each enemy
    for (int i = 0; i < enemy_array_len; i++) {
        // If they are not dead
        if (!enemy_array[i].dead) {
            // Get their hitbox
            EntityGetHitBox(&enemy_array[i], enemy_hitbox);
            // Check for collision against the player
            if (RectangleCollision(player_hitbox, enemy_hitbox)) {
                // DEATH FOR THE DEATH GOD
                EntityTakeDamage(player, 1);
            }
        }
    }
}