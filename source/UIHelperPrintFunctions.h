#ifndef BULLET_HELL_UI_ADDITIONAL_FUNCTIONS_H
#define BULLET_HELL_UI_ADDITIONAL_FUNCTIONS_H

///
/// PREFIX: UIHPF
///
/// Holds declarations for helper print functions for the UI
///

//

/// Prints the seed at the given line number
void UIHPFPrintSeed(int line_num, const char *seed_string);

/// Prints the difficulty at the line number
void UIHPFPrintDifficulty(int line_num, char difficulty);

/// Prints the version at the line number, aligned to the right if specified
void UIHPFPrintVersion(int line_num, int char_offset, int right_align, const char *version_string);

#endif
