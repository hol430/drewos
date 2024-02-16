#include "vga.h"
#include "idt.h"
#include "util.h"

void main() {
    clrscr();

    println("Kernel has been loaded successfully.");
    cprintln("Welcome to drewOS!", GREEN, BLACK);
    println("");
    const int NCOLOUR = 16;
    for (int i = 0; i < NCOLOUR; i++) {
        // colour_t fg = i % 16;
        colour_t bg = i % 16;
        cprint("     ", bg, bg);
    }
    println("\n");

    println("Initialising interrupts...");
    idt_init();
    println("Interrupts successfully initialised.");

    println("Thank you for using DrewOS!");
}
