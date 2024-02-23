#ifndef _DREWOS_RSDT_H_
#define _DREWOS_RSDT_H_

#include <stdint.h>
#include <stdbool.h>

#define SDT_HEADER_LEN 4

typedef struct {
  char signature[SDT_HEADER_LEN];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char oem_id[6];
  char oem_table_id[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
} __attribute__((packed)) acpi_sdt_header_t;

typedef struct {
  uint8_t address_space;
  uint8_t bit_width;
  uint8_t bit_offset;
  uint8_t access_size;
  uint64_t address;
} __attribute__((packed)) address_t;

typedef struct {
    acpi_sdt_header_t header;
    uint32_t *tables;
} __attribute__((packed)) rsdt_t;

typedef struct {
    acpi_sdt_header_t header;
    uint64_t *tables;
} __attribute__((packed)) xsdt_t;

/*
Return true iff the SDT header has a valid checksum.
*/
bool validate_checksum(const acpi_sdt_header_t *header);

/*
Get the SDT with the specified signature, if one exists with a valid signature
and checksum. If none is found, NULL will be returned.
*/
acpi_sdt_header_t *get_sdt(const char signature[SDT_HEADER_LEN]);

void rsdt_init(rsdt_t *rsdt, xsdt_t *xsdt);

#endif // _DREWOS_RSDT_H_
