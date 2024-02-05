#include "vga.h"

void main() {
    // char *video_memory = (char *)0xb8000;
    // *video_memory = 'x';
    // *(video_memory + 1) = 0x0f;
    clrscr();
    // *((int*)0xb8320)=0x07690748;
    print("Welcome to drewOS!");
    print("Kernel has been loaded successfully.");
}
