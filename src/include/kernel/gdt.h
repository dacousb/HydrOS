#include <stdint.h>

typedef struct gdt_descriptor
{
    uint16_t size; /* this is the descriptor for the GDT */
    uint64_t base; /* it is a ref for where the GDT is */
} __attribute__((packed)) gdt_descriptor_t;

typedef struct gdt_entry
{
    uint16_t low_limit; /* each one is a selector */
    uint16_t low_base;
    uint8_t mid_base;
    uint8_t access;
    uint8_t flags; /* also granularity */
    uint8_t hig_base;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt
{                            /* the GDT, where there are several descriptors */
    gdt_entry_t null;        /* each one of these is a descriptor */
    gdt_entry_t kernel_code; /* each descriptor will have its selectors, see gdt_entry */
    gdt_entry_t kernel_data;
    gdt_entry_t user_null;
    gdt_entry_t user_code;
    gdt_entry_t user_data;
} __attribute__((packed)) gdt_t;

extern void gdt_load(gdt_descriptor_t *gdt_descriptor);

void init_gdt();