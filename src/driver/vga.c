#include <stdint.h>

#include "vga.h"
#include "low_level.h"
#include "util.h"

// Screen device I/O ports.
#define REG_CTRL 0x3d4
#define REG_DATA 0x3d5

// Number of columns available in VGA colour text mode.
#define NCOL 80

// Number of rows available in VGA colour text mode.
#define NROW 25

// Start of video memory in VGA colour text mode.
#define VIDEO_MEMORY 0xb8000

// Current screen coordinates. Note: this is not threadsafe!
static uint8_t x, y;

void clrscr() {
    char *buf = (void *)VIDEO_MEMORY;
    const int NVID_BUF = NROW * NCOL * 2;

    for (int i = 0; i < NVID_BUF; i += 2) {
        *(buf++) = (char)' ';
        *(buf++) = (char)0;
    }
    x = 0;
    y = 0;
}

void print_coords();

void print_offset();

/*
Convert screen coordinates to a buffer offset.

Offset can go up to 4000 (2 * 80 * 25), so uint8_t is too small for this.
*/
uint16_t get_offset(uint8_t x, uint8_t y) {
    return 2 * (x + y * NCOL);
}

/*
Get the address in video memory representing the given coordinates.
*/
char *get_address(uint8_t x, uint8_t y) {
    return get_offset(x, y) + (char *)VIDEO_MEMORY;
}

/*
Get the current offset of the cursor from the start of VIDEO_MEMORY.
*/
uint16_t get_cursor() {
    // The device uses its control register as an index to select its internal
    // registers, of which we are interested in:
    // - reg 14: which is the high byte of the cursor's offset
    // - reg 15: which is the low byte of the cursor's offset.
    // Once the internal register has been selected, we may read or write a byte
    // on the data register.

    // Select register 14.
    write_byte(REG_CTRL, 0x0e);

    // Read the selected register. This is the high byte, so left bitshift by 8.
    uint16_t offset = read_byte(REG_DATA) << 8;

    // Select register 15.
    write_byte(REG_CTRL, 0x0f);

    // Read the selected register. This is the low byte.
    offset += read_byte(REG_DATA);

    // The cursor offset reported is the number of characters, so we must
    // multiply this by two to get the cell offset in the buffer.
    return offset * 2;
}

/*
Set the location of the cursor.

Cursor will appear iff something has been written to this offset in the buffer.

@param offset: Offset in characters from the start of VIDEO_MEMORY.
*/
void set_cursor(uint16_t offset) {
    // This is similar to get_cursor(), except that we need to write bytes to
    // those same registers.

    // Select register 15.
    write_byte(REG_CTRL, 0x0f);

    // Write the low byte.
    write_byte(REG_DATA, (uint8_t)(offset & 0xff) );

    // Select register 14.
    write_byte(REG_CTRL, 0x0e);

    // Write the high byte.
    write_byte(REG_DATA, (uint8_t)((offset >> 8) & 0xff));
}

/*
Set the location of the cursor to the given screen coordinates.
*/
void set_cursor_screen(uint8_t x, uint8_t y) {
    uint16_t offset = get_offset(x, y) % (NCOL * NROW * 2);
    set_cursor(offset);
}

void enable_cursor(uint8_t start, uint8_t end) {
    write_byte(REG_CTRL, 0x0a);
    start |= read_byte(REG_DATA) & 0xc0;
    write_byte(REG_DATA, start);

    write_byte(REG_CTRL, 0x0b);
    end |= read_byte(REG_DATA) & 0xe0;
    write_byte(REG_DATA, end);
}

void disable_cursor() {
    // Select register 0x0a.
    write_byte(REG_CTRL, 0x0a);

    // Bits 6-7 unused, bit 5 disables the cursor, bits 0-4 control the shape.
    // 00100000
    write_byte(REG_DATA, 0x20);
}

/*
Ensure that the current x and y coordinates point to a cell on the screen (ie
within the first NROW rows. If not, scroll all existing text up by 1 row and
empty the bottom row of text.
*/
void handle_scrolling() {
    if (y < NROW) {
        return;
    }

    // Move all rows back by 1.
    for (uint8_t i = 1; i < NROW; i++) {
        char *src = get_address(0, i);
        char *dst = get_address(0, i - 1);
        copy_memory(src, dst, NCOL * 2);
    }

    // Empty the last line.
    char *prv = get_address(0, NROW - 1);
    for (uint8_t i = 0; i < NCOL * 2; i += 2) {
        prv[i] = ' ';
        prv[i + 1] = 0;
    }

    x = 0;
    y = NROW - 1;
}

/*
Write the specified character to the screen at the current x/y coordinates.
*/
void write_char(char c, colour_t fg, colour_t bg) {
    if (c == '\n') {
        // Writing a newline character at bottom of screen shouldn't necessarily
        // cause the screen contents to scroll. We can do that if and when we
        // actually attempt to write a printable character past the last row.
        y++;
        x = 0;
        return;
    } else if (c == '\r') {
        x = 0;
        return;
    }

    // TODO: handle other non-printing characters?

    // Ensure we're printing onto the screen, and scroll existing text buffer
    // up 1 row if not.
    handle_scrolling();

    char *buf = get_address(x, y);
    (*buf++) = c;
    (*buf++) = (bg << 4) + fg;

    // Get the offset in number of characters from the start of the buffer.
    uint16_t offset = (buf - (char *)VIDEO_MEMORY) / 2;

    // Move the cursor to this location.
    set_cursor(offset - 1);

    // Update x/y globals.
    y = offset / NCOL;
    x = offset % NCOL;

    // Handle scrolling if necessary.
    handle_scrolling();
}

void cprint(const char *msg, colour_t fg, colour_t bg) {
    if (!msg) {
        return/* 0*/;
    }

    while (*msg) {
        write_char(*msg++, fg, bg);
    }
}

void cprintln(const char *msg, colour_t fg, colour_t bg) {
    cprint(msg, fg, bg);
    // print_offset();
    write_char('\n', fg, bg);
}

void println(const char *msg) {
    cprintln(msg, WHITE, BLACK);
}

void print(const char *msg) {
    cprint(msg, WHITE, BLACK);
}

void print_offset() {
    uint16_t offset = get_offset(x, y);
    print(" (offset = ");
    // Maximum value of x is 80, which requires 2 digits + NULL terminator.
    // Largest 32-bit int requires 11 decimal digits (if negative).
    const uint8_t BUFSIZE = 16;
    char buf[BUFSIZE];
    uint16_t res = itoa(offset, buf, BUFSIZE);
    if (res == 0) {
        print(buf);
    }

    print(")");
}

void print_coords() {
    uint16_t offset = get_offset(x, y);
    print(" (");
    // Maximum value of x is 80, which requires 2 digits + NULL terminator.
    // Largest 32-bit int requires 11 decimal digits (if negative).
    const uint8_t BUFSIZE = 16;
    char buf[BUFSIZE];
    uint16_t res = itoa(x, buf, BUFSIZE);
    if (res == 0) {
        print(buf);
    }

    print(", ");

    res = itoa(y, buf, BUFSIZE);
    if (res == 0) {
        print(buf);
    }

    print(", ");
    res = itoa(offset, buf, BUFSIZE);
    if (res == 0) {
        print(buf);
    }

    print(")");
}
