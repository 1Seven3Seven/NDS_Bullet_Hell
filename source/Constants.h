#ifndef BULLET_HELL_CONSTANTS_H
#define BULLET_HELL_CONSTANTS_H

// Universal constants

#define TILE_SIZE 256         // The number of pixels in the tiles
#define SPRITE_SHEET_WIDTH 16 // Number of tiles the sprite sheet is wide

// Bullet constants

#define MAX_BULLET_COUNT 119 //
#define NUMBER_OF_BULLETS 4  // The number of different types of bullets
#define FRAMES_PER_BULLET 4  // The number of frames of animation of the bullets
#define BULLET_ID_START 9    // The beginning of the oam offset of the bullets

// Type constants

#define PLAYER_TYPE 0   // The type of the player used in the player entity
#define SENTINEL_TYPE 1 // The type of the sentinel used in the player entity
#define SHREDDER_TYPE 2 // The type of the shredder used in the player entity
#define MINER_TYPE 3    // The type of the miner used in the player entity

// Bullet types - id numbers for the bullets

#define PLAYER_BULLET 0
#define SENTINEL_BULLET 1
#define MINER_MINE 2
#define MINER_MINE_BULLET 3

// Player constants

#define PLAYER_START_X 122
#define PLAYER_START_Y 90
#define PLAYER_HEALTH 1
#define PLAYER_BULLET_DELAY 10
#define EXIT_DELAY 60 // Upon death or win, wait this long before exiting the game loop

// Enemy constructor constants

#define SENTINEL_HEALTH 10
#define SENTINEL_BULLET_DELAY 60

#define SHREDDER_HEALTH 3
#define SHREDDER_BULLET_DELAY 60
#define SHREDDER_SPEED 3

#define MINER_HEALTH 5
#define MINER_BULLET_DELAY 120    // Waits this long before trying to lay a mine
#define MINER_PLACE_MINE_DELAY 30 // Waits this long to place a mine

#define ENEMY_START_DELAY 59 // I would love this to be 60, but it messes with the animations of the sentinel

// Enemy animation information

#define PLAYER_ANIMATION_FRAMES 8
#define PLAYER_EXPLOSION_ANIMATION_FRAMES 8
#define NUM_SENTINEL_ROTATIONS 2
#define SENTINEL_ANIMATION_FRAMES 8
#define MINER_ANIMATION_FRAMES 8
#define SHREDDER_ANIMATION_FRAMES 4
#define ENEMY_EXPLOSION_ANIMATION_FRAMES 8
#define PORTAL_ANIMATION_FRAMES 16

// Returned by move functions to determine what was happening when collisions were detected

#define COLLISION_X 1
#define COLLISION_Y 2

#endif //BULLET_HELL_CONSTANTS_H
