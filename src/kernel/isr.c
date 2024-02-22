#include <stdint.h>

#include "isr.h"
#include "vga.h"

void exception_handler() {
    println("INTERRUPT");
}
