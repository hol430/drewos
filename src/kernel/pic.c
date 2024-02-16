#include <stdint.h>

#include "pic.h"
#include "low_level.h"

// PIC1 = PIC_MASTER
// PIC2 = PIC_SLAVE

#define PIC_MASTER_COMMAND 0x0020
#define PIC_MASTER_DATA 0x0021
#define PIC_SLAVE_COMMAND 0x00a0
#define PIC_SLAVE_DATA 0x00a1

// End-of-interrupt command code.
#define PIC_EOI 0x20

// Indicates that ICW4 will be present.
#define ICW1_ICW4 0x01

// Single (cascade) mode.
#define ICW1_SINGLE 0x02

// Call address interval 4 (8).
#define ICW1_INTERVAL4 0x04

// Level triggered (edge) mode.
#define ICW1_LEVEL 0x08

// Initialisation - required!
#define ICW1_INIT 0x10

// 8086/88 (MCS-80/85 mode).
#define ICW4_8086 0x01

// Auto (normal EOI).
#define ICW4_AUTO 0x02

// Buffered mode/slave.
#define ICW4_BUF_SLAVE 0x08

// Buffered mode/master.
#define ICW4_BUF_MASTER 0x0c

// Special fully nested (not).
#define ICW4_SFNM 0x10

// Command to read the Interrupt Request Register (IRR), which tells us which
// interrupts have been raised.
#define PIC_READ_IRR 0x0a

// Command to read the In-Service Register (ISR), which tells us which
// interrupts are being serviced.
#define PIC_READ_ISR 0x0b

/*
Send an end-of-interrupt command to the PIC.
*/
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        write_byte(PIC_SLAVE_COMMAND, PIC_EOI);
    }
    write_byte(PIC_MASTER_COMMAND, PIC_EOI);
}

/*
Re-initialise the PIC controllers, giving them the specified vector offsets.

@param offset1: Vector offset for the master PIC (offset1..offset1+7).
@param offset2: Vector offset for the slave PIC (offset2..offset2+7).
*/
void pic_remap(int offset1, int offset2) {
    // The io_wait() calls may be necessary on older machines to give the PIC
    // some time to react to commands, as they might not be processed quickly.

    uint8_t mask_master = read_byte(PIC_MASTER_DATA);
    uint8_t mask_slave = read_byte(PIC_SLAVE_DATA);

    // ICW1: Initialisation.

    // Start the initialisation sequence (in cascade mode).
    write_byte(PIC_MASTER_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    write_byte(PIC_SLAVE_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // ICW2: Vector offset.

    // Write master PIC vector offset.
    write_byte(PIC_MASTER_DATA, offset1);
    io_wait();

    // Write slave PIC vector offset.
    write_byte(PIC_SLAVE_DATA, offset2);
    io_wait();

    // ICW3: Master/slave wiring configuration.

    // Tell master PIC that there is a slave PIC at IRQ2 (0000 0100).
    write_byte(PIC_MASTER_DATA, 4);
    io_wait();

    // Tell slave PIC its cascade identity (0000 0010).
    write_byte(PIC_SLAVE_DATA, 2);
    io_wait();

    // ICW4: Additional information about the environment.

    // Have the PICs use 8086 mode (not 8080 mode).
    write_byte(PIC_MASTER_DATA, ICW4_8086);
    io_wait();

    write_byte(PIC_SLAVE_DATA, ICW4_8086);
    io_wait();

    // Restore the saved masks.
    write_byte(PIC_MASTER_DATA, mask_master);
    write_byte(PIC_SLAVE_DATA, mask_slave);
}

void pic_disable() {
    write_byte(PIC_MASTER_DATA, 0xff);
    write_byte(PIC_SLAVE_DATA, 0xff);
}

void irq_set_mask(uint8_t irq_line) {
    uint16_t port;

    if (irq_line < 8) {
        port = PIC_MASTER_DATA;
    } else {
        port = PIC_SLAVE_DATA;
        irq_line -= 8;
    }

    uint8_t value = read_byte(port) | (1 << irq_line);
    write_byte(port, value);
}

void irq_clear_mask(uint8_t irq_line) {
    uint16_t port;

    if (irq_line < 8) {
        port = PIC_MASTER_DATA;
    } else {
        port = PIC_SLAVE_DATA;
        irq_line -= 8;
    }

    uint8_t value = read_byte(port) & ~(1 << irq_line);
    write_byte(port, value);
}

static uint16_t pic_get_irq_reg(int ocw3) {
    // OCW3 to PIC CMD to get the register values. The slave PIC is chained, and
    // represents IRQs 8-15. The master PIC is IRQs 0-7, with 2 being the chain.
    write_byte(PIC_MASTER_COMMAND, ocw3);
    write_byte(PIC_SLAVE_COMMAND, ocw3);

    uint8_t slave_irq = read_byte(PIC_SLAVE_COMMAND);
    uint8_t master_irq = read_byte(PIC_MASTER_COMMAND);
    return (slave_irq << 8) | master_irq;
}

uint16_t pic_get_irr() {
    return pic_get_irq_reg(PIC_READ_IRR);
}

uint16_t pic_get_isr() {
    return pic_get_irq_reg(PIC_READ_ISR);
}
