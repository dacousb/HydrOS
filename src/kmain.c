#include <stivale2.h>
#include <stddef.h>

#include <fb/fb.h>
#include <string/string.h>

#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/irq.h>
#include <kernel/pci.h>

#include <drivers/kb.h>
#include <drivers/serial.h>

void _start(struct stivale2_struct *stivale2_struct);

static uint8_t stack[4096];

static struct stivale2_header_tag_framebuffer fb_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0},         /* there's no tag after this one, so next = 0 */
    .framebuffer_width = 0, /* let the bootloader decide the fb resolution */
    .framebuffer_height = 0,
    .framebuffer_bpp = 0};

__attribute__((section(".stivale2hdr"), used))

static struct stivale2_header stivale_hdr = {
    .entry_point = (uint64_t)_start,           /* tells the bootloader that the main function is _start */
    .stack = (uintptr_t)stack + sizeof(stack), /* above, static uint8_t stack[4096]; */
    .flags = (1 << 1),
    .tags = (uintptr_t)&fb_tag}; /* tells the bootloader where our fb preferences are */

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id)
{
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
    for (;;)
    { /* iterate over the tags till we find the desired one */
        if (current_tag == NULL)
            return NULL;
        if (current_tag->identifier == id)
            return current_tag;
        current_tag = (void *)current_tag->next;
    }
}

void _start(struct stivale2_struct *stivale2_struct)
{
    /* init the framebuffer */
    init_fb(stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID));
    kprintf("[OK] FB\n");

    /* init the serial */
    int serial = init_serial();
    kprintf((serial) ? "[ER] SERIAL\n" : "[OK] SERIAL\n");

    /* init the GDT (Global Descriptor Table) */
    init_gdt();
    kprintf("[OK] GDT\n");

    /* init the IDT */
    init_idt();
    kprintf("[OK] IDT\n");

    /* init the ISRs */
    init_isr();
    kprintf("[OK] ISR\n");

    /* init the IRQs */
    init_irq();
    kprintf("[OK] IRQ\n");

    /* init the PCI */
    init_pci();

    /* init the keyboard */
    init_kb();
    kprintf("[OK] KB\n");

    for (;;)
    {
    }
}