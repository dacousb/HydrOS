#include <kernel/irq.h>
#include <kernel/idt.h>
#include <kernel/ports.h>
#include <task/sched.h>

void (*irq_handlers[256])(); /* different handlers (eg. kb) are stored here */

void init_irq()
{
    port_byte_out(PIC1, 0x11);
    port_byte_out(PIC2, 0x11);

    port_byte_out(PIC1_DATA, IRQ_BASE);
    port_byte_out(PIC2_DATA, IRQ_BASE + 8);

    port_byte_out(PIC1_DATA, 0x04);
    port_byte_out(PIC2_DATA, 0x02);

    port_byte_out(PIC1_DATA, 0x01);
    port_byte_out(PIC2_DATA, 0x01);

    port_byte_out(PIC1_DATA, 0x00);
    port_byte_out(PIC2_DATA, 0x00);

    idt_set_entry(32, (uint64_t)irq0);
    idt_set_entry(33, (uint64_t)irq1);
    idt_set_entry(34, (uint64_t)irq2);

    asm("sti"); /* set the interrupt flag */
}

void irq_handler(uint64_t irq, regs_t *regs)
{
    if (irq >= 40)
        port_byte_out(PIC2, PIC_EOI);

    if (irq == 32)
        schedule(regs);

    void (*handler)() = irq_handlers[irq];

    if (handler)
        handler();

    port_byte_out(PIC1, PIC_EOI);
}

void irq_set_handler(uint16_t irq, void(*handler))
{
    irq_handlers[irq] = handler;
}