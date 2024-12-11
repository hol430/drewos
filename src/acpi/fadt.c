#include <stdint.h>
#include <stdbool.h>

#include "fadt.h"
#include "rsdt.h"

#include "vga.h"
#include "util.h"
#include "low_level.h"

typedef struct {
    acpi_sdt_header_t header;

    uint32_t firmware_ctrl;
    uint32_t dsdt;
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t  reserved;
 
    uint8_t  preferred_power_management_profile;
    uint16_t sci_interrupt;
    uint32_t smi_command_port;
    uint8_t  acpi_enable;
    uint8_t  acpi_disable;
    uint8_t  s4_bios_req;
    uint8_t  pstate_control;
    uint32_t pm1a_event_block;
    uint32_t pm1b_event_block;
    uint32_t pm1a_control_block;
    uint32_t pm1b_control_block;
    uint32_t pm2_control_block;
    uint32_t pm_timer_block;
    uint32_t gpe0_block;
    uint32_t gpe1_block;
    uint8_t  pm1_event_length;
    uint8_t  pm1_control_length;
    uint8_t  pm2_control_length;
    uint8_t  pm_timer_length;
    uint8_t  gpe0_length;
    uint8_t  gpe1_length;
    uint8_t  gpe1_base;
    uint8_t  c_state_control;
    uint16_t worst_c2_latency;
    uint16_t worst_c3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t  duty_offset;
    uint8_t  duty_width;
    uint8_t  day_alarm;
    uint8_t  month_alarm;
    uint8_t  century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t boot_architecture_flags;
 
    uint8_t  reserved2;
    uint32_t flags;
 
    // 12 byte structure; see below for details
    address_t reset_reg;
 
    uint8_t  reset_value;
    uint8_t  reserved3[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    uint64_t                x_firmware_control;
    uint64_t                x_dsdt;
 
    address_t x_pm1a_event_block;
    address_t x_pm1b_event_block;
    address_t x_pm1a_control_block;
    address_t x_pm1b_control_block;
    address_t x_pm2c_ontrol_block;
    address_t x_pm_timer_block;
    address_t x_gpe0_block;
    address_t x_gpe1_block;
} __attribute__((packed)) fadt_t;

const char FADT_SIGNATURE[SDT_HEADER_LEN] = "FACP";

static fadt_t *fadt = 0x00;

void fadt_init() {
    fadt = (fadt_t *)get_sdt(FADT_SIGNATURE);

    // TODO: better error handling.
    if (!fadt) {
        println("Failed to locate FADT");
    }
}

void acpi_enable() {
    write_byte(fadt->smi_command_port, fadt->acpi_enable);
    while ( (read_word(fadt->pm1a_control_block) & 1) == 0);
}

bool ps2_controller_exists() {
    if (!fadt) {
        // System doesn't support ACPI.
        return true;
    }

    if (fadt->header.revision < 2) {
        // FADT <2.0, assume PS/2 controller exists.
        return true;
    }

    // The IA boot architecture flags are only used in FADT >= 2.0.
    // If bit 2 is set, we have a PS/2 controller.
    return (fadt->boot_architecture_flags & 2) == 2;
}
