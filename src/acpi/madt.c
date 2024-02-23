#include <stdint.h>
#include <stdbool.h>

#include "madt.h"
#include "rsdt.h"

#include "vga.h"

typedef struct {
    uint8_t entry_type;
    uint8_t length;
} __attribute__((packed)) madt_entry_header_t;

typedef struct {
    acpi_sdt_header_t *header;

    // Local APIC address.
    uint32_t apic_address;

    // Flags (1 = Dual 8259 Legacy PICs installed).
    uint32_t flags;
} __attribute__((packed)) madt_t;

const char MADT_SIGNATURE[SDT_HEADER_LEN] = "APIC";

static madt_t *madt = 0x00;

void madt_init() {
    madt = (madt_t *)get_sdt(MADT_SIGNATURE);

    // TODO: better error handling.
    if (!madt) {
        println("Failed to locate MADT");
    }
}
