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

///
UIInterfaceStruct UIIDCreateTestMenuInterface(void);

///
UIInterfaceStruct UIIDCreateTestLevelsInterface(void);

///
UIInterfaceStruct UIIDCreateTestBossesInterface(void);

///
UIInterfaceStruct UIIDCreateTestMenusInterface(void);

//
// Unimplemented interface
//

///
UIInterfaceStruct UIIDCreateUnimplementedInterface(void);

#endif
