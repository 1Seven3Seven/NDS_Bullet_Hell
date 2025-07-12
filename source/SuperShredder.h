#ifndef BULLET_HELL_SUPER_SHREDDER_H
#define BULLET_HELL_SUPER_SHREDDER_H

#include "Bullet.h"
#include "Entity.h"

/// State regarding the 'thinking' of the Super Shredder.
typedef enum _SuperShredderState_e
{
    /// Choosing which attack to use.
    SuperShredderState_AttackPreamble,

    /// Setting up and telegraphing the rush attack.
    SuperShredderState_ThinkAboutRushAttack,

    /// We are performing the rush attack.
    SuperShredderState_RushAttack,

    /// Setting up the second attack.
    SuperShredderState_ThinkAboutZoomAttack,

    /// We are performing the second attack.
    /// ToDo: A better name is needed.
    SuperShredderState_ZoomAttack,
} _SuperShredderState;

/// General information about the Super Shredder.
typedef struct _SuperShredderInformation_s
{
    /// Movement vector.
    float vector[2];

    /// The location on the screen boarder where the vector initially hits.
    float screen_boarder_hit_location[2];

    /// The step between rush telegraph bullets.
    float telegraph_vector[2];

    /// Specific to the Zoom attack, which direction we are moving in.
    ///
    /// - 0 -> up/down
    /// - 1 -> left/right
    int move_direction;

    /// For the second stage, if the boss should fire bullets when hitting the wall.
    int should_fire;

    /// Specific to the Zoom attack, indicates if the wall has been hit.
    int boarder_hit;

    /// The current state of the Super Shredder.
    _SuperShredderState state;
} _SuperShredderInformation;

/// Contains information about the Super Shredder, not necessary to touch directly.
/// Initialised by <c>SuperShredder_Setup</c>.
extern _SuperShredderInformation SuperShredder_Information;

/// Sets up the enemy entity array
void SuperShredder_Setup(Entity enemy_array[], int enemy_array_len);

/// Moves the Super Shredder according to the vector stored in <c>SuperShredder_Information</c>.
void SuperShredder_Move(Entity *super_shredder);

/// Handles the 'thinking' of the Super Shredder.
/// Includes responsibilities such as
///
/// - Setting up the attack state,
/// - Telegraphing attacks,
/// - Something else if I think of it.
void SuperShredder_Think(
    Entity *super_shredder,
    const Entity *player,
    Bullet bullet_array[],
    int frame_number,
    int bg_id,
    const int screen_shake_x[9],
    const int screen_shake_y[9],
    int *screen_shake_index
);

/// Animates the Super Shredder.
void SuperShredder_Animate(Entity *super_shredder, int priority, int frame_number);

/// Handles firing bullets.
void SuperShredder_FireBullets(
    Entity *super_shredder,
    Bullet bullet_array[],
    int bullet_array_len,
    const int my_centre[2],
    const int player_centre[2]
);

/// Sets up the game loop.
/// Initialises the player, bullet array, and entity array.
/// An aesthetic game loop.
/// Run <c>SuperShredder_RunGameLoop</c> after.
///
/// <c>num_stages</c> controls the number of sides the boss should travel along before attacking the player.
/// Default is 4, minimum is 2.
void SuperShredder_SetupForGameLoop(
    Entity *player,
    Entity enemy_array[],
    int enemy_array_len,
    Bullet bullet_array[],
    int bullet_array_len,
    int *frame_number,
    int bg_id,
    int num_stages
);

/// Runs the game loop till either:
///
/// The player wins (returns 1),
/// The player dies (returns 0),
/// Select - pause - is pressed (returns -1).
int SuperShredder_RunGameLoop(
    Entity *player,
    Entity enemy_array[],
    int enemy_array_len,
    Bullet bullet_array[],
    int bullet_array_len,
    int *frame_number,
    int bg_id,
    int playable_area[4],
    int hitbox_array[][4], int hitbox_array_len
);

#endif
