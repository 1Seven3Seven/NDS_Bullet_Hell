#ifndef BULLET_HELL_SUPER_SENTINEL_H
#define BULLET_HELL_SUPER_SENTINEL_H

#include "GFX.h"

#include "Bullet.h"

#include "Entity.h"

/*! Super sentinel has 3 attacks:
 * \n Strafing; moving from side to side firing bullets.
 * \n Strafing with lasers.
 * \n Pincer, lasers move to the side of the screen, bullets being fired at the player whilst the lasers move inward.
 */

// Contains the indexes of the laser to be handled
typedef struct {
    int Initialised;
    int StartIndex;
    int NumSegmentIndexes;
    int *SegmentIndexes;
    int Pilot;
}SSLaserStruct;

// MUST be run before using a laser struct
// Gives some variables default values
void LaserPreInitLaserStruct(SSLaserStruct *laser);

// Initialises the laser to be at the given position
// pilot determines if the laser is a pilot laser and does no damage
void LaserInitLaserStruct(SSLaserStruct *laser, Bullet bullet_array[], int bullet_array_len, int x, int y, int pilot);

// Un-Initialises the given laser
// Deallocates the allocated memory
void LaserDeInitLaserStruct(SSLaserStruct *laser, Bullet bullet_array[]);

// Must be called every frame before bullets are handled to keep the laser alive
void LaserUpdate(SSLaserStruct *laser, Bullet bullet_array[], int new_x, int new_y);

// Please don't use this
// There is already floating around called SSSuperSentinelInformationStruct
typedef struct {
    // The amount of bullets fired in the current salvo
    int BulletsFired;

    // If the super sentinel should move
    int Moving;

    // 0 = left
    // 1 = right
    int MoveDirection;

    // If the super sentinel hit the boarder
    int HitBoarder;

    // If the laser is to be fired
    int FireLaser;

    // The delay to fire the laser
    int FireLaserDelay;

    // The laser to fire from, duh
    //      0 = both
    //      1 = left
    //      2 = right
    int LaserToFireFrom;

    // Whether to move the lasers independently of the main body
    //      -1 = lasers have moved and no more movement is necessary
    //      0  = no independent movement
    //      1  = move lasers to edges
    //      2  = move lasers back to main body
    int MoveLasers;

    //
    SSLaserStruct Lasers[2];

}SSSuperSentinelInformationStruct;

// Contains information about the super sentinel, not necessary to touch
// Initialised when SSSetup is called
extern SSSuperSentinelInformationStruct SSSuperSentinelInformation;

// Sets up the super sentinel and the SSSuperSentinelInformation struct
// Initialises the first 3 entities in the entity array
//      Index 0 is the main body
//      Index 1 is the left laser
//      Index 2 is the right laser
// Assumes the enemy array is at least size of 3
void SSSetup(Entity enemy_array[], int enemy_array_len);

// Sets the position of the top left of the main body and the rest of the parts accordingly
void SSSetPosition(Entity enemy_array[], float x, float y);

// Gets the health of the boss
// Health is split between 3 parts so use this to for easier retrieval
int SSGetHealth(Entity enemy_array[]);

// Moves both the super sentinel main body and laser sections when necessary
void SSMove(Entity enemy_array[], int hitbox_array[][4], int hitbox_array_len);

// Animates the super sentinel
void SSAnimate(Entity enemy_array[], int priority, int frame_number, u16 *body_gfx_mem[4][8],
               u16 *laser_weapon_gfx_mem[4], u16 *bang_gfx_mem[4]);

// Fires both the salvo and lasers when necessary
void SSFireBullets(Entity enemy_array[], int player_center[2], Bullet bullet_array[], int bullet_array_len);

// Changes values in SSSuperSentinelInformation to change the behaviour of the super sentinel
void SSThink(Entity enemy_array[]);

// Sets up the boss fight
// Initialises the player, bullet array and enemy array
// This is its own game loop that lasts for 112 frames
// Purely aesthetic
// SSRunGameLoop should be run after
void SSSetupForGameLoop(Entity *player, Entity enemy_array[], int enemy_array_len, Bullet bullet_array[],
                        int bullet_array_len, int *frame_number, GFXSpritesStruct *all_sprite_gfx, int bg_id);

// Runs the game loop till either
//      The boss is dead and there are no enemy bullets
//          Returns 1
//      The player dies
//          Returns 0
//      Select is pressed
//          Returns -1
//          Considered pause, re-run SSRunGameLoop to resume
// Assumes SSSetupForGameLoop has already been run
int SSRunGameLoop(Entity *player, Entity enemy_array[], int enemy_array_len, Bullet bullet_array[],
                  int bullet_array_len, int *frame_number, GFXSpritesStruct *all_sprite_gfx, int playable_area[4],
                  int hitbox_array[][4], int hitbox_array_len);

// Runs the end cinematic thingy
//      The super sentinel explodes and falls apart
// Should only be run after SSRunGameLoop returns 1
void SSRunEndLoop(Entity *player, Entity enemy_array[], int enemy_array_len, Bullet bullet_array[],
                  int bullet_array_len, int *frame_number, GFXSpritesStruct *all_sprite_gfx);

#endif // BULLET_HELL_SUPER_SENTINEL_H