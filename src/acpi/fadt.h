#ifndef _DREWOS_FADT_H_
#define _DREWOS_FADT_H_

#include <stdint.h>
#include <stdbool.h>

#include "rsdt.h"

/*
Locate and initialise the FADT.
*/
void fadt_init();

/*
Switch to ACPI mode.
*/
void acpi_enable();

/*
Check whether a PS2 controller exists.
*/
bool ps2_controller_exists();

#endif // _DREWOS_FADT_H_
