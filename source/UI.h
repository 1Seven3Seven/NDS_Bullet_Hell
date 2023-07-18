#ifndef BULLET_HELL_UI_H
#define BULLET_HELL_UI_H

// For UIInitInterface
#include <stdarg.h>

// Constants for the amount of characters that can be displayed

#define UI_NUM_CHARS 32 // The width of the ui in characters - non-inclusive of '\0'
#define UI_NUM_LINES 24 // The height of the ui in lines

// A UI struct
// Contains the choices of the user and the currently selected choice
// The choices need to be allocated
typedef struct {
    // The name of the interface
    char *Name;

    // An array of pointers to the strings representing the ui options
    char **UIOptions;

    // The number of options in UIOptions
    int NumUIOptions;

    // The separation amount in lines between the ui options
    // Default 0, must be set manually `some_ui.Separation = x;`
    int Separation;

    // The currently selected choice
    int Choice;

    // Toggles the select >
    int Toggle;

    // The offset in frames for the toggle
    int ToggleOffset;

    // The previous states of the up and down keys
    int PreUpState, PreDownState;

    // If a choice was selected
    int ChoiceSelected;

} UIInterfaceStruct;

// An array of strings representing each line on the lower screen
extern char UIDisplayBuffer[UI_NUM_LINES][UI_NUM_CHARS + 1];

// Resets the display buffer to just spaces ' ' and null terminators at the end
// Should be run before editing the display buffer
void UIResetDisplayBuffer();

// Prints the display buffer
void UIPrintDisplayBuffer();

// Writes the given text to the display buffer on the given line
// The line number must be between 0 and 23, inclusive
//      If this isn't the case then nothing is written
// Ignores everything over 32 chars long
// Doesn't write the null terminator
//      This WILL break if there is no null terminator
void UIWriteText(const char *string, int line_number);

// Does the same as UIWriteText but at an offset
// The max length will now be  32 - char_offset
void UIWriteTextAtOffset(const char *string, int line_number, int char_offset);

// Initialises the interface to have num_options amount of options
// After num_options there should be num_options amount of strings representing the options
void UIInitInterface(UIInterfaceStruct *interface, const char *name, int num_options, ...);

// Does what UIDisplayInterface does at the given offsets
void UIDisplayInterfaceAtOffset(UIInterfaceStruct *interface, int line_start, int char_offset);

// Writes the interface to the display buffer starting at line number line_start
// Toggles the select > every 30 frames
void UIDisplayInterface(UIInterfaceStruct *interface);

// Updates the interface by changing the currently selected choice according to keys
// Returns 1 if a choice was selected, 0 otherwise
//      There is a 16 frame delay between selecting a choice and returning 1, for the select animation
int UIUpdateInterface(UIInterfaceStruct *interface, int frame_number, int keys);

// Does what UIHandleInterfaceAtOffset but with a function intended to print extra information
int UIHandleInterfaceAtOffsetWithFunction(UIInterfaceStruct *interface, int *frame_number, int line_start,
                                          int char_offset, void (*print_function)(void));

// Does what UIHandleInterface does at a given offset
int UIHandleInterfaceAtOffset(UIInterfaceStruct *interface, int *frame_number, int line_start, int char_offset);

// Repeatedly calls display and update interface till an option is selected
// Returns the index of the chosen option
int UIHandleInterface(UIInterfaceStruct *interface, int *frame_number);

#endif // BULLET_HELL_UI_H