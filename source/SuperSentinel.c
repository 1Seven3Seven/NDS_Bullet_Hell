#include "SuperSentinel.h"

#include <nds.h>

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

void LaserPreInitLaserStruct(SSLaserStruct *laser)
{
    laser->Initialised = 0;
}

void LaserInitLaserStruct(
    SSLaserStruct *laser,
    Bullet bullet_array[],
    const int bullet_array_len,
    const int x,
    const int y,
    const int pilot)
{
    if (laser->Initialised)
        return;

    BulletType start_type, segment_type;
    if (pilot)
    {
        start_type = BulletType_SSLaserPilotStart;
        segment_type = BulletType_SSLaserPilotSegment;
    }
    else
    {
        start_type = BulletType_SSLaserStart;
        segment_type = BulletType_SSLaserSegment;
    }

    laser->Initialised = 1;
    laser->StartIndex = BulletSetupInBulletArrayReversed(
        bullet_array, bullet_array_len,
        (float) (x + pilot * 4),
        (float) y,
        14, 16,
        0,
        0,
        24,
        1,
        start_type
    );

    laser->NumSegmentIndexes = (int) ((float) (SCREEN_HEIGHT - y) / 16.f + 0.5f);
    laser->SegmentIndexes = malloc(sizeof(int) * laser->NumSegmentIndexes);
    for (int i = 0; i < laser->NumSegmentIndexes; ++i)
    {
        laser->SegmentIndexes[i] = BulletSetupInBulletArrayReversed(
            bullet_array, bullet_array_len,
            (float) x + 3.f + (pilot ? 3.5f : 0.f),
            (float) (y + 16 * (i + 1)),
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

void LaserDeInitLaserStruct(SSLaserStruct *laser, Bullet bullet_array[])
{
    if (!laser->Initialised) { return; }

    laser->Initialised = 0;

    bullet_array[laser->StartIndex].to_die = 1;

    for (int i = 0; i < laser->NumSegmentIndexes; ++i)
    {
        bullet_array[laser->SegmentIndexes[i]].to_die = 1;
    }
    free(laser->SegmentIndexes);
}

void LaserUpdate(SSLaserStruct *laser, Bullet bullet_array[], const int new_x, const int new_y)
{
    if (!laser->Initialised) { return; }

    if (laser->StartIndex != -1)
    {
        bullet_array[laser->StartIndex].x = (float) (new_x + laser->Pilot * 4);
        bullet_array[laser->StartIndex].y = (float) new_y;

        bullet_array[laser->StartIndex].to_die = 0;
        bullet_array[laser->StartIndex].alive = 1;

        if (bullet_array[laser->StartIndex].lifespan % 24 == 0)
        {
            bullet_array[laser->StartIndex].lifespan += 24;
        }
    }

    for (int i = 0; i < laser->NumSegmentIndexes; ++i)
    {
        if (laser->SegmentIndexes[i] != -1)
        {
            bullet_array[laser->SegmentIndexes[i]].x = (float) new_x + 3 + (laser->Pilot ? 3.5f : 0.f);
            bullet_array[laser->SegmentIndexes[i]].y = (float) (new_y + 16 * (i + 1));

            bullet_array[laser->SegmentIndexes[i]].to_die = 0;
            bullet_array[laser->SegmentIndexes[i]].alive = 1;

            if (bullet_array[laser->SegmentIndexes[i]].lifespan % 24 == 0)
            {
                bullet_array[laser->SegmentIndexes[i]].lifespan += 24;
            }
        }
    }
}

_SSSuperSentinelInformation SSSuperSentinelInformation;

void SSSetup(Entity enemy_array[], const int enemy_array_len)
{
    // Reset all the entities
    EntityInitEntityArray(enemy_array, enemy_array_len);

    // Set up the main body
    EntitySetup(
        &enemy_array[0],
        SS_START_X, SS_START_Y,
        32, 29,
        SS_HEALTH,
        EntityType_SSBody,
        SS_BULLET_DELAY
    );

    // Set up the left laser
    EntitySetup(
        &enemy_array[1],
        SS_START_X - 16, SS_START_Y,
        16, 32,
        SS_HEALTH,
        EntityType_SSLaser,
        -1
    );

    // Set up the right laser
    EntitySetup(
        &enemy_array[2],
        SS_START_X + 32, SS_START_Y,
        16, 32,
        SS_HEALTH,
        EntityType_SSLaser,
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

void SSSetPosition(Entity enemy_array[], const float x, const float y)
{
    enemy_array[0].x = x;
    enemy_array[1].x = x - 16;
    enemy_array[2].x = x + 32;

    enemy_array[0].y = y;
    enemy_array[1].y = y;
    enemy_array[2].y = y;
}

int SSGetHealth(const Entity enemy_array[])
{
    return SS_HEALTH - SS_HEALTH * 3 + enemy_array[0].health + enemy_array[1].health + enemy_array[2].health;
}

static void SSStrafeMovement(Entity enemy_array[])
{
    if (!SSSuperSentinelInformation.Moving) { return; }

    // If there is no delay to fire the laser, then move
    if (SSSuperSentinelInformation.FireLaserDelay <= 0)
    {
        float x_movement;
        if (SSSuperSentinelInformation.MoveDirection) { x_movement = 0.5f; }
        else { x_movement = -0.5f; }

        SSSetPosition(
            enemy_array,
            enemy_array[0].x + x_movement,
            enemy_array[0].y
        );
    }

    // Check against the boarders
    SSSuperSentinelInformation.HitBoarder = 0;
    if (SSSuperSentinelInformation.MoveDirection && enemy_array[0].x >= SS_MAX_RIGHT_X)
    {
        SSSetPosition(
            enemy_array,
            SS_MAX_RIGHT_X,
            enemy_array[0].y
        );
        SSSuperSentinelInformation.HitBoarder = 1;
    }
    else if (!SSSuperSentinelInformation.MoveDirection && enemy_array[0].x <= SS_MAX_LEFT_X)
    {
        SSSetPosition(
            enemy_array,
            SS_MAX_LEFT_X,
            enemy_array[0].y
        );
        SSSuperSentinelInformation.HitBoarder = 1;
    }
}

static void SSMoveLasersToEdge(Entity enemy_array[], int hitbox_array[][4], const int hitbox_array_len)
{
    //
    // Moving lasers to the edge of the screen
    //

    if (SSSuperSentinelInformation.MoveLasers == 1)
    {
        int x_collision = EntityMoveX(
            &enemy_array[1],
            -0.5f,
            hitbox_array, hitbox_array_len
        );

        x_collision |= EntityMoveX(
            &enemy_array[2],
            0.5f,
            hitbox_array, hitbox_array_len
        );

        if (x_collision) { SSSuperSentinelInformation.MoveLasers = 2; }

        return;
    }

    //
    // If at the edge of the screen and there is no laser delay, then move to centre
    //

    if (SSSuperSentinelInformation.MoveLasers == 2 && SSSuperSentinelInformation.FireLaserDelay <= 0)
    {
        enemy_array[1].x += 0.25f;
        enemy_array[2].x -= 0.25f;

        const float some_pos = EntityGetRight(&enemy_array[1]);

        if (some_pos - 0.1f < enemy_array[0].x && enemy_array[0].x < some_pos + 0.1f)
        {
            EntitySetRight(&enemy_array[1], enemy_array[0].x);
            EntitySetLeft(&enemy_array[2], EntityGetRight(&enemy_array[0]));
            SSSuperSentinelInformation.MoveLasers = -1;
        }
    }
}

void SSMove(Entity enemy_array[], int hitbox_array[][4], const int hitbox_array_len)
{
    SSStrafeMovement(enemy_array);
    SSMoveLasersToEdge(enemy_array, hitbox_array, hitbox_array_len);
}

void SSAnimate(
    Entity enemy_array[],
    const int priority,
    const int frame_number,
    u16 *body_gfx_mem[4][8],
    u16 *laser_weapon_gfx_mem[4],
    u16 *bang_gfx_mem[4])
{
    if (!(frame_number % 6))
    {
        enemy_array[0].animation_frame_number++;
        enemy_array[0].animation_frame_number %= 4;
    }

    //
    // Drawing the body
    //

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            int bullet_fired = 0;

            if (SSSuperSentinelInformation.BulletsFired >= i * 2 + j + 1)
            {
                bullet_fired = 1;
            }

            oamSet(
                &oamMain,
                1 + j + i * 2,
                (int) enemy_array[0].x + j * 16,
                (int) enemy_array[0].y + i * 16,
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

    //
    // Drawing the laser arms
    //

    oamSet(
        &oamMain,
        5,
        (int) enemy_array[1].x,
        (int) enemy_array[1].y,
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
        (int) enemy_array[2].x,
        (int) enemy_array[2].y,
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
}

void SSFireSalvo(Entity enemy_array[], int player_center[2], Bullet bullet_array[], int bullet_array_len)
{
    if (enemy_array[0].current_bullet_delay > 0) { enemy_array[0].current_bullet_delay--; }
    else
    {
        if (SSSuperSentinelInformation.BulletsFired < 4)
        {
            //
            // Firing the salvo
            //

            // Get the top left of the main body
            float segment_centre[2] = {enemy_array[0].x, enemy_array[0].y};

            // Adjusting position for the centre of the segment to fire
            switch (SSSuperSentinelInformation.BulletsFired)
            {
                case 0: // Top left bullet
                    segment_centre[0] += 8;
                    segment_centre[1] += 8;
                    break;
                case 1: // Top right bullet
                    segment_centre[0] += 16 + 8;
                    segment_centre[1] += 8;
                    break;
                case 2: // Bottom left bullet
                    segment_centre[0] += 10;
                    segment_centre[1] += 16 + 7;
                    break;
                case 3: // Bottom right bullet
                    segment_centre[0] += 16 + 6;
                    segment_centre[1] += 16 + 7;
                    break;
                default: // Should never occur
                    break;
            }

            // Angle to the player
            const float angle = GetAngleFromOriginTo(
                (float) player_center[0] - segment_centre[0],
                (float) player_center[1] - segment_centre[1]
            );

            // Adjusting position for the start position of the bullets
            switch (SSSuperSentinelInformation.BulletsFired)
            {
                case 0: // Top left bullet
                case 1: // Top right bullet
                    segment_centre[0] -= 4;
                    segment_centre[1] -= 4;
                    break;
                case 2: // Bottom left bullet
                case 3: // Bottom right bullet
                    segment_centre[0] -= 4;
                    segment_centre[1] -= 5;
                    break;
                default: // Should never occur
                    break;
            }

            BulletSetupInBulletArray(
                bullet_array, bullet_array_len,
                segment_centre[0], segment_centre[1],
                8,
                8,
                angle,
                1,
                270,
                1,
                BulletType_BossBullet
            );

            SSSuperSentinelInformation.BulletsFired++;

            enemy_array[0].current_bullet_delay = SS_SALVO_DELAY;
        }
        else
        {
            // Reloading the salvo
            enemy_array[0].current_bullet_delay = enemy_array[0].bullet_delay;
            SSSuperSentinelInformation.BulletsFired = 0;
        }
    }
}

void SSFireLaser(Entity enemy_array[], Bullet bullet_array[], const int bullet_array_len)
{
    if (!SSSuperSentinelInformation.FireLaser)
    {
        // Don't fire the laser
        // De init the lasers just in case
        // Mainly because sometimes they are left on for too long
        LaserDeInitLaserStruct(&SSSuperSentinelInformation.Lasers[0], bullet_array);
        LaserDeInitLaserStruct(&SSSuperSentinelInformation.Lasers[1], bullet_array);
        return;
    }

    if (SSSuperSentinelInformation.FireLaserDelay > 0)
    {
        // Fire pilot lasers
        if (SSSuperSentinelInformation.FireLaserDelay == SS_LASER_DELAY)
        {
            // Create them
            // De-initialise just in case
            LaserDeInitLaserStruct(
                &SSSuperSentinelInformation.Lasers[0],
                bullet_array
            );
            LaserDeInitLaserStruct(
                &SSSuperSentinelInformation.Lasers[1],
                bullet_array
            );

            if (SSSuperSentinelInformation.LaserToFireFrom)
            {
                // If only from left or right
                LaserInitLaserStruct(
                    &SSSuperSentinelInformation.Lasers[0],
                    bullet_array, bullet_array_len,
                    (int) enemy_array[SSSuperSentinelInformation.LaserToFireFrom].x + 1,
                    (int) enemy_array[SSSuperSentinelInformation.LaserToFireFrom].y + 32,
                    1
                );
            }
            else
            {
                // If from both at once
                for (int i = 0; i < 2; ++i)
                {
                    LaserInitLaserStruct(
                        &SSSuperSentinelInformation.Lasers[i],
                        bullet_array, bullet_array_len,
                        (int) enemy_array[i + 1].x + 1,
                        (int) enemy_array[i + 1].y + 32,
                        1
                    );
                }
            }
        }
        SSSuperSentinelInformation.FireLaserDelay--;
    }

    if (SSSuperSentinelInformation.FireLaserDelay == 0)
    {
        // Create the lasers
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

        if (SSSuperSentinelInformation.LaserToFireFrom)
        {
            // If only from left or right
            LaserInitLaserStruct(
                &SSSuperSentinelInformation.Lasers[0],
                bullet_array, bullet_array_len,
                (int) enemy_array[SSSuperSentinelInformation.LaserToFireFrom].x + 1,
                (int) enemy_array[SSSuperSentinelInformation.LaserToFireFrom].y + 32,
                0
            );
        }
        else
        {
            // If from both at once
            for (int i = 0; i < 2; ++i)
            {
                LaserInitLaserStruct(
                    &SSSuperSentinelInformation.Lasers[i],
                    bullet_array, bullet_array_len,
                    (int) enemy_array[i + 1].x + 1,
                    (int) enemy_array[i + 1].y + 32,
                    0
                );
            }
        }
    }

    if (SSSuperSentinelInformation.LaserToFireFrom)
    {
        // If only from left or right
        LaserUpdate(
            &SSSuperSentinelInformation.Lasers[0],
            bullet_array,
            (int) enemy_array[SSSuperSentinelInformation.LaserToFireFrom].x + 1,
            (int) enemy_array[SSSuperSentinelInformation.LaserToFireFrom].y + 32
        );
    }
    else
    {
        // If from both at once
        for (int i = 0; i < 2; ++i)
        {
            LaserUpdate(
                &SSSuperSentinelInformation.Lasers[i],
                bullet_array,
                (int) enemy_array[i + 1].x + 1,
                (int) enemy_array[i + 1].y + 32
            );
        }
    }
}

void SSThink(Entity enemy_array[])
{
    //
    // Understanding if I have died
    //

    if (SSGetHealth(enemy_array) <= 0)
    {
        SSSuperSentinelInformation.FireLaser = 0;
        enemy_array[0].current_bullet_delay = 15;
        return;
    }

    //
    // Swapping my direction if a boarder is hit
    //

    if (SSSuperSentinelInformation.HitBoarder)
    {
        if (SSSuperSentinelInformation.MoveDirection && enemy_array[0].x >= SS_MAX_RIGHT_X)
        {
            SSSuperSentinelInformation.MoveDirection = 0;
            SSSetPosition(
                enemy_array,
                SS_MAX_RIGHT_X,
                enemy_array[0].y
            );

            if (SSGetHealth(enemy_array) < SS_LASER_HEALTH)
            {
                SSSuperSentinelInformation.LaserToFireFrom = 2;
                if (SSSuperSentinelInformation.FireLaserDelay <= 0)
                {
                    SSSuperSentinelInformation.FireLaserDelay = SS_LASER_DELAY;
                }
                SSSuperSentinelInformation.FireLaser = 1;
            }
        }
        else if (enemy_array[0].x <= SS_MAX_LEFT_X)
        {
            SSSuperSentinelInformation.MoveDirection = 1;
            SSSetPosition(
                enemy_array,
                SS_MAX_LEFT_X,
                enemy_array[0].y
            );

            if (SSGetHealth(enemy_array) < SS_LASER_HEALTH)
            {
                SSSuperSentinelInformation.LaserToFireFrom = 1;
                if (SSSuperSentinelInformation.FireLaserDelay <= 0)
                {
                    SSSuperSentinelInformation.FireLaserDelay = SS_LASER_DELAY;
                }
                SSSuperSentinelInformation.FireLaser = 1;
            }
        }
    }

    // Checking my health level for the final stage
    const int health = SSGetHealth(enemy_array);
    if (health > SS_FINAL_HEALTH) { return; }

    //
    // If my health is below a certain amount, then navigate to the middle, stop moving and stop laser firing
    //

    if (SSSuperSentinelInformation.Moving)
    {
        // Check for if we are at the middle
        const float left_x = (float) SS_CENTER_X - 0.1f;
        const float right_x = (float) SS_CENTER_X + 0.1f;
        if (left_x < enemy_array[0].x && enemy_array[0].x < right_x)
        {
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

    //
    // If I am at the middle, then wait till a bullet salvo finishes, then release the lasers
    //

    if (!SSSuperSentinelInformation.MoveLasers)
    {
        if (SSSuperSentinelInformation.BulletsFired == 0 &&
            enemy_array[0].current_bullet_delay == enemy_array[0].bullet_delay)
        {
            SSSuperSentinelInformation.MoveLasers = 1;
        }
    }

    //
    // Once lasers have reached the edge, then fire them and move them back
    //

    if (SSSuperSentinelInformation.FireLaser <= 0 && SSSuperSentinelInformation.MoveLasers == 2)
    {
        SSSuperSentinelInformation.FireLaserDelay = SS_LASER_DELAY;
        SSSuperSentinelInformation.LaserToFireFrom = 0;
        SSSuperSentinelInformation.FireLaser = 1;
    }
}

void SSSetupForGameLoop(
    Entity *player,
    Entity enemy_array[],
    const int enemy_array_len,
    Bullet bullet_array[],
    const int bullet_array_len,
    int *frame_number,
    _GFXAllSpriteGFX *all_sprite_gfx,
    const int bg_id)
{
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
    while (player_spawn_counter || boss_spawn_counter)
    {
        // Clear the text
        consoleClear();
        // Frame increment
        (*frame_number)++;

        // Player timer
        if (player_spawn_counter)
        {
            player_spawn_counter--;
        }

        // Animating the player portal
        oamSet(
            &oamMain,
            BULLET_ID_START, // Use the bullet sprite IDs as there are no bullets in the setup
            (int) player->x - 1,
            (int) player->y - 1,
            0,
            0,
            SpriteSize_16x16,
            SpriteColorFormat_256Color,
            all_sprite_gfx->PortalGFXMem[15 - player_spawn_counter / 2],
            -1,
            0,
            !player_spawn_counter,
            0,
            0,
            0
        );

        // Drawing the player when the portal is halfway done
        if (player_spawn_counter < 16)
        {
            PlayerAnimate(
                player,
                (bool) player_spawn_counter,
                *frame_number,
                all_sprite_gfx->PlayerGFXMem,
                all_sprite_gfx->PortalGFXMem);
        }

        // If the player is finished
        if (!player_spawn_counter)
        {
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

int SSRunGameLoop(
    Entity *player,
    Entity enemy_array[],
    const int enemy_array_len,
    Bullet bullet_array[],
    const int bullet_array_len,
    int *frame_number,
    _GFXAllSpriteGFX *all_sprite_gfx,
    int playable_area[4],
    int hitbox_array[][4],
    const int hitbox_array_len)
{
    // Keys

    // Data
    int player_center[2];

    //
    BulletType bullet_collision_type = BulletType_PlayerBullet;
    BulletType bullet_collision_type_temp = BulletType_PlayerBullet;

    EntityType enemy_collision_type = EntityType_Player;
    EntityType enemy_collision_type_temp = EntityType_Player;

    // Exit delays
    int player_death_exit_delay = EXIT_DELAY;
    int win_condition_exit_delay = EXIT_DELAY;

    // True when the player has won
    int win_condition = 0;

    // Scanning information
    char *super_sentinel_states[4] = {
        "Optimal",
        "Damaged",
        "Vulnerable",
        "Critical"
    };

    while (1)
    {
        // Clear the text
        consoleClear();
        // Get key presses
        scanKeys();
        const u32 keys = keysHeld();
        // Frame number
        (*frame_number)++;

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

        // Get the player centre
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

        //
        // Checking what killed the player if they died
        //

        if (bullet_collision_type_temp != BulletType_PlayerBullet && bullet_collision_type == BulletType_PlayerBullet)
        {
            bullet_collision_type = bullet_collision_type_temp;
        }
        if (enemy_collision_type_temp != EntityType_Player && enemy_collision_type == EntityType_Player)
        {
            enemy_collision_type = enemy_collision_type_temp;
        }

        //
        // Checking for end condition
        // If end condition is met, wait 60 frames before exiting
        // Player death is checked first
        //

        // If the player is dead
        if (player->dead)
        {
            player_death_exit_delay--;
            if (!player_death_exit_delay)
            {
                return 0;
            }
        }
        else if (!win_condition)
        {
            // If the boss is dead and no bullets are alive, then assume true
            win_condition = 1;

            // Check if the boss is dead
            if (SSGetHealth(enemy_array) > 0)
            {
                win_condition = 0;
            }
            // If the boss is dead, check for bullets that are alive
            if (win_condition)
            {
                for (int i = 0; i < bullet_array_len; i++)
                {
                    // If not a player bullet and if I am alive, then the win condition is false
                    if (bullet_array[i].type != BulletType_PlayerBullet && bullet_array[i].alive)
                    {
                        win_condition = 0;
                        break;
                    }
                }
            }
        }

        if (win_condition)
        {
            if (win_condition_exit_delay)
            {
                win_condition_exit_delay--;
            }
            else
            {
                break;
            }
        }

        //
        // Showing information to the user
        //

        UIResetDisplayBuffer();

        if (!player->dead)
        {
            UIWriteTextAtOffset("Engaging in Combat", 1, 1);

            UIWriteTextAtOffset("Enemy State: ", 3, 1);

            const int super_sentinel_health = SSGetHealth(enemy_array);

            if (super_sentinel_health > SS_LASER_HEALTH)
            {
                UIWriteTextAtOffset(super_sentinel_states[0], 3, 14);
            }
            else if (super_sentinel_health > SS_FINAL_HEALTH)
            {
                UIWriteTextAtOffset(super_sentinel_states[1], 3, 14);
            }
            else if (super_sentinel_health > SS_CRITICAL_HEALTH)
            {
                UIWriteTextAtOffset(super_sentinel_states[2], 3, 14);
            }
            else
            {
                UIWriteTextAtOffset(super_sentinel_states[3], 3, 14);
            }
        }
        else
        {
            UIWriteTextAtOffset("Initiating Temporal Reset", 1, 1);
        }

        UIPrintDisplayBuffer();

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
        // To exit
        if (keys & KEY_SELECT) { return -1; }
    }

    return 1;
}

void SSRunEndLoop(
    Entity *player,
    Entity enemy_array[],
    int enemy_array_len,
    Bullet bullet_array[],
    int bullet_array_len,
    int *frame_number,
    _GFXAllSpriteGFX *all_sprite_gfx)
{
    // Hide everything not being used, aka player and boss sprites 0-6, aka just the bullets
    for (int i = 7; i < 128; i++)
    {
        oamSetHidden(
            &oamMain,
            i,
            true
        );
    }

    // Lasers shake then three explosions before they explode
    // Then the middle section falls apart

    // Setting up rotation matrices
    for (int i = 0; i < 32; i++)
    {
        oamRotateScale(
            &oamMain,
            i,
            (int) (32767.f / 32 * (float) i),
            256, 256);
    }

    // Setup of other values
    float vectors[4][2];
    int angles[4];
    float positions[4][2];
    int bullets_fired[4] = {0};
    int rotation_directions[6] = {0, 0, 0, 1, 1, 1}; // At least 1 will be in a different direction

    // To make some rotations in different directions
    ShuffleIntArray(rotation_directions, 6);

    // Creating angles n vectors n stuff
    for (int i = 0; i < 4; i++)
    {
        angles[i] = rand() % 180 + 180; // Angle pointing down
        // Muh vector
        const float angle_in_radians = (float) angles[i] / 180.f * 3.14f;
        vectors[i][0] = cosf(angle_in_radians);
        vectors[i][1] = -sinf(angle_in_radians);

        // Getting the position
        const int x = i % 2;
        const int y = i > 1;
        positions[i][0] = enemy_array[0].x + (float) x * 16 - 8;
        positions[i][1] = enemy_array[0].y + (float) y * 16 - 8;

        // Making sure the right sprite is used
        if (SSSuperSentinelInformation.BulletsFired >= i + 1)
        {
            bullets_fired[i] = 1;
        }
    }

    // Shakey shake shake
    int shakes[6] = {-1, -1, 0, 0, 1, 1};
    int shakes_index = 0;
    ShuffleIntArray(shakes, 6);

    // Hiding the old lasers as the new lasers have a different id
    oamSetHidden(
        &oamMain,
        5,
        true);
    oamSetHidden(
        &oamMain,
        6,
        true);

    // Exploding the lasers
    // Shakey explosive time
    int laser_death_counter = 120; // 64 + 16 + 14 * 2 + (12 cause round number and I want some delay)
    // 64 frames of shake
    // Then the explosions
    while (laser_death_counter > 0)
    {
        // Clear the text
        consoleClear();
        // Frame number
        (*frame_number)++;

        // Drawing the lasers
        if (laser_death_counter > 13)
        {
            oamSet(
                &oamMain,
                20,
                (int) enemy_array[1].x + shakes[shakes_index],
                (int) enemy_array[1].y + shakes[(shakes_index + 1) % 6],
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
                (int) enemy_array[2].x + shakes[(shakes_index + 2) % 6],
                (int) enemy_array[2].y + shakes[(shakes_index + 3) % 6],
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
            if (laser_death_counter % 4 == 1)
            {
                shakes_index++;
                shakes_index = shakes_index % 6;
            }
        }

        // EXPLOOOOOOSION!!!
        // Three explosions on the laser body
        // The height is 32, the middle one has its centre at 16 and the other two are 8 above and below
        if (laser_death_counter < 56)
        {
            const int frame = 55 - laser_death_counter;
            if (frame < 16)
            {
                oamSet(
                    &oamMain,
                    12,
                    (int) enemy_array[1].x,
                    (int) enemy_array[1].y - 2,
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
                    (int) enemy_array[2].x,
                    (int) enemy_array[2].y - 2,
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
        if (laser_death_counter < 42)
        {
            const int frame = 41 - laser_death_counter;
            if (frame < 16)
            {
                oamSet(
                    &oamMain,
                    11,
                    (int) enemy_array[1].x,
                    (int) enemy_array[1].y + 8,
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
                    (int) enemy_array[2].x,
                    (int) enemy_array[2].y + 8,
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
        if (laser_death_counter < 28)
        {
            const int frame = 27 - laser_death_counter;
            if (frame < 16)
            {
                oamSet(
                    &oamMain,
                    10,
                    (int) enemy_array[1].x,
                    (int) enemy_array[1].y + 18,
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
                    (int) enemy_array[2].x,
                    (int) enemy_array[2].y + 18,
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

        // Hiding the lasers
        if (laser_death_counter == 13)
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

        laser_death_counter--;

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
    }

    // Hiding the old body id for the same reason as the lasers
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
                (int) positions[i][0],
                (int) positions[i][1],
                0,
                0,
                SpriteSize_16x16,
                SpriteColorFormat_256Color,
                all_sprite_gfx->SSBodyGFXMem[i][4 * bullets_fired[i] + enemy_array[0].animation_frame_number],
                rotation_directions[i] ? body_split_counter / 8 : 31 - body_split_counter / 8,
                true,
                false,
                false,
                false,
                false);
        }

        // Moving the body and lasers
        body_split_counter--;
        if (body_split_counter % 8 == 1)
        {
            for (int i = 0; i < 4; i++)
            {
                positions[i][0] += vectors[i][0];
                positions[i][1] += vectors[i][1];
            }
        }

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
                // As these positions are for the sized doubled versions
                (int) positions[i][0] + 8,
                (int) positions[i][1] + 8,
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

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
    }

    // Finally, we hide the explosions
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
    // Waiting
    swiWaitForVBlank();
    // Update the screen
    oamUpdate(&oamMain);
}
