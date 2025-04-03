#include <stdint.h>

#include "idt.h"
#include "vga.h"
#include "pic.h"

#define MAX_DESCRIPTORS 256

#define IDT_DESCRIPTOR_X16_INTERRUPT	0x06
#define IDT_DESCRIPTOR_X16_TRAP 		0x07
#define IDT_DESCRIPTOR_X32_TASK 		0x05
#define IDT_DESCRIPTOR_X32_INTERRUPT  	0x0E
#define IDT_DESCRIPTOR_X32_TRAP			0x0F
#define IDT_DESCRIPTOR_RING1  			0x40
#define IDT_DESCRIPTOR_RING2  			0x20
#define IDT_DESCRIPTOR_RING3  			0x60
#define IDT_DESCRIPTOR_PRESENT			0x80

#define IDT_DESCRIPTOR_EXCEPTION		(IDT_DESCRIPTOR_X32_INTERRUPT | IDT_DESCRIPTOR_PRESENT)
#define IDT_DESCRIPTOR_EXTERNAL			(IDT_DESCRIPTOR_X32_INTERRUPT | IDT_DESCRIPTOR_PRESENT)
#define IDT_DESCRIPTOR_CALL				(IDT_DESCRIPTOR_X32_INTERRUPT | IDT_DESCRIPTOR_PRESENT | IDT_DESCRIPTOR_RING3)

// Array of IDT entries aligned for performance.
__attribute__((aligned(0x10)))
static idt_entry_t idt[MAX_DESCRIPTORS];

static idtr_t idtr;
extern void *isr_stub_table[];
extern void generic_isr_stub(); // interrupts.asm

void *handlers[MAX_DESCRIPTORS];

void generic_handler(uint32_t vector);

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
        void *isr = (vector != 8 && vector != 10 && vector != 11 && vector != 12 && vector != 13 && vector != 14 && vector != 17) ? (void *)(uintptr_t)generic_handler : isr_stub_table[vector];
        idt_set_descriptor(vector, isr, 0x8e);
    }

    // Load the new IDT.
    __asm__ volatile ("lidt %0" : : "m"(idtr));

    // Set the interrupt flag.
    __asm__ volatile ("sti");
}

void idt_install_irq_handler(uint8_t vector, void (*handler)()) {
    handlers[vector] = (void *)(uintptr_t)handler;
    idt_set_descriptor(vector, (void *)(uintptr_t)generic_isr_stub, IDT_DESCRIPTOR_EXTERNAL);
}

void idt_uninstall_irq_handler(uint8_t vector) {
    // idt_set_descriptor(vector, isr_stub_table[vector], IDT_DESCRIPTOR_EXTERNAL);
    handlers[vector] = 0;
}

void generic_handler(uint32_t vector) {
    uint16_t isr = pic_get_isr();
    uint16_t irr = pic_get_irr();
    println("INTERRUPT %d (%x), isr = %x, irr = %x", vector, vector, isr, irr);
    if (handlers[vector] != 0) {
        println("A handler has been attached to vector %x and will be invoked.");
        void (*handler)() = (void (*)())(uintptr_t)handlers[vector];
        handler();
    }
    // switch (vector) {
    //     case 0x21: // IRQ1 - Keyboard
    //         keyboard_handler();
    //         break;
    //     case 0x20: // IRQ0 - Timer
    //         timer_handler();
    //         break;
    //     default:
    //         printf("Unhandled interrupt: %d\n", vector);
    //         break;
    // }
}
