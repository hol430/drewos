#ifndef _DREWOS_PIC_H_
#define _DREWOS_PIC_H_

#include <stdint.h>

void pic_send_eoi(uint8_t irq);
void pic_remap(int offset1, int offset2);
uint8_t pic_get_vector(uint8_t irq);
void pic_init();
void pic_disable();
void irq_set_mask(uint8_t irq_line);
void irq_clear_mask(uint8_t irq_line);
uint16_t pic_get_irr();
uint16_t pic_get_isr();

#endif // _DREWOS_PIC_H_
