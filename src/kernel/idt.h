#ifndef _DREWOS_IDT_H_
#define _DREWOS_IDT_H_

#include <stdint.h>

typedef struct {
    // The lower 16 bits of the ISR's address.
    uint16_t isr_low;

    // The GDT segment selector that the CPU will load into CS before calling
    // the ISR.
    uint16_t kernel_cs;

    // Set to zero.
    uint8_t reserved;

    // Type and attributes.
    uint8_t attributes;

    // The higher 16 bits of the ISR's address.
    uint16_t isr_high;

} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags);

void idt_init();

#endif // _DREWOS_IDT_H_
