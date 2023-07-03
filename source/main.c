/* =====================================================================================================================
 * Important
 * ToDo:
 *  Fix up the super sentinel lasers:                                                                               TICK
 *      Proper sprites.                                                                                             TICK
 *      Proper hitboxes.                                                                                            TICK
 *  Boss fight woo.                                                                                                 TICK
 *      The same one worked on in versions 15 ad 16.                                                                TICK
 *      BUT BETTER.                                                                                                 TICK
 *      Strafing firing bullets.                                                                                    TICK
 *      Bouncing from side to side with LASERS.                                                                     TICK
 *      Pincer attack.                                                                                              TICK
 *      Okay, so it is very similar, but this time it will work.                                                    TICK
 *      Boss entry.                                                                                                 TICK
 *      Boss exit:                                                                                                  TICK
 *          More explosions YAY.                                                                                    TICK
 *          Spinny roundy parts                                                                                     TICK
 *      Fix the lasers because I broke them:                                                                        TICK
 *          PAIN.                                                                                                   TICK
 *          Laser sprites are wrong, idk why.                                                                       TICK
 *      Exclamation marks BANG just before firing the lasers or...                                                  TICK
 *      Pilot lasers for the lasers.                                                                                TICK
 *      Fix up the boss fight text.                                                                                 TICK
 *      The lazers sometimes remain for too long when transitioning to the last phase.                              TICK
 *          They may not be despawned correctly or not at all.                                                      TICK
 *  Fix ghost bullets when going from 8 enemies to the boss.                                                        TICK
 *  There seems to be a bug where sometimes the win condition is met when there are only death bullets alive.
 *  Add an option to the main menu for improvements/next version.
 *  Scanning sector for extra information.                                                                          TICK
 *      Sectors                                                                                                     TICK
 *      Boss                                                                                                        TICK
 *  Basically spruce up the pause menu.                                                                             TICK
 *  Last stage is a bit too difficult when compared to the boss, maybe leave it as a challenge?
 *  Maybe come up with a way to make the normal enemy text screen a little more interesting
 *
 * Not important things and ideas
 * ToDo:
 *  Portal out of the sector / Level transition.
 *  Loading screen - before the game starts.
 *      Kinda similar to an old computer booting.
 *  Ahh yes, a tutorial would be cool.
 *      Custom background for tutorial.
 *          Maybe with a station or big ship in the background, something like that.
 *          Maybe like you are with a fleet.
 *  Scroll ui interface.
 *      Mainly for the credits if they get too large.
 *          I don't know if they will though.
 *  Scoreboard.
 *      Different screens for easy, normal, hard.
 *  Stats.
 *      Keep track of things like time played and deaths.
 *      Half implemented already because the check collision functions return the type of the collision.
 *  Custom seed input.
 *  Figure out how to layer backgrounds.
 *      Make the screen boarder the top most layer.
 *          So other stuff is drawn underneath it.
 *      Also when the enemies portal in they are drawn behind the background.
 *          Kinda not visible when not looking for it, but it annoys me.
 *          SO FIX IT FUTURE ME.
 *      We have some more freedom then.
 *      May be something to do with it being BgType_Bmp8.
 *      Try other stuff and see if it works.
 *  If there becomes too much sprite GFX to be loaded all at once then use smart loading where only the necessary
 *  sprites for the current activity are loaded.
 *      It is already set up for loading the sprites, just the unloading needs to be done
 *      ONLY if necessary
 * ================================================================================================================== */

// NDS
#include <nds.h>

// None of the following are necessary
// They are here so vs code actually knows they exist and does not constantly yell at me
#include <nds/arm9/video.h>
#include <nds/arm9/background.h>
#include <nds/arm9/sprite.h>

// Standard libraries
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h> // Needed for the random number seeding - Mr. King
#include <string.h>

// My stuff
#include "GameLibrary.h"

// Backgrounds
#include "BasicBackground.h"

// Version
#define VERSION "Development 18"

//---------------------------------------------------------------------------------
// Bullets, player and enemies
//---------------------------------------------------------------------------------

// The array containing all the bullets
Bullet BulletArray[MAX_BULLET_COUNT];
// Player
Entity Player;
// Array containing the enemies
Entity EnemyEntityArray[8];

//---------------------------------------------------------------------------------
// Miscellaneous
//---------------------------------------------------------------------------------

int FrameNumber = 0; // Take a guess

// The playable area, equivalent to the screen
// Bullets are deleted when exiting this
int PlayableArea[4] = {0, 0, 256, 192};
// The hitboxes of the boarder of the screen
// Can be more, but I don't want more
int ScreenBoarder[4][4] = {
        {0,   0,   8,   192},
        {0,   0,   256, 8},
        {0,   184, 256, 8},
        {248, 0,   8,   192}
};

// The Seed
long long int Seed = 0;
// The Seed in string form
// 20 chars long because the max length of a long long int is 20 chars
char SeedString[21] = "12345678901234567890";

// Numbers that are known to produce bugs
// Seed = 1668036031; srand((unsigned) Seed); // Two miners spawned in same place with 1 set of enemies
// Seed = 1668446882; srand((unsigned) Seed); // Two shredders spawned in same place with 2 set of enemies

// The difficulty, duh
//    E = Easy
//    N = Normal
//    H = Hard
char Difficulty = 'N';
// The number of enemy groups to spawn in
int NumEnemyGroups = 1;

// Lives
//      Positive numbers = num lives left
//      -1 = infinite
int Lives;

// Returns the number of lives depending on the difficulty
int GetNumLives() {
    switch (Difficulty) {
        case 'E':
            return -1;
        case 'N':
            return 5;
        case 'H':
            return 1;
    }

    return -1;
}

// The current activity to perform
//  Menus
//      M = Main menu
//      D = Difficulty select
//      C = Credits
//      N = Next version details
//      L = Lose screen
//      W = Win screen
//      P = Pause screen
//  Games
//      G = Game
//      R = Resume game
//  Bosses
//      S = Super sentinel
//      0 = Resume super sentinel
//  Challenge round
//      Y = Challenge
//      Z = Resume challenge
char CurrentActivity = 'M';

// What the current activity should be set to after a pause is resumed
char ResumeAfterPause;

// region - Print functions to add extra information to the UIs

// Prints the seed at the line number
void __PrintSeed(int line_num) {
    UIWriteText(
            "Seed: ",
            line_num
    );
    UIWriteTextAtOffset(
            SeedString,
            line_num,
            6
    );
}

// Prints the difficulty at the line number
void __PrintDifficulty(int line_num) {
    UIWriteText(
            "Difficulty: ",
            line_num
    );
    switch (Difficulty) {
        case 'E':
            UIWriteTextAtOffset(
                    "Easy",
                    line_num,
                    12
            );
            break;
        case 'N':
            UIWriteTextAtOffset(
                    "Normal",
                    line_num,
                    12
            );
            break;
        case 'H':
            UIWriteTextAtOffset(
                    "Hard",
                    line_num,
                    12
            );
            break;
    }
}

// Prints the version at the line number, aligned to the right
void __PrintVersion(int line_num, int char_offset, int right_align) {
    int len = strlen(VERSION);
    if (right_align) {
        int true_offset = UI_NUM_CHARS - len - char_offset;
        if (true_offset < 9)
            true_offset = 9;
        UIWriteTextAtOffset("Version:", line_num, true_offset - 9);
        UIWriteTextAtOffset(VERSION, line_num, true_offset);
    } else {
        UIWriteTextAtOffset("Version:", line_num, char_offset);
        UIWriteTextAtOffset(VERSION, line_num, char_offset + 9);
    }
}

// Print function to display the extra information
void PrintDifficultyAndSeedFunc(void) {
    __PrintDifficulty(22);
    __PrintSeed(23);
}

// Main Func WOO!!!
void MainMenuPrintFunc(void) {
    UIWriteTextAtOffset(
            "Work In Progress",
            15,
            14
    );
    UIWriteTextAtOffset(
            "Not For Distribution",
            16,
            12
    );

    __PrintDifficulty(23);
}

// Prints the explanations of the difficulties
// Needs to be rewritten each time the separation is changed on the difficulty interface
void DifficultySelectPrintFunc(void) {
    UIWriteTextAtOffset(
            "Infinite lives",
            4,
            8
    );
    UIWriteTextAtOffset(
            "Five lives",
            7,
            8
    );
    UIWriteTextAtOffset(
            "One life",
            10,
            8
    );
}

// Extra information to control the scanning

int StartFrameNum = -1;
int ScanningToggle = 0;
int ScanningFinished = 0;

void __SectorScanPrintFunction(void) {
    if (StartFrameNum == -1)
        StartFrameNum = FrameNumber;

    if ((FrameNumber - StartFrameNum) % 30 == 0)
        ScanningToggle = !ScanningToggle;

    if (!ScanningFinished) {
        if (ScanningToggle)
            UIWriteTextAtOffset("SCANNING", 7, 1);
    } else {
        UIWriteTextAtOffset("SCAN COMPLETE", 7, 1);
    }

    char temp[UI_NUM_CHARS + 1];

    int time_since_pause = FrameNumber - StartFrameNum;
    int time_to_display = 60;
    int line_num = 9;

    if (time_since_pause > time_to_display) {
        UIWriteTextAtOffset("Hull integrity:", line_num, 3);
        if (Player.dead) {
            time_to_display += 60;
            UIWriteTextAtOffset("  0%", line_num, 25);
            if (time_since_pause > time_to_display) {
                UIWriteTextAtOffset("Temporal Reset", line_num + 1, 5);
                UIWriteTextAtOffset("Recommended", line_num + 2, 5);
            }
        } else {
            UIWriteTextAtOffset("100%", line_num, 25);
        }
    }

    line_num += 2;
    if (Player.dead)
        line_num += 2;
    time_to_display += 60;

    if (time_since_pause > time_to_display) {
        if (Difficulty != 'E') {
            UIWriteTextAtOffset("Temporal Resets:", line_num, 3);
            char num[2];
            itoa(Lives, num, 10);
            UIWriteTextAtOffset(num, line_num, 27);
        }
    }

    line_num += 2;
    time_to_display += 60;

    if (time_since_pause > time_to_display) {
        UIWriteTextAtOffset("Enemies Detected:", line_num, 3);
        int num = 0;
        for (int i = 0; i < 8; ++i) {
            if (!EnemyEntityArray[i].dead)
                num++;
        }
        itoa(num, temp, 10);
        UIWriteTextAtOffset(temp, line_num, 27);
    }

    time_to_display += 60;

    if (time_since_pause > time_to_display)
        ScanningFinished = 1;
}

int SuperSentinelStartIndexes[26] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                                     13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};

void __SuperSentinelScanPrintFunction(void) {
    if (StartFrameNum == -1) {
        StartFrameNum = FrameNumber;
        ShuffleIntArray(SuperSentinelStartIndexes, 26);
    }

    if ((FrameNumber - StartFrameNum) % 30 == 0) ScanningToggle = !ScanningToggle;

    if (!ScanningFinished) {
        if (ScanningToggle) UIWriteTextAtOffset("SCANNING", 7, 1);
    } else {
        UIWriteTextAtOffset("SCAN COMPLETE", 7, 1);
    }

    // char temp[UI_NUM_CHARS + 1];

    int time_since_pause = FrameNumber - StartFrameNum;
    int time_to_display = 60;
    int line_num = 9;

    if (time_since_pause <= time_to_display) return;

    UIWriteTextAtOffset("Hull integrity:", line_num, 3);
    if (Player.dead) {
        time_to_display += 60;
        UIWriteTextAtOffset("  0%", line_num, 25);
        if (time_since_pause > time_to_display) {
            UIWriteTextAtOffset("Temporal Reset", line_num + 1, 5);
            UIWriteTextAtOffset("Recommended", line_num + 2, 5);
        }
    } else {
        UIWriteTextAtOffset("100%", line_num, 25);
    }

    line_num += 2;
    if (Player.dead) line_num += 2;
    time_to_display += 60;

    if (time_since_pause <= time_to_display) return;

    if (Difficulty != 'E') {
        UIWriteTextAtOffset("Temporal Resets:", line_num, 3);
        char num[2];
        itoa(Lives, num, 10);
        UIWriteTextAtOffset(num, line_num, 27);
    }

    line_num += 2;
    time_to_display += 60;

    if (time_since_pause <= time_to_display) return;

    UIWriteTextAtOffset("Enemies Detected:       1", line_num, 3);

    line_num += 2;
    time_to_display += 60;

    if (time_since_pause <= time_to_display) return;

    const int super_sentinel_health = SSGetHealth(EnemyEntityArray);

    if (super_sentinel_health > SS_LASER_HEALTH) {
        UIWriteTextAtOffset("Disobedience Detected", line_num, 3);
    } else if (super_sentinel_health > SS_FINAL_HEALTH) {
        UIWriteTextAtOffset("Insolence Detected", line_num, 3);
    } else if (super_sentinel_health > SS_CRITICAL_HEALTH) {
        UIWriteTextAtOffset("Anomaly Detected", line_num, 3);  // Better string needed
    } else {
        UIWriteTextAtOffset("Reporting Anomaly", line_num, 3);
    }

    time_to_display += 30;

    for (int j = 0; j < 6; j++) {
        if (time_since_pause <= time_to_display + 23 * j) break;

        if (j == 5) {
            UIWriteTextAtOffset("                          ", line_num, 3);
            break;
        }

        int loopLimit = (!j) ? 6 : 5;

        for (int i = 0; i < loopLimit; i++) {
            int offset = SuperSentinelStartIndexes[(bool)j * 6 + (bool)j * (j - 1) * 5 + i];
            UIWriteTextAtOffset("-", line_num, 3 + offset);
        }
    }

    // This happens independently to the previous for loop
    time_to_display += 30;

    if (time_since_pause > time_to_display) ScanningFinished = 1;
}

// Displays the seed, difficulty and the amount of lives left
// Also displays some extra information
void PauseScreenPrintFunc(void) {
    switch (ResumeAfterPause) {
        case 'R': // Resume normal game
        case 'Z': // Resume challenge game
            __SectorScanPrintFunction();
            break;

        case '0': // Resume boss fight
            __SuperSentinelScanPrintFunction();
            break;

        default:
            UIWriteTextAtOffset("Something broke", 10, 3);
    }
}

// endregion

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

    // Loading the backgrounds
    int bg3 = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    dmaCopy(BasicBackgroundBitmap, bgGetGfxPtr(bg3), 256 * 256);
    dmaCopy(BasicBackgroundPal, BG_PALETTE, sizeof(BasicBackgroundPal));

    // int bg2 = bgInit(2, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    // dmaCopy(PauseBGBitmap, bgGetGfxPtr(bg2), 256 * 256);
    // dmaCopy(PauseBGPal, BG_PALETTE, sizeof(PauseBGPal));

    // bgHide(bg2);
    // bgUpdate();

    // Seeding the random number generator
    srand((unsigned) time(&Seed));

    // Sprite Memory Allocation and Loading
    GFXInit();
    GFXLoadAllSprites();

    // #region - Creating the Interfaces
    UIInterfaceStruct main_menu_interface, difficulty_select_interface, pause_interface, credits_interface,
        lose_interface, main_win_interface, challenge_win_interface, boss_win_interface, unimplemented_interface;
    UIInterfaceStruct *win_interface_to_use = &unimplemented_interface; // Just in case it is not set to not crash things
    UIInitInterface(
            &main_menu_interface,
            "Main Menu",
            6,
            "Play",
            "Difficulty Select",
            "Credits",
            "Next version",
            "Boss Quick Start",
            "Challenge"
    );
    main_menu_interface.Separation = 1;
    // I should not really do this as it comes with the repercussion that any function that interacts with this will be
    //  given the incorrect number of ui options.
    // I am doing it to hide the boss and challenge options until you beat the game normally.
    // This should not cause any problems with how I use the struct and because I do not un-initilise them.
    main_menu_interface.NumUIOptions = 4;
    UIInitInterface(
            &difficulty_select_interface,
            "Difficulty Select",
            3,
            "Easy",
            "Normal",
            "Hard"
    );
    difficulty_select_interface.Choice = 1; // Default normal
    difficulty_select_interface.Separation = 2;
    UIInitInterface(
            &credits_interface,
            "Credits",
            13,
            "Developer:",
            "    Seven",
            "",
            "Art:",
            "    Bax",
            "    Seven",
            "",
            "Advice/Testing:",
            "    Bax",
            "    Nicole",
            "    Dennis",
            "",
            "Return to Main Menu"
    );
    credits_interface.Choice = credits_interface.NumUIOptions - 1;
    UIInitInterface(
            &pause_interface,
            "Temporal Disturbance Detected",
            2,
            "Resume",
            "Abort"
    );
    UIInitInterface(
            &lose_interface,
            "Temporal Reset Failed",
            1,
            "Return to Main Menu"
    );
    UIInitInterface(
            &main_win_interface,
            "Main Game Completed Well Done",
            1,
            "Return to Main Menu"
    );
    UIInitInterface(
            &challenge_win_interface,
            "Challenge Completed Well Done",
            1,
            "Return to Main Menu"
    );
    UIInitInterface(
            &boss_win_interface,
            "Boss Defeated",
            1,
            "Return to Main Menu"
    );
    UIInitInterface(
            &unimplemented_interface,
            "Unimplemented Interface",
            1,
            "Return to Main Menu"
    );
    // #endregion

    // The current choice from the interface
    int ui_choice;
    // The result from the game
    int game_result;

    while (1) {
        switch (CurrentActivity) {
            // #region - Handle the main menu
            case 'M':
                HideEverySprite();

                // Reseeding the random number generator
                Seed = rand() % 100000000;
                srand((unsigned) Seed);
                itoa(Seed, SeedString, 10);
                GameRandomiseEnemySpawns();

                // Resetting gameplay variables
                NumEnemyGroups = 1;
                Lives = GetNumLives();

                ui_choice = UIHandleInterfaceAtOffsetWithFunction(
                        &main_menu_interface,
                        &FrameNumber,
                        1,
                        1,
                        &MainMenuPrintFunc // PrintDifficulty
                );

                switch (ui_choice) {
                    case 0: // Play
                        CurrentActivity = 'G';
                        break;
                    case 1: // Difficulty select
                        CurrentActivity = 'D';
                        break;
                    case 2: // Credits
                        CurrentActivity = 'C';
                        break;
                    case 3: // Next version information
                        CurrentActivity = 'N';
                        break;
                    case 4: // Boss
                        CurrentActivity = 'S';
                        break;
                    case 5: // Challenge
                        CurrentActivity = 'Y';
                        NumEnemyGroups = 4;
                        break;
                }

                main_menu_interface.Choice = 0;

                break;
            // #endregion

            // #region - Handle the difficulty select
            case 'D':
                ui_choice = UIHandleInterfaceAtOffsetWithFunction(
                        &difficulty_select_interface,
                        &FrameNumber,
                        1,
                        1,
                        &DifficultySelectPrintFunc
                );

                switch (ui_choice) {
                    case 0: // Easy
                        Difficulty = 'E';
                        Lives = -1;
                        break;
                    case 1: // Medium
                        Difficulty = 'N';
                        Lives = 5;
                        break;
                    case 2: // Hard
                        Difficulty = 'H';
                        Lives = 1;
                        break;
                }

                CurrentActivity = 'M';

                break;
            // #endregion

            // #region - Credits screen
            case 'C':
                ui_choice = UIHandleInterfaceAtOffset(
                        &credits_interface,
                        &FrameNumber,
                        1,
                        1
                );

                if (ui_choice == 5) main_menu_interface.NumUIOptions = 6;

                if (ui_choice == credits_interface.NumUIOptions - 1) CurrentActivity = 'M';

                break;
            // #endregion

            // #region - Handling sector gameplay
            case 'G': // Start a new game
            case 'R': // Resume the game
                // If starting anew run the setup
                if (CurrentActivity == 'G')
                    GameSectorSetup(
                            &Player,
                            EnemyEntityArray, 8,
                            BulletArray, MAX_BULLET_COUNT,
                            &FrameNumber,
                            &EnemiesAllEnemyData, &GFXAllSpriteGFX,
                            NumEnemyGroups
                    );

                // If no setup was run then resumed
                // Changing activity back to game
                CurrentActivity = 'G';

                game_result = GameRunGameLoop(
                        &Player,
                        EnemyEntityArray, 8,
                        BulletArray, MAX_BULLET_COUNT,
                        &FrameNumber,
                        &EnemiesAllEnemyData, &GFXAllSpriteGFX,
                        PlayableArea,
                        ScreenBoarder, 4,
                        0
                );

                switch (game_result) {
                    case -1: // Pause
                        CurrentActivity = 'P';
                        ResumeAfterPause = 'R';
                        break;

                    case 0: // Player dies
                        // To prevent endless run after death
                        CurrentActivity = 'G';

                        // Reduce life considering difficulty
                        if (Difficulty == 'N' || Difficulty == 'H')
                            Lives--;

                        // If all lives lost
                        if (Lives == 0)
                            CurrentActivity = 'L';

                        break;

                    case 1: // Player wins
                        // Checking for increase difficulty or win
                        if (NumEnemyGroups < 4) { // Add more enemies
                            NumEnemyGroups++;
                            GameRandomiseEnemySpawns();
                        } else { // Summon the boss
                            CurrentActivity = 'S';
                        }

                        // Reset the number of lives
                        Lives = GetNumLives();

                        break;
                }

                break;
            // #endregion

            // #region - Pause screen
            case 'P':
                ui_choice = UIHandleInterfaceAtOffsetWithFunction(
                        &pause_interface,
                        &FrameNumber,
                        1,
                        1,
                        &PauseScreenPrintFunc
                );

                // Reset this to reset the scanning
                StartFrameNum = -1;
                ScanningToggle = 0;
                ScanningFinished = 0;

                switch (ui_choice) {
                    case 0: // Resume
                        CurrentActivity = ResumeAfterPause;
                        break;

                    case 1: // Main menu
                        CurrentActivity = 'M';
                        break;
                }

                pause_interface.Choice = 0;

                break;

            // #endregion

            // #region - Lose screen
            case 'L':
                ui_choice = UIHandleInterfaceAtOffsetWithFunction(
                        &lose_interface,
                        &FrameNumber,
                        1,
                        1,
                        PrintDifficultyAndSeedFunc
                );
                CurrentActivity = 'M';
                break;
            // #endregion

            // #region - Win screen
            case 'W':
                ui_choice = UIHandleInterfaceAtOffsetWithFunction(
                        win_interface_to_use,
                        &FrameNumber,
                        1,
                        1,
                        PrintDifficultyAndSeedFunc
                );
                CurrentActivity = 'M';
                break;
            // #endregion

            // #region - Super Sentinel battle
            case 'S': // Start the super sentinel battle
            case '0': // Resume from pause
                if (CurrentActivity == 'S')
                    SSSetupForGameLoop(
                            &Player,
                            EnemyEntityArray, 8,
                            BulletArray, MAX_BULLET_COUNT,
                            &FrameNumber,
                            &GFXAllSpriteGFX,
                            bg3
                    );

                // Back to normal
                CurrentActivity = 'S';

                game_result = SSRunGameLoop(
                        &Player,
                        EnemyEntityArray, 8,
                        BulletArray, MAX_BULLET_COUNT,
                        &FrameNumber,
                        &GFXAllSpriteGFX,
                        PlayableArea,
                        ScreenBoarder, 4
                );

                switch (game_result) {
                    case -1: // Pause
                        CurrentActivity = 'P';
                        ResumeAfterPause = '0';
                        break;

                    case 0: // Player dies
                        // To prevent endless run after death
                        CurrentActivity = 'S';

                        // Reduce life considering difficulty
                        if (Difficulty == 'N' || Difficulty == 'H')
                            Lives--;

                        // If all lives lost
                        if (Lives == 0)
                            CurrentActivity = 'L';

                        break;

                    case 1: // region - Player wins
                        CurrentActivity = 'W'; // WIN SCREEN YAY

                        // Choosing the win interface to use
                        if (NumEnemyGroups == 4) {
                            win_interface_to_use = &main_win_interface;
                            main_menu_interface.NumUIOptions = 6;
                        } else {
                            win_interface_to_use = &boss_win_interface;
                        }

                        // Run the death animation
                        SSRunEndLoop(
                            &Player,
                            EnemyEntityArray, 8,
                            BulletArray, MAX_BULLET_COUNT,
                            &FrameNumber,
                            &GFXAllSpriteGFX
                        );

                        break;
                        // endregion
                }

                break;
            // #endregion

            // #region - Challenge battle
            case 'Y': // New challenge attempt
            case 'Z': // Resume challenge attempt
                if (CurrentActivity == 'Y') {
                    GameSectorSetup(
                        &Player,
                        EnemyEntityArray, 8,
                        BulletArray, MAX_BULLET_COUNT,
                        &FrameNumber,
                        &EnemiesAllEnemyData, &GFXAllSpriteGFX,
                        4
                    );
                }

                // Back to normal if resume
                CurrentActivity = 'Y';

                game_result = GameRunGameLoop(
                    &Player,
                    EnemyEntityArray, 8,
                    BulletArray, MAX_BULLET_COUNT,
                    &FrameNumber,
                    &EnemiesAllEnemyData, &GFXAllSpriteGFX,
                    PlayableArea,
                    ScreenBoarder, 4,
                    1
                );

                // Processing game result
                switch (game_result) {
                case -1:  // Pause
                    CurrentActivity = 'P';
                    ResumeAfterPause = 'Z';
                    break;

                case 0:  // Player dies
                    // To prevent endless run after death
                    CurrentActivity = 'Y';

                    // Reduce life considering difficulty
                    if (Difficulty == 'N' || Difficulty == 'H') Lives--;

                    // If all lives lost
                    if (Lives == 0) CurrentActivity = 'L';

                    break;

                case 1:  // Player wins
                    // Choosing the win interface to use
                    win_interface_to_use = &challenge_win_interface;

                    // We won yay
                    CurrentActivity = 'W';

                    // Reset the number of lives
                    Lives = GetNumLives();

                    break;
                }

                break;
            // #endregion

            // #region - For currently unimplemented interfaces
            default:
                UIHandleInterfaceAtOffset(
                        &unimplemented_interface,
                        &FrameNumber,
                        1,
                        1
                );

                CurrentActivity = 'M';

                break;
            // #endregion
        }
    }

    return 0;
}

/* Notes
 * Screen size is 256 × 192 pixels (4:3 aspect ratio)
 * Screen is 32 char wide and 24 lines tall
 */