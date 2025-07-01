#include "SuperShredder.h"

#include <nds.h>
#include <stdio.h>
#include <stdlib.h>

#include "Constants.h"
#include "Entity.h"
#include "GFX.h"
#include "Other.h"
#include "Player.h"
#include "UI.h"

_SuperShredderInformation SuperShredder_Information;

void SuperShredder_Setup(Entity enemy_array[], const int enemy_array_len)
{
    // Reset all the entities
    EntityInitEntityArray(enemy_array, enemy_array_len);

    // We use one entity, the first one, as the Super Shredder entity
    EntitySetup(
        &enemy_array[0],
        SUPERSHREDDER_START_X,
        SUPERSHREDDER_START_Y,
        21, 21,
        SUPERSHREDDER_HEALTH,
        EntityType_SuperShredder,
        SUPERSHREDDER_BULLET_DELAY
    );

    // Setting up the information struct
    SuperShredder_Information.vx = 0;
    SuperShredder_Information.vy = 0;

    // We can do a fun thing to try and get the shredders in the paint scheme of the Super Sentinel.
    // Mainly by messing about with the colour palette.
    // By inspecting the palette memory, we can figure out which one is the dark-green and replace that with dark-blue.
}

void SuperShredder_Move(Entity enemy_array[])
{
    enemy_array[0].x += SuperShredder_Information.vx;
    enemy_array[0].y += SuperShredder_Information.vy;
}

/// Determines if the Super Shredder is out of bounds given its movement vector.
/// Used during the entry of the Super Shredder and only works if the vector has one element 0.
static int IsSuperShredderOutOfBounds(const float vx, const float vy, const Entity enemy_array[])
{
    return (vx > 0 && enemy_array[0].x > SCREEN_WIDTH + SUPERSHREDDER_WIDTH)      // Moving right
           || (vx < 0 && enemy_array[0].x < -SUPERSHREDDER_WIDTH * 2)             // Moving left
           || (vy > 0 && enemy_array[0].y > SCREEN_HEIGHT + SUPERSHREDDER_HEIGHT) // Moving up
           || (vy < 0 && enemy_array[0].y < -SUPERSHREDDER_HEIGHT * 2);           // Moving down
}

/// Selects a screen edge and direction to move along for the Super Shredder entry.
/// Modifies the movement vector in <c>SuperShredder_Information</c> and the position of the boss entity in
///  <c>enemy_array</c>.
static void PickScreenEdgeAndDirection(const int screen_boarders[], const int boss_entry_stage, Entity enemy_array[])
{
    const int edge_and_direction[4][2][4] = {
        //                       ^  ^  ^-- [x, y, vx, vy]
        //                       |  +----- Going left/up or right/down direction
        //                       |-------- [Top, Left, Bottom, Right] directions

        // Top of the screen
        {
            // Travel Left
            {
                SCREEN_WIDTH + SUPERSHREDDER_WIDTH, -SUPERSHREDDER_HEIGHT / 2,
                -SUPERSHREDDER_ENTRY_SPEED, 0
            },
            // Travel Right
            {
                -SUPERSHREDDER_WIDTH, -SUPERSHREDDER_HEIGHT / 2,
                SUPERSHREDDER_ENTRY_SPEED, 0
            },
        },
        // Left of the screen
        {
            // Travel Up
            {
                -SUPERSHREDDER_WIDTH / 2, SCREEN_HEIGHT + SUPERSHREDDER_HEIGHT,
                0, -SUPERSHREDDER_ENTRY_SPEED
            },
            // Travel Down
            {
                -SUPERSHREDDER_WIDTH / 2, -SUPERSHREDDER_HEIGHT,
                0, SUPERSHREDDER_ENTRY_SPEED
            },
        },
        // Bottom of the screen
        {
            // Travel left
            {
                SCREEN_WIDTH + SUPERSHREDDER_WIDTH, SCREEN_HEIGHT - SUPERSHREDDER_HEIGHT / 2,
                -SUPERSHREDDER_ENTRY_SPEED, 0
            },
            // Travel right
            {
                -SUPERSHREDDER_WIDTH, SCREEN_HEIGHT - SUPERSHREDDER_HEIGHT / 2,
                SUPERSHREDDER_ENTRY_SPEED, 0
            },
        },
        // Right of the screen
        {
            // Travel Up
            {
                SCREEN_WIDTH - SUPERSHREDDER_WIDTH / 2, SCREEN_HEIGHT + SUPERSHREDDER_HEIGHT,
                0, -SUPERSHREDDER_ENTRY_SPEED
            },
            // Travel Down
            {
                SCREEN_WIDTH - SUPERSHREDDER_WIDTH / 2, -SUPERSHREDDER_HEIGHT,
                0, SUPERSHREDDER_ENTRY_SPEED
            }
        }
    };

    // Choose a side and direction
    const int screen_boarder = screen_boarders[boss_entry_stage];
    const int movement_dir = rand() % 2;

    enemy_array[0].x = (float) edge_and_direction[screen_boarder][movement_dir][0];
    enemy_array[0].y = (float) edge_and_direction[screen_boarder][movement_dir][1];

    SuperShredder_Information.vx = (float) edge_and_direction[screen_boarder][movement_dir][2];
    SuperShredder_Information.vy = (float) edge_and_direction[screen_boarder][movement_dir][3];
}

void SuperShredder_SetupForGameLoop(
    Entity *player,
    Entity enemy_array[],
    const int enemy_array_len,
    Bullet bullet_array[],
    const int bullet_array_len,
    int *frame_number,
    const int bg_id
)
{
    // Simple setup
    PlayerSetup(player);
    BulletInitBulletArray(bullet_array, bullet_array_len);
    SuperShredder_Setup(enemy_array, enemy_array_len);

    // Hide the sprites
    HideEverySprite();

    // Screen shakey time
    int screen_shake_x[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    int screen_shake_y[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    ShuffleIntArray(screen_shake_x, 9);
    ShuffleIntArray(screen_shake_y, 9);
    int screen_shake_index = 0;

    // Boss entry stuff
    // 0 -> top
    // 1 -> left
    // 2 -> bottom
    // 3 -> right
    int screen_boarders[4] = {0, 1, 2, 3};
    ShuffleIntArray(screen_boarders, 4);

    // Loop timing stuff
    int player_spawn_counter = 32;
    int random_counter = 120;
    int boss_entry_stage = 0;
    int boss_moving = 0;
    while (boss_entry_stage != 6 || player_spawn_counter)
    {
        consoleClear();
        (*frame_number)++;

        // Player timer
        if (player_spawn_counter > 0) { player_spawn_counter--; }

        // Player portal
        oamSet(
            &oamMain,
            BULLET_ID_START,
            (int) player->x - 1,
            (int) player->y - 1,
            0,
            0,
            SpriteSize_16x16,
            SpriteColorFormat_256Color,
            GFXAllSpriteGFX.PortalGFXMem[15 - player_spawn_counter / 2],
            -1,
            0,
            !player_spawn_counter,
            0,
            0,
            0
        );

        // Drawing the player when halfway through the portal
        if (player_spawn_counter < 16)
        {
            PlayerAnimate(
                player,
                (bool) player_spawn_counter,
                *frame_number,
                GFXAllSpriteGFX.PlayerGFXMem,
                GFXAllSpriteGFX.PlayerExplosionGFXMem
            );
        }

        oamSet(
            &oamMain,
            37,
            (int) enemy_array[0].x, (int) enemy_array[0].y,
            0,
            0,
            SpriteSize_32x32,
            SpriteColorFormat_256Color,
            GFXAllSpriteGFX.SuperShredderGFXMem[(*frame_number / 2) % 4],
            -1,
            0,
            0,
            0,
            0,
            0
        );

        // If the player is finished
        if (player_spawn_counter == 0 && boss_entry_stage < 5)
        {
            // If not moving, pick a screen edge and a direction to move and set position
            if (boss_moving == 0 && boss_entry_stage < 4)
            {
                PickScreenEdgeAndDirection(screen_boarders, boss_entry_stage, enemy_array);
                boss_entry_stage++;
                boss_moving = 1;
            }

            // Move the boss according to its vector
            SuperShredder_Move(enemy_array);

            // Check if the boss is outside the screen in the direction of movement
            if (IsSuperShredderOutOfBounds(SuperShredder_Information.vx, SuperShredder_Information.vy, enemy_array))
            {
                boss_moving = 0;
            }

            // Finally screen shake
            bgSetScroll(bg_id, screen_shake_x[screen_shake_index], screen_shake_y[screen_shake_index]);
            bgUpdate();
            screen_shake_index++;
            screen_shake_index %= 9;
        }

        // If the boss has finished moving, and we are at the last stage, then we are all done
        if (boss_moving == 0 && boss_entry_stage == 4) { boss_entry_stage = 5; }

        if (boss_entry_stage == 5) // ToDo: Remove this and set loop end condition to 5 instead of 6
        {
            if (random_counter > 0) { random_counter--; }
            else { boss_entry_stage = 6; }
        }

        // Lore, kinda
        UIResetDisplayBuffer();

        UIWriteTextAtOffset(
            "Warping in",
            1,
            1
        );

        char temp[UI_NUM_CHARS + 1];

        sprintf(temp, "psc = %d", player_spawn_counter);
        UIWriteTextAtOffset(temp, 3, 1);

        sprintf(temp, "bes = %d", boss_entry_stage);
        UIWriteTextAtOffset(temp, 4, 1);

        sprintf(temp, "rc = %d", random_counter);
        UIWriteTextAtOffset(temp, 5, 1);

        sprintf(temp, "boss pos = %.1f, %.1f", enemy_array[0].x, enemy_array[0].y);
        UIWriteTextAtOffset(temp, 6, 1);

        sprintf(temp, "boss vec = %.1f, %.1f", SuperShredder_Information.vx, SuperShredder_Information.vy);
        UIWriteTextAtOffset(temp, 7, 1);

        UIPrintDisplayBuffer();

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
    }

    // Finishing up by clearing any scroll from the shaking
    bgSetScroll(bg_id, 0, 0);
    bgUpdate();
}
