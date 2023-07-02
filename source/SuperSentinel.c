#include "SuperSentinel.h"

#include <nds.h>
#include <nds/arm9/sprite.h>

#include <math.h>
#include <stdlib.h>

#include "Constants.h"
#include "Other.h"

#include "UI.h"

#include "GFX.h"

#include "Bullet.h"

#include "Entity.h"

#include "Player.h"

#include "Enemies.h"

void LaserPreInitLaserStruct(SSLaserStruct *laser) {
    laser->Initialised = 0;
}

void LaserInitLaserStruct(SSLaserStruct *laser, Bullet bullet_array[], int bullet_array_len, int x, int y, int pilot) {
    if (laser->Initialised)
        return;

    int start_type, segment_type;
    if (pilot) {
        start_type = SS_LASER_PILOT_START;
        segment_type = SS_LASER_PILOT_SEGMENT;
    } else {
        start_type = SS_LASER_START_BULLET;
        segment_type = SS_LASER_SEGMENT_BULLET;
    }

    laser->Initialised = 1;
    laser->StartIndex = BulletSetupInBulletArrayReversed(
            bullet_array, bullet_array_len,
            x + pilot * 4, y,
            14, 16,
            0,
            0,
            24,
            1,
            start_type
    );

    laser->NumSegmentIndexes = (int) ((float) (SCREEN_HEIGHT - y) / 16.f + 0.5f);
    laser->SegmentIndexes = malloc(sizeof(int) * laser->NumSegmentIndexes);
    for (int i = 0; i < laser->NumSegmentIndexes; ++i) {
        laser->SegmentIndexes[i] = BulletSetupInBulletArrayReversed(
                bullet_array, bullet_array_len,
                x + 3 + pilot * 3.5f, y + 16 * (i + 1),
                8, 16,
                0,
                0,
                24,
                1,
                segment_type
        );
    }

    laser->Pilot = pilot;
}

void LaserDeInitLaserStruct(SSLaserStruct *laser, Bullet bullet_array[]) {
    if (!laser->Initialised)
        return;

    laser->Initialised = 0;

    bullet_array[laser->StartIndex].to_die = 1;

    for (int i = 0; i < laser->NumSegmentIndexes; ++i) {
        bullet_array[laser->SegmentIndexes[i]].to_die = 1;
    }
    free(laser->SegmentIndexes);
}

void LaserUpdate(SSLaserStruct *laser, Bullet bullet_array[], int new_x, int new_y) {
    if (!laser->Initialised)
        return;

    if (laser->StartIndex != -1) {
        bullet_array[laser->StartIndex].x = new_x + laser->Pilot * 4;
        bullet_array[laser->StartIndex].y = new_y;

        bullet_array[laser->StartIndex].to_die = 0;
        bullet_array[laser->StartIndex].alive = 1;

        if (bullet_array[laser->StartIndex].lifespan % 24 == 0) {
            bullet_array[laser->StartIndex].lifespan += 24;
        }
    }

    for (int i = 0; i < laser->NumSegmentIndexes; ++i) {
        if (laser->SegmentIndexes[i] != -1) {
            bullet_array[laser->SegmentIndexes[i]].x = new_x + 3 + laser->Pilot * 3.5f,
                    bullet_array[laser->SegmentIndexes[i]].y = new_y + 16 * (i + 1);

            bullet_array[laser->SegmentIndexes[i]].to_die = 0;
            bullet_array[laser->SegmentIndexes[i]].alive = 1;

            if (bullet_array[laser->SegmentIndexes[i]].lifespan % 24 == 0) {
                bullet_array[laser->SegmentIndexes[i]].lifespan += 24;
            }
        }
    }
}

SSSuperSentinelInformationStruct SSSuperSentinelInformation;

void SSSetup(Entity enemy_array[], int enemy_array_len) {
    // Reset all the entities
    EntityInitEntityArray(enemy_array, enemy_array_len);

    // Set up the main body
    EntitySetup(
            &enemy_array[0],
            SS_START_X, SS_START_Y,
            32, 29,
            SS_HEALTH,
            SS_BODY_TYPE,
            SS_BULLET_DELAY
    );

    // Set up the left laser
    EntitySetup(
            &enemy_array[1],
            SS_START_X - 16, SS_START_Y,
            16, 32,
            SS_HEALTH,
            SS_LASER_TYPE,
            -1
    );

    // Set up the right laser
    EntitySetup(
            &enemy_array[2],
            SS_START_X + 32, SS_START_Y,
            16, 32,
            SS_HEALTH,
            SS_LASER_TYPE,
            -1
    );

    // Setup SSSuperSentinelInformation
    SSSuperSentinelInformation.BulletsFired = 0;
    SSSuperSentinelInformation.Moving = 1;
    SSSuperSentinelInformation.MoveDirection = 0;
    SSSuperSentinelInformation.HitBoarder = 0;
    SSSuperSentinelInformation.FireLaser = 0;
    SSSuperSentinelInformation.FireLaserDelay = 0;
    SSSuperSentinelInformation.MoveLasers = 0;

    // The LASERS!!!
    LaserPreInitLaserStruct(&SSSuperSentinelInformation.Lasers[0]);
    LaserPreInitLaserStruct(&SSSuperSentinelInformation.Lasers[1]);
}

void SSSetPosition(Entity enemy_array[], float x, float y) {
    enemy_array[0].x = x;
    enemy_array[1].x = x - 16;
    enemy_array[2].x = x + 32;

    enemy_array[0].y = y;
    enemy_array[1].y = y;
    enemy_array[2].y = y;
}

int SSGetHealth(Entity enemy_array[]) {
    return SS_HEALTH - SS_HEALTH * 3 + enemy_array[0].health + enemy_array[1].health + enemy_array[2].health;
}

void __SSStrafeMovement(Entity enemy_array[]) {
    if (!SSSuperSentinelInformation.Moving)
        return;

    // If there is no delay to fire the laser then move
    if (SSSuperSentinelInformation.FireLaserDelay <= 0) {
        float x_movement;
        if (SSSuperSentinelInformation.MoveDirection)
            x_movement = 0.5;
        else
            x_movement = -0.5;

        SSSetPosition(
                enemy_array,
                enemy_array[0].x + x_movement,
                enemy_array[0].y
        );
    }

    // Check against the boarders
    SSSuperSentinelInformation.HitBoarder = 0;
    if (SSSuperSentinelInformation.MoveDirection && enemy_array[0].x >= SS_MAX_RIGHT_X) {
        SSSetPosition(
                enemy_array,
                SS_MAX_RIGHT_X,
                enemy_array[0].y
        );
        SSSuperSentinelInformation.HitBoarder = 1;
    } else if (!SSSuperSentinelInformation.MoveDirection && enemy_array[0].x <= SS_MAX_LEFT_X) {
        SSSetPosition(
                enemy_array,
                SS_MAX_LEFT_X,
                enemy_array[0].y
        );
        SSSuperSentinelInformation.HitBoarder = 1;
    }
}

void __SSMoveLasersToEdge(Entity enemy_array[], int hitbox_array[][4], int hitbox_array_len) {
    // region - Moving lasers to the edge of the screen
    if (SSSuperSentinelInformation.MoveLasers == 1) {
        int x_collision;
        x_collision = EntityMoveX(
                &enemy_array[1],
                -0.5f,
                hitbox_array, hitbox_array_len
        );

        x_collision |= EntityMoveX(
                &enemy_array[2],
                0.5f,
                hitbox_array, hitbox_array_len
        );

        if (x_collision) {
            SSSuperSentinelInformation.MoveLasers = 2;
        }

        return;
    }
    // endregion

    // region - If at the edge of the screen and there is no laser delay, then move to center
    if (SSSuperSentinelInformation.MoveLasers == 2 && SSSuperSentinelInformation.FireLaserDelay <= 0) {
        int some_pos;
        enemy_array[1].x += 0.25f;
        enemy_array[2].x -= 0.25f;

        some_pos = EntityGetRight(&enemy_array[1]);

        if (some_pos - 0.1f < enemy_array[0].x && enemy_array[0].x < some_pos + 0.1f) {
            EntitySetRight(&enemy_array[1], enemy_array[0].x);
            EntitySetLeft(&enemy_array[2], EntityGetRight(&enemy_array[0]));
            SSSuperSentinelInformation.MoveLasers = -1;
        }
    }
    // endregion
}

void SSMove(Entity enemy_array[], int hitbox_array[][4], int hitbox_array_len) {
    __SSStrafeMovement(enemy_array);
    __SSMoveLasersToEdge(enemy_array, hitbox_array, hitbox_array_len);
}

void SSAnimate(Entity enemy_array[], int priority, int frame_number, u16 *body_gfx_mem[4][8],
               u16 *laser_weapon_gfx_mem[4], u16 *bang_gfx_mem[4]) {
    if (!(frame_number % 6)) {
        enemy_array[0].animation_frame_number++;
        enemy_array[0].animation_frame_number %= 4;
    }

    // region - Drawing the body
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            int bullet_fired = 0;

            if (SSSuperSentinelInformation.BulletsFired >= i * 2 + j + 1) {
                bullet_fired = 1;
            }

            oamSet(
                    &oamMain,
                    1 + j + i * 2,
                    enemy_array[0].x + j * 16, enemy_array[0].y + i * 16,
                    priority,
                    0,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color,
                    body_gfx_mem[j + i * 2][4 * bullet_fired + enemy_array[0].animation_frame_number],
                    -1,
                    false,
                    false,
                    false,
                    false,
                    false
            );
        }
    }
    // endregion

    // region - Drawing the laser arms
    oamSet(
            &oamMain,
            5,
            enemy_array[1].x, enemy_array[1].y,
            0,
            0,
            SpriteSize_16x32,
            SpriteColorFormat_256Color,
            laser_weapon_gfx_mem[enemy_array[0].animation_frame_number],
            -1,
            false,
            false,
            false,
            false,
            false
    );
    oamSet(
            &oamMain,
            6,
            enemy_array[2].x, enemy_array[2].y,
            0,
            0,
            SpriteSize_16x32,
            SpriteColorFormat_256Color,
            laser_weapon_gfx_mem[enemy_array[0].animation_frame_number],
            -1,
            false,
            false,
            false,
            false,
            false
    );
    // endregion
}

void SSFireSalvo(Entity enemy_array[], int player_center[2], Bullet bullet_array[], int bullet_array_len) {
    if (enemy_array[0].current_bullet_delay > 0)
        enemy_array[0].current_bullet_delay--;
    else {
        if (SSSuperSentinelInformation.BulletsFired < 4) {
            // Firing the salvo

            // Get the top left of the main body
            int my_center[2] = {enemy_array[0].x, enemy_array[0].y};

            // Adjusting position for the center of the segment to fire
            switch (SSSuperSentinelInformation.BulletsFired) {
                case 0: // Top left bullet
                    my_center[0] += 8;
                    my_center[1] += 8;
                    break;
                case 1: // Top right bullet
                    my_center[0] += 16 + 8;
                    my_center[1] += 8;
                    break;
                case 2: // Bottom left bullet
                    my_center[0] += 10;
                    my_center[1] += 16 + 7;
                    break;
                case 3: // Bottom right bullet
                    my_center[0] += 16 + 6;
                    my_center[1] += 16 + 7;
                    break;
            }

            // Angle to the player
            float angle = GetAngleFromOriginTo(
                    player_center[0] - my_center[0],
                    player_center[1] - my_center[1]
            );

            // Adjusting position for the start position of the bullets
            switch (SSSuperSentinelInformation.BulletsFired) {
                case 0: // Top left bullet
                case 1: // Top right bullet
                    my_center[0] -= 4;
                    my_center[1] -= 4;
                    break;
                case 2: // Bottom left bullet
                case 3: // Bottom right bullet
                    my_center[0] -= 4;
                    my_center[1] -= 5;
                    break;
            }

            BulletSetupInBulletArray(
                    bullet_array, bullet_array_len,
                    my_center[0], my_center[1],
                    8,
                    8,
                    angle,
                    1,
                    270,
                    1,
                    SS_BULLET
            );

            SSSuperSentinelInformation.BulletsFired++;

            enemy_array[0].current_bullet_delay = SS_SALVO_DELAY;

        } else {
            // Reloading the salvo
            enemy_array[0].current_bullet_delay = enemy_array[0].bullet_delay;
            SSSuperSentinelInformation.BulletsFired = 0;
        }
    }
}

void SSFireLaser(Entity enemy_array[], Bullet bullet_array[], int bullet_array_len) {
    if (!SSSuperSentinelInformation.FireLaser) { // Don't fire the laser
        return;
    }

    if (SSSuperSentinelInformation.FireLaserDelay > 0) { // Fire pilot lasers
        if (SSSuperSentinelInformation.FireLaserDelay == SS_LASER_DELAY) { // Create them
            // De-initialise just in case
            LaserDeInitLaserStruct(
                    &SSSuperSentinelInformation.Lasers[0],
                    bullet_array
            );
            LaserDeInitLaserStruct(
                    &SSSuperSentinelInformation.Lasers[1],
                    bullet_array
            );

            if (SSSuperSentinelInformation.LaserToFireFrom) { // If only from left or right
                LaserInitLaserStruct(
                        &SSSuperSentinelInformation.Lasers[0],
                        bullet_array, bullet_array_len,
                        enemy_array[SSSuperSentinelInformation.LaserToFireFrom].x + 1,
                        enemy_array[SSSuperSentinelInformation.LaserToFireFrom].y + 32,
                        1
                );
            } else { // If from both at once
                for (int i = 0; i < 2; ++i) {
                    LaserInitLaserStruct(
                            &SSSuperSentinelInformation.Lasers[i],
                            bullet_array, bullet_array_len,
                            enemy_array[i + 1].x + 1,
                            enemy_array[i + 1].y + 32,
                            1
                    );
                }
            }
        }
        SSSuperSentinelInformation.FireLaserDelay--;
    }

    if (SSSuperSentinelInformation.FireLaserDelay == 0) { // Create the lasers
        SSSuperSentinelInformation.FireLaserDelay = -1;

        // De-initialise just in case
        LaserDeInitLaserStruct(
                &SSSuperSentinelInformation.Lasers[0],
                bullet_array
        );
        LaserDeInitLaserStruct(
                &SSSuperSentinelInformation.Lasers[1],
                bullet_array
        );

        if (SSSuperSentinelInformation.LaserToFireFrom) { // If only from left or right
            LaserInitLaserStruct(
                    &SSSuperSentinelInformation.Lasers[0],
                    bullet_array, bullet_array_len,
                    enemy_array[SSSuperSentinelInformation.LaserToFireFrom].x + 1,
                    enemy_array[SSSuperSentinelInformation.LaserToFireFrom].y + 32,
                    0
            );
        } else { // If from both at once
            for (int i = 0; i < 2; ++i) {
                LaserInitLaserStruct(
                        &SSSuperSentinelInformation.Lasers[i],
                        bullet_array, bullet_array_len,
                        enemy_array[i + 1].x + 1,
                        enemy_array[i + 1].y + 32,
                        0
                );
            }
        }
    }

    if (SSSuperSentinelInformation.LaserToFireFrom) { // If only from left or right
        LaserUpdate(
                &SSSuperSentinelInformation.Lasers[0],
                bullet_array,
                enemy_array[SSSuperSentinelInformation.LaserToFireFrom].x + 1,
                enemy_array[SSSuperSentinelInformation.LaserToFireFrom].y + 32
        );
    } else { // If from both at once
        for (int i = 0; i < 2; ++i) {
            LaserUpdate(
                    &SSSuperSentinelInformation.Lasers[i],
                    bullet_array,
                    enemy_array[i + 1].x + 1,
                    enemy_array[i + 1].y + 32
            );
        }
    }
}

void SSThink(Entity enemy_array[]) {
    // region - Understanding if I have died
    if (SSGetHealth(enemy_array) <= 0) {
        SSSuperSentinelInformation.FireLaser = 0;
        enemy_array[0].current_bullet_delay = 15;
        return;
    }
    // endregion

    // region - Swapping my direction if a boarder is hit
    if (SSSuperSentinelInformation.HitBoarder) {
        if (SSSuperSentinelInformation.MoveDirection && enemy_array[0].x >= SS_MAX_RIGHT_X) {
            SSSuperSentinelInformation.MoveDirection = 0;
            SSSetPosition(
                    enemy_array,
                    SS_MAX_RIGHT_X,
                    enemy_array[0].y
            );

            if (SSGetHealth(enemy_array) < SS_LASER_HEALTH) {
                SSSuperSentinelInformation.LaserToFireFrom = 2;
                if (SSSuperSentinelInformation.FireLaserDelay <= 0)
                    SSSuperSentinelInformation.FireLaserDelay = SS_LASER_DELAY;
                SSSuperSentinelInformation.FireLaser = 1;
            }
        } else if (enemy_array[0].x <= SS_MAX_LEFT_X) {
            SSSuperSentinelInformation.MoveDirection = 1;
            SSSetPosition(
                    enemy_array,
                    SS_MAX_LEFT_X,
                    enemy_array[0].y
            );

            if (SSGetHealth(enemy_array) < SS_LASER_HEALTH) {
                SSSuperSentinelInformation.LaserToFireFrom = 1;
                if (SSSuperSentinelInformation.FireLaserDelay <= 0)
                    SSSuperSentinelInformation.FireLaserDelay = SS_LASER_DELAY;
                SSSuperSentinelInformation.FireLaser = 1;
            }
        }
    }
    // endregion

    // region - Checking my health level for final stage
    const int health = SSGetHealth(enemy_array);
    if (health > SS_FINAL_HEALTH)
        return;
    // endregion

    // region - If my health is below a certain amount then navigate to the middle, stop moving and stop laser firing
    if (SSSuperSentinelInformation.Moving) {
        // Check for middle
        const float left_x = (float) SS_CENTER_X - 0.1f;
        const float right_x = (float) SS_CENTER_X + 0.1f;
        if (left_x < enemy_array[0].x && enemy_array[0].x < right_x) {
            SSSuperSentinelInformation.Moving = 0;
            SSSetPosition(
                    enemy_array,
                    SS_CENTER_X,
                    enemy_array[0].y
            );
            SSSuperSentinelInformation.FireLaser = 0;
        }
        return;
    }
    // endregion

    // region - If I am at the middle then wait till a bullet salvo finishes, then release the lasers
    if (!SSSuperSentinelInformation.MoveLasers) {
        if (SSSuperSentinelInformation.BulletsFired == 0 &&
            enemy_array[0].current_bullet_delay == enemy_array[0].bullet_delay) {
            SSSuperSentinelInformation.MoveLasers = 1;
        }
    }
    // endregion

    // region - Once lasers have reached the edge then fire them and move them back
    if (SSSuperSentinelInformation.FireLaser <= 0 && SSSuperSentinelInformation.MoveLasers == 2) {
        SSSuperSentinelInformation.FireLaserDelay = SS_LASER_DELAY;
        SSSuperSentinelInformation.LaserToFireFrom = 0;
        SSSuperSentinelInformation.FireLaser = 1;
    }
    // endregion
}

void SSSetupForGameLoop(Entity *player, Entity enemy_array[], int enemy_array_len, Bullet bullet_array[],
                        int bullet_array_len, int *frame_number, GFXSpritesStruct *all_sprite_gfx, int bg_id) {
    // Setup
    PlayerSetup(player);
    BulletInitBulletArray(bullet_array, bullet_array_len);
    SSSetup(enemy_array, enemy_array_len);

    // Hiding everything
    HideEverySprite();

    // For the screen shake
    int screen_shake_x[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    ShuffleIntArray(screen_shake_x, 9);
    int screen_shake_y[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    ShuffleIntArray(screen_shake_y, 9);
    int screen_shake_index = 0;

    // Timers
    int player_spawn_counter = 32, boss_spawn_counter = 80;

    // Loop
    while (player_spawn_counter || boss_spawn_counter) {
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

        // If the player is finished
        if (!player_spawn_counter) {
            // Boss entrance
            boss_spawn_counter--;

            // Move the boss
            SSSetPosition(enemy_array, enemy_array[0].x, enemy_array[0].y + 0.5f);

            // Screen shake
            bgSetScroll(bg_id, screen_shake_x[screen_shake_index], screen_shake_y[screen_shake_index]);
            bgUpdate();
            screen_shake_index++;
            screen_shake_index %= 9;

            SSAnimate(
                    enemy_array,
                    0,
                    *frame_number,
                    GFXAllSpriteGFX.SSBodyGFXMem,
                    GFXAllSpriteGFX.SSLaserWeaponGFXMem,
                    GFXAllSpriteGFX.BangGFXMem
            );
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

    // Finishing up
    bgSetScroll(bg_id, 0, 0);
    bgUpdate();
    SSSetPosition(enemy_array, enemy_array[0].x, 8);
}

int SSRunGameLoop(Entity *player, Entity enemy_array[], int enemy_array_len, Bullet bullet_array[],
                  int bullet_array_len, int *frame_number, GFXSpritesStruct *all_sprite_gfx, int playable_area[4],
                  int hitbox_array[][4], int hitbox_array_len) {
    // Keys
    int keys;

    // Data
    int player_center[2];

    //
    int bullet_collision_type = PLAYER_BULLET, bullet_collision_type_temp = PLAYER_BULLET;
    int enemy_collision_type = PLAYER_TYPE, enemy_collision_type_temp = PLAYER_TYPE;

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

        if (keys & KEY_START)
            enemy_array[0].health--;

        // Boss fire laser
        // This is done first to avoid potential unwanted behaviour when spawning bullets before being overwritten
        SSFireLaser(
                enemy_array,
                bullet_array, bullet_array_len
        );

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

        // THINKING IS HARD OKAY
        SSThink(enemy_array);

        // Moving the boss
        SSMove(
                enemy_array,
                hitbox_array, hitbox_array_len
        );

        // Boss firing salvo
        SSFireSalvo(
                enemy_array,
                player_center,
                bullet_array, bullet_array_len
        );

        // Handling bullets
        BulletHandleBulletArray(
                bullet_array, bullet_array_len,
                playable_area
        );

        // Handling bullet collisions
        bullet_collision_type_temp = BulletArrayCollisionWithPlayerAndEnemies(
                bullet_array, bullet_array_len,
                enemy_array, enemy_array_len,
                player
        );

        // Collisions between player and enemies
        enemy_collision_type_temp = EnemiesCheckCollisionAgainstPlayer(
                enemy_array, enemy_array_len,
                player
        );

        // DRAWING and ANIMATION
        PlayerAnimate(
                player,
                0,
                *frame_number,
                all_sprite_gfx->PlayerGFXMem,
                all_sprite_gfx->PlayerExplosionGFXMem
        );

        SSAnimate(
                enemy_array,
                0,
                *frame_number,
                all_sprite_gfx->SSBodyGFXMem,
                all_sprite_gfx->SSLaserWeaponGFXMem,
                GFXAllSpriteGFX.BangGFXMem
        );

        BulletDrawArray(bullet_array, bullet_array_len, GFXAllSpriteGFX.BulletGFXMem, GFXAllSpriteGFX.WildBulletGFXMem);

        // region - Checking what killed the player if they died
        if (bullet_collision_type_temp != PLAYER_BULLET && bullet_collision_type == PLAYER_BULLET)
            bullet_collision_type = bullet_collision_type_temp;
        if (enemy_collision_type_temp != PLAYER_TYPE && enemy_collision_type == PLAYER_TYPE)
            enemy_collision_type = enemy_collision_type_temp;
        // endregion

        // region - Checking for end condition
        // If end condition is met, wait 60 frames before exiting
        // Player death is checked first

        // If player is dead
        if (player->dead) {
            player_death_exit_delay--;
            if (!player_death_exit_delay) {
                return 0;
            }
        } else if (!win_condition) { // If the boss is dead and no bullets are alive
            // Assume true
            win_condition = 1;

            // Check if the boss is dead
            if (SSGetHealth(enemy_array) > 0) {
                win_condition = 0;
            }
            // If the boss is dead, check for alive bullets
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

        char temp[UI_NUM_CHARS + 1];

        UIWriteText("Total Health:", 1);
        itoa(SSGetHealth(enemy_array), temp, 10);
        UIWriteTextAtOffset(temp, 1, 14);

        UIWriteText("Segments health:", 3);
        itoa(enemy_array[0].health, temp, 10);
        UIWriteTextAtOffset(temp, 4, 1);
        itoa(enemy_array[1].health, temp, 10);
        UIWriteTextAtOffset(temp, 4, 6);
        itoa(enemy_array[2].health, temp, 10);
        UIWriteTextAtOffset(temp, 4, 11);

        itoa(SSSuperSentinelInformation.FireLaser, temp, 10);
        UIWriteText("Fire laser?", 6);
        UIWriteTextAtOffset(temp, 6, 12);

        itoa(SSSuperSentinelInformation.FireLaserDelay, temp, 10);
        UIWriteText("Delay", 7);
        UIWriteTextAtOffset(temp, 7, 6);

        itoa(SSSuperSentinelInformation.MoveLasers, temp, 10);
        UIWriteText("Move Lasers?", 9);
        UIWriteTextAtOffset(temp, 9, 13);

        itoa(BulletGetNumberAliveBulletsInBulletArray(bullet_array, bullet_array_len), temp, 10);
        UIWriteText("Num Bullets:", 11);
        UIWriteTextAtOffset(temp, 11, 13);

        itoa(bullet_collision_type, temp, 10);
        UIWriteText("Bullet Collision Type:", 13);
        UIWriteTextAtOffset(temp, 13, 23);
        itoa(enemy_collision_type, temp, 10);
        UIWriteText("Enemy Collision Type:", 15);
        UIWriteTextAtOffset(temp, 15, 23);

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

void SSRunEndLoop(Entity *player, Entity enemy_array[], int enemy_array_len, Bullet bullet_array[],
                  int bullet_array_len, int *frame_number, GFXSpritesStruct *all_sprite_gfx)
{
    // Hide everything not being used, aka player and boss sprites 0-6, aka just the bullets
    for (int i = 7; i < 128; i++) {
        oamSetHidden(
            &oamMain,
            i,
            true
        );
    }

    // Lazers shake then three explosions before they explode
    // Then the middle section falls appart

    // Setting up rotation matrices
    for (int i = 0; i < 32; i++)
    {
        oamRotateScale(
            &oamMain,
            i,
            32767.f / 32 * i,
            256, 256);
    }

    // Setup of other values
    float vectors[4][2];
    int angles[4];
    float positions[4][2];
    int bullets_fired[4] = {0};
    int rotation_directions[6] = {0, 0, 0, 1, 1, 1}; // At least 1 will be in a different direction

    // To make some of the rotations in different directions
    ShuffleIntArray(rotation_directions, 6);

    // Creating angles n vectors n stuff
    for (int i = 0; i < 4; i++)
    {
        angles[i] = rand() % 180 + 180; // Angle pointing down
        // Muh vector
        float angle_in_radians = angles[i] / 180.f * 3.14f;
        vectors[i][0] = cos(angle_in_radians);
        vectors[i][1] = -sin(angle_in_radians);

        // Getting the position
        int x = i % 2;
        int y = i > 1;
        positions[i][0] = enemy_array[0].x + x * 16 - 8;
        positions[i][1] = enemy_array[0].y + y * 16 - 8;

        // Making sure the right sprite is used
        if (SSSuperSentinelInformation.BulletsFired >= i + 1)
            bullets_fired[i] = 1;
    }

    // Shakey shake shake
    int shakes[6] = {-1, -1, 0, 0, 1, 1};
    int shakes_index = 0;
    ShuffleIntArray(shakes, 6);

    // Hiding the old lazers as the new lazers have a different id
    oamSetHidden(
        &oamMain,
        5,
        true);
    oamSetHidden(
        &oamMain,
        6,
        true);

    // Exploding the lazers
    // Skakey explosive time
    int lazer_death_counter = 120; // 64 + 16 + 14 * 2 + (12 cause round number and I want some delay)
    // 64 frames of shake
    // Then the explosions
    while (lazer_death_counter > 0)
    {
        // Clear the text
        consoleClear();
        // Frame number
        (*frame_number)++;

        // Drawing the lazers
        if (lazer_death_counter > 13)
        {
            oamSet(
                &oamMain,
                20,
                enemy_array[1].x + shakes[shakes_index], enemy_array[1].y + shakes[(shakes_index + 1) % 6],
                0,
                0,
                SpriteSize_16x32,
                SpriteColorFormat_256Color,
                all_sprite_gfx->SSLaserWeaponGFXMem[enemy_array[0].animation_frame_number],
                -1,
                false,
                false,
                false,
                false,
                false);
            oamSet(
                &oamMain,
                21,
                enemy_array[2].x + shakes[(shakes_index + 2) % 6], enemy_array[2].y + shakes[(shakes_index + 3) % 6],
                0,
                0,
                SpriteSize_16x32,
                SpriteColorFormat_256Color,
                all_sprite_gfx->SSLaserWeaponGFXMem[enemy_array[0].animation_frame_number],
                -1,
                false,
                false,
                false,
                false,
                false);

            // Index changing
            if (lazer_death_counter % 4 == 1)
            {
                shakes_index++;
                shakes_index = shakes_index % 6;
            }
        }

        // EXPLOOOOOOSION!!!
        // Three explosions on the lazer body
        // Height is 32, middle one has its center at 16 and the other two 8 above and below
        if (lazer_death_counter < 56)
        {
            int frame = 55 - lazer_death_counter;
            if (frame < 16)
            {
                oamSet(
                    &oamMain,
                    12,
                    enemy_array[1].x, enemy_array[1].y - 2,
                    0,
                    0,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color,
                    all_sprite_gfx->SSExplosionGFXMem[frame / 2],
                    -1,
                    false,
                    false,
                    false,
                    false,
                    false);
                oamSet(
                    &oamMain,
                    15,
                    enemy_array[2].x, enemy_array[2].y - 2,
                    0,
                    0,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color,
                    all_sprite_gfx->SSExplosionGFXMem[frame / 2],
                    -1,
                    false,
                    false,
                    false,
                    false,
                    false);
            }
            else if (frame == 16)
            {
                oamSetHidden(
                    &oamMain,
                    12,
                    true);
                oamSetHidden(
                    &oamMain,
                    15,
                    true);
            }
        }
        if (lazer_death_counter < 42)
        {
            int frame = 41 - lazer_death_counter;
            if (frame < 16)
            {
                oamSet(
                    &oamMain,
                    11,
                    enemy_array[1].x, enemy_array[1].y + 8,
                    0,
                    0,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color,
                    all_sprite_gfx->SSExplosionGFXMem[frame / 2],
                    -1,
                    false,
                    false,
                    false,
                    false,
                    false);
                oamSet(
                    &oamMain,
                    14,
                    enemy_array[2].x, enemy_array[2].y + 8,
                    0,
                    0,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color,
                    all_sprite_gfx->SSExplosionGFXMem[frame / 2],
                    -1,
                    false,
                    false,
                    false,
                    false,
                    false);
            }
            else if (frame == 16)
            {
                oamSetHidden(
                    &oamMain,
                    11,
                    true);
                oamSetHidden(
                    &oamMain,
                    14,
                    true);
            }
        }
        if (lazer_death_counter < 28)
        {
            int frame = 27 - lazer_death_counter;
            if (frame < 16)
            {
                oamSet(
                    &oamMain,
                    10,
                    enemy_array[1].x, enemy_array[1].y + 18,
                    0,
                    0,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color,
                    all_sprite_gfx->SSExplosionGFXMem[frame / 2],
                    -1,
                    false,
                    false,
                    false,
                    false,
                    false);
                oamSet(
                    &oamMain,
                    13,
                    enemy_array[2].x, enemy_array[2].y + 18,
                    0,
                    0,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color,
                    all_sprite_gfx->SSExplosionGFXMem[frame / 2],
                    -1,
                    false,
                    false,
                    false,
                    false,
                    false);
            }
            else if (frame == 16)
            {
                oamSetHidden(
                    &oamMain,
                    10,
                    true);
                oamSetHidden(
                    &oamMain,
                    13,
                    true);
            }
        }

        // Hiding the lazers
        if (lazer_death_counter == 13)
        {
            oamSetHidden(
                &oamMain,
                20,
                true);
            oamSetHidden(
                &oamMain,
                21,
                true);
        }

        lazer_death_counter--;

        // UI wooo
        UIResetDisplayBuffer();

        ; // Pretend this actually prints stuff

        UIPrintDisplayBuffer();

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
    }

    // Hiding the old body id for the same reason as the lazers
    for (int i = 0; i < 4; i++)
    {
        oamSetHidden(
            &oamMain,
            1 + i,
            true);
    }

    // Splitting the boss up
    int body_split_counter = 255;
    while (body_split_counter > 0)
    {
        // Clear the text
        consoleClear();
        // Frame number
        (*frame_number)++;

        // Drawing the body
        for (int i = 0; i < 4; i++)
        {
            oamSet(
                &oamMain,
                20 + i,
                positions[i][0], positions[i][1],
                0,
                0,
                SpriteSize_16x16,
                SpriteColorFormat_256Color,
                all_sprite_gfx->SSBodyGFXMem[i][4 * bullets_fired[i] + enemy_array[0].animation_frame_number],
                (rotation_directions[i]) ? body_split_counter / 8 : 31 - body_split_counter / 8,
                true,
                false,
                false,
                false,
                false);
        }

        // Moving the body and lazers
        body_split_counter--;
        if (body_split_counter % 8 == 1)
        {
            for (int i = 0; i < 4; i++)
            {
                positions[i][0] += vectors[i][0];
                positions[i][1] += vectors[i][1];
            }
        }

        // UI wooo
        UIResetDisplayBuffer();

        ; // Pretend this actually prints stuff

        UIPrintDisplayBuffer();

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
    }

    // Last 16 frames of exploding the body
    for (int explosion_index = 0; explosion_index < 16; explosion_index++)
    {
        // Clear the text
        consoleClear();
        // Frame number
        (*frame_number)++;

        // Explosion time
        for (int i = 0; i < 4; i++)
        {
            oamSet(
                &oamMain,
                10 + i,
                positions[i][0] + 8, positions[i][1] + 8, // As these positions are for the sized doubled versions
                0,
                -1,
                SpriteSize_16x16,
                SpriteColorFormat_256Color,
                all_sprite_gfx->SSExplosionGFXMem[explosion_index / 2],
                -1,
                false,
                false,
                false,
                false,
                false);
        }

        // Hide the body parts
        if (explosion_index == 11)
        {
            for (int i = 0; i < 4; i++)
            {
                oamSetAffineIndex(
                    &oamMain,
                    20 + i,
                    -1,
                    false);
                oamSetHidden(
                    &oamMain,
                    20 + i,
                    true);
            }
        }

        // UI wooo
        UIResetDisplayBuffer();

        ; // Pretend this actually prints stuff

        UIPrintDisplayBuffer();

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
    }

    // Finally we hide the explosions
    // Clear the text
    consoleClear();
    // Frame number
    (*frame_number)++;
    for (int i = 0; i < 4; i++)
    {
        oamSetHidden(
            &oamMain,
            10 + i,
            true);
    }
    // UI wooo
    UIResetDisplayBuffer();
    ; // Pretend this actually prints stuff
    UIPrintDisplayBuffer();
    // Waiting
    swiWaitForVBlank();
    // Update the screen
    oamUpdate(&oamMain);
}