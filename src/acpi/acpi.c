#include <stdbool.h>

#include "acpi.h"
#include "vga.h"
#include "rsdp.h"

void acpi_init() {
    init_rsdp();
}
