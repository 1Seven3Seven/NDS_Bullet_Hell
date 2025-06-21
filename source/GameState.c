#include "GameState.h"

_GameState GameState = {
    // Default of normal difficulty
    .Difficulty = 'N',
    .FrameNumber = 0,

    // The screen size in pixels
    .PlayableArea = {0, 0, 256, 192},
    .ScreenBoarder = {
        {0, 0, 8, 192},
        {0, 0, 256, 8},
        {0, 184, 256, 8},
        {248, 0, 8, 192}
    },

    // Not strictly necessary to set the seed as it is set in the main function
    .Seed = 0,
    // This is necessary though
    .SeedString = "12345678901234567890",

    .NumEnemyGroups = 1,
    // Should start on the main menu
    .CurrentActivity = GameState_MainMenu,
};
