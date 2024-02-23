#ifndef _DREWOS_FADT_H_
#define _DREWOS_FADT_H_

#include <stdint.h>
#include <stdbool.h>

#include "rsdt.h"

/*
Locate and initialise the FADT.
*/
void fadt_init();

#endif // _DREWOS_FADT_H_
