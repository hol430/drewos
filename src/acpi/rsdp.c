#include <stdbool.h>
#include <stdint.h>

#include "vga.h"
#include "util.h"

#include "rsdp.h"
#include "rsdt.h"

#include "fadt.h"
#include "madt.h"

// A 2-byte pointer to the EBDA (>>4) is stored at this address.
#define EBDA_BASE_ADDRESS 0x040e

// EBDA is 1KiB.
#define EBDA_SIZE 1024

// Start of main bios area.
#define BIOS_MAIN_START 0x000E0000

// End of main bios area.
#define BIOS_MAIN_END 0x000FFFFF

// The RSDP is always aligned to this value.
#define RSDP_ALIGN 16

// RSDP (Root System Description Pointer). Use this struct only under ACPI v1.
typedef struct {
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;

    // Physical address of the RSDT. Deprecated under version 2 - must use XSDP
    // instead.
    uint32_t rsdt_address;
} __attribute__((packed)) rsdp_t;

// Extended RSDT. Must use this under ACPI version 2.
typedef struct {
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;

    // deprecated -- use xsdt_address instead.
    uint32_t rsdt_address;

    uint32_t length;

    // Physical address of the XSDT. Use this under all architectures, but cast
    // to uint32_t under IA-32.
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed)) xsdp_t;

// An RSDP is only valid if its signature matches this value.
const char RSDP_SIGNATURE[8] = "RSD PTR ";

// Pointer to RSDP.
static rsdp_t *rsdp = 0x00;

// Pointer to the XSDP.
static xsdp_t *xsdp = 0x00;

// Pointer to the SSDT.
// static ssdt_t *ssdt = 0x00;

static bool validate_rsdp(const rsdp_t *rsdp) {
    // Validate the signature.
    if (*(uint64_t *)rsdp->signature != *(uint64_t *)RSDP_SIGNATURE) {
        return false;
    }

    // Validate the checksum.
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < sizeof(rsdp_t); i++) {
        checksum += *((char *)rsdp + i);
    }

    return checksum == 0;
}

/*
Search for a valid RSDP pointer between the start and end addresses.
*/
static rsdp_t *sdp_search(void *start, void *end) {
    rsdp_t *r;

    // Align start to nearest multiple of RSDP_ALIGN. In theory, this shouldn't
    // be necessary.
    // if ((uintptr_t)start % RSDP_ALIGN != 0) {
    //     start = ((uintptr_t)start / RSDP_ALIGN + 1) * (uintptr_t)RSDP_ALIGN;
    // }

    for (uint8_t *i = start; i < (uint8_t *)end; i += RSDP_ALIGN) {
        r = (rsdp_t *)i;

        if (validate_rsdp(r)) {
            return r;
        }
    }
    return 0;
}

/*
Search standard locations for the RSDP. Return true if found, false otherwise.
*/
static void *locate_sdp() {
    uint16_t *ebda_ptr = (uint16_t *)EBDA_BASE_ADDRESS;
    uint32_t ebda_start = (*ebda_ptr) << 4;
    uint32_t ebda_end = ebda_start + EBDA_SIZE;

    void *sdp;
    if ( (sdp = sdp_search((void *)ebda_start, (void *)ebda_end)) ) {
        return sdp;
    }

    if ( (sdp = sdp_search((void *)BIOS_MAIN_START, (void *)BIOS_MAIN_END)) ) {
        return sdp;
    }

    return false;
}

/*
Locate the RSDP/XSDP.
*/
void init_rsdp() {
    // Attempt to locate the SDP.
    void *sdp = locate_sdp();

    if (!sdp) {
        println("Failed to locate valid RSDP/XSDP signature");
        return;
    }

    // If the RSDP version field is 0, then ACPI version 1.0 is used. For
    // subsequent versions (ACPI 2.0 to 6.1), the value 2 is used. The exact
    // ACPI version can be deduced from the FADT.
    rsdt_t *rsdt = 0x00;
    xsdt_t *xsdt = 0x00;
    if (((rsdp_t *)sdp)->revision == 2) {
        rsdp = 0x00;
        xsdp = (xsdp_t *)sdp;
        xsdt = (xsdt_t *)(uintptr_t)xsdp->xsdt_address;
    } else {
        rsdp = (rsdp_t *)sdp;
        xsdp = 0x00;
        rsdt = (rsdt_t *)rsdp->rsdt_address;
    }

    rsdt_init(rsdt, xsdt);
}
