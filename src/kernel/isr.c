#include <kernel/isr.h>
#include <kernel/idt.h>
#include <string/string.h>

const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"};

void init_isr()
{
    idt_set_entry(0, (uint64_t)isr0);
    idt_set_entry(1, (uint64_t)isr1);
    idt_set_entry(2, (uint64_t)isr2);
    idt_set_entry(3, (uint64_t)isr3);
    idt_set_entry(4, (uint64_t)isr4);
    idt_set_entry(5, (uint64_t)isr5);
    idt_set_entry(6, (uint64_t)isr6);
    idt_set_entry(7, (uint64_t)isr7);
    idt_set_entry(8, (uint64_t)isr8);
    idt_set_entry(9, (uint64_t)isr9);
    idt_set_entry(10, (uint64_t)isr10);
    idt_set_entry(11, (uint64_t)isr11);
    idt_set_entry(12, (uint64_t)isr12);
    idt_set_entry(13, (uint64_t)isr13);
    idt_set_entry(14, (uint64_t)isr14);
    idt_set_entry(15, (uint64_t)isr15);
    idt_set_entry(16, (uint64_t)isr16);
    idt_set_entry(17, (uint64_t)isr17);
    idt_set_entry(18, (uint64_t)isr18);
    idt_set_entry(19, (uint64_t)isr19);
    idt_set_entry(20, (uint64_t)isr20);
    idt_set_entry(21, (uint64_t)isr21);
    idt_set_entry(22, (uint64_t)isr22);
    idt_set_entry(23, (uint64_t)isr23);
    idt_set_entry(24, (uint64_t)isr24);
    idt_set_entry(25, (uint64_t)isr25);
    idt_set_entry(26, (uint64_t)isr26);
    idt_set_entry(27, (uint64_t)isr27);
    idt_set_entry(28, (uint64_t)isr28);
    idt_set_entry(29, (uint64_t)isr29);
    idt_set_entry(30, (uint64_t)isr30);
    idt_set_entry(31, (uint64_t)isr31);
}

void isr_handler(uint64_t id)
{
    kprintf("[!] Received interrupt: %s\n", exception_messages[id]);
    for (;;)
        asm("sti\nhlt\n");
}