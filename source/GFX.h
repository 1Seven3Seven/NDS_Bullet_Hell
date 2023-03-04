#ifndef BULLET_HELL_GFX_H
#define BULLET_HELL_GFX_H

#include <nds.h>

#include "Constants.h"

// Please don't use this
// There is an initialised one already floating around called GFXAllSpriteGFX
// Please call GFXLoadAllSprites before using
typedef struct {
    u16 *PlayerGFXMem[PLAYER_ANIMATION_FRAMES];
    u16 *PlayerExplosionGFXMem[PLAYER_EXPLOSION_ANIMATION_FRAMES];

    u16 *SentinelGFXMem[NUM_SENTINEL_ROTATIONS][SENTINEL_ANIMATION_FRAMES];
    u16 *ShredderGFXMem[SHREDDER_ANIMATION_FRAMES];
    u16 *MinerGFXMem[MINER_ANIMATION_FRAMES];
    u16 *EnemyExplosionGFXMem[ENEMY_EXPLOSION_ANIMATION_FRAMES];

    u16 *BulletGFXMem[NUMBER_OF_BULLETS][FRAMES_PER_BULLET];

    u16 *PortalGFXMem[PORTAL_ANIMATION_FRAMES];
} GFXSpritesStruct;

// A struct containing pointers to all the sprites
// Mainly used to just keep everything under one name
// Do not edit
extern GFXSpritesStruct GFXAllSpriteGFX;

// Loads all the sprite data into GFXAllSpriteGFX and sets the colour pallet
// Must be run before using GFXAllSpriteGFX
void GFXLoadAllSprites();

#endif // BULLET_HELL_GFX_H