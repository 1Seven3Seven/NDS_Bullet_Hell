#include "GFX.h"

#include <nds.h>
#include <nds/arm9/sprite.h>

#include "SpriteSheet.h"

#include "Constants.h"

GFXSpritesStruct GFXAllSpriteGFX;

void GFXInit() {
    GFXAllSpriteGFX.PlayerGFXMemLoaded = 0;
    GFXAllSpriteGFX.SentinelGFXMemLoaded = 0;
    GFXAllSpriteGFX.ShredderGFXMemLoaded = 0;
    GFXAllSpriteGFX.MinerGFXMemLoaded = 0;
    GFXAllSpriteGFX.SSBodyGFXMemLoaded = 0;
    GFXAllSpriteGFX.SSLaserWeaponGFXMemLoaded = 0;

    GFXAllSpriteGFX.PlayerExplosionGFXMemLoaded = 0;
    GFXAllSpriteGFX.EnemyExplosionGFXMemLoaded = 0;

    for (int i = 0; i < NUMBER_OF_BULLETS; ++i) {
        GFXAllSpriteGFX.BulletGFXMemLoaded[i] = 0;
    }
    for (int i = 0; i < NUMBER_OF_WILD_BULLETS; ++i) {
        GFXAllSpriteGFX.WildBulletGFXMemLoaded[i] = 0;
    }

    GFXAllSpriteGFX.PortalGFXMemLoaded = 0;
}

void GFXSetSpritePaletteDefault() {
    dmaCopy(SpriteSheetPal, SPRITE_PALETTE, 512);
}

void GFXLoadPlayerSprites() {
    if (GFXAllSpriteGFX.PlayerGFXMemLoaded)
        return;

    for (int a = 0; a < 8; a++) {
        GFXAllSpriteGFX.PlayerGFXMem[a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
        );
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * a,
                GFXAllSpriteGFX.PlayerGFXMem[a],
                16 * 16
        );
    }

    GFXAllSpriteGFX.PlayerGFXMemLoaded = 1;
}
void GFXFreePlayerSprites() {
    if (!GFXAllSpriteGFX.PlayerGFXMemLoaded)
        return;

    for (int a = 0; a < 8; ++a) {
        oamFreeGfx(
                &oamMain,
                GFXAllSpriteGFX.PlayerGFXMem[a]
        );
    }

    GFXAllSpriteGFX.PlayerGFXMemLoaded = 0;
}

void GFXLoadSentinelSprites() {
    if (GFXAllSpriteGFX.SentinelGFXMemLoaded)
        return;

    for (int a = 0; a < 2; a++) {
        for (int b = 0; b < 8; b++) {
            GFXAllSpriteGFX.SentinelGFXMem[a][b] = oamAllocateGfx(
                    &oamMain,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color
            );
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * (a + 1) + TILE_SIZE * b,
                    GFXAllSpriteGFX.SentinelGFXMem[a][b],
                    16 * 16
            );
        }
    }

    GFXAllSpriteGFX.SentinelGFXMemLoaded = 1;
}

void GFXLoadShredderSprites() {
    if (GFXAllSpriteGFX.ShredderGFXMemLoaded)
        return;

    for (int a = 0; a < 4; a++) {
        GFXAllSpriteGFX.ShredderGFXMem[a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
        );
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 3 + TILE_SIZE * a,
                GFXAllSpriteGFX.ShredderGFXMem[a],
                16 * 16
        );
    }

    GFXAllSpriteGFX.ShredderGFXMemLoaded = 1;
}

void GFXLoadMinerSprites() {
    if (GFXAllSpriteGFX.MinerGFXMemLoaded)
        return;

    for (int a = 0; a < 8; a++) {
        GFXAllSpriteGFX.MinerGFXMem[a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
        );
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 4 + TILE_SIZE * a,
                GFXAllSpriteGFX.MinerGFXMem[a],
                16 * 16
        );
    }

    GFXAllSpriteGFX.MinerGFXMemLoaded = 1;
}

void GFXLoadSuperSentinelSprites() {
    if (!GFXAllSpriteGFX.SSBodyGFXMemLoaded) { // Super sentinel body
        for (int a = 0; a < 4; ++a) {
            for (int b = 0; b < 8; ++b) {
                GFXAllSpriteGFX.SSBodyGFXMem[a][b] = oamAllocateGfx(
                        &oamMain,
                        SpriteSize_16x16,
                        SpriteColorFormat_256Color
                );
                dmaCopy(
                        (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * (5 + a) + TILE_SIZE * 8 +
                        TILE_SIZE * b,
                        GFXAllSpriteGFX.SSBodyGFXMem[a][b],
                        16 * 16
                );
            }
        }

        GFXAllSpriteGFX.SSBodyGFXMemLoaded = 1;
    }

    if (!GFXAllSpriteGFX.SSLaserWeaponGFXMemLoaded) { // Super sentinel laser weapons
        for (int a = 0; a < 4; a++) {
            GFXAllSpriteGFX.SSLaserWeaponGFXMem[a] = oamAllocateGfx(
                    &oamMain,
                    SpriteSize_16x32,
                    SpriteColorFormat_256Color
            );
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 4 + TILE_SIZE * 8 + TILE_SIZE * a,
                    GFXAllSpriteGFX.SSLaserWeaponGFXMem[a],
                    16 * 16
            );
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 4 + TILE_SIZE * 8 + TILE_SIZE * (a + 4),
                    GFXAllSpriteGFX.SSLaserWeaponGFXMem[a] + 16 * 8,
                    16 * 16
            );
        }

        GFXAllSpriteGFX.SSLaserWeaponGFXMemLoaded = 1;
    }
}

void GFXLoadPlayerExplosion() {
    if (GFXAllSpriteGFX.PlayerExplosionGFXMemLoaded)
        return;

    for (int a = 0; a < 8; a++) {
        GFXAllSpriteGFX.PlayerExplosionGFXMem[a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
        );
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * 8 + TILE_SIZE * a,
                GFXAllSpriteGFX.PlayerExplosionGFXMem[a],
                16 * 16
        );
    }

    GFXAllSpriteGFX.PlayerExplosionGFXMemLoaded = 1;
}

void GFXLoadEnemyExplosion() {
    if (GFXAllSpriteGFX.EnemyExplosionGFXMemLoaded)
        return;

    for (int a = 0; a < 8; a++) {
        GFXAllSpriteGFX.EnemyExplosionGFXMem[a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
        );
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH + TILE_SIZE * 8 + TILE_SIZE * a,
                GFXAllSpriteGFX.EnemyExplosionGFXMem[a],
                16 * 16
        );
    }

    GFXAllSpriteGFX.EnemyExplosionGFXMemLoaded = 1;
}

void GFXLoadSSExplosion()
{
    if (GFXAllSpriteGFX.SSExplosionGFXMemLoaded)
        return;

    for (int a = 0; a < 8; a++) {
        GFXAllSpriteGFX.SSExplosionGFXMem[a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
        );
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 9 + TILE_SIZE * 8 + TILE_SIZE * a,
                GFXAllSpriteGFX.SSExplosionGFXMem[a],
                16 * 16
        );
    }

    GFXAllSpriteGFX.SSExplosionGFXMemLoaded = 1;
}

void GFXLoadPlayerBullets() {
    if (GFXAllSpriteGFX.BulletGFXMemLoaded[PLAYER_BULLET])
        return;

    for (int a = 0; a < 4; a++) {
        GFXAllSpriteGFX.BulletGFXMem[PLAYER_BULLET][a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
        );
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 7 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[PLAYER_BULLET][a],
                16 * 16
        );
    }

    GFXAllSpriteGFX.BulletGFXMemLoaded[PLAYER_BULLET] = 1;
}

void GFXLoadSentinelBullets() {
    if (GFXAllSpriteGFX.BulletGFXMemLoaded[SENTINEL_BULLET])
        return;

    for (int a = 0; a < 4; a++) {
        GFXAllSpriteGFX.BulletGFXMem[SENTINEL_BULLET][a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
        );
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 6 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[SENTINEL_BULLET][a],
                16 * 16
        );
    }

    GFXAllSpriteGFX.BulletGFXMemLoaded[SENTINEL_BULLET] = 1;
}

void GFXLoadMinerBullets() {
    // Miner mines
    if (!GFXAllSpriteGFX.BulletGFXMemLoaded[MINER_MINE]) {
        for (int a = 0; a < 4; a++) {
            GFXAllSpriteGFX.BulletGFXMem[MINER_MINE][a] = oamAllocateGfx(
                    &oamMain,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color
            );
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 5 + TILE_SIZE * a,
                    GFXAllSpriteGFX.BulletGFXMem[MINER_MINE][a],
                    16 * 16
            );
        }

        GFXAllSpriteGFX.BulletGFXMemLoaded[MINER_MINE] = 1;
    }

    // Smol mine explosion bullets
    if (!GFXAllSpriteGFX.BulletGFXMemLoaded[MINER_MINE_BULLET]) {
        for (int a = 0; a < 4; a++) {
            GFXAllSpriteGFX.BulletGFXMem[MINER_MINE_BULLET][a] = oamAllocateGfx(
                    &oamMain,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color
            );
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 3 + TILE_SIZE * 4 + TILE_SIZE * a,
                    GFXAllSpriteGFX.BulletGFXMem[MINER_MINE_BULLET][a],
                    16 * 16
            );
        }

        GFXAllSpriteGFX.BulletGFXMemLoaded[MINER_MINE_BULLET] = 1;
    }
}

void GFXLoadDeathBullets() {
    if (GFXAllSpriteGFX.BulletGFXMemLoaded[DEATH_BULLET])
        return;

    for (int a = 0; a < 4; a++) {
        GFXAllSpriteGFX.BulletGFXMem[DEATH_BULLET][a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
        );
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 3 + TILE_SIZE * 4 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[DEATH_BULLET][a],
                16 * 16
        );
    }

    GFXAllSpriteGFX.BulletGFXMemLoaded[DEATH_BULLET] = 1;
}

void __GFXLoadSuperSentinelNormalBullets() {
    // Super sentinel bullets
    if (!GFXAllSpriteGFX.BulletGFXMemLoaded[SS_BULLET]) {
        for (int a = 0; a < 4; a++) {
            GFXAllSpriteGFX.BulletGFXMem[SS_BULLET][a] = oamAllocateGfx(
                    &oamMain,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color
            );
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 5 + TILE_SIZE * 4 + TILE_SIZE * a,
                    GFXAllSpriteGFX.BulletGFXMem[SS_BULLET][a],
                    16 * 16
            );
        }

        GFXAllSpriteGFX.BulletGFXMemLoaded[SS_BULLET] = 1;
    }

    // Super sentinel laser start
    if (!GFXAllSpriteGFX.BulletGFXMemLoaded[SS_LASER_START_BULLET]) {
        for (int a = 0; a < 4; a++) {
            GFXAllSpriteGFX.BulletGFXMem[SS_LASER_START_BULLET][a] = oamAllocateGfx(
                    &oamMain,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color
            );
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 8 + TILE_SIZE * a,
                    GFXAllSpriteGFX.BulletGFXMem[SS_LASER_START_BULLET][a],
                    16 * 16
            );
        }

        GFXAllSpriteGFX.BulletGFXMemLoaded[SS_LASER_START_BULLET] = 1;
    }

    // Super sentinel laser segments
    if (!GFXAllSpriteGFX.BulletGFXMemLoaded[SS_LASER_SEGMENT_BULLET]) {
        for (int a = 0; a < 4; a++) {
            GFXAllSpriteGFX.BulletGFXMem[SS_LASER_SEGMENT_BULLET][a] = oamAllocateGfx(
                    &oamMain,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color
            );
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 9 + TILE_SIZE * a,
                    GFXAllSpriteGFX.BulletGFXMem[SS_LASER_SEGMENT_BULLET][a],
                    16 * 16
            );
        }

        GFXAllSpriteGFX.BulletGFXMemLoaded[SS_LASER_SEGMENT_BULLET] = 1;
    }
}

void __GFXLoadSuperSentinelWildBullets() {
    // Super sentinel pilot laser start
    if (!GFXAllSpriteGFX.WildBulletGFXMemLoaded[IndexForBulletType(SS_LASER_PILOT_START)]) {
        for (int a = 0; a < 4; ++a) {
            GFXAllSpriteGFX.WildBulletGFXMem[IndexForBulletType(SS_LASER_PILOT_START)][a] = oamAllocateGfx(
                    &oamMain,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color
            );
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 8 + TILE_SIZE * 4 + TILE_SIZE * a,
                    GFXAllSpriteGFX.WildBulletGFXMem[IndexForBulletType(SS_LASER_PILOT_START)][a],
                    16 * 16
            );
        }
    }

    // Super sentinel pilot laser segment
    if (!GFXAllSpriteGFX.WildBulletGFXMemLoaded[IndexForBulletType(SS_LASER_PILOT_SEGMENT)]) {
        for (int a = 0; a < 4; ++a) {
            GFXAllSpriteGFX.WildBulletGFXMem[IndexForBulletType(SS_LASER_PILOT_SEGMENT)][a] = oamAllocateGfx(
                    &oamMain,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color
            );
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 9 + TILE_SIZE * 4 + TILE_SIZE * a,
                    GFXAllSpriteGFX.WildBulletGFXMem[IndexForBulletType(SS_LASER_PILOT_SEGMENT)][a],
                    16 * 16
            );
        }
    }
}

void GFXLoadSuperSentinelBullets() {
    __GFXLoadSuperSentinelNormalBullets();
    __GFXLoadSuperSentinelWildBullets();
}

void GFXLoadPortalSprites() {
    if (GFXAllSpriteGFX.PortalGFXMemLoaded)
        return;

    for (int a = 0; a < 2; a++) {
        for (int b = 0; b < 8; b++) {
            GFXAllSpriteGFX.PortalGFXMem[a * 8 + b] = oamAllocateGfx(
                    &oamMain,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color
            );
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * (2 + a) + TILE_SIZE * 8 + TILE_SIZE * b,
                    GFXAllSpriteGFX.PortalGFXMem[a * 8 + b],
                    16 * 16
            );
        }
    }

    GFXAllSpriteGFX.PortalGFXMemLoaded = 1;
}

void GFXLoadAllSprites() {
    // Setting the sprite palette
    GFXSetSpritePaletteDefault();

    // region - Entity Sprites

    GFXLoadPlayerSprites();
    GFXLoadSentinelSprites();
    GFXLoadShredderSprites();
    GFXLoadMinerSprites();

    GFXLoadSuperSentinelSprites();

    // endregion

    // region - Explosions

    GFXLoadPlayerExplosion();
    GFXLoadEnemyExplosion();
    GFXLoadSSExplosion();

    // endregion

    // region - Bullets

    GFXLoadPlayerBullets();
    GFXLoadSentinelBullets();
    GFXLoadMinerBullets();

    GFXLoadDeathBullets();

    GFXLoadSuperSentinelBullets();

    // endregion

    // region - Miscellaneous

    GFXLoadPortalSprites();

    // BANG -> !
    for (int a = 0; a < 4; ++a) {
        GFXAllSpriteGFX.BangGFXMem[a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
        );
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 6 + TILE_SIZE * 4 + TILE_SIZE * a,
                GFXAllSpriteGFX.BangGFXMem[a],
                16 * 16
        );
    }

    // endregion
}