#include "Enemies.h"

#include "Constants.h"
#include "Other.h"

#include "Entity.h"
#include "Sentinel.h"
#include "Shredder.h"
#include "Miner.h"

EnemiesEnemyDataStruct EnemiesAllEnemyData;

void EnemiesDrawAll(
    Entity enemy_array[],
    const int enemy_array_len,
    const int priority,
    const int frame_number,
    const EnemiesEnemyDataStruct *all_enemy_data,
    _GFXAllSpriteGFX *all_sprite_gfx)
{
    for (int i = 0; i < enemy_array_len; i++)
    {
        switch (enemy_array[i].type)
        {
            case EntityType_Sentinel:
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

            case EntityType_Shredder:
                ShredderAnimate(
                    &enemy_array[i],
                    priority,
                    frame_number,
                    i,
                    all_sprite_gfx->ShredderGFXMem,
                    all_sprite_gfx->EnemyExplosionGFXMem
                );
                break;

            case EntityType_Miner:
                MinerAnimate(
                    &enemy_array[i],
                    priority,
                    frame_number,
                    i,
                    all_sprite_gfx->MinerGFXMem,
                    all_sprite_gfx->EnemyExplosionGFXMem
                );
                break;

            default: // Should not occur, but just in case
                break;
        }
    }
}

void EnemiesHandleAll(
    Entity enemy_array[],
    const int enemy_array_len,
    Bullet bullet_array[],
    const int bullet_array_len,
    int player_center[2],
    EnemiesEnemyDataStruct *all_enemy_data,
    int hitbox_array[][4],
    const int hitbox_array_len)
{
    for (int i = 0; i < enemy_array_len; i++)
    {
        if (!enemy_array[i].dead)
        {
            switch (enemy_array[i].type)
            {
                case EntityType_Sentinel:
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

                case EntityType_Shredder:
                    ShredderMove(
                        &enemy_array[i],
                        all_enemy_data->ShredderMoveVectors[i],
                        player_center,
                        hitbox_array, hitbox_array_len
                    );
                    break;

                case EntityType_Miner:
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

                default: // Should not occur, but just in case
                    break;
            }
        }
    }
}

int EnemiesCheckCollisionAgainstPlayer(Entity enemy_array[], int enemy_array_len, Entity *player)
{
    // To tell what killed the player
    int player_collision_type = EntityType_Player;

    if (player->dead) // If the player is dead, then there is no point in checking for a collision
        return player_collision_type;

    // Hitboxes
    int player_hitbox[4], enemy_hitbox[4];
    EntityGetHitBox(player, player_hitbox);

    // For each enemy
    for (int i = 0; i < enemy_array_len; i++)
    {
        // If they are not dead
        if (!enemy_array[i].dead)
        {
            // Get their hitbox
            EntityGetHitBox(&enemy_array[i], enemy_hitbox);
            // Check for collision against the player
            if (RectangleCollision(player_hitbox, enemy_hitbox))
            {
                // DEATH FOR THE DEATH GOD
                EntityTakeDamage(player, 1);
                //
                player_collision_type = enemy_array[i].type;
            }
        }
    }

    return player_collision_type;
}
