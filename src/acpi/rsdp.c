#include <stdbool.h>
#include <stdint.h>

#include "vga.h"
#include "util.h"

#include "rsdp.h"

#define EBDA_BASE_ADDRESS 0x040e

// EBDA is 1KiB.
#define EBDA_SIZE 1024

// Start of main bios area.
#define BIOS_MAIN_START 0x000E0000

// End of main bios area.
#define BIOS_MAIN_END 0x000FFFFF

// The RSDP is always aligned to this value.
#define RSDP_ALIGN 16

typedef struct {
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;
    uint32_t rsdt_address;
} __attribute__((packed)) rsdp_t;

typedef struct {
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;
    uint32_t rsdt_address; // deprecated -- see xsdt_address

    uint32_t length;
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

rsdp_t *rsdp_search(void *start, void *end) {
    rsdp_t *r;

    // for (r = start; r < (rsdp_t *)end; r += RSDP_ALIGN) {
    for (uint8_t *i = start; i < (uint8_t *)end; i += RSDP_ALIGN) {
        r = (rsdp_t *)i;

        if (validate_rsdp(r)) {
            return r;
        }
    }
    return 0;
}

bool locate_rsdp() {
    uint16_t *ebda_ptr = (uint16_t *)EBDA_BASE_ADDRESS;
    uint32_t ebda_start = (*ebda_ptr) << 4;
    uint32_t ebda_end = ebda_start + EBDA_SIZE;

    if ( (rsdp = rsdp_search((void *)ebda_start, (void *)ebda_end)) ) {
        return true;
    }

    if ( (rsdp = rsdp_search((void *)BIOS_MAIN_START, (void *)BIOS_MAIN_END)) ) {
        return true;
    }

    return false;
}

bool locate_xsdp() {
    if (xsdp) {
        return true;
    }
    return false;
}
