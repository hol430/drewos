#include <stdbool.h>

#include "acpi.h"
#include "vga.h"
#include "rsdp.h"

void acpi_init() {
    if (locate_rsdp()) {
        println("RSDP located successfully.");
    } else {
        println("Failed to locate RSDP.");
    }
}
