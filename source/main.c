// NDS
#include <nds.h>

// Standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h> // Needed for the random number seeding - Mr. King
#include <stdbool.h>

// My stuff
#include "GameState.h"
#include "GameLibrary.h"

// Backgrounds
#include "BattleBackground.h"
#include "BossBackground.h"
#include "TitleBackground.h"

// Returns the number of lives depending on the difficulty
int GetNumLives()
{
    switch (GameState.Difficulty)
    {
    case 'E':
        return -1;
    case 'N':
        return 5;
    case 'H':
        return 1;
    default:
        return -1;
    }
}

void SeedRNG()
{
    GameState.Seed = rand() % 100000000;
    srand((unsigned) GameState.Seed);
    itoa(GameState.Seed, GameState.SeedString, 10);
}

//---------------------------------------------------------------------------------
// ReSharper disable once CppDFAConstantFunctionResult
int main(void)
//---------------------------------------------------------------------------------
{
    //
    // Generic NDS stuff
    //

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

    // Seeding the random number generator
    srand((unsigned) time(NULL));

    //
    // Sprite Memory Allocation and Loading
    //

    GFXInit();
    GFXLoadAllSprites();

    //
    // Creating the Interfaces
    //

    UIInterfaceStruct main_menu_interface = UIIDCreateMainMenuInterface();
    UIInterfaceStruct difficulty_select_interface = UIIDCreateDifficultySelectInterface();

    UIInterfaceStruct credits_interface = UIIDCreateCreditsInterface();
    UIInterfaceStruct next_version_interface = UIIDCreateNextVersionInterface();
    UIInterfaceStruct update_information_interface = UIIDCreateUpdateInformationInterface();

    UIInterfaceStruct pause_interface = UIIDCreatePauseInterface();
    UIInterfaceStruct lose_interface = UIIDCreateLoseInterface();
    UIInterfaceStruct main_win_interface = UIIDCreateMainWinInterface();
    UIInterfaceStruct challenge_win_interface = UIIDCreateChallengeWinInterface();
    UIInterfaceStruct boss_win_interface = UIIDCreateBossWinInterface();

    UIInterfaceStruct test_menu_interface = UIIDCreateTestMenuInterface();
    UIInterfaceStruct test_seed_input_interface = UIIDCreateTestSeedInputInterface();
    UIInterfaceStruct test_levels_interface = UIIDCreateTestLevelsInterface();
    UIInterfaceStruct test_bosses_interface = UIIDCreateTestBossesInterface();
    UIInterfaceStruct test_super_sentinels_interface = UIIDCreateTestSuperSentinelInterface();
    UIInterfaceStruct test_finished_interface = UIIDCreateTestFinishedInterface();

    UIInterfaceStruct unimplemented_interface = UIIDCreateUnimplementedInterface();

    // Default to unimplemented interface just in case
    UIInterfaceStruct* win_interface_to_use = &unimplemented_interface;

    //
    // Interface/game results
    //

    // The current choice from the interface
    int ui_choice;
    // The result from the game
    int game_result;
    // If we should exit the game
    bool exit_game = 0;
    // For the secret test menu
    u8 test_menu_secret = 0;
    //
    bool running_test = false;
    /// 0 -> normal
    /// 1 -> second stage
    /// 2 -> final stage
    int test_super_sentinel_stage = 0;
    GameStateState return_after_test = GameState_TestMenu;

    //
    // Game loop?
    // Menu loop is more correct,
    // But games are run in here as well...
    //

    do
    {
        switch (GameState.CurrentActivity)
        {
        case GameState_MainMenu:
            {
                // Back to our title screen
                dmaCopy(TitleBackgroundBitmap, bgGetGfxPtr(bg3), TitleBackgroundBitmapLen);
                dmaCopy(TitleBackgroundPal, BG_PALETTE, TitleBackgroundPalLen);

                // Hide everything
                HideEverySprite();

                // Resetting gameplay variables
                GameState.NumEnemyGroups = 1;
                GameState.Lives = GetNumLives();

                ui_choice = UIHandleInterfaceAtOffset(
                    &main_menu_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                running_test = false; // Not running a test

                switch (ui_choice)
                {
                case 0: // Play
                    {
                        // Random seed for each playthrough
                        SeedRNG();
                        GameRandomiseEnemySpawns();
                        GameState.CurrentActivity = GameState_Game;
                        break;
                    }
                case 1: // Difficulty select
                    GameState.CurrentActivity = GameState_DifficultySelectMenu;
                    break;
                case 2: // Credits
                    GameState.CurrentActivity = GameState_CreditsMenu;
                    break;
                case 3: // Next version information
                    GameState.CurrentActivity = GameState_NextVersionMenu;
                    break;
                case 4: // Update information
                    GameState.CurrentActivity = GameState_UpdateInformationMenu;
                    break;
                case 5: // Exit
                    exit_game = 1;
                    break;
                case 6: // Boss
                    {
                        // Probably does not affect things, but just in case
                        SeedRNG();
                        GameState.CurrentActivity = GameState_SuperSentinel;
                        break;
                    }
                case 7: // Challenge
                    {
                        SeedRNG();
                        GameRandomiseEnemySpawns();
                        GameState.CurrentActivity = GameState_ChallengeRound;
                        GameState.NumEnemyGroups = 4;
                    }
                    break;
                case 8: // Test menu
                    GameState.CurrentActivity = GameState_TestMenu;
                    break;
                default: // An unknown value returned, go to unimplemented interface
                    GameState.CurrentActivity = GameState_UnimplementedMenu;
                    break;
                }

                // Leave the choice on what the player last selected
                // main_menu_interface.Choice = 0;

                break;
            }

        case GameState_DifficultySelectMenu:
            {
                ui_choice = UIHandleInterfaceAtOffset(
                    &difficulty_select_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                switch (ui_choice)
                {
                case 0: // Easy
                    GameState.Difficulty = 'E';
                    GameState.Lives = -1;
                    break;
                case 1: // Medium
                    GameState.Difficulty = 'N';
                    GameState.Lives = 5;
                    break;
                case 2: // Hard
                    GameState.Difficulty = 'H';
                    GameState.Lives = 1;
                    break;
                default: // An unknown value returned, go to unimplemented interface
                    GameState.CurrentActivity = GameState_UnimplementedMenu;
                    break;
                }

                GameState.CurrentActivity = GameState_MainMenu;

                break;
            }

        case GameState_CreditsMenu:
            {
                ui_choice = UIHandleInterfaceAtOffset(
                    &credits_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                switch (ui_choice)
                {
                case 0: // Do nothing
                case 1: // Do nothing
                    break;
                case 2: // Whitespace
                    test_menu_secret |= 0b00000001;
                    break;
                case 3: // Do nothing
                case 4: // Do nothing
                    break;
                case 5: // Secret to display challenge and boss quick start
                    if (main_menu_interface.NumUIOptions < 8)
                    {
                        main_menu_interface.NumUIOptions = 8;
                    }
                    break;
                case 6: // Whitespace
                    test_menu_secret |= 0b00000010;
                    break;
                case 7: // Do nothing
                case 8: // Do nothing
                case 9: // Do nothing
                case 10: // Do nothing
                    break;
                case 11:
                    test_menu_secret |= 0b00000100;
                    break;
                case 12: // Return to the main menu
                    if (test_menu_secret != 0b00000111) { test_menu_secret = 0; }
                    GameState.CurrentActivity = GameState_MainMenu;
                    break;

                default: // An unknown value returned, go to unimplemented interface
                    GameState.CurrentActivity = GameState_UnimplementedMenu;
                    break;
                }

                // Secret option to load the test menu options
                // Must press each whitespace line in any order
                if (test_menu_secret == 0b00000111)
                {
                    main_menu_interface.NumUIOptions = 9;
                }

                break;
            }

        case GameState_Game: // Start a new game
        case GameState_ResumeGame: // Resume the game
            {
                // If starting anew run the setup
                if (GameState.CurrentActivity == GameState_Game)
                {
                    // Set the correct background
                    dmaCopy(BattleBackgroundBitmap, bgGetGfxPtr(bg3), BattleBackgroundBitmapLen);
                    dmaCopy(BattleBackgroundPal, BG_PALETTE, BattleBackgroundPalLen);

                    GameSectorSetup(
                        &GameState.Player,
                        GameState.EnemyEntityArray, 8,
                        GameState.BulletArray, MAX_BULLET_COUNT,
                        &GameState.FrameNumber,
                        &EnemiesAllEnemyData, &GFXAllSpriteGFX,
                        GameState.NumEnemyGroups
                    );
                }

                // If no setup was run, then resumed
                // Changing activity back to game
                GameState.CurrentActivity = GameState_Game;

                game_result = GameRunGameLoop(
                    &GameState.Player,
                    GameState.EnemyEntityArray, 8,
                    GameState.BulletArray, MAX_BULLET_COUNT,
                    &GameState.FrameNumber,
                    &EnemiesAllEnemyData, &GFXAllSpriteGFX,
                    GameState.PlayableArea,
                    GameState.ScreenBoarder, 4,
                    0
                );

                switch (game_result)
                {
                case -1: // Pause
                    {
                        GameState.CurrentActivity = GameState_PauseMenu;
                        GameState.ResumeAfterPause = GameState_ResumeGame;
                        break;
                    }

                case 0: // Player dies
                    {
                        // To prevent endless run after death
                        GameState.CurrentActivity = GameState_Game;

                        // Reduce life considering difficulty
                        if (GameState.Difficulty == 'N' || GameState.Difficulty == 'H')
                        {
                            GameState.Lives--;
                        }

                        // If all lives lost
                        if (GameState.Lives == 0)
                        {
                            if (running_test)
                            {
                                GameState.CurrentActivity = GameState_TestFinishedMenu;
                            }
                            else
                            {
                                GameState.CurrentActivity = GameState_LoseMenu;
                            }
                        }

                        break;
                    }

                case 1: // Player wins
                    {
                        if (running_test)
                        {
                            GameState.CurrentActivity = GameState_TestFinishedMenu;
                        }
                        else
                        {
                            // Checking for increase difficulty or win
                            if (GameState.NumEnemyGroups < 4)
                            {
                                // Add more enemies
                                GameState.NumEnemyGroups++;
                                GameRandomiseEnemySpawns();
                            }
                            else
                            {
                                // Summon the boss
                                GameState.CurrentActivity = GameState_SuperSentinel;
                            }

                            // Reset the number of lives
                            GameState.Lives = GetNumLives();
                        }

                        break;
                    }

                default: // An unknown value returned, go to unimplemented interface
                    GameState.CurrentActivity = GameState_UnimplementedMenu;
                    break;
                }

                break;
            }

        case GameState_PauseMenu:
            {
                // Reset this to reset the scanning
                UISSFResetState();

                ui_choice = UIHandleInterfaceAtOffset(
                    &pause_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                switch (ui_choice)
                {
                case 0: // Resume
                    GameState.CurrentActivity = GameState.ResumeAfterPause;
                    break;

                case 1: // Main menu or back to the test menu
                    if (running_test)
                    {
                        GameState.CurrentActivity = return_after_test;
                    }
                    else
                    {
                        GameState.CurrentActivity = GameState_MainMenu;
                    }
                    break;

                default: // An unknown value returned, go to unimplemented interface
                    GameState.CurrentActivity = GameState_UnimplementedMenu;
                    break;
                }

                pause_interface.Choice = 0;

                break;
            }

        case GameState_LoseMenu:
            {
                // ui_choice not used after here
                UIHandleInterfaceAtOffset(
                    &lose_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );
                GameState.CurrentActivity = GameState_MainMenu;
                break;
            }

        case GameState_WinMenu:
            {
                // ui_choice not used after here
                UIHandleInterfaceAtOffset(
                    win_interface_to_use,
                    &GameState.FrameNumber,
                    1,
                    1
                );
                GameState.CurrentActivity = GameState_MainMenu;
                break;
            }

        case GameState_SuperSentinel: // Start the super sentinel battle
        case GameState_ResumeSuperSentinel: // Resume from pause
            {
                if (GameState.CurrentActivity == GameState_SuperSentinel)
                {
                    // Set the correct background
                    dmaCopy(BossBackgroundBitmap, bgGetGfxPtr(bg3), BossBackgroundBitmapLen);
                    dmaCopy(BossBackgroundPal, BG_PALETTE, BossBackgroundPalLen);

                    SSSetupForGameLoop(
                        &GameState.Player,
                        GameState.EnemyEntityArray, 8,
                        GameState.BulletArray, MAX_BULLET_COUNT,
                        &GameState.FrameNumber,
                        &GFXAllSpriteGFX,
                        bg3
                    );
                }

                // Testing stuff
                if (running_test && GameState.CurrentActivity == GameState_SuperSentinel)
                {
                    switch (test_super_sentinel_stage)
                    {
                    case 1:
                        // Health 199, aka second stage
                        GameState.EnemyEntityArray[0].health = 266;
                        GameState.EnemyEntityArray[1].health = 267;
                        GameState.EnemyEntityArray[2].health = 266;
                        break;
                    case 2:
                        // Health 99, aka final stage
                        GameState.EnemyEntityArray[0].health = 233;
                        GameState.EnemyEntityArray[1].health = 233;
                        GameState.EnemyEntityArray[2].health = 233;
                        break;
                    case 0: // Do nothing
                    default:
                        break;
                    }
                }

                // Back to normal
                GameState.CurrentActivity = GameState_SuperSentinel;

                game_result = SSRunGameLoop(
                    &GameState.Player,
                    GameState.EnemyEntityArray, 8,
                    GameState.BulletArray, MAX_BULLET_COUNT,
                    &GameState.FrameNumber,
                    &GFXAllSpriteGFX,
                    GameState.PlayableArea,
                    GameState.ScreenBoarder, 4
                );

                switch (game_result)
                {
                // Pause
                case -1:
                    {
                        GameState.CurrentActivity = GameState_PauseMenu;
                        GameState.ResumeAfterPause = GameState_ResumeSuperSentinel;
                        break;
                    }

                // Player dies
                case 0:
                    {
                        // To prevent endless run after death
                        GameState.CurrentActivity = GameState_SuperSentinel;

                        // Reduce life considering difficulty
                        if (GameState.Difficulty == 'N' || GameState.Difficulty == 'H')
                        {
                            GameState.Lives--;
                        }

                        // If all lives lost
                        if (GameState.Lives == 0)
                        {
                            if (running_test)
                            {
                                GameState.CurrentActivity = GameState_TestFinishedMenu;
                            }
                            else
                            {
                                GameState.CurrentActivity = GameState_LoseMenu;
                            }
                        }

                        break;
                    }

                case 1: // Player wins
                    {
                        GameState.CurrentActivity = GameState_WinMenu; // WIN SCREEN YAY

                        // Choosing the win interface to use
                        if (running_test)
                        {
                            GameState.CurrentActivity = GameState_TestFinishedMenu;
                        }
                        else
                        {
                            if (GameState.NumEnemyGroups == 4)
                            {
                                win_interface_to_use = &main_win_interface;
                                main_menu_interface.NumUIOptions = 8;
                            }
                            else
                            {
                                win_interface_to_use = &boss_win_interface;
                            }
                        }

                        // Run the death animation
                        SSRunEndLoop(
                            &GameState.Player,
                            GameState.EnemyEntityArray, 8,
                            GameState.BulletArray, MAX_BULLET_COUNT,
                            &GameState.FrameNumber,
                            &GFXAllSpriteGFX
                        );

                        break;
                    }

                default: // An unknown value returned, go to unimplemented interface
                    {
                        GameState.CurrentActivity = GameState_UnimplementedMenu;
                        break;
                    }
                }

                break;
            }

        case GameState_ChallengeRound: // New challenge attempt
        case GameState_ResumeChallengeRound: // Resume challenge attempt
            {
                if (GameState.CurrentActivity == GameState_ChallengeRound)
                {
                    // Set the correct background
                    dmaCopy(BattleBackgroundBitmap, bgGetGfxPtr(bg3), BattleBackgroundBitmapLen);
                    dmaCopy(BattleBackgroundPal, BG_PALETTE, BattleBackgroundPalLen);

                    GameSectorSetup(
                        &GameState.Player,
                        GameState.EnemyEntityArray, 8,
                        GameState.BulletArray, MAX_BULLET_COUNT,
                        &GameState.FrameNumber,
                        &EnemiesAllEnemyData, &GFXAllSpriteGFX,
                        4
                    );
                }

                // Back to normal if resume
                GameState.CurrentActivity = GameState_ChallengeRound;

                game_result = GameRunGameLoop(
                    &GameState.Player,
                    GameState.EnemyEntityArray, 8,
                    GameState.BulletArray, MAX_BULLET_COUNT,
                    &GameState.FrameNumber,
                    &EnemiesAllEnemyData, &GFXAllSpriteGFX,
                    GameState.PlayableArea,
                    GameState.ScreenBoarder, 4,
                    1
                );

                // Processing the game result
                switch (game_result)
                {
                case -1: // Pause
                    {
                        GameState.CurrentActivity = GameState_PauseMenu;
                        GameState.ResumeAfterPause = GameState_ResumeChallengeRound;
                        break;
                    }

                case 0: // Player dies
                    {
                        // To prevent endless run after death
                        GameState.CurrentActivity = GameState_ChallengeRound;

                        // Reduce life considering difficulty
                        if (GameState.Difficulty == 'N' || GameState.Difficulty == 'H') { GameState.Lives--; }

                        // If all lives lost
                        if (GameState.Lives == 0)
                        {
                            if (running_test)
                            {
                                GameState.CurrentActivity = GameState_TestFinishedMenu;
                            }
                            else
                            {
                                GameState.CurrentActivity = GameState_LoseMenu;
                            }
                        }

                        break;
                    }

                case 1: // Player wins
                    {
                        if (running_test)
                        {
                            GameState.CurrentActivity = GameState_TestFinishedMenu;
                        }
                        else
                        {
                            // Choosing the win interface to use
                            win_interface_to_use = &challenge_win_interface;

                            // We won yay
                            GameState.CurrentActivity = GameState_WinMenu;
                        }

                        break;
                    }

                default: // An unknown value returned, go to unimplemented interface
                    {
                        GameState.CurrentActivity = GameState_UnimplementedMenu;
                        break;
                    }
                }

                break;
            }

        case GameState_NextVersionMenu:
            {
                ui_choice = UIHandleInterfaceAtOffset(
                    &next_version_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                if (ui_choice == next_version_interface.NumUIOptions - 1)
                {
                    GameState.CurrentActivity = GameState_MainMenu;
                }

                break;
            }

        case GameState_UpdateInformationMenu:
            {
                ui_choice = UIHandleInterfaceAtOffset(
                    &update_information_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                if (ui_choice == update_information_interface.NumUIOptions - 1)
                {
                    GameState.CurrentActivity = GameState_MainMenu;
                }

                break;
            }

        case GameState_TestMenu:
            {
                ui_choice = UIHandleInterfaceAtOffset(
                    &test_menu_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                switch (ui_choice)
                {
                case 0: // Seed input
                    {
                        // Doing this here to prevent doing it each time the seed is edited

                        // Load the current seed into the interface
                        int seed_len = (int) strlen(GameState.SeedString);

                        // Zero the highest order chars
                        int i;
                        for (i = 0; i < 9 - seed_len; i++)
                        {
                            test_seed_input_interface.UIOptions[i][0] = '0';
                        }
                        // Load the lower chars
                        for (; i < 9; i++)
                        {
                            test_seed_input_interface.UIOptions[i][0] = GameState.SeedString[i - 9 + seed_len];
                        }

                        GameState.CurrentActivity = GameState_TestSeedInputMenu;
                        break;
                    }
                case 1: // Levels
                    GameState.CurrentActivity = GameState_TestLevelsMenu;
                    break;
                case 2: // Bosses
                    GameState.CurrentActivity = GameState_TestBossesMenu;
                    break;
                case 3: // Return to the main menu
                    GameState.CurrentActivity = GameState_MainMenu;
                    break;
                default: // An unknown value returned, go to unimplemented interface
                    GameState.CurrentActivity = GameState_UnimplementedMenu;
                    break;
                }

                break;
            }

        case GameState_TestSeedInputMenu:
            {
                ui_choice = UIHandleInterfaceAtOffset(
                    &test_seed_input_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                if (ui_choice == test_seed_input_interface.NumUIOptions - 1)
                {
                    // Save the seed, then re-seed
                    for (int i = 0; i < 9; i++)
                    {
                        GameState.SeedString[i] = test_seed_input_interface.UIOptions[i][0];
                    }
                    GameState.SeedString[9] = '\0';
                    GameState.Seed = (int) strtol(GameState.SeedString, NULL, 10);
                    srand((unsigned) GameState.Seed);
                    itoa(GameState.Seed, GameState.SeedString, 10); // Redo this to remove any leading zeros

                    // Randomise the spawns
                    GameRandomiseEnemySpawns();

                    GameState.CurrentActivity = GameState_TestMenu;
                }
                else
                {
                    // Increment the number at the UI option index
                    // If greater than 9, then set to 0
                    char* selected_digit = test_seed_input_interface.UIOptions[ui_choice];
                    if (selected_digit[0] == '9')
                    {
                        selected_digit[0] = '0';
                    }
                    else
                    {
                        selected_digit[0]++;
                    }
                }

                break;
            }

        case GameState_TestLevelsMenu:
            {
                ui_choice = UIHandleInterfaceAtOffset(
                    &test_levels_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                running_test = true; // We are running a test
                return_after_test = GameState_TestLevelsMenu;

                switch (ui_choice)
                {
                case 0: // 2 enemies level
                    GameState.NumEnemyGroups = 1;
                    GameState.Lives = GetNumLives();
                    GameState.CurrentActivity = GameState_Game;
                    break;

                case 1: // 4 enemies level
                    GameState.NumEnemyGroups = 2;
                    GameState.Lives = GetNumLives();
                    GameState.CurrentActivity = GameState_Game;
                    break;

                case 2: // 6 enemies level
                    GameState.NumEnemyGroups = 3;
                    GameState.Lives = GetNumLives();
                    GameState.CurrentActivity = GameState_Game;
                    break;

                case 3: // 8 enemies level
                    GameState.NumEnemyGroups = 4;
                    GameState.Lives = GetNumLives();
                    GameState.CurrentActivity = GameState_Game;
                    break;

                case 4: // 8 enemies challenge level
                    // No need to set num enemy groups here as it is hardcoded in
                    GameState.Lives = GetNumLives();
                    GameState.CurrentActivity = GameState_ChallengeRound;
                    break;

                case 5: // return to the test menu
                    GameState.CurrentActivity = GameState_TestMenu;
                    break;

                default: // An unknown value returned, go to unimplemented interface
                    GameState.CurrentActivity = GameState_UnimplementedMenu;
                    break;
                }

                break;
            }

        case GameState_TestBossesMenu:
            {
                ui_choice = UIHandleInterfaceAtOffset(
                    &test_bosses_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                running_test = true; // We are running a test

                switch (ui_choice)
                {
                case 0: // Super Sentinel
                    GameState.CurrentActivity = GameState_TestSuperSentinelMenu;
                    GameState.Lives = GetNumLives();
                    break;

                case 1: // Unimplemented 1
                case 2: // Unimplemented 2
                    break;

                case 3: // Return to the test menu
                    GameState.CurrentActivity = GameState_TestMenu;
                    break;

                default: // An unknown value returned, go to unimplemented interface
                    GameState.CurrentActivity = GameState_UnimplementedMenu;
                    break;
                }

                break;
            }

        case GameState_TestSuperSentinelMenu:
            {
                ui_choice = UIHandleInterfaceAtOffset(
                    &test_super_sentinels_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                running_test = true; // We are running a test
                return_after_test = GameState_TestSuperSentinelMenu;

                switch (ui_choice)
                {
                case 0: // Initial stage
                    test_super_sentinel_stage = 0;
                    GameState.CurrentActivity = GameState_SuperSentinel;
                    break;
                case 1: // Second stage
                    test_super_sentinel_stage = 1;
                    GameState.CurrentActivity = GameState_SuperSentinel;
                    break;
                case 2: // Final stage
                    test_super_sentinel_stage = 2;
                    GameState.CurrentActivity = GameState_SuperSentinel;
                    break;
                case 3: // Return to Test Bosses Menu
                    GameState.CurrentActivity = GameState_TestBossesMenu;
                    break;
                default: // An unknown value returned, go to unimplemented interface
                    GameState.CurrentActivity = GameState_UnimplementedMenu;
                }

                break;
            }

        case GameState_TestFinishedMenu:
            {
                // ui_choice not used after here
                UIHandleInterfaceAtOffset(
                    &test_finished_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                GameState.CurrentActivity = return_after_test;

                break;
            }

        // For currently unimplemented interfaces
        // Or a fallback upon receiving an unknown menu enum value
        default:
            {
                UIHandleInterfaceAtOffset(
                    &unimplemented_interface,
                    &GameState.FrameNumber,
                    1,
                    1
                );

                GameState.CurrentActivity = GameState_MainMenu;

                break;
            }
        }
    }
    while (exit_game == 0);

    return 0;
}

/* Notes
 * Screen size is 256 × 192 pixels (4:3 aspect ratio)
 * Screen is 32 char wide and 24 lines tall
 */
