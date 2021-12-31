#include <stdint.h>

typedef struct gdt_descriptor
{
    uint16_t size;
    uint64_t base;
} __attribute__((packed)) gdt_descriptor_t;

typedef struct gdt_entry
{
    uint16_t low_limit;
    uint16_t low_base;
    uint8_t mid_base;
    uint8_t access;
    uint8_t limit_flags;
    uint8_t hig_base;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt
{
    gdt_entry_t null;
    gdt_entry_t kernel_code;
    gdt_entry_t kernel_data;
    gdt_entry_t user_code;
    gdt_entry_t user_data;
} __attribute__((packed)) gdt_t;

extern void gdt_load(gdt_descriptor_t *gdt_descriptor);

void init_gdt();