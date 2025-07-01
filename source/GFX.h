#ifndef BULLET_HELL_GFX_H
#define BULLET_HELL_GFX_H

#include <nds.h>

#include "Constants.h"

/// Please don't make one of these.
/// There is an initialised one already floating around called `GFXAllSpriteGFX`.
///
/// Please call `GFXLoadAllSprites` before using.
///     Or at least use the correct load functions to make sure the sprites that will be used are loaded.
typedef struct _GFXAllSpriteGFX_s
{
    //
    // Entity Sprites
    //

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

    int SuperShredderGFXMemLoaded;
    u16 *SuperShredderGFXMem[SUPERSHREDDER_ANIMATION_FRAMES];

    //
    // Explosion Sprites
    //

    int PlayerExplosionGFXMemLoaded;
    u16 *PlayerExplosionGFXMem[PLAYER_EXPLOSION_ANIMATION_FRAMES];
    int EnemyExplosionGFXMemLoaded;
    u16 *EnemyExplosionGFXMem[ENEMY_EXPLOSION_ANIMATION_FRAMES];
    int SSExplosionGFXMemLoaded;
    u16 *SSExplosionGFXMem[SS_EXPLOSION_ANIMATION_FRAMES];

    //
    // Bullet Sprites
    //

    int BulletGFXMemLoaded[NUMBER_OF_BULLETS];
    u16 *BulletGFXMem[NUMBER_OF_BULLETS][FRAMES_PER_BULLET];
    int WildBulletGFXMemLoaded[NUMBER_OF_WILD_BULLETS];
    u16 *WildBulletGFXMem[NUMBER_OF_WILD_BULLETS][FRAMES_PER_BULLET];

    //
    // Miscellaneous Sprites
    //

    int PortalGFXMemLoaded;
    u16 *PortalGFXMem[PORTAL_ANIMATION_FRAMES];
    u16 *BangGFXMem[BANG_ANIMATION_FRAMES];
} _GFXAllSpriteGFX;

/// A struct containing pointers to all the sprites.
/// Mainly used to just keep everything under one name.
/// Do not edit directly.
extern _GFXAllSpriteGFX GFXAllSpriteGFX;

/// Initialises the main GFX struct.
void GFXInit();

//
// Palette stuff
//

///
void GFXSetSpritePaletteDefault();

//
// Entity Sprites
//

///
void GFXLoadPlayerSprites();

///
void GFXFreePlayerSprites();

///
void GFXLoadSentinelSprites();

///
void GFXLoadShredderSprites();

///
void GFXLoadMinerSprites();

///
void GFXLoadSuperSentinelSprites();

///
void GFXLoadSuperShredderSprites();

//
// EXPLOSIONS!
//

///
void GFXLoadPlayerExplosion();

///
void GFXLoadEnemyExplosion();

///
void GFXLoadSSExplosion();

//
// Bullets
//

///
void GFXLoadPlayerBullets();

///
void GFXLoadSentinelBullets();

///
void GFXLoadMinerBullets();

///
void GFXLoadDeathBullets();

///
void GFXLoadBossNormalBullets();

///
void GFXLoadSuperSentinelPilotLaserBullets();

///
void GFXLoadSuperSentinelLaserBullets();

///
void GFXLoadAllSuperSentinelBullets();

//
// Other
//

///
void GFXLoadPortalSprites();

//
// Would you like a bit of everything?
// Or just everything?
//

/// Loads all the sprite data into `GFXAllSpriteGFX` and sets the colour pallet.
/// Must be run before using `GFXAllSpriteGFX`.
void GFXLoadAllSprites();

#endif
