#ifndef BULLET_HELL_SUPER_SHREDDER_H
#define BULLET_HELL_SUPER_SHREDDER_H

#include "Bullet.h"
#include "Entity.h"

///
typedef struct _SuperShredderInformation_s
{
    /// Movement vector.
    float vx, vy;
} _SuperShredderInformation;

/// Contains information about the Super Shredder, not necessary to touch directly.
/// Initialised by <c>SuperShredder_Setup</c>.
extern _SuperShredderInformation SuperShredder_Information;

/// Sets up the enemy entity array
void SuperShredder_Setup(Entity enemy_array[], int enemy_array_len);

/// Moves the Super Shredder according to the vector stored in <c>SuperShredder_Information</c>.
void SuperShredder_Move(Entity enemy_array[]);

///
void SuperShredder_SetupForGameLoop(
    Entity *player,
    Entity enemy_array[],
    int enemy_array_len,
    Bullet bullet_array[],
    int bullet_array_len,
    int *frame_number,
    int bg_id
);

#endif
