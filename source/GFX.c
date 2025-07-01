#include "GFX.h"

#include <nds.h>

#include "Bullet.h"
#include "SpriteSheet.h"
#include "Constants.h"

_GFXAllSpriteGFX GFXAllSpriteGFX;

void GFXInit()
{
    GFXAllSpriteGFX.PlayerGFXMemLoaded = 0;
    GFXAllSpriteGFX.SentinelGFXMemLoaded = 0;
    GFXAllSpriteGFX.ShredderGFXMemLoaded = 0;
    GFXAllSpriteGFX.MinerGFXMemLoaded = 0;
    GFXAllSpriteGFX.SSBodyGFXMemLoaded = 0;
    GFXAllSpriteGFX.SSLaserWeaponGFXMemLoaded = 0;

    GFXAllSpriteGFX.PlayerExplosionGFXMemLoaded = 0;
    GFXAllSpriteGFX.EnemyExplosionGFXMemLoaded = 0;

    for (int i = 0; i < NUMBER_OF_BULLETS; ++i)
    {
        GFXAllSpriteGFX.BulletGFXMemLoaded[i] = 0;
    }
    for (int i = 0; i < NUMBER_OF_WILD_BULLETS; ++i)
    {
        GFXAllSpriteGFX.WildBulletGFXMemLoaded[i] = 0;
    }

    GFXAllSpriteGFX.PortalGFXMemLoaded = 0;
}

//
// Palette stuff
//

void GFXSetSpritePaletteDefault()
{
    dmaCopy(SpriteSheetPal, SPRITE_PALETTE, 512);
}

//
// Entity Sprites
//

void GFXLoadPlayerSprites()
{
    if (GFXAllSpriteGFX.PlayerGFXMemLoaded) { return; }

    for (int a = 0; a < 8; a++)
    {
        GFXAllSpriteGFX.PlayerGFXMem[a] = oamAllocateGfx(
            &oamMain,
            SpriteSize_16x16,
            SpriteColorFormat_256Color
        );
        dmaCopy(
            (u8*) SpriteSheetTiles + TILE_SIZE * a,
            GFXAllSpriteGFX.PlayerGFXMem[a],
            16 * 16
        );
    }

    GFXAllSpriteGFX.PlayerGFXMemLoaded = 1;
}

void GFXFreePlayerSprites()
{
    if (!GFXAllSpriteGFX.PlayerGFXMemLoaded) { return; }

    for (int a = 0; a < 8; ++a)
    {
        oamFreeGfx(
            &oamMain,
            GFXAllSpriteGFX.PlayerGFXMem[a]
        );
    }

    GFXAllSpriteGFX.PlayerGFXMemLoaded = 0;
}

void GFXLoadSentinelSprites()
{
    if (GFXAllSpriteGFX.SentinelGFXMemLoaded) { return; }

    for (int a = 0; a < 2; a++)
    {
        for (int b = 0; b < 8; b++)
        {
            GFXAllSpriteGFX.SentinelGFXMem[a][b] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
            );
            dmaCopy(
                (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * (a + 1) + TILE_SIZE * b,
                GFXAllSpriteGFX.SentinelGFXMem[a][b],
                16 * 16
            );
        }
    }

    GFXAllSpriteGFX.SentinelGFXMemLoaded = 1;
}

void GFXLoadShredderSprites()
{
    if (GFXAllSpriteGFX.ShredderGFXMemLoaded) { return; }

    for (int a = 0; a < 4; a++)
    {
        GFXAllSpriteGFX.ShredderGFXMem[a] = oamAllocateGfx(
            &oamMain,
            SpriteSize_16x16,
            SpriteColorFormat_256Color
        );
        dmaCopy(
            (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 3 + TILE_SIZE * a,
            GFXAllSpriteGFX.ShredderGFXMem[a],
            16 * 16
        );
    }

    GFXAllSpriteGFX.ShredderGFXMemLoaded = 1;
}

void GFXLoadMinerSprites()
{
    if (GFXAllSpriteGFX.MinerGFXMemLoaded) { return; }

    for (int a = 0; a < 8; a++)
    {
        GFXAllSpriteGFX.MinerGFXMem[a] = oamAllocateGfx(
            &oamMain,
            SpriteSize_16x16,
            SpriteColorFormat_256Color
        );
        dmaCopy(
            (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 4 + TILE_SIZE * a,
            GFXAllSpriteGFX.MinerGFXMem[a],
            16 * 16
        );
    }

    GFXAllSpriteGFX.MinerGFXMemLoaded = 1;
}

void GFXLoadSuperSentinelSprites()
{
    if (!GFXAllSpriteGFX.SSBodyGFXMemLoaded)
    {
        // Super sentinel body
        for (int a = 0; a < 4; ++a)
        {
            for (int b = 0; b < 8; ++b)
            {
                GFXAllSpriteGFX.SSBodyGFXMem[a][b] = oamAllocateGfx(
                    &oamMain,
                    SpriteSize_16x16,
                    SpriteColorFormat_256Color
                );
                dmaCopy(
                    (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * (5 + a) + TILE_SIZE * 8 +
                    TILE_SIZE * b,
                    GFXAllSpriteGFX.SSBodyGFXMem[a][b],
                    16 * 16
                );
            }
        }

        GFXAllSpriteGFX.SSBodyGFXMemLoaded = 1;
    }

    if (!GFXAllSpriteGFX.SSLaserWeaponGFXMemLoaded)
    {
        // Super sentinel laser weapons
        for (int a = 0; a < 4; a++)
        {
            GFXAllSpriteGFX.SSLaserWeaponGFXMem[a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x32,
                SpriteColorFormat_256Color
            );
            dmaCopy(
                (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 4 + TILE_SIZE * 8 + TILE_SIZE * a,
                GFXAllSpriteGFX.SSLaserWeaponGFXMem[a],
                16 * 16
            );
            dmaCopy(
                (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 4 + TILE_SIZE * 8 + TILE_SIZE * (a + 4),
                GFXAllSpriteGFX.SSLaserWeaponGFXMem[a] + 16 * 8,
                16 * 16
            );
        }

        GFXAllSpriteGFX.SSLaserWeaponGFXMemLoaded = 1;
    }
}

void GFXLoadSuperShredderSprites()
{
    if (GFXAllSpriteGFX.SuperShredderGFXMemLoaded) { return; }

    // This is a bit of fun...
    // The Super Shredder's sprites are 32x32 and have no subparts (at time of writing).
    // So, we must allocate space for 32x32 sprites and copy over the data in a way that properly fills the sprite.

    for (int a = 0; a < 4; a++)
    {
        GFXAllSpriteGFX.SuperShredderGFXMem[a] = oamAllocateGfx(
            &oamMain,
            SpriteSize_32x32,
            SpriteColorFormat_256Color
        );

        const u8 *sprite_first_line = (u8*) SpriteSheetTiles
            + TILE_SIZE * SPRITE_SHEET_WIDTH * 10
            + TILE_SIZE * (8 + a * 2);
        const u8 *sprite_second_line = (u8*) SpriteSheetTiles
            + TILE_SIZE * SPRITE_SHEET_WIDTH * 11
            + TILE_SIZE * (8 + a * 2);

        // For easier indexing below
        const u8 *sprite_lines[2] = {sprite_first_line, sprite_second_line};

        for (int y = 0; y < 2; y++)
        {
            for (int x = 0; x < 2; x++)
            {
                const u8 *src = sprite_lines[y] + 16 * 16 * x;
                u16 *dst = GFXAllSpriteGFX.SuperShredderGFXMem[a] + 16 * 4 * x + 16 * 16 * y;

                dmaCopy(src, dst, 16 * 8);
                src += 16 * 8;
                dst += 16 * 8;
                dmaCopy(src, dst, 16 * 8);
            }
        }
    }

    GFXAllSpriteGFX.SuperShredderGFXMemLoaded = 1;
}

void GFXLoadPlayerExplosion()
{
    if (GFXAllSpriteGFX.PlayerExplosionGFXMemLoaded) { return; }

    for (int a = 0; a < 8; a++)
    {
        GFXAllSpriteGFX.PlayerExplosionGFXMem[a] = oamAllocateGfx(
            &oamMain,
            SpriteSize_16x16,
            SpriteColorFormat_256Color
        );
        dmaCopy(
            (u8*) SpriteSheetTiles + TILE_SIZE * 8 + TILE_SIZE * a,
            GFXAllSpriteGFX.PlayerExplosionGFXMem[a],
            16 * 16
        );
    }

    GFXAllSpriteGFX.PlayerExplosionGFXMemLoaded = 1;
}

void GFXLoadEnemyExplosion()
{
    if (GFXAllSpriteGFX.EnemyExplosionGFXMemLoaded) { return; }

    for (int a = 0; a < 8; a++)
    {
        GFXAllSpriteGFX.EnemyExplosionGFXMem[a] = oamAllocateGfx(
            &oamMain,
            SpriteSize_16x16,
            SpriteColorFormat_256Color
        );
        dmaCopy(
            (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH + TILE_SIZE * 8 + TILE_SIZE * a,
            GFXAllSpriteGFX.EnemyExplosionGFXMem[a],
            16 * 16
        );
    }

    GFXAllSpriteGFX.EnemyExplosionGFXMemLoaded = 1;
}

void GFXLoadSSExplosion()
{
    if (GFXAllSpriteGFX.SSExplosionGFXMemLoaded) { return; }

    for (int a = 0; a < 8; a++)
    {
        GFXAllSpriteGFX.SSExplosionGFXMem[a] = oamAllocateGfx(
            &oamMain,
            SpriteSize_16x16,
            SpriteColorFormat_256Color
        );
        dmaCopy(
            (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 9 + TILE_SIZE * 8 + TILE_SIZE * a,
            GFXAllSpriteGFX.SSExplosionGFXMem[a],
            16 * 16
        );
    }

    GFXAllSpriteGFX.SSExplosionGFXMemLoaded = 1;
}

//
// Bullets
//

void GFXLoadPlayerBullets()
{
    if (GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_PlayerBullet]) { return; }

    for (int a = 0; a < 4; a++)
    {
        GFXAllSpriteGFX.BulletGFXMem[BulletType_PlayerBullet][a] = oamAllocateGfx(
            &oamMain,
            SpriteSize_16x16,
            SpriteColorFormat_256Color
        );
        dmaCopy(
            (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 7 + TILE_SIZE * a,
            GFXAllSpriteGFX.BulletGFXMem[BulletType_PlayerBullet][a],
            16 * 16
        );
    }

    GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_PlayerBullet] = 1;
}

void GFXLoadSentinelBullets()
{
    if (GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_SentinelBullet]) { return; }

    for (int a = 0; a < 4; a++)
    {
        GFXAllSpriteGFX.BulletGFXMem[BulletType_SentinelBullet][a] = oamAllocateGfx(
            &oamMain,
            SpriteSize_16x16,
            SpriteColorFormat_256Color
        );
        dmaCopy(
            (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 6 + TILE_SIZE * a,
            GFXAllSpriteGFX.BulletGFXMem[BulletType_SentinelBullet][a],
            16 * 16
        );
    }

    GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_SentinelBullet] = 1;
}

void GFXLoadMinerBullets()
{
    // Miner mines
    if (!GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_MinerMine])
    {
        for (int a = 0; a < 4; a++)
        {
            GFXAllSpriteGFX.BulletGFXMem[BulletType_MinerMine][a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
            );
            dmaCopy(
                (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 5 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[BulletType_MinerMine][a],
                16 * 16
            );
        }

        GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_MinerMine] = 1;
    }

    // Smol mine explosion bullets
    if (!GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_MinerMineBullet])
    {
        for (int a = 0; a < 4; a++)
        {
            GFXAllSpriteGFX.BulletGFXMem[BulletType_MinerMineBullet][a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
            );
            dmaCopy(
                (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 3 + TILE_SIZE * 4 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[BulletType_MinerMineBullet][a],
                16 * 16
            );
        }

        GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_MinerMineBullet] = 1;
    }
}

void GFXLoadDeathBullets()
{
    if (GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_DeathBullet]) { return; }

    for (int a = 0; a < 4; a++)
    {
        GFXAllSpriteGFX.BulletGFXMem[BulletType_DeathBullet][a] = oamAllocateGfx(
            &oamMain,
            SpriteSize_16x16,
            SpriteColorFormat_256Color
        );
        dmaCopy(
            (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 3 + TILE_SIZE * 4 + TILE_SIZE * a,
            GFXAllSpriteGFX.BulletGFXMem[BulletType_DeathBullet][a],
            16 * 16
        );
    }

    GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_DeathBullet] = 1;
}

void GFXLoadBossNormalBullets()
{
    // Super sentinel bullets
    if (!GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_BossBullet])
    {
        for (int a = 0; a < 4; a++)
        {
            GFXAllSpriteGFX.BulletGFXMem[BulletType_BossBullet][a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
            );
            dmaCopy(
                (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 5 + TILE_SIZE * 4 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[BulletType_BossBullet][a],
                16 * 16
            );
        }

        GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_BossBullet] = 1;
    }
}

void GFXLoadSuperSentinelPilotLaserBullets()
{
    // Super sentinel pilot laser start
    if (!GFXAllSpriteGFX.WildBulletGFXMemLoaded[IndexForBulletType(BulletType_SSLaserPilotStart)])
    {
        for (int a = 0; a < 4; ++a)
        {
            GFXAllSpriteGFX.WildBulletGFXMem[IndexForBulletType(BulletType_SSLaserPilotStart)][a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
            );
            dmaCopy(
                (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 8 + TILE_SIZE * 4 + TILE_SIZE * a,
                GFXAllSpriteGFX.WildBulletGFXMem[IndexForBulletType(BulletType_SSLaserPilotStart)][a],
                16 * 16
            );
        }
    }

    // Super sentinel pilot laser segment
    if (!GFXAllSpriteGFX.WildBulletGFXMemLoaded[IndexForBulletType(BulletType_SSLaserPilotSegment)])
    {
        for (int a = 0; a < 4; ++a)
        {
            GFXAllSpriteGFX.WildBulletGFXMem[IndexForBulletType(BulletType_SSLaserPilotSegment)][a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
            );
            dmaCopy(
                (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 9 + TILE_SIZE * 4 + TILE_SIZE * a,
                GFXAllSpriteGFX.WildBulletGFXMem[IndexForBulletType(BulletType_SSLaserPilotSegment)][a],
                16 * 16
            );
        }
    }
}

void GFXLoadSuperSentinelLaserBullets()
{
    // Super sentinel laser start
    if (!GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_SSLaserStart])
    {
        for (int a = 0; a < 4; a++)
        {
            GFXAllSpriteGFX.BulletGFXMem[BulletType_SSLaserStart][a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
            );
            dmaCopy(
                (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 8 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[BulletType_SSLaserStart][a],
                16 * 16
            );
        }

        GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_SSLaserStart] = 1;
    }

    // Super sentinel laser segments
    if (!GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_SSLaserSegment])
    {
        for (int a = 0; a < 4; a++)
        {
            GFXAllSpriteGFX.BulletGFXMem[BulletType_SSLaserSegment][a] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
            );
            dmaCopy(
                (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 9 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[BulletType_SSLaserSegment][a],
                16 * 16
            );
        }

        GFXAllSpriteGFX.BulletGFXMemLoaded[BulletType_SSLaserSegment] = 1;
    }
}

void GFXLoadAllSuperSentinelBullets()
{
    GFXLoadBossNormalBullets();
    GFXLoadSuperSentinelPilotLaserBullets();
    GFXLoadSuperSentinelLaserBullets();
}

//
// Other
//

void GFXLoadPortalSprites()
{
    if (GFXAllSpriteGFX.PortalGFXMemLoaded) { return; }

    for (int a = 0; a < 2; a++)
    {
        for (int b = 0; b < 8; b++)
        {
            GFXAllSpriteGFX.PortalGFXMem[a * 8 + b] = oamAllocateGfx(
                &oamMain,
                SpriteSize_16x16,
                SpriteColorFormat_256Color
            );
            dmaCopy(
                (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * (2 + a) + TILE_SIZE * 8 + TILE_SIZE * b,
                GFXAllSpriteGFX.PortalGFXMem[a * 8 + b],
                16 * 16
            );
        }
    }

    GFXAllSpriteGFX.PortalGFXMemLoaded = 1;
}

//
// All of the above, please.
//

void GFXLoadAllSprites()
{
    // Setting the sprite palette
    GFXSetSpritePaletteDefault();

    //
    // Entity Sprites
    //

    GFXLoadPlayerSprites();
    GFXLoadSentinelSprites();
    GFXLoadShredderSprites();
    GFXLoadMinerSprites();

    GFXLoadSuperSentinelSprites();

    GFXLoadSuperShredderSprites();

    //
    // Explosions
    //

    GFXLoadPlayerExplosion();
    GFXLoadEnemyExplosion();
    GFXLoadSSExplosion();

    //
    // Bullets
    //

    GFXLoadPlayerBullets();
    GFXLoadSentinelBullets();
    GFXLoadMinerBullets();

    GFXLoadDeathBullets();

    GFXLoadAllSuperSentinelBullets();

    //
    // Miscellaneous
    //

    GFXLoadPortalSprites();

    // BANG -> !
    for (int a = 0; a < 4; ++a)
    {
        GFXAllSpriteGFX.BangGFXMem[a] = oamAllocateGfx(
            &oamMain,
            SpriteSize_16x16,
            SpriteColorFormat_256Color
        );
        dmaCopy(
            (u8*) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 6 + TILE_SIZE * 4 + TILE_SIZE * a,
            GFXAllSpriteGFX.BangGFXMem[a],
            16 * 16
        );
    }
}
