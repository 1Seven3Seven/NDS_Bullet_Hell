#ifndef BULLET_HELL_CONSTANTS_H
#define BULLET_HELL_CONSTANTS_H

//
// Version
//

#define VERSION "Version 1.1 Dev 20"

//
// Universal constants
//

#define TILE_SIZE 256         // The number of pixels in the tiles
#define SPRITE_SHEET_WIDTH 16 // Number of tiles the sprite sheet is wide

//
// Not used, and also predefined in libnds files, somewhere
//

// #define SCREEN_WIDTH 256
// #define SCREEN_HEIGHT 192

//
// Bullet constants
//

#define MAX_BULLET_COUNT 119
#define NUMBER_OF_BULLETS 8         // The number of different types of bullets
#define NUMBER_OF_WILD_BULLETS 3    // The number of different types of wild bullets
#define FRAMES_PER_BULLET 4         // The number of frames of animation of the bullets
#define BULLET_ID_START 9           // The beginning of the oam offset of the bullets

//
// Player constants
//

#define PLAYER_START_X 122
#define PLAYER_START_Y 90
#define PLAYER_HEALTH 1
#define PLAYER_BULLET_DELAY 10
#define EXIT_DELAY 60 // Upon death or win, wait this long before exiting the game loop

//
// Enemy constructor constants
//

#define SENTINEL_HEALTH 10
#define SENTINEL_BULLET_DELAY 60

#define SHREDDER_HEALTH 3
#define SHREDDER_BULLET_DELAY 60
#define SHREDDER_SPEED 3

#define MINER_HEALTH 5
#define MINER_BULLET_DELAY 120    // Waits this long before trying to lay a mine
#define MINER_PLACE_MINE_DELAY 30 // Waits this long to place a mine

#define ENEMY_START_DELAY 59 // I would love this to be 60, but it messes with the animations of the sentinel

#define SS_HEALTH 300         // Super sentinel health
#define SS_START_X 112        // The x coord of the start position of the main body
#define SS_START_Y -32        // The y coord of the start position of the main body (starts off the screen)
#define SS_BULLET_DELAY 40    // The delay between firing salvos of bullets
#define SS_SALVO_DELAY 20     // The delay between firing bullets in a salvo
#define SS_MAX_LEFT_X 24      // The maximum left the main body can travel
#define SS_MAX_RIGHT_X 200    // The maximum right the main body can travel
#define SS_LASER_DELAY 30     // The delay to fire the lasers, to make some attack patterns more fair
#define SS_LASER_HEALTH 200   // The health to get below to activate the laser
#define SS_FINAL_HEALTH 100   // The health to get below to activate the final attack pattern
#define SS_CRITICAL_HEALTH 10 // Does not affect the fight but effects text displayed
#define SS_CENTER_X 112       // The x coord of the main body to be set when centered

#define SUPERSHREDDER_HEALTH 240                /// Not as health as the Super Sentinel.
#define SUPERSHREDDER_START_X -100              /// Initial start position off-screen.
#define SUPERSHREDDER_START_Y -100              /// Initial start position off-screen.
#define SUPERSHREDDER_BULLET_DELAY 60           /// Defines the time between attacks.
#define SUPERSHREDDER_WIDTH 32                  /// The sprite width.
#define SUPERSHREDDER_HEIGHT 32                 /// The sprite height.
#define SUPERSHREDDER_ENTRY_SPEED 6             /// The speed during the entry animation.
#define SUPERSHREDDER_SPEED 4                   /// The speed during normal gameplay.
#define SUPERSHREDDER_SECOND_STAGE_HEALTH 160   /// The health level to be below to activate the second stage.
#define SUPERSHREDDER_FINAL_STAGE_HEALTH 80     /// The health level to be below to activate the last stage.

//
// Enemy animation information
//

#define PLAYER_ANIMATION_FRAMES 8
#define PLAYER_EXPLOSION_ANIMATION_FRAMES 8
#define NUM_SENTINEL_ROTATIONS 2
#define SENTINEL_ANIMATION_FRAMES 8
#define MINER_ANIMATION_FRAMES 8
#define SHREDDER_ANIMATION_FRAMES 4
#define ENEMY_EXPLOSION_ANIMATION_FRAMES 8
#define PORTAL_ANIMATION_FRAMES 16
#define BANG_ANIMATION_FRAMES 4

#define SS_BODY_PARTS 4                     // The number of segments/parts the body of the super sentinel has
#define SS_BODY_ANIMATION_FRAMES 8
#define SS_LASER_ANIMATION_FRAMES 4
#define SS_EXPLOSION_ANIMATION_FRAMES 8

#define SUPERSHREDDER_ANIMATION_FRAMES 4
#define SUPERSHREDDER_NUM_RUSH_TELEGRAPH_POINTS 8

//
// Returned by move functions to determine what was happening when collisions were detected
//

#define COLLISION_X 1
#define COLLISION_Y 2

#endif
