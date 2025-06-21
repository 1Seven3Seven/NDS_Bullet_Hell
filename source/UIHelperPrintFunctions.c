#include "UIHelperPrintFunctions.h"

#include <string.h>

#include "UI.h"

void UIHPFPrintSeed(const int line_num, const char *seed_string)
{
    UIWriteText(
        "Seed: ",
        line_num
    );
    UIWriteTextAtOffset(
        seed_string,
        line_num,
        6
    );
}

void UIHPFPrintDifficulty(const int line_num, const char difficulty)
{
    UIWriteText(
        "Difficulty:",
        line_num
    );
    switch (difficulty)
    {
        case 'E':
            UIWriteTextAtOffset(
                "Easy",
                line_num,
                12
            );
            break;
        case 'N':
            UIWriteTextAtOffset(
                "Normal",
                line_num,
                12
            );
            break;
        case 'H':
            UIWriteTextAtOffset(
                "Hard",
                line_num,
                12
            );
            break;
        default:
            UIWriteTextAtOffset(
                "Unknown",
                line_num,
                12
            );
    }
}

void UIHPFPrintVersion(const int line_num, const int char_offset, const int right_align, const char *version_string)
{
    const size_t len = strlen(version_string);
    if (right_align)
    {
        int true_offset = UI_NUM_CHARS - len - char_offset;
        if (true_offset < 9)
            true_offset = 9;
        UIWriteTextAtOffset("Version:", line_num, true_offset - 12);
        UIWriteTextAtOffset(version_string, line_num, true_offset);
    } else
    {
        UIWriteTextAtOffset("Version:", line_num, char_offset);
        UIWriteTextAtOffset(version_string, line_num, char_offset + 12);
    }
}
