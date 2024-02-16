#include <stdint.h>

#include "isr.h"

__attribute__((noreturn))
void exception_handler() {
    while (1) {
        __asm__ volatile("cli; hlt");
    }
}
