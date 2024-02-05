#include <stdint.h>

#include "vga.h"
#include "util.h"

#define WHITE_ON_BLACK 0x0f
#define BLACK_ON_BLACK 0x00
#define CHAR_NULL 0x00

#define NCOL 80
#define NROW 25

#define VIDEO_MEMORY 0xb8000
static int x, y;

void clrscr() {
    char *buf = (void *)VIDEO_MEMORY;
    const int NVID_BUF = NROW * NCOL * 2;

    for (int i = 0; i < NVID_BUF; i += 2) {
        *(buf++) = (char)BLACK_ON_BLACK;
        *(buf++) = (char)CHAR_NULL;
    }
    x = 0;
    y = 0;
}

void print_coords(int offset);

void write(const char *msg) {
    if (!msg) {
        return/* 0*/;
    }

    char *buf = (char *)VIDEO_MEMORY + 2 * (x + y * NCOL) * sizeof(char);
    while (*msg) {
        *(buf++) = *(msg++);
        *(buf++) = WHITE_ON_BLACK;
    }
    int offset = (buf - (char *)VIDEO_MEMORY) / 2;
    y = offset / NCOL;
    x = offset % NCOL;
    // return offset;
}

void print(const char *msg) {
    write(msg);
}

void println(const char *msg) {
    write(msg);
    y++;
    x = 0;
}

void print_coords(int offset) {
    write(" (");
    // Maximum value of x is 80, which requires 2 digits + NULL terminator.
    // Largest 32-bit int requires 11 decimal digits (if negative).
    const uint16_t BUFSIZE = 16;
    char buf[BUFSIZE];
    uint16_t res = itoa(x, buf, BUFSIZE);
    if (res == 0) {
        write(buf);
    }

    write(", ");

    res = itoa(y, buf, BUFSIZE);
    if (res == 0) {
        write(buf);
    }

    write(", ");
    res = itoa(offset, buf, BUFSIZE);
    if (res == 0) {
        write(buf);
    }

    write(")");
}
