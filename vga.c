#include <stdint.h>

#include "vga.h"

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

void print(const char *msg) {
    char *buf = (char *)VIDEO_MEMORY;
    while (msg) {
        *(buf++) = *(msg++);
        *(buf++) = WHITE_ON_BLACK;
    }
}

void println(const char *msg) {
    char *buf = (char *)VIDEO_MEMORY + 2 * (x + y * NCOL) * sizeof(char);
    while (msg) {
        *(buf++) = *(msg++);
        *(buf++) = WHITE_ON_BLACK;
    }
    int offset = (buf - (char *)VIDEO_MEMORY) / 2;
    y = 1 + offset / NCOL;
    x = 0;
}
