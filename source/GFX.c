#include "GFX.h"

#include <nds.h>
#include "SpriteSheet.h"

#include "Constants.h"

GFXSpritesStruct GFXAllSpriteGFX;

void GFXLoadAllSprites() {
    // Setting the sprite palette
    dmaCopy(SpriteSheetPal, SPRITE_PALETTE, 512);

    // Player sprites
    for (int a = 0; a < 8; a++) {
        GFXAllSpriteGFX.PlayerGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * a,
                GFXAllSpriteGFX.PlayerGFXMem[a],
                16 * 16
        );
    }
    // Sentinel sprites
    for (int a = 0; a < 2; a++) {
        for (int b = 0; b < 8; b++) {
            GFXAllSpriteGFX.SentinelGFXMem[a][b] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * (a + 1) + TILE_SIZE * b,
                    GFXAllSpriteGFX.SentinelGFXMem[a][b],
                    16 * 16
            );
        }
    }
    // Shredder sprites
    for (int a = 0; a < 4; a++) {
        GFXAllSpriteGFX.ShredderGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 3 + TILE_SIZE * a,
                GFXAllSpriteGFX.ShredderGFXMem[a],
                16 * 16
        );
    }
    // Miner sprites
    for (int a = 0; a < 8; a++) {
        GFXAllSpriteGFX.MinerGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 4 + TILE_SIZE * a,
                GFXAllSpriteGFX.MinerGFXMem[a],
                16 * 16
        );
    }

    // Player explosion
    for (int a = 0; a < 8; a++) {
        GFXAllSpriteGFX.PlayerExplosionGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * 8 + TILE_SIZE * a,
                GFXAllSpriteGFX.PlayerExplosionGFXMem[a],
                16 * 16
        );
    }
    // Enemy explosion
    for (int a = 0; a < 8; a++) {
        GFXAllSpriteGFX.EnemyExplosionGFXMem[a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH + TILE_SIZE * 8 + TILE_SIZE * a,
                GFXAllSpriteGFX.EnemyExplosionGFXMem[a],
                16 * 16
        );
    }

    // Player bullets
    for (int a = 0; a < 4; a++) {
        GFXAllSpriteGFX.BulletGFXMem[PLAYER_BULLET][a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 7 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[PLAYER_BULLET][a],
                16 * 16
        );
    }
    // Sentinel bullets
    for (int a = 0; a < 4; a++) {
        GFXAllSpriteGFX.BulletGFXMem[SENTINEL_BULLET][a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 6 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[SENTINEL_BULLET][a],
                16 * 16
        );
    }
    // Miner mines
    for (int a = 0; a < 4; a++) {
        GFXAllSpriteGFX.BulletGFXMem[MINER_MINE][a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 5 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[MINER_MINE][a],
                16 * 16
        );
    }
    // Smol mine explosion bullets
    for (int a = 0; a < 4; a++) {
        GFXAllSpriteGFX.BulletGFXMem[MINER_MINE_BULLET][a] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
        dmaCopy(
                (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * 5 + TILE_SIZE * 4 + TILE_SIZE * a,
                GFXAllSpriteGFX.BulletGFXMem[MINER_MINE_BULLET][a],
                16 * 16
        );
    }

    // PORTALS!!!
    for (int a = 0; a < 2; a++) {
        for (int b = 0; b < 8; b++) {
            GFXAllSpriteGFX.PortalGFXMem[a * 8 + b] = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
            dmaCopy(
                    (u8 *) SpriteSheetTiles + TILE_SIZE * SPRITE_SHEET_WIDTH * (2 + a) + TILE_SIZE * 8 + TILE_SIZE * b,
                    GFXAllSpriteGFX.PortalGFXMem[a * 8 + b],
                    16 * 16
            );
        }
    }
}