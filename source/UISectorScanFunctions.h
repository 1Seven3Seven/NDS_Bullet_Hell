#ifndef BULLET_HELL_UI_SECTOR_SCAN_FUNCTIONS_H
#define BULLET_HELL_UI_SECTOR_SCAN_FUNCTIONS_H

#include "Entity.h"

///
/// PREFIX: UISSF
///
/// Holds declarations for functions used for scanning sectors
///

//

struct UISSFState_s
{
    /// The frame in which the scanning function was started.
    int StartFrameNum;
    /// Bool used to determine if the string "SCANNING" should be displayed.
    /// Toggled every 30 frames when a scanning function is running.
    int ScanningToggle;
    /// Bool used to change the "SCANNING" string to "SCAN COMPLETE" once the scan has, well, completed.
    int ScanningFinished;
};

extern struct UISSFState_s UISSFState;

/// To be called after or before calling a scan function in a loop.
/// Resets some global shared state.
void UISSFResetState();

/// Scan function to be used when facing a normal set of enemies.
void UISSFEnemiesScanPrintFunction(
    int frame_number,
    char difficulty,
    int lives,
    const Entity *player,
    const Entity enemy_entity_array[]
);

/// Scan function to be used when facing the Super Sentinel.
void UISSFSuperSentinelScanPrintFunction(
    int frame_number,
    char difficulty,
    int lives,
    const Entity *player,
    const Entity enemy_entity_array[]
);

///
void UISSFChallengeScanPrintFunction(
    int frame_number,
    char difficulty,
    int lives,
    const Entity *player,
    const Entity enemy_entity_array[]
);

#endif
