#include <stdint.h>
#include <kernel/idt.h>

/* remember that IDT stands for Interrupt Descriptor Table
   the CPU will use this to know what to do when an interrupt is fired */

__attribute__((aligned(0x10))) /* alignment is a matter of performance */
static idt_entry_t idt[256];   /* define a new IDT with 256 gates/entries */
static idtr_t idtr;

/* this function will be called to define the ISRs and IRQs */
void idt_set_entry(uint16_t n, uint64_t handler)
{
    idt[n].isr_low = (uint16_t)handler;
    idt[n].isr_mid = (uint16_t)(handler >> 16);
    idt[n].isr_hig = (uint32_t)(handler >> 32);
    idt[n].kernel_cs = 0x08; /* this is the <kernel code> selector offset, see GDT */
    idt[n].attributes = (1 << 7) /* present */ | (1 << 5) /* ring 0 */ | 0xE /* interrupt */;
}

void init_idt()
{
    idtr.base = (uint64_t)&idt;
    idtr.size = 256 * sizeof(idt_entry_t) - 1;
    asm("lidt %0" /* see idt.h */
        :
        : "m"(idtr));
}