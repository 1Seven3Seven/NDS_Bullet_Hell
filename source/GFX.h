#ifndef BULLET_HELL_GFX_H
#define BULLET_HELL_GFX_H

#include <nds.h>

#include "Constants.h"

// Please don't use this
// There is an initialised one already floating around called GFXAllSpriteGFX
// Please call GFXLoadAllSprites before using
//      Or at least use the correct load functions to make sure the sprites that will be used are loaded
typedef struct {
    // Entity Sprites

    int PlayerGFXMemLoaded;
    u16 *PlayerGFXMem[PLAYER_ANIMATION_FRAMES];
    int SentinelGFXMemLoaded;
    u16 *SentinelGFXMem[NUM_SENTINEL_ROTATIONS][SENTINEL_ANIMATION_FRAMES];
    int ShredderGFXMemLoaded;
    u16 *ShredderGFXMem[SHREDDER_ANIMATION_FRAMES];
    int MinerGFXMemLoaded;
    u16 *MinerGFXMem[MINER_ANIMATION_FRAMES];

    int SSBodyGFXMemLoaded;
    u16 *SSBodyGFXMem[SS_BODY_PARTS][SS_BODY_ANIMATION_FRAMES];
    int SSLaserWeaponGFXMemLoaded;
    u16 *SSLaserWeaponGFXMem[SS_LASER_ANIMATION_FRAMES];

    // Explosion Sprites

    int PlayerExplosionGFXMemLoaded;
    u16 *PlayerExplosionGFXMem[PLAYER_EXPLOSION_ANIMATION_FRAMES];
    int EnemyExplosionGFXMemLoaded;
    u16 *EnemyExplosionGFXMem[ENEMY_EXPLOSION_ANIMATION_FRAMES];
    int SSExplosionGFXMemLoaded;
    u16 *SSExplosionGFXMem[SS_EXPLOSION_ANIMATION_FRAMES];

    // Bullet Sprites

    int BulletGFXMemLoaded[NUMBER_OF_BULLETS];
    u16 *BulletGFXMem[NUMBER_OF_BULLETS][FRAMES_PER_BULLET];
    int WildBulletGFXMemLoaded[NUMBER_OF_WILD_BULLETS];
    u16 *WildBulletGFXMem[NUMBER_OF_WILD_BULLETS][FRAMES_PER_BULLET];

    // Miscellaneous Sprites

    int PortalGFXMemLoaded;
    u16 *PortalGFXMem[PORTAL_ANIMATION_FRAMES];
    u16 *BangGFXMem[BANG_ANIMATION_FRAMES];
} GFXSpritesStruct;

// A struct containing pointers to all the sprites
// Mainly used to just keep everything under one name
// Do not edit
extern GFXSpritesStruct GFXAllSpriteGFX;

// Initialises the main GFX struct
void GFXInit();

//
void GFXSetSpritePaletteDefault();

//
void GFXLoadPlayerSprites();
void GFXFreePlayerSprites();

//
void GFXLoadSentinelSprites();

//
void GFXLoadShredderSprites();

//
void GFXLoadMinerSprites();

//
void GFXLoadSuperSentinelSprites();

//
void GFXLoadPlayerExplosion();

//
void GFXLoadEnemyExplosion();

//
void GFXLoadSSExplosion();

//
void GFXLoadPlayerBullets();

//
void GFXLoadSentinelBullets();

//
void GFXLoadMinerBullets();

//
void GFXLoadDeathBullets();

//
void __GFXLoadSuperSentinelNormalBullets();

//
void __GFXLoadSuperSentinelWildBullets();

//
void GFXLoadSuperSentinelBullets();

//
void GFXLoadPortalSprites();

// Loads all the sprite data into GFXAllSpriteGFX and sets the colour pallet
// Must be run before using GFXAllSpriteGFX
void GFXLoadAllSprites();

#endif // BULLET_HELL_GFX_H