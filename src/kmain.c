#include <stivale2.h>
#include <stddef.h>

#include <fb/fb.h>
#include <string/string.h>

#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/irq.h>
#include <kernel/pci.h>
#include <kernel/acpi.h>

#include <drivers/kb.h>
#include <drivers/serial.h>
#include <drivers/cmos.h>

#include <mem/phys.h>

void _start(struct stivale2_struct *stivale2_struct);

static uint8_t stack[4096];

static struct stivale2_header_tag_framebuffer fb_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0},          /* there's no tag after this one, so next = 0  */
    .framebuffer_width = 0,  /* let the bootloader decide the fb resolution */
    .framebuffer_height = 0, /*                                             */
    .framebuffer_bpp = 0};   /* bits per pixel, see color depth             */

__attribute__((section(".stivale2hdr"), used))

/* the stivale_hdr or stivale_header, refer to the official documentation
 * https://github.com/stivale/stivale/blob/master/STIVALE2.md#stivale2-header */
static struct stivale2_header stivale_hdr = {
    .entry_point = (uint64_t)_start,           /* tells the bootloader that the entry point is _start         */
    .stack = (uintptr_t)stack + sizeof(stack), /* above, static uint8_t stack[4096];                          */
    .flags = (1 << 1),                         /* bit 1 set, all pointers are to be offset to the higher half */
    .tags = (uintptr_t)&fb_tag};               /* points to the first tag of the linked list of header tags   */

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
    struct stivale2_struct_tag_framebuffer *fb_tag = (struct stivale2_struct_tag_framebuffer *)
        stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    init_fb(fb_tag);
    kprintf("[OK] FB %ux%u\n", fb_tag->framebuffer_width, fb_tag->framebuffer_height);

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

    /* init the ACPI */
    init_acpi(stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_RSDP_ID));

    /* after the interrupts and before the PCI,
     * we initialize the physical memory */
    init_phys(stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID));

    /* init the PCI */
    init_pci();

    /* print the hour */
    print_cmos();

    /* init the keyboard */
    init_kb();
    kprintf("[OK] KB\n");

    for (;;)
    {
    }
}