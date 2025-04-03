#include <stdint.h>

#include "isr.h"
#include "vga.h"
#include "pic.h"

void exception_handler() {
    println("INTERRUPT stub");
    // __asm__ volatile("cli; hlt"); // hang
}
