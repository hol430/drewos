#include "vga.h"
#include "idt.h"
#include "util.h"
#include "ps2.h"
#include "acpi.h"

void main() {
    clrscr();
    disable_cursor();

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

    idt_init();
    println("Interrupts successfully initialised.");

    // todo: disable usb legacy support
    // todo: init acpi
    acpi_init();
    println("ACPI successfully initialised.");

    ps2_init();

    println("\nThank you for using DrewOS!");
}
