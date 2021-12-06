#pragma once
#include <stdint.h>

typedef struct idt_entry
{ /* in this case, we can call this an entry or a gate; IDT entries are called gates */
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_hig;
    uint32_t reserved;
} __attribute__((packed)) idt_entry_t;

typedef struct
{ /* lidt instruction will load this into the IDTR(egister) */
    uint16_t size;
    uint64_t base;
} __attribute__((packed)) idtr_t;

void idt_set_entry(uint16_t n, uint64_t handler);
void init_idt();