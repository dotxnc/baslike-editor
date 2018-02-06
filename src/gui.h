/*
VERY largely inspired by raysan5's Raygui to the point where
90% of the code is just directly taken from his library.

This is closed source tho and Raygui is under zlib so I guess
this text doesn't really matter.
*/

#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include <raylib.h>

typedef enum GUISTATE {
    DISABLED,
    NORMAL,
    FOCUSED,
    PRESSED
} GUISTATE;

void gui_label(const char*, int, int, int, int);
bool gui_button(const char*, int, int, int, int);
bool gui_toggle(const char*, int, int, int, int, bool);
int gui_tgroup(char**, int, int, int, int, int, int);
void gui_textbox(char*, int, int, int, int, int, bool*);

#endif
