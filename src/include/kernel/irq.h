#pragma once

#include <stdint.h>

#define PIC1 0x20 /* master PIC */
#define PIC2 0xA0 /* slave PIC */
#define PIC1_DATA (PIC1 + 1)
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI 0x20 /* end of interrupt */
#define IRQ_BASE 0x20

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34

/* see __int.asm */
extern void irq0();
extern void irq1();
extern void irq2();

void init_irq();
void irq_set_handler(uint16_t irq, void(*handler));