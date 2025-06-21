#include "UI.h"

#include <nds.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h> // For UIInitInterface

#include "Other.h"

char UIDisplayBuffer[UI_NUM_LINES][UI_NUM_CHARS + 1];

void UIResetDisplayBuffer()
{
    for (int i = 0; i < UI_NUM_LINES; ++i)
    {
        memset(UIDisplayBuffer[i], ' ', UI_NUM_CHARS);
        UIDisplayBuffer[i][UI_NUM_CHARS] = '\0';
    }
}

void UIPrintDisplayBuffer()
{
    for (int i = 0; i < UI_NUM_LINES; i++)
    {
        iprintf("%s", UIDisplayBuffer[i]);
    }
}

void UIWriteText(const char *string, const int line_number)
{
    if (line_number < 0 || line_number > 23)
    {
        return;
    }

    size_t len = strlen(string);

    if (len > 32)
    {
        len = 32;
    }

    // Use `memcpy` instead of `strcpy` to avoid copying the null terminator
    memcpy(UIDisplayBuffer[line_number], string, len);
}

void UIWriteTextAtOffset(const char *string, const int line_number, const int char_offset)
{
    if (line_number < 0 || line_number > 23)
    {
        return;
    }

    size_t len = strlen(string);

    if (len > 32 - char_offset)
    {
        len = 32 - char_offset;
    }

    memcpy(UIDisplayBuffer[line_number] + char_offset, string, len);
}

void UIInitInterface(UIInterfaceStruct *interface, const char *name, const int num_options, ...)
{
    interface->Name = malloc(strlen(name) + 1);
    strcpy(interface->Name, name);

    interface->NumUIOptions = num_options;

    interface->Separation = 0;

    interface->Choice = 0;

    interface->Toggle = 0;
    interface->ToggleOffset = 0;

    interface->PreUpState = 0;
    interface->PreDownState = 0;

    interface->ChoiceSelected = 0;

    interface->UIOptions = malloc(sizeof(char *) * num_options);

    // Grabbing all the ui options
    va_list list;
    va_start(list, num_options);
    for (int i = 0; i < num_options; ++i)
    {
        // Get the next string
        const char *temp_option_ptr = va_arg(list, char *);
        // Get the length
        const size_t temp_option_len = strlen(temp_option_ptr);
        // Allocate space
        interface->UIOptions[i] = malloc(temp_option_len + 1);
        // Copy the option
        strcpy(interface->UIOptions[i], temp_option_ptr);
    }
    va_end(list);

    interface->PrintFunction = 0;
}

void UIDisplayInterfaceAtOffset(UIInterfaceStruct *interface, const int line_start, const int char_offset)
{
    UIWriteTextAtOffset(
        interface->Name,
        line_start,
        char_offset
    );

    if (interface->Toggle)
    {
        UIWriteTextAtOffset(
            ">",
            interface->Choice + line_start + 2 + interface->Choice * interface->Separation,
            char_offset
        );
    }

    for (int i = 0; i < interface->NumUIOptions; ++i)
    {
        UIWriteTextAtOffset(
            interface->UIOptions[i],
            line_start + i + 2 + i * interface->Separation,
            char_offset + 2
        );
    }
}

void UIDisplayInterface(UIInterfaceStruct *interface)
{
    UIDisplayInterfaceAtOffset(interface, 0, 0);
}

static void UpdateInterfaceQuery(
    UIInterfaceStruct *interface,
    const int frame_number,
    const u32 up_key,
    const u32 down_key)
{
    if (!((frame_number - interface->ToggleOffset) % 30))
    {
        interface->Toggle = !interface->Toggle;
    }

    int movement = 0;
    int change = 0;

    if (up_key)
    {
        if (!interface->PreUpState)
        {
            movement--;
            interface->PreUpState = 1;
            change = 1;
        }
    }
    else
    {
        interface->PreUpState = 0;
    }

    if (down_key)
    {
        if (!interface->PreDownState)
        {
            movement++;
            interface->PreDownState = 1;
            change = 1;
        }
    }
    else
    {
        interface->PreDownState = 0;
    }

    if (change)
    {
        interface->ToggleOffset = frame_number % 30;
        interface->Choice = (int) modulo(interface->Choice + movement, interface->NumUIOptions);
        interface->Toggle = 1;
    }
}

static int UpdateInterfaceSelected(UIInterfaceStruct *interface, const int frame_number)
{
    if (!((frame_number - interface->ToggleOffset) % 4))
    {
        interface->Toggle = !interface->Toggle;
    }

    if (frame_number - interface->ToggleOffset == 16)
    {
        return 1;
    }
    return 0;
}

int UIUpdateInterface(
    UIInterfaceStruct *interface,
    const int frame_number,
    const u32 keys)
{
    if (keys & KEY_A && !interface->ChoiceSelected)
    {
        interface->ChoiceSelected = 1;
        interface->Toggle = 1;
        interface->ToggleOffset = frame_number;
    }

    if (!interface->ChoiceSelected)
    {
        UpdateInterfaceQuery(
            interface,
            frame_number,
            keys & KEY_UP,
            keys & KEY_DOWN
        );
    }
    else
    {
        const int result = UpdateInterfaceSelected(interface, frame_number);
        if (result)
        {
            interface->ChoiceSelected = 0;
            return 1;
        }
    }

    return 0;
}

int HandleInterfaceDefault(
    UIInterfaceStruct *interface,
    const u32 keys,
    const int frame_number,
    const int line_start,
    const int char_offset)
{
    const int ui_result = UIUpdateInterface(
        interface,
        frame_number,
        keys
    );

    UIDisplayInterfaceAtOffset(
        interface,
        line_start,
        char_offset
    );

    return ui_result;
}

int UIHandleInterfaceAtOffset(
    UIInterfaceStruct *interface,
    int *frame_number,
    const int line_start,
    const int char_offset)
{
    while (1)
    {
        consoleClear();
        scanKeys();
        const u32 keys = keysHeld();
        (*frame_number)++;

        UIResetDisplayBuffer();

        const int ui_result = HandleInterfaceDefault(interface, keys, *frame_number, line_start, char_offset);

        if (interface->PrintFunction != 0)
        {
            interface->PrintFunction();
        }

        UIPrintDisplayBuffer();

        // Waiting
        swiWaitForVBlank();
        // Update the screen
        oamUpdate(&oamMain);
        // Exit
        if (ui_result)
        {
            break;
        }
    }

    return interface->Choice;
}

int UIHandleInterface(UIInterfaceStruct *interface, int *frame_number)
{
    return UIHandleInterfaceAtOffset(
        interface,
        frame_number,
        0, 0
    );
}
