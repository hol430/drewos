#include <stdbool.h>

#include "acpi.h"
#include "rsdp.h"
#include "fadt.h"

#include "vga.h"

void acpi_init() {
    init_rsdp();

    // Switch to ACPI mode. Once this happens, power events will generate SCIs,
    // rather than SMIs.
    acpi_enable();
}
