#ifndef BULLET_HELL_BULLET_H
#define BULLET_HELL_BULLET_H

#include <nds.h>

#include "Constants.h"
#include "Entity.h"

/// Bullet types - id numbers for the bullets.
///      Only the player bullet will collide with enemies.
///      Everything above 0 is an enemy bullet and should only collide with the player.
///      Everything below 0 is a wild bullet and doesn't have any collision.
///
/// The specific values themselves are used to index into the bullet GFX memory, and so should not be changed
///  willy-nilly.
typedef enum _BulletType_e
{
    /// If the bullet hasn't been created yet.
    BulletType_UndefinedBullet = -999,

    /// Bullet fired by the player.
    BulletType_PlayerBullet = 0,

    /// Bullet fired by the sentinel.
    BulletType_SentinelBullet = 1,
    /// Bullet/Mine dropped by the miner.
    BulletType_MinerMine = 2,
    /// Bullets released when a mine explodes.
    BulletType_MinerMineBullet = 3,
    /// Bullets fired upon enemy death.
    BulletType_DeathBullet = 4,
    /// The main bullet fired.
    BulletType_BossBullet = 5,
    /// The start of the laser, animated differently.
    BulletType_SSLaserStart = 6,
    /// The segments of the lasers.
    BulletType_SSLaserSegment = 7,

    /// The start of the pilot laser.
    BulletType_SSLaserPilotStart = -1,
    /// The segments of the pilot laser.
    BulletType_SSLaserPilotSegment = -2,
    /// The telegraphing 'bullet' for the Super Shredder
    BulletType_SuperShredderTelegraph = -3,
} BulletType;

/// Macro to return the index for the given wild bullet id.
/// Also works for normal bullets, but no need to use it with those.
///
/// - -1 -> 0
/// - -2 -> 1
/// - -3 -> 2
/// - ...
#define IndexForBulletType(id) (id < 0 ? -1 - id : id)

/// Contains information regarding a bullet.
typedef struct _Bullet_s
{
    /// Position.
    float x, y;

    /// Size.
    int w, h;

    /// Angle to travel along.
    float angle;
    /// Velocity for movement.
    float velocity;
    /// Vector to travel along, made from angle and velocity.
    float vector[2];

    /// Lifespan in frames.
    int lifespan;
    /// Damage to be dealt upon collision.
    int damage;

    /// If the bullet should stop being handled.
    /// Mainly to check for the condition when a bullet expires.
    int to_die;

    /// If the bullet should be handled.
    int alive;

    /// Type of the bullet.
    /// Indicates how it should be displayed and what entities it can hurt.
    BulletType type;
} Bullet;

//
// Setup
//

/// Changes the necessary values to make the bullet not be updated.
/// Used for bullets you have created but don't want to use immediately.
void BulletInit(Bullet *self);

/// Calls BulletInit on every bullet struct in the given array.
void BulletInitBulletArray(Bullet bullet_array[], int bullet_array_len);

/// Sets up the given bullet.
void BulletSetup(
    Bullet *self,
    float x,
    float y,
    int w,
    int h,
    float angle,
    float velocity,
    int lifespan,
    int damage,
    BulletType type);

/// Attempts to set up a bullet in the given array of bullets.
/// Returns the index of the bullet if the setup was successful.
/// Returns -1 if not successful, aka, there were no free bullets.
int BulletSetupInBulletArray(
    Bullet bullet_array[],
    int bullet_array_len,
    float x,
    float y,
    int w,
    int h,
    float angle,
    float velocity,
    int lifespan,
    int damage,
    BulletType type);

/// Does the same as `BulletSetupInBulletArray` but starts from the end of the array.
int BulletSetupInBulletArrayReversed(
    Bullet bullet_array[],
    int bullet_array_len,
    float x,
    float y,
    int w,
    int h,
    float angle,
    float velocity,
    int lifespan,
    int damage,
    BulletType type);

//
// Retrieving information
//

/// Get the rectangle of the hitbox for the bullet and place it in the given array.
void BulletGetHitBox(const Bullet *self, int rect_array[4]);

/// Get the centre of the bullet and place it in the given array.
void BulletGetCenterArray(const Bullet *self, int center_array[2]);

/// Counts the number of bullets being updated in the frame.
int BulletGetNumberAliveBulletsInBulletArray(Bullet bullet_array[], int bullet_array_len);

//
// Moving and updating
//

/// Moves the bullet by its vector.
void BulletMove(Bullet *self);

/// Moves the bullet and reduces its lifespan.
/// If lifespan is below 0 marks for deletion.
void BulletUpdate(Bullet *self);

/// Calls BulletUpdate on every bullet in the bullet array.
/// Deletes any bullets that are outside the boundary rectangle.
void BulletHandleBulletArray(Bullet bullet_array[], int bullet_array_len, int boundary_rectangle[4]);

/// Checks for any collision between the bullets in the array with both enemies and the player.
///
/// Player bullets, id 0, only collide with enemies.
/// Enemy bullets, id > 0, only collide with the players.
/// Wild bullets, id < 0, have no collision.
///
/// If a bullet collides with the player, the type of the bullet is returned, if not, then the player bullet type (0) is
///  returned.
BulletType BulletArrayCollisionWithPlayerAndEnemies(
    Bullet bullet_array[],
    int bullet_array_len,
    Entity enemy_array[],
    int enemy_array_len,
    Entity *player);

/// Spawns bullets upon the enemies deaths.
void BulletSpawnDeathBullets(
    Bullet bullet_array[],
    int bullet_array_len,
    Entity enemy_array[],
    int enemy_array_len,
    Entity *player);

//
// Drawing
//

/// Draws the given bullet to the screen.
void BulletDraw(const Bullet *self, int id_offset, u16 *bullet_gfx_mem[][FRAMES_PER_BULLET]);

/// Draws all bullets in the array to the screen.
void BulletDrawArray(
    Bullet bullet_array[],
    int bullet_array_len,
    u16 *bullet_gfx_mem[][FRAMES_PER_BULLET],
    u16 *wild_bullet_gfx_mem[][FRAMES_PER_BULLET]);

#endif
