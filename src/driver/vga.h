#ifndef _DREWOS_VGA_H_
#define _DREWOS_VGA_H_

#include <stdint.h>

// Colours usable in VGA colour text mode.
typedef enum {
    BLACK        = 0,
    BLUE         = 1,
    GREEN        = 2,
    CYAN         = 3,
    RED          = 4,
    PURPLE       = 5,
    BROWN        = 6,
    GRAY         = 7,
    DARK_GRAY    = 8,
    LIGHT_BLUE   = 9,
    LIGHT_GREEN  = 10,
    LIGHT_CYAN   = 11,
    LIGHT_RED    = 12,
    LIGHT_PURPLE = 13,
    YELLOW       = 14,
    WHITE        = 15
} colour_t;

void clrscr();

/*
Print a message in the specified colour with a newline at the end.

@param msg: The text to be printed.
@param fg: Foreground colour of the text.
@param bg: Background colour of the text.
*/
void cprintln(const char *msg, colour_t fg, colour_t bg);

/*
Print a message.

@param msg: The text to be printed.
@param fg: Foreground colour of the text.
@param bg: Background colour of the text.
*/
void cprint(const char *msg, colour_t fg, colour_t bg);

/*
Print a message with default colours and a newline at the end.

@param msg: The text to be printed.
*/
void println(const char *msg);

/*
Print a message with default colours (white on black).

@param msg: The text to be printed.
*/
void print(const char *msg);

/*
Enable the blinking cursor.

@param start: Start position of the cursor.
@param end: End position of the cursor.
*/
void enable_cursor(uint8_t start, uint8_t end);

/*
Disable the blinking cursor.
*/
void disable_cursor();

#endif // _DREWOS_VGA_H_
