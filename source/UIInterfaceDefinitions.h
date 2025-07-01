#ifndef BULLET_HELL_UI_INTERFACE_DEFINITIONS
#define BULLET_HELL_UI_INTERFACE_DEFINITIONS

#include "UI.h"

//
// Main menu and difficulty select
//

///
UIInterfaceStruct UIIDCreateMainMenuInterface(void);

///
UIInterfaceStruct UIIDCreateDifficultySelectInterface(void);

//
// Informational menus
//

///
UIInterfaceStruct UIIDCreateCreditsInterface(void);

///
UIInterfaceStruct UIIDCreateNextVersionInterface(void);

///
UIInterfaceStruct UIIDCreateUpdateInformationInterface(void);

//
// Game related menus
//

///
UIInterfaceStruct UIIDCreatePauseInterface(void);

///
UIInterfaceStruct UIIDCreateLoseInterface(void);

///
UIInterfaceStruct UIIDCreateMainWinInterface(void);

///
UIInterfaceStruct UIIDCreateChallengeWinInterface(void);

///
UIInterfaceStruct UIIDCreateBossWinInterface(void);

//
// Test menus
//

/// Creates the main testing interface providing access to all sub testing interfaces.
UIInterfaceStruct UIIDCreateTestMenuInterface(void);

/// Creates the testing interface allowing one to quickly jump to each level in the game.
UIInterfaceStruct UIIDCreateTestLevelsInterface(void);

/// Creates the testing interface providing access to each individual bosses testing interface.
UIInterfaceStruct UIIDCreateTestBossesInterface(void);

/// Creates the testing interface for the Super Sentinel allowing one to quickly jump to each stage of the boss fight.
UIInterfaceStruct UIIDCreateTestSuperSentinelInterface(void);

/// Creates the testing interface for the Super Shredder allowing one to quickly jump to each stage of the boss fight.
UIInterfaceStruct UIIDCreateTestSuperShredderInterface(void);

/// Creates the testing interface used when a game-related test is finished.
/// A game is considered finished when either the player loses all lives or kills all enemies.
UIInterfaceStruct UIIDCreateTestFinishedInterface(void);

/// Creates the testing interface to hardcode a seed value for tests.
UIInterfaceStruct UIIDCreateTestSeedInputInterface(void);

//
// Unimplemented interface
//

///
UIInterfaceStruct UIIDCreateUnimplementedInterface(void);

#endif
