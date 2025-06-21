#include "UIInterfaceDefinitions.h"

#include "UIHelperPrintFunctions.h"
#include "UISectorScanFunctions.h"
#include "Constants.h"
#include "GameState.h"

//
// Main menu and difficulty select
//

static void MainMenuPrintFunc(void)
{
    UIHPFPrintVersion(22, 0, false, VERSION);
    UIHPFPrintDifficulty(23, GameState.Difficulty);
}

UIInterfaceStruct UIIDCreateMainMenuInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Main Menu",
        8,
        "Play",
        "Difficulty Select",
        "Credits",
        "Next version Ideas",
        "Update Information",
        "Exit",
        "Boss Quick Start",
        "Challenge"
    );
    ui.Separation = 1;
    // I should not really do this as it comes with the repercussion that any function that interacts with this will be
    //  given the incorrect number of ui options.
    // I am doing it to hide the boss and challenge options until you beat the game normally.
    // This should not cause any problems with how I use the struct and because I do not un-initialise them.
    ui.NumUIOptions = 6;
    ui.PrintFunction = &MainMenuPrintFunc;

    return ui;
}

static void DifficultySelectPrintFunc(void)
{
    UIWriteTextAtOffset(
        "Infinite attempts",
        4,
        8
    );
    UIWriteTextAtOffset(
        "Five attempts",
        7,
        8
    );
    UIWriteTextAtOffset(
        "One attempt",
        10,
        8
    );
}

UIInterfaceStruct UIIDCreateDifficultySelectInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Difficulty Select",
        3,
        "Easy",
        "Normal",
        "Hard"
    );
    ui.Choice = 1; // Default normal
    ui.Separation = 2;
    ui.PrintFunction = &DifficultySelectPrintFunc;

    return ui;
}

//
// Informational menus
//

UIInterfaceStruct UIIDCreateCreditsInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
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
        "    Nelole",
        "    Dennis",
        "",
        "Return to Main Menu"
    );
    ui.Choice = ui.NumUIOptions - 1; // Selected option on return to the main menu

    return ui;
}

UIInterfaceStruct UIIDCreateNextVersionInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Next Version Ideas",
        13,
        "More boss enemies.",
        "  At least 2 more.",
        "Saving progress.",
        "Sound effects.",
        "Keeping track of stats.",
        "Aka a scoreboard of:",
        "  Enemy kills.",
        "  Player deaths.",
        "A tutorial could be cool.",
        "Custom seed input.",
        "Tank-ier player?",
        "",
        "Return to Main Menu"
    );
    ui.Choice = ui.NumUIOptions - 1; // Selected option on return to the main menu

    return ui;
}

UIInterfaceStruct UIIDCreateUpdateInformationInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Update Information",
        3,
        "Hello, World!",
        "",
        "Return to Main Menu"
    );
    ui.Choice = ui.NumUIOptions - 1;

    return ui;
}

//
// Game related menus
//

static void PauseScreenPrintFunc(void)
{
    switch (GameState.ResumeAfterPause)
    {
        case GameState_ResumeGame: // Resume normal game
            UISSFEnemiesScanPrintFunction(
                GameState.FrameNumber,
                GameState.Difficulty,
                GameState.Lives,
                &GameState.Player,
                GameState.EnemyEntityArray
            );
            break;

        case GameState_ResumeSuperSentinel: // Resume boss fight
            UISSFSuperSentinelScanPrintFunction(
                GameState.FrameNumber,
                GameState.Difficulty,
                GameState.Lives,
                &GameState.Player,
                GameState.EnemyEntityArray
            );
            break;

        case GameState_ResumeChallengeRound: // Resume challenge game
            UISSFChallengeScanPrintFunction(
                GameState.FrameNumber,
                GameState.Difficulty,
                GameState.Lives,
                &GameState.Player,
                GameState.EnemyEntityArray
            );
            break;

        default:
            UIWriteTextAtOffset("Something broke", 10, 3);
    }
}

UIInterfaceStruct UIIDCreatePauseInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Temporal Disturbance Detected",
        2,
        "Resume",
        "Abort"
    );
    ui.PrintFunction = &PauseScreenPrintFunc;

    return ui;
}

static void PrintDifficultyAndSeedFunc(void)
{
    UIHPFPrintDifficulty(22, GameState.Difficulty);
    UIHPFPrintSeed(23, GameState.SeedString);
}

UIInterfaceStruct UIIDCreateLoseInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Temporal Reset Failed",
        1,
        "Return to Main Menu"
    );
    ui.PrintFunction = &PrintDifficultyAndSeedFunc;

    return ui;
}

UIInterfaceStruct UIIDCreateMainWinInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Main Game Completed Well Done",
        1,
        "Return to Main Menu"
    );
    ui.PrintFunction = &PrintDifficultyAndSeedFunc;

    return ui;
}

UIInterfaceStruct UIIDCreateChallengeWinInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Challenge Completed Well Done",
        1,
        "Return to Main Menu"
    );

    return ui;
}

UIInterfaceStruct UIIDCreateBossWinInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Boss Defeated",
        1,
        "Return to Main Menu"
    );

    return ui;
}

//
// Test menus
//

UIInterfaceStruct UIIDCreateTestMenuInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Test Menu",
        5,
        "Levels",
        "Bosses",
        "Menus",
        "",
        "Return to Main Menu"
    );
    ui.Choice = ui.NumUIOptions - 1;

    return ui;
}

UIInterfaceStruct UIIDCreateTestLevelsInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Test Levels",
        9,
        "Seed Input",
        "",
        "2 Enemies",
        "4 Enemies",
        "6 Enemies",
        "8 Enemies",
        "8 Enemies Challenge",
        "",
        "Return to Test Menu"
    );
    ui.Choice = ui.NumUIOptions - 1;

    return ui;
}

UIInterfaceStruct UIIDCreateTestBossesInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Test Levels",
        3,
        "Hello, World!",
        "",
        "Return to Test Menu"
    );
    ui.Choice = ui.NumUIOptions - 1;

    return ui;
}

UIInterfaceStruct UIIDCreateTestMenusInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Test Levels",
        3,
        "Hello, World!",
        "",
        "Return to Test Menu"
    );
    ui.Choice = ui.NumUIOptions - 1;

    return ui;
}

//
// Unimplemented interface
//

UIInterfaceStruct UIIDCreateUnimplementedInterface(void)
{
    UIInterfaceStruct ui;
    UIInitInterface(
        &ui,
        "Unimplemented Interface",
        1,
        "Return to Main Menu"
    );

    return ui;
}
