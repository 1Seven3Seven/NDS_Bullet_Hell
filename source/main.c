/*
 * Important
 * ToDo:
 *
 * Not important, just ideas
 * ToDo:
 *  Get a basic ui working
 *      Can be either text based or gameplay based, idk yet
 *      I think text based in a similar vein to Duskers would be cool
 *      Or a mixture of both where you use text to select difficulty and gameplay to select tutorial/game
 *  Ahh yes, a tutorial would be cool
 *      Custom background for tutorial, maybe with a station or big ship in the background, something like that
 *      Maybe like you are with a fleet
 *  Level transition
 *  Death screen
 */

// NDS
#include <nds.h>

// Standard libraries
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h> // Needed for the random number seeding - Mr. King
#include "string.h"

// My stuff
#include "GameLibrary.h"

// Backgrounds
#include "BasicBackground.h"

//---------------------------------------------------------------------------------
// Bullets, player and enemies
//---------------------------------------------------------------------------------
Bullet BulletArray[MAX_BULLET_COUNT];
Entity Player;
Entity EnemyEntityArray[8];

//---------------------------------------------------------------------------------
// Miscellaneous
//---------------------------------------------------------------------------------
int FrameNumber = 0; // Take a guess

// Main boarder hitboxes
int PlayableArea[4] = {0, 0, 256, 192};
int ScreenBoarder[4][4] = {
        {0,   0,   8,   192},
        {0,   0,   256, 8},
        {0,   184, 256, 8},
        {248, 0,   8,   192}
};

// Seed
long long int Seed;

// Difficulty
int Difficulty = 0;
int NumEnemies;

//---------------------------------------------------------------------------------
int main(void)
//---------------------------------------------------------------------------------
{
    // Enable the main screen
    videoSetMode(MODE_5_2D);
    // Setting and Initialising VRAM Bank A to sprites
    vramSetBankA(VRAM_A_MAIN_SPRITE);
    oamInit(&oamMain, SpriteMapping_1D_128, false);
    // Setting and Initialising VRAM bank B to background slot 0
    vramSetBankB(VRAM_B_MAIN_BG_0x06000000);

    // Initialise the bottom screen for text
    consoleDemoInit();

    // Seeding the random number generator
    srand((unsigned) time(&Seed));
    // Seed = 1737; srand((unsigned) t); // Code to prevent randomness
    // Seed = 1668036031; srand((unsigned) t); // Two miners spawned in same place with 1 set of enemies

    // Loading the background
    int bg3 = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    dmaCopy(BasicBackgroundBitmap, bgGetGfxPtr(bg3), 256 * 256);
    dmaCopy(BasicBackgroundPal, BG_PALETTE, sizeof(BasicBackgroundPal));

    // Sprite Memory Allocation and Loading
    GFXLoadAllSprites();

    // ========== Game loading screen here ==========

    // ========== Main menu here ==========

    RESTART:

    // Setup
    GameRandomiseEnemySpawns();

    NumEnemies = Difficulty + 1;
    if (NumEnemies > 4) {
        NumEnemies = 4;
    }

    GameSectorSetup(
            &Player,
            EnemyEntityArray, 8,
            BulletArray, MAX_BULLET_COUNT,
            &FrameNumber,
            &EnemiesAllEnemyData,
            &GFXAllSpriteGFX,
            NumEnemies
    );

    int result = GameRunGameLoop(
            &Player,
            EnemyEntityArray, 8,
            BulletArray, MAX_BULLET_COUNT,
            &FrameNumber,
            &EnemiesAllEnemyData,
            &GFXAllSpriteGFX,
            PlayableArea,
            ScreenBoarder, 4,
            Seed,
            Difficulty == 4
    );

    if (Difficulty < 5 && result == 1) {
        Difficulty++;
        goto RESTART;
    }

    int keys, pressed;

    // Finished
    while (1) {
        // Clear the text
        consoleClear();
        // Get key presses
        scanKeys();
        keys = keysHeld();
        pressed = keysDown();
        // Frame number
        FrameNumber++;

        // Displaying result
        iprintf("Result: ");
        switch (result) {
            case 1:
                iprintf("You win\n");
                break;

            case 0:
                iprintf("You lose\n");
                break;

            case -1:
                iprintf("Inconclusive\n");
                break;
        }
        iprintf(
                "Difficulty reached: %d\n",
                Difficulty + 1
        );

        // Exit condition
        iprintf("\nReload rom to play again\n");
        iprintf("\nAll done\nPress start and select at the\nsame time to exit\n");

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
        // To exit
        if (pressed & KEY_START && pressed & KEY_SELECT)
            break;
    }

    return 0;
}

// Notes
// Screen size is 256 × 192 pixels (4:3 aspect ratio)
