#ifndef BULLET_HELL_GAME_STATE_H
#define BULLET_HELL_GAME_STATE_H

#include <time.h>

#include "Constants.h"
#include "Bullet.h"
#include "Entity.h"

/// An enum with values for each state the game can be in.
typedef enum _GameStateStates_e
{
    //
    // Main Menu States
    // Not just the Main Menu, just the main states for the menus
    //

    GameState_MainMenu,
    GameState_DifficultySelectMenu,

    GameState_CreditsMenu,
    GameState_NextVersionMenu,
    GameState_UpdateInformationMenu,

    GameState_PauseMenu,
    GameState_LoseMenu,
    GameState_WinMenu,
    // MainWinMenu,
    // ChallengeWinMenu,
    // BossWinMenu,

    GameState_UnimplementedMenu,

    //
    // Test Menu States
    //

    GameState_TestMenu,

    GameState_TestSeedInputMenu,

    GameState_TestLevelsMenu,

    GameState_TestBossesMenu,
    GameState_TestSuperSentinelMenu,
    GameState_TestSuperShredderMenu,

    /// Game State for when a test game (normal or boss-related) is finished.
    GameState_TestFinishedMenu,

    GameState_TestMenusMenu,

    //
    // Game States
    //

    GameState_Game,
    GameState_ResumeGame,

    GameState_ChallengeRound,
    GameState_ResumeChallengeRound,

    //
    // Boss States
    //

    GameState_SuperSentinel,
    GameState_ResumeSuperSentinel,

    GameState_SuperShredder,
    GameState_ResumeSuperShredder,
} GameStateState;

/// A collection of variables regarding the current state of the game.
/// Should only be used in `main.c`, but has a little use in the UI Interface Definitions to allow for more information
///  to be printed in some of them.
///
/// I suppose it would be better to say it should only be modified in `main.c`, but I do not want to rely on global
///  variables, so saying used is good.
typedef struct _GameState_s
{
    //---------------------------------------------------------------------------------
    // Bullets, player and enemies
    //---------------------------------------------------------------------------------

    /// The array containing all the bullets.
    Bullet BulletArray[MAX_BULLET_COUNT];
    /// Player.
    Entity Player;
    /// Array containing the enemies.
    Entity EnemyEntityArray[8];

    //---------------------------------------------------------------------------------
    // Miscellaneous
    //---------------------------------------------------------------------------------

    int FrameNumber; // Take a guess

    /// The playable area, equivalent to the screen.
    /// Bullets are deleted when exiting this.
    int PlayableArea[4];

    /// The hitboxes for the boarder of the screen.
    /// Can be more, but I don't want more.
    int ScreenBoarder[4][4];

    /// The Seed.
    int Seed;

    /// The Seed in string form.
    /// 9 chars long because we % by 1,000,000,000.
    char SeedString[10];

    // Numbers that are known to produce bugs
    // Seed = 1668036031; srand((unsigned) Seed); // Two miners spawned in the same place with 1 set of enemies
    // Seed = 1668446882; srand((unsigned) Seed); // Two shredders spawned in the same place with 2 sets of enemies

    /// The difficulty, duh.
    ///
    /// E = Easy.
    /// N = Normal.
    /// H = Hard.
    char Difficulty;

    /// The number of enemy groups to spawn in.
    int NumEnemyGroups;

    /// Lives.
    /// Positive numbers = num lives left.
    /// -1 = infinite.
    int Lives;

    /// The current activity to perform.
    GameStateState CurrentActivity;

    /// What the current activity should be set to after a pause is resumed
    GameStateState ResumeAfterPause;
} _GameState;

extern _GameState GameState;

#endif
