#include "UISectorScanFunctions.h"

#include <stdlib.h>

#include "UI.h"
#include "Entity.h"
#include "Other.h"
#include "SuperSentinel.h"

struct UISSFState_s UISSFState;

void UISSFResetState()
{
    UISSFState.StartFrameNum = -1;
    UISSFState.ScanningToggle = 0;
    UISSFState.ScanningFinished = 0;
}

static void InitStartFrameNum(const int frame_number)
{
    if (UISSFState.StartFrameNum == -1)
    {
        UISSFState.StartFrameNum = frame_number;
    }
}

static void ToggleScanning30Frames(const int frame_number)
{
    // Toggle scanning if 30 frames have passed
    if ((frame_number - UISSFState.StartFrameNum) % 30 == 0)
    {
        UISSFState.ScanningToggle = !UISSFState.ScanningToggle;
    }
}

static void DisplayScanningOrScanningComplete(void)
{
    if (!UISSFState.ScanningFinished)
    {
        if (UISSFState.ScanningToggle) { UIWriteTextAtOffset("SCANNING", 7, 1); }
    }
    else
    {
        UIWriteTextAtOffset("SCAN COMPLETE", 7, 1);
    }
}

/// Displays the player hull integrity alongside some other information.
/// Increments the time to display value by 60 if the player is dead.
static void DisplayPlayerHullIntegrity(
    const int line_num,
    const Entity *player)
{
    // Display player health and the death message if needed
    UIWriteTextAtOffset("Hull integrity:", line_num, 3);
    if (player->dead)
    {
        UIWriteTextAtOffset("  0%", line_num, 25);
        UIWriteTextAtOffset("Temporal Reset", line_num + 1, 5);
        UIWriteTextAtOffset("Recommended", line_num + 2, 5);
    }
    else
    {
        UIWriteTextAtOffset("100%", line_num, 25);
    }
}

void UISSFEnemiesScanPrintFunction(
    const int frame_number,
    const char difficulty,
    const int lives,
    const Entity *player,
    const Entity enemy_entity_array[])
{
    InitStartFrameNum(frame_number);
    ToggleScanning30Frames(frame_number);
    DisplayScanningOrScanningComplete();

    const int time_since_pause = frame_number - UISSFState.StartFrameNum;

    // Spacing and delay
    int time_to_display = 60;
    int line_num = 9;

    //
    // Player health and death message
    //

    if (time_since_pause < time_to_display) { return; }

    DisplayPlayerHullIntegrity(line_num, player);

    // Spacing and delay
    if (player->dead) { line_num += 2; }
    time_to_display += 60;
    // If we are dead, increase the line number by 2 to allow space for "TEMPORAL RESET RECOMMENDED"
    line_num += 2;

    ///
    // Display the number of attempts left if necessary
    //

    if (time_since_pause < time_to_display) { return; }

    if (difficulty != 'E')
    {
        UIWriteTextAtOffset("Temporal Resets:", line_num, 3);
        char num[2];
        itoa(lives - 1, num, 10);
        UIWriteTextAtOffset(num, line_num, 27);
    }
    else
    {
        line_num -= 2;
        time_to_display -= 60;
    }

    // Spacing and delay
    line_num += 2;
    time_to_display += 60;

    //
    // The number of enemies left
    //

    if (time_since_pause < time_to_display) { return; }

    UIWriteTextAtOffset("Enemies Detected:", line_num, 3);
    int num = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (!enemy_entity_array[i].dead) { num++; }
    }

    char temp[UI_NUM_CHARS + 1];
    itoa(num, temp, 10);
    UIWriteTextAtOffset(temp, line_num, 27);

    // Delay
    time_to_display += 60;

    //
    // All done after some more delay
    //

    if (time_since_pause < time_to_display) { return; }

    UISSFState.ScanningFinished = 1;
}

void UISSFSuperSentinelScanPrintFunction(
    const int frame_number,
    const char difficulty,
    const int lives,
    const Entity *player,
    const Entity enemy_entity_array[]
)
{
    static int super_sentinel_start_indexes[26] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25
    };

    if (UISSFState.StartFrameNum == -1)
    {
        UISSFState.StartFrameNum = frame_number;
        ShuffleIntArray(super_sentinel_start_indexes, 26);
    }

    ToggleScanning30Frames(frame_number);
    DisplayScanningOrScanningComplete();

    // Spacing and delay
    const int time_since_pause = frame_number - UISSFState.StartFrameNum;
    int time_to_display = 60;
    int line_num = 9;

    //
    // Player health and death message
    //

    if (time_since_pause <= time_to_display) { return; }

    DisplayPlayerHullIntegrity(line_num, player);

    // Spacing and delay
    line_num += 2;
    if (player->dead) { line_num += 2; }
    time_to_display += 60;

    ///
    // Display the number of attempts left if necessary
    //

    if (time_since_pause <= time_to_display) { return; }

    if (difficulty != 'E')
    {
        UIWriteTextAtOffset("Temporal Resets:", line_num, 3);
        char num[2];
        itoa(lives - 1, num, 10);
        UIWriteTextAtOffset(num, line_num, 27);
    }
    else
    {
        line_num -= 2;
        time_to_display -= 60;
    }

    // Spacing and delay
    line_num += 2;
    time_to_display += 60;

    //
    // The amount of health left for the Super Sentinel
    //

    if (time_since_pause <= time_to_display) { return; }

    char temp[4];
    const int super_sentinel_health = SSGetHealth(enemy_entity_array);
    itoa(super_sentinel_health, temp, 10);

    UIWriteTextAtOffset("Enemies Detected:", line_num, 3);
    UIWriteTextAtOffset(temp, line_num, 28 - (int) strlen(temp));

    // Spacing and delay
    line_num += 2;
    time_to_display += 60;

    //
    // Custom message based on the current health of the Super Sentinel
    //

    if (time_since_pause <= time_to_display) return;

    if (super_sentinel_health > SS_LASER_HEALTH)
    {
        UIWriteTextAtOffset("Disobedience Detected", line_num, 3);
    }
    else if (super_sentinel_health > SS_FINAL_HEALTH)
    {
        UIWriteTextAtOffset("Defiance Detected", line_num, 3);
    }
    else if (super_sentinel_health > SS_CRITICAL_HEALTH)
    {
        UIWriteTextAtOffset("Insolence Detected", line_num, 3);
    }
    else
    {
        UIWriteTextAtOffset("Reporting Anomaly", line_num, 3);
    }

    // Delay
    time_to_display += 30;

    //
    // Fun little thing to clear the previous message over time
    //

    for (int j = 0; j < 6; j++)
    {
        if (time_since_pause <= time_to_display + 23 * j) break;

        if (j == 5)
        {
            UIWriteTextAtOffset("                          ", line_num, 3);
            break;
        }

        const int loopLimit = (!j) ? 6 : 5;

        for (int i = 0; i < loopLimit; i++)
        {
            const int offset = super_sentinel_start_indexes[(bool) j * 6 + (bool) j * (j - 1) * 5 + i];
            UIWriteTextAtOffset("-", line_num, 3 + offset);
        }
    }

    // Delay
    time_to_display += 30;

    //
    // Scanning finished message
    // This happens independently of the previous for loop
    // So the clearing is still happening even though it says scan finished
    //

    if (time_since_pause > time_to_display) { UISSFState.ScanningFinished = 1; }
}

void UISSFChallengeScanPrintFunction(
    const int frame_number,
    const char difficulty,
    const int lives,
    const Entity *player,
    const Entity enemy_entity_array[]
)
{
    InitStartFrameNum(frame_number);
    ToggleScanning30Frames(frame_number);
    DisplayScanningOrScanningComplete();

    const int time_since_pause = frame_number - UISSFState.StartFrameNum;

    // Spacing and delay
    int time_to_display = 60;
    int line_num = 9;

    //
    // Player health and death message
    //

    if (time_since_pause <= time_to_display) { return; }

    DisplayPlayerHullIntegrity(line_num, player);

    // Spacing and delay
    line_num += 2;
    if (player->dead) { line_num += 2; }
    time_to_display += 60;

    //
    // Display the number of attempts left if necessary
    //

    if (time_since_pause <= time_to_display) return;

    if (difficulty != 'E')
    {
        UIWriteTextAtOffset("Temporal Resets:", line_num, 3);
        char num[2];
        itoa(lives - 1, num, 10);
        UIWriteTextAtOffset(num, line_num, 27);
    }
    else
    {
        line_num -= 2;
        time_to_display -= 60;
    }

    // Spacing and delay
    line_num += 2;
    time_to_display += 60;

    //
    // The number of enemies left
    //

    if (time_since_pause <= time_to_display) { return; }

    char temp[UI_NUM_CHARS + 1];

    UIWriteTextAtOffset("Enemies Detected:", line_num, 3);
    int num = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (!enemy_entity_array[i].dead) { num++; }
    }
    itoa(num, temp, 10);
    UIWriteTextAtOffset(temp, line_num, 27);

    // Spacing and delay
    line_num += 2;
    time_to_display += 60;

    //
    // Some custom text for the death bullets
    //

    if (time_since_pause <= time_to_display) { return; }

    UIWriteTextAtOffset("Explosive Enemy Finale?", line_num, 3);

    //
    // All done after some more delay
    //

    time_to_display += 60;

    if (time_since_pause > time_to_display) { UISSFState.ScanningFinished = 1; }
}
