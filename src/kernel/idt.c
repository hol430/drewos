#include <stdint.h>

#include "idt.h"

#define MAX_DESCRIPTORS 256

// Array of IDT entries aligned for performance.
__attribute__((aligned(0x10)))
static idt_entry_t idt[MAX_DESCRIPTORS];

static idtr_t idtr;
extern void *isr_stub_table[];

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
    idt_entry_t *descriptor = &idt[vector];

    descriptor->isr_low = (uint32_t)isr & 0xffff;

    // The code segment, as defined in the GDT.
    descriptor->kernel_cs = 0x08;

    descriptor->attributes = flags;
    descriptor->isr_high = (uint32_t)isr >> 16;
    descriptor->reserved = 0;
}

void idt_init() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8e);
    }

    // Load the new IDT.
    __asm__ volatile ("lidt %0" : : "m"(idtr));

    // Set the interrupt flag.
    __asm__ volatile ("sti");
}
