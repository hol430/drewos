#include <stdint.h>
#include <stdbool.h>

#include "rsdt.h"

#include "fadt.h"
#include "madt.h"

#include "vga.h"

// Pointer to the RSDT/XSDT. Only one of these will be non-null.
static rsdt_t *rsdt = 0x00;
static xsdt_t *xsdt = 0x00;

/*
Return true iff the SDT header contains a valid checksum.
*/
bool validate_checksum(const acpi_sdt_header_t *header) {
    unsigned char checksum = 0;
    for (uint32_t i = 0; i < header->length; i++) {
        checksum += ((char *)header)[i];
    }
    return checksum == 0;
}

/*
Return true iff the SDT header contains a signature matching the expected signature.
*/
bool validate_signature(const acpi_sdt_header_t *header, const char signature[SDT_HEADER_LEN]) {
    for (int i = 0; i < SDT_HEADER_LEN; i++) {
        if (header->signature[i] != signature[i]) {
            return false;
        }
    }
    return true;
}

/*
Get the number of tables in the RSDT (or XSDT if using ACPI >= v2).
*/
static uint32_t get_num_tables() {
    // TODO: is sizeof(uintptr_t) correct here? Unsure if we can rely on its
    // width.
    if (rsdt) {
        return (rsdt->header.length - sizeof(acpi_sdt_header_t)) / sizeof(uintptr_t);
    }
    if (xsdt) {
        return (xsdt->header.length - sizeof(acpi_sdt_header_t)) / sizeof(uintptr_t);
    }

    return 0;
}

/*
Get the i-th table in the RSDT (or XSDT if using ACPI >= v2).
*/
static acpi_sdt_header_t *get_table(uint32_t i) {
    if (rsdt) {
        uint32_t *tables = (uint32_t *)((char *)rsdt + sizeof(acpi_sdt_header_t));
        acpi_sdt_header_t *header = (acpi_sdt_header_t *)tables[i];
        return header;
    }

    if (xsdt) {
        // Note this will need to be revised if we move to 64-bit. Not sure if
        // uintptr_t is correct here.
        uint64_t *tables = (uint64_t *)((char *)xsdt + sizeof(acpi_sdt_header_t));
        acpi_sdt_header_t *header = (acpi_sdt_header_t *)(uintptr_t)tables[i];
        return header;
    }

    return 0;
}

acpi_sdt_header_t *get_sdt(const char signature[SDT_HEADER_LEN]) {
    uint32_t n = get_num_tables();

    for (uint32_t i = 0; i < n; i++) {
        acpi_sdt_header_t *header = get_table(i);

        if (validate_checksum(header) && validate_signature(header, signature)) {
            return header;
        }
    }
    return 0;
}

void rsdt_init(rsdt_t *r, xsdt_t *x) {
    rsdt = r;
    xsdt = x;

    // TODO: better error handling.

    if (rsdt && !validate_checksum(&rsdt->header)) {
        println("Invalid RSDT checksum");
        return;
    }

    if (xsdt && !validate_checksum(&xsdt->header)) {
        println("Invalid XSDT checksum");
        return;
    }

    fadt_init();
    madt_init();
}
