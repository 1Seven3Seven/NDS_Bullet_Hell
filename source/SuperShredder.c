#include "SuperShredder.h"

#include <nds.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Constants.h"
#include "Enemies.h"
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
        SUPERSHREDDER_WIDTH, SUPERSHREDDER_HEIGHT,
        SUPERSHREDDER_HEALTH,
        EntityType_SuperShredder,
        SUPERSHREDDER_BULLET_DELAY
    );

    // Setting up the information struct
    SuperShredder_Information.vector[0] = 0;
    SuperShredder_Information.vector[1] = 0;

    // Initially we choose the attack to be performed.
    SuperShredder_Information.State = SuperShredderState_AttackPreamble;

    // We can do a fun thing to try and get the shredders in the paint scheme of the Super Sentinel.
    // Mainly by messing about with the colour palette.
    // By inspecting the palette memory, we can figure out which one is the dark-green and replace that with dark-blue.
}

void SuperShredder_Move(Entity *super_shredder)
{
    if (super_shredder->current_bullet_delay != 0) { return; }

    // We don't use the EntityMove function as we do not need collision with environment hitboxes
    super_shredder->x += SuperShredder_Information.vector[0];
    super_shredder->y += SuperShredder_Information.vector[1];
}

/// Choose the attack to be used based on the current health.
/// Also set up the bullet/attack delay.
static void AttackPreamble(Entity *super_shredder)
{
    // Choose an attack to be used
    // ToDo: think about the other attacks
    SuperShredder_Information.State = SuperShredderState_ThinkAboutRushAttack;

    // Default wait period
    super_shredder->current_bullet_delay = super_shredder->bullet_delay;
}

/// Chooses a random starting position for the rush attack around the edge of the screen.
static void ChooseRushStartPosition(Entity *super_shredder)
{
    // 0 -> top
    // 1 -> left
    // 2 -> bottom
    // 3 -> right
    const int choice = rand() % 4;

    if (choice % 2 == 0) // Top or bottom
    {
        // Choose a random x position
        super_shredder->x = (float) (rand() % (SCREEN_WIDTH + SUPERSHREDDER_WIDTH))
                            - (float) SUPERSHREDDER_WIDTH / 2;

        // Choose the top or bottom based on the choice
        super_shredder->y = choice == 2 ? SCREEN_HEIGHT + SUPERSHREDDER_HEIGHT : -SUPERSHREDDER_HEIGHT;
    }
    else // Left or right
    {
        // Choose a random y position
        super_shredder->y = (float) (rand() % (SCREEN_HEIGHT + SUPERSHREDDER_HEIGHT))
                            - (float) SUPERSHREDDER_HEIGHT / 2;

        // Choose the left or right based on the choice
        super_shredder->x = choice == 3 ? SCREEN_WIDTH + SUPERSHREDDER_WIDTH : -SUPERSHREDDER_WIDTH;
    }
}

/// Contains some information regarding a collision with a wall.
typedef struct WallHitInformation_s
{
    /// If this struct's information is valid.
    int valid;
    /// The point of intersection/collision.
    float x, y;
    /// The length of the vector up to this point of intersection.
    float t;
} WallHitInformation;

/// Given the centre of the Super shredder and the y value of a horizontal wall, find the point of intersection.
///
/// `valid` will be false if the vector in <c>SuperShredder_Information</c> is parallel.
/// `t` represents the length of the vector from the centre of the Super Shredder to the point of intersection.
static WallHitInformation FindHorizontalWallHit(const float my_centre[2], const float wall_y)
{
    WallHitInformation horizontal_hit_info = {0};

    if (SuperShredder_Information.vector[0] != 0)
    {
        horizontal_hit_info.y = wall_y;
        // Parametric equations go brr
        horizontal_hit_info.t = (horizontal_hit_info.y - (float) my_centre[1]) / SuperShredder_Information.vector[1];
        horizontal_hit_info.x = (float) my_centre[0] + SuperShredder_Information.vector[0] * horizontal_hit_info.t;

        // A little extra just in case
        if (horizontal_hit_info.x >= -0.001 && horizontal_hit_info.x <= SCREEN_WIDTH + 0.001)
        {
            horizontal_hit_info.valid = 1;
        }
    }

    return horizontal_hit_info;
}

/// Given the centre of the Super shredder and the x value of a vertical wall, find the point of intersection.
///
/// `valid` will be false if the vector in <c>SuperShredder_Information</c> is parallel.
/// `t` represents the length of the vector from the centre of the Super Shredder to the point of intersection.
static WallHitInformation FindVerticalWallHit(const float my_centre[2], const float wall_x)
{
    WallHitInformation vertical_hit_info = {0};
    if (SuperShredder_Information.vector[1] != 0)
    {
        vertical_hit_info.x = wall_x;
        vertical_hit_info.t = (vertical_hit_info.x - (float) my_centre[0]) / SuperShredder_Information.vector[0];
        vertical_hit_info.y = (float) my_centre[1] + SuperShredder_Information.vector[1] * vertical_hit_info.t;

        if (vertical_hit_info.y >= -0.001 && vertical_hit_info.y <= SCREEN_HEIGHT + 0.001)
        {
            vertical_hit_info.valid = 1;
        }
    }
    return vertical_hit_info;
}

/// Given two points of intersection, pick the closest valid point of intersection and save it in the given vector.
///
/// Assumes that one of the points of intersection is valid.
static void SaveClosestValidHitLocation(
    const WallHitInformation *horizontal_hit_info,
    const WallHitInformation *vertical_hit_info,
    float save_location[2]
)
{
    if (horizontal_hit_info->valid && vertical_hit_info->valid)
    {
        // Both are valid, so pick the closest one
        if (horizontal_hit_info->t < vertical_hit_info->t)
        {
            save_location[0] = horizontal_hit_info->x;
            save_location[1] = horizontal_hit_info->y;
        }
        else
        {
            save_location[0] = vertical_hit_info->x;
            save_location[1] = vertical_hit_info->y;
        }
    }
    else if (horizontal_hit_info->valid)
    {
        // Only the x collision is valid
        save_location[0] = horizontal_hit_info->x;
        save_location[1] = horizontal_hit_info->y;
    }
    else
    {
        // Only the y collision is valid
        save_location[0] = vertical_hit_info->x;
        save_location[1] = vertical_hit_info->y;
    }

    // We do not need to handle the case where both are invalid as that should never occur given the circumstances where
    //  this is called
}

/// Determines if the Super Shredder is out of bounds.
static int IsSuperShredderOutOfBounds(const Entity *super_shredder)
{
    return (SuperShredder_Information.vector[0] > 0 && super_shredder->x > SCREEN_WIDTH)              // Moving right
           || (SuperShredder_Information.vector[0] < 0 && super_shredder->x < -SUPERSHREDDER_WIDTH)   // Moving left
           || (SuperShredder_Information.vector[1] > 0 && super_shredder->y > SCREEN_HEIGHT)          // Moving up
           || (SuperShredder_Information.vector[1] < 0 && super_shredder->y < -SUPERSHREDDER_HEIGHT); // Moving down
}

/// Using the Super Shredder's Position and the vector from it to the player, we find the collision point using the line
///  from the centre of the Super Shredder and its vector with the screen boarders.
///
/// Sets the <c>vector</c>, <c>screen_boarder_hit_location</c> and <c>rush_telegraph_vector_step</c> values of
///  <c>SuperShredder_Information</c>.
///
/// A big boi function.
static void DevelopVectorFindScreenEdgeCollision(const Entity *super_shredder, const Entity *player)
{
    // Our beautiful vector

    int my_centre[2];
    EntityGetCenterArray(super_shredder, my_centre);
    const float my_centre_f[2] = {(float) my_centre[0], (float) my_centre[1]};

    int player_center[2];
    EntityGetCenterArray(player, player_center);

    GetVectorFromTo(
        (float) my_centre[0], (float) my_centre[1],
        (float) player_center[0], (float) player_center[1],
        SUPERSHREDDER_SPEED,
        SuperShredder_Information.vector
    );

    // Find the two collision points, one with the closest vertical wall, the other with the closest horizontal one

    const WallHitInformation close_horizontal_hit_info = FindHorizontalWallHit(
        my_centre_f,
        SuperShredder_Information.vector[1] > 0 ? 0 : SCREEN_HEIGHT
    );

    const WallHitInformation vertical_hit_info = FindVerticalWallHit(
        my_centre_f,
        SuperShredder_Information.vector[0] > 0 ? 0 : SCREEN_WIDTH
    );

    // Decide which collision to use

    SaveClosestValidHitLocation(
        &close_horizontal_hit_info,
        &vertical_hit_info,
        SuperShredder_Information.screen_boarder_hit_location
    );

    // Find the far wall hit location as well

    const WallHitInformation far_horizontal_hit_info = FindHorizontalWallHit(
        my_centre_f,
        SuperShredder_Information.vector[1] < 0 ? 0 : SCREEN_HEIGHT
    );

    const WallHitInformation far_vertical_hit_info = FindVerticalWallHit(
        my_centre_f,
        SuperShredder_Information.vector[0] < 0 ? 0 : SCREEN_WIDTH
    );

    float far_hit_info_location[2];
    SaveClosestValidHitLocation(
        &far_horizontal_hit_info,
        &far_vertical_hit_info,
        far_hit_info_location
    );

    // Find the distance between the two hit locations

    const float x_diff = far_hit_info_location[0] - SuperShredder_Information.screen_boarder_hit_location[0];
    const float y_diff = far_hit_info_location[1] - SuperShredder_Information.screen_boarder_hit_location[1];

    // All done

    SuperShredder_Information.rush_telegraph_vector_step[0] = x_diff / (SUPERSHREDDER_NUM_RUSH_TELEGRAPH_POINTS + 1);
    SuperShredder_Information.rush_telegraph_vector_step[1] = y_diff / (SUPERSHREDDER_NUM_RUSH_TELEGRAPH_POINTS + 1);
}

/// Sets up the attack vector to be used.
/// Summons in the telegraphing sprites using the reload counter to space them.
static void RushPreambleAndTelegraph(Entity *super_shredder, const Entity *player, Bullet bullet_array[])
{
    // If the bullet delay is the max, then we choose a position and develop the vector
    if (super_shredder->current_bullet_delay == super_shredder->bullet_delay)
    {
        ChooseRushStartPosition(super_shredder);
        DevelopVectorFindScreenEdgeCollision(super_shredder, player);
    }

    // Every multiple of 6 for the bullet delay we spawn a wild bullet for the path
    // Ignore the last multiple of 6

    // ToDo: base this off of the number of telegraphs rather than just 6
    const int time_between = 6;
    if (super_shredder->current_bullet_delay > time_between && super_shredder->current_bullet_delay % time_between == 0)
    {
        const int step = (super_shredder->bullet_delay - super_shredder->current_bullet_delay) / time_between + 1;

        BulletSetupInBulletArray(
            bullet_array, MAX_BULLET_COUNT,
            SuperShredder_Information.screen_boarder_hit_location[0]
            + SuperShredder_Information.rush_telegraph_vector_step[0] * (float) step
            - 4, // ToDo: use correct offsets when telegraph sprites are created
            SuperShredder_Information.screen_boarder_hit_location[1]
            + SuperShredder_Information.rush_telegraph_vector_step[1] * (float) step
            - 4, // ToDo: here as well
            3, 3,
            0, 0,
            super_shredder->current_bullet_delay + 8 * (int) step,
            0,
            BulletType_SuperShredderTelegraph
        );
    }

    if (super_shredder->current_bullet_delay > 0) { super_shredder->current_bullet_delay--; }
    else { SuperShredder_Information.State = SuperShredderState_AttackingAkaThinkAboutNoThink; }
}

void SuperShredder_Think(Entity *super_shredder, const Entity *player, Bullet bullet_array[])
{
    // If we are to choose an attack, aka an attack has finished
    if (SuperShredder_Information.State == SuperShredderState_AttackPreamble)
    {
        AttackPreamble(super_shredder);
    }

    switch (SuperShredder_Information.State)
    {
    case SuperShredderState_ThinkAboutRushAttack:
        RushPreambleAndTelegraph(super_shredder, player, bullet_array);
        break;

    case SuperShredderState_AttackingAkaThinkAboutNoThink:
        {
            if (IsSuperShredderOutOfBounds(super_shredder))
            {
                super_shredder->current_bullet_delay = super_shredder->bullet_delay;
                SuperShredder_Information.State = SuperShredderState_AttackPreamble;
            }

            break;
        }

    default: ; // SuperShredderState_AttackPreamble, aka we have already handled this
    }
}

void SuperShredder_Animate(Entity *super_shredder, const int priority, const int frame_number)
{
    if (frame_number % 2)
    {
        super_shredder->animation_frame_number++;
        super_shredder->animation_frame_number %= 4;
    }

    oamSet(
        &oamMain,
        1,
        (int) super_shredder->x,
        (int) super_shredder->y,
        priority,
        0,
        SpriteSize_32x32,
        SpriteColorFormat_256Color,
        GFXAllSpriteGFX.SuperShredderGFXMem[super_shredder->animation_frame_number],
        -1,
        false,
        false,
        false,
        false,
        false
    );
}

void SuperShredder_FireBullets(
    Entity *super_shredder,
    Bullet bullet_array[],
    const int bullet_array_len,
    const int my_centre[2],
    const int player_centre[2]
)
{
    if (super_shredder->health > SUPERSHREDDER_SECOND_STAGE_HEALTH
        && super_shredder->health <= SUPERSHREDDER_HEALTH - 40)
    {
        // Attempt to fire a bullet, we will use the counter attribute of the entity
        // This is normally used for animation purposes but is not used for the Super Shredder
        if (super_shredder->counter > 0)
        {
            super_shredder->counter--;
            return;
        }

        super_shredder->counter = 20;

        const float angle = GetAngleFromOriginTo(
            (float) (player_centre[0] - my_centre[0]),
            (float) (player_centre[1] - my_centre[1])
        );

        for (int i = 0; i < 4; i++)
        {
            BulletSetupInBulletArray(
                bullet_array, bullet_array_len,
                (float) my_centre[0] - 4,
                (float) my_centre[1] - 4,
                8, 8,
                angle + (float) M_PI_2 * (float) i,
                1,
                320,
                1,
                BulletType_BossBullet
            );
        }
    }
}

/// Determines if the Super Shredder is out of bounds given its movement vector.
/// Used during the entry of the Super Shredder and only works if the vector has one element 0.
///
/// Uses slightly different values compared to <c>IsSuperShredderOutOfBounds</c> for timing purposes.
static int IsSuperShredderOutOfBoundsDuringEntry(const float vx, const float vy, const Entity *super_shredder)
{
    return (vx > 0 && super_shredder->x > SCREEN_WIDTH + SUPERSHREDDER_WIDTH)      // Moving right
           || (vx < 0 && super_shredder->x < -SUPERSHREDDER_WIDTH * 2)             // Moving left
           || (vy > 0 && super_shredder->y > SCREEN_HEIGHT + SUPERSHREDDER_HEIGHT) // Moving up
           || (vy < 0 && super_shredder->y < -SUPERSHREDDER_HEIGHT * 2);           // Moving down
}

/// Selects a screen edge and direction to move along for the Super Shredder entry.
/// Modifies the movement vector in <c>SuperShredder_Information</c> and the position of the boss entity in
///  <c>enemy_array</c>.
static void PickScreenEdgeAndDirection(const int screen_boarders[], const int boss_entry_stage, Entity *super_shredder)
{
    const int edge_and_direction[4][2][4] = {
        //                       ^  ^  ^-- [x, y, vx, vy]
        //                       |  |----- Going left/up or right/down direction
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

    super_shredder->x = (float) edge_and_direction[screen_boarder][movement_dir][0];
    super_shredder->y = (float) edge_and_direction[screen_boarder][movement_dir][1];

    SuperShredder_Information.vector[0] = (float) edge_and_direction[screen_boarder][movement_dir][2];
    SuperShredder_Information.vector[1] = (float) edge_and_direction[screen_boarder][movement_dir][3];
}

void SuperShredder_SetupForGameLoop(
    Entity *player,
    Entity enemy_array[],
    const int enemy_array_len,
    Bullet bullet_array[],
    const int bullet_array_len,
    int *frame_number,
    const int bg_id,
    const int num_stages
)
{
    // Simple setup
    PlayerSetup(player);
    BulletInitBulletArray(bullet_array, bullet_array_len);
    SuperShredder_Setup(enemy_array, enemy_array_len);
    Entity *super_shredder = &enemy_array[0];

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
    int boss_entry_stage = 0;
    int boss_moving = 0;
    while (boss_entry_stage != num_stages + 1 || player_spawn_counter)
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
            // ReSharper disable once CppRedundantParentheses
            GFXAllSpriteGFX.SuperShredderGFXMem[(*frame_number / 2) % 4],
            -1,
            0,
            0,
            0,
            0,
            0
        );

        // If the player is finished
        if (player_spawn_counter == 0 && boss_entry_stage < num_stages + 1)
        {
            // If not moving, pick a screen edge and a direction to move and set position
            if (boss_moving == 0 && boss_entry_stage < num_stages)
            {
                PickScreenEdgeAndDirection(screen_boarders, boss_entry_stage, super_shredder);
                boss_entry_stage++;
                boss_moving = 1;
            }

            // Move the boss according to its vector
            SuperShredder_Move(super_shredder);

            // Check if the boss is outside the screen in the direction of movement
            if (IsSuperShredderOutOfBoundsDuringEntry(
                SuperShredder_Information.vector[0],
                SuperShredder_Information.vector[1],
                super_shredder
            ))
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
        if (boss_moving == 0 && boss_entry_stage == num_stages) { boss_entry_stage = num_stages + 1; }

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

        sprintf(temp, "boss pos = %.1f, %.1f", super_shredder->x, super_shredder->y);
        UIWriteTextAtOffset(temp, 6, 1);

        sprintf(temp, "boss vec = %.1f, %.1f",
                SuperShredder_Information.vector[0],
                SuperShredder_Information.vector[1]);
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

static const char *GetSuperShredderStateString()
{
    switch (SuperShredder_Information.State)
    {
    case SuperShredderState_AttackPreamble:
        return "Attack Preamble";
    case SuperShredderState_ThinkAboutRushAttack:
        return "Think About Rush Attack";
    case SuperShredderState_AttackingAkaThinkAboutNoThink:
        return "Attacking";
    default:
        return "Unknown/Unimplemented";
    }
}

int SuperShredder_RunGameLoop(
    Entity *player,
    Entity enemy_array[],
    const int enemy_array_len,
    Bullet bullet_array[],
    const int bullet_array_len,
    int *frame_number,
    int playable_area[4],
    int hitbox_array[][4],
    const int hitbox_array_len
)
{
    // The blade of death
    Entity *super_shredder = &enemy_array[0];

    // Some data
    int player_centre[2];
    int my_centre[2];

    // Exit delays
    int player_death_exit_delay = EXIT_DELAY;
    int win_condition_exit_delay = EXIT_DELAY;

    // True if the player wins
    int win_condition = 0;

    // Set to false when we need to exit the loop
    int exit_condition = 1;
    int return_value = 0;

    do
    {
        // Goodbye text
        consoleClear();
        // Keys
        scanKeys();
        const u32 keys = keysHeld();
        // Counting
        (*frame_number)++;

        // Player movement
        PlayerMovement(player, keys, hitbox_array, hitbox_array_len);

        // Player bullets
        PlayerFireBullet(
            player,
            keys,
            bullet_array, bullet_array_len,
            hitbox_array, hitbox_array_len
        );

        // Player centre for thinking
        EntityGetCenterArray(player, player_centre);
        EntityGetCenterArray(super_shredder, my_centre);

        // ToDo: Super Shredder thinking
        SuperShredder_Think(super_shredder, player, bullet_array);

        // ToDo: Super Shredder moving
        SuperShredder_Move(super_shredder);

        // ToDo: Super Shredder bullets
        SuperShredder_FireBullets(
            super_shredder,
            bullet_array, MAX_BULLET_COUNT,
            my_centre,
            player_centre
        );


        // Handling bullets
        BulletHandleBulletArray(
            bullet_array, bullet_array_len,
            playable_area
        );

        // Handling bullet collisions
        BulletArrayCollisionWithPlayerAndEnemies(
            bullet_array, bullet_array_len,
            enemy_array, enemy_array_len,
            player
        );

        // Collision between player and enemies
        int super_shredder_hitbox[4];
        EntityGetHitBox(super_shredder, super_shredder_hitbox);
        super_shredder_hitbox[0] += 4;
        super_shredder_hitbox[1] += 4;
        super_shredder_hitbox[2] -= 8;
        super_shredder_hitbox[3] -= 8;
        int player_hitbox[4];
        PlayerGetHitBox(player, player_hitbox);
        if (RectangleCollision(super_shredder_hitbox, player_hitbox))
        {
            EntityTakeDamage(player, 1);
        }
        // ToDo: uncomment if we end up having mini shredders
        // EnemiesCheckCollisionAgainstPlayer(
        //     enemy_array + 1, enemy_array_len - 1,
        //     player
        // );

        //
        // Drawing and animation
        //

        PlayerAnimate(
            player,
            0,
            *frame_number,
            GFXAllSpriteGFX.PlayerGFXMem,
            GFXAllSpriteGFX.PlayerExplosionGFXMem
        );

        SuperShredder_Animate(
            super_shredder,
            0,
            *frame_number
        );

        BulletDrawArray(
            bullet_array, bullet_array_len,
            GFXAllSpriteGFX.BulletGFXMem,
            GFXAllSpriteGFX.WildBulletGFXMem
        );

        //
        // Checking for the end condition
        //

        // Player death
        if (player->dead)
        {
            player_death_exit_delay--;
            if (!player_death_exit_delay)
            {
                return_value = 0;
                exit_condition = 0;
            }
        }
        // Check for win condition if not achieved
        else if (!win_condition)
        {
            // Assume won and check if not
            win_condition = 1;

            // Check if the Super Shredder and other enemies are dead
            // ToDo: simplify this if we do not have other enemies during the fight
            for (int i = 0; i < enemy_array_len; i++)
            {
                if (enemy_array[i].health > 0)
                {
                    // If not, then we have not won
                    win_condition = 0;
                    break;
                }
            }
            // If the boss is dead, make sure no enemy bullets are 'alive'
            if (win_condition)
            {
                for (int i = 0; i < bullet_array_len; i++)
                {
                    if (bullet_array[i].type > 0 // If not a wild bullet
                        && bullet_array[i].type != BulletType_PlayerBullet
                        && bullet_array[i].alive)
                    {
                        win_condition = 0;
                        break;
                    }
                }
            }
        }

        // We have reached the win condition, perform count down
        if (win_condition)
        {
            win_condition_exit_delay--;
            if (!win_condition_exit_delay)
            {
                return_value = 1;
                exit_condition = 0;
            }
        }

        //
        // Showing other information to the player
        //

        UIResetDisplayBuffer();

        char temp[UI_NUM_CHARS + 1];

        sprintf(temp, "position = %.1f, %.1f", super_shredder->x, super_shredder->y);
        UIWriteTextAtOffset(temp, 1, 1);

        sprintf(temp, "bullet delay = %d", super_shredder->current_bullet_delay);
        UIWriteTextAtOffset(temp, 2, 1);

        sprintf(temp, "move vector = %.1f, %.1f",
                SuperShredder_Information.vector[0],
                SuperShredder_Information.vector[1]);
        UIWriteTextAtOffset(temp, 3, 1);

        sprintf(temp, "boarder hit loc = %.1f, %.1f",
                SuperShredder_Information.screen_boarder_hit_location[0],
                SuperShredder_Information.screen_boarder_hit_location[1]);
        UIWriteTextAtOffset(temp, 4, 1);

        sprintf(temp, "step vector = %.1f, %.1f",
                SuperShredder_Information.rush_telegraph_vector_step[0],
                SuperShredder_Information.rush_telegraph_vector_step[1]);
        UIWriteTextAtOffset(temp, 5, 1);

        sprintf(temp, "bullet count = %d", BulletGetNumberAliveBulletsInBulletArray(bullet_array, MAX_BULLET_COUNT));
        UIWriteTextAtOffset(temp, 6, 1);

        sprintf(temp, "health = %d", super_shredder->health);
        UIWriteTextAtOffset(temp, 22, 1);

        UIWriteTextAtOffset(GetSuperShredderStateString(), 23, 1);

        UIPrintDisplayBuffer();

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
        // Pause
        if (keys & KEY_SELECT)
        {
            // No need to set exit condition and return value here
            // No difference as it is in basically the same position
            return -1;
        }
    }
    while (exit_condition);

    return return_value;
}
