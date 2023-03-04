#ifndef BULLET_HELL_BULLET_H
#define BULLET_HELL_BULLET_H

#include <nds.h>

#include "Constants.h"
#include "Entity.h"

// Contains information regarding a bullet
typedef struct {
    // Position
    float x, y;

    // Size
    int w, h;

    // Angle to travel along
    float angle;
    // Velocity for movement
    float velocity;
    // Vector to travel along, made from angle and velocity
    float vector[2];

    // Lifespan in frames
    int lifespan;
    // Damage to be dealt upon collision
    int damage;

    // If the bullet should stop being handled
    // Mainly to check for the condition when a bullet expires
    int to_die;

    // If the bullet should be handled
    int alive;

    // Type of the bullet
    // Indicates how it should be displayed and what entities it can hurt
    // -1 means undefined
    int type;

} Bullet;

/*
 *  Setup
 */

// Changes the necessary values to make the bullet not be updated
// Used for bullets you have created but don't want to use immediately
void BulletInit(Bullet *self);

// Calls BulletInit on every bullet struct in the given array
void BulletInitBulletArray(Bullet bullet_array[], int bullet_array_len);

// Sets up the given bullet
void BulletSetup(Bullet *self, float x, float y, int w, int h, float angle, float velocity, int lifespan, int damage,
                 int type);

// Attempts to set up a bullet in the given array of bullets
// Will return the index of the setup bullet if the setup was successful
// Will return -1 if not successful, there was no free bullets
int BulletSetupInBulletArray(Bullet bullet_array[], int bullet_array_len, float x, float y, int w, int h, float angle,
                             float velocity, int lifespan, int damage, int type);

/*
 *  Retrieving information
 */

// Get the rectangle of the hitbox for the bullet and place it in the given array
void BulletGetHitBox(Bullet *self, int rect_array[4]);

// Get the center of the bullet and place it in the given array
void BulletGetCenterArray(Bullet *self, int center_array[2]);

// Counts the number of bullets being updated in the frame
int BulletGetNumberAliveBulletsInBulletArray(Bullet bullet_array[], int bullet_array_len);

/*
 *  Moving and updating
 */

// Moves the bullet by its vector
void BulletMove(Bullet *self);

// Moves the bullet and reduces lifespan, if lifespan is below 0 marks for deletion
void BulletUpdate(Bullet *self);

// Calls BulletUpdate on every bullet in the bullet array
// Deletes any bullets that are outside the boundary rectangle
void BulletHandleBulletArray(Bullet bullet_array[], int bullet_array_len, int boundary_rectangle[4]);

// Checks for any collision between the bullets in the array with both enemies and the player
void BulletArrayCollisionWithPlayerAndEnemies(Bullet bullet_array[], int bullet_array_len, Entity enemy_array[],
                                              int enemy_array_len, Entity *player);

// Spawns bullets upon the enemies deaths
void BulletSpawnDeathBullets(Bullet bullet_array[], int bullet_array_len, Entity enemy_array[], int enemy_array_len,
                             Entity *player);

/*
 * Drawing
 */

// Draws the given bullet to the screen
void BulletDraw(Bullet *self, int id_offset, u16 *bullet_gfx_mem[][FRAMES_PER_BULLET]);

// Draws all bullets in the array to the screen
void BulletDrawArray(Bullet bullet_array[], int bullet_array_len, u16 *bullet_gfx_mem[][FRAMES_PER_BULLET]);

#endif // BULLET_HELL_BULLET_H