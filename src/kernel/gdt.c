#include <kernel/gdt.h>

/* our GDT that we will load (see init_gdt() and __gdt.asm) */
gdt_t GDT = {
    {0, 0, 0, 0x00, 0x00, 0}, /* gdt_entry null;        */
    {0, 0, 0, 0x9A, 0xA0, 0}, /* gdt_entry kernel_code; */
    {0, 0, 0, 0x92, 0xA0, 0}, /* gdt_entry kernel_data; */
    {0, 0, 0, 0x00, 0x00, 0}, /* gdt_entry user_null;   */
    {0, 0, 0, 0x9A, 0xA0, 0}, /* gdt_entry user_code;   */
    {0, 0, 0, 0x92, 0xA0, 0}  /* gdt_entry user_data;   */
};

void init_gdt()
{
    gdt_descriptor_t gdt_descriptor;
    gdt_descriptor.size = sizeof(gdt_t) - 1;
    gdt_descriptor.base = (uint64_t)&GDT; /* the base address of our GDT */
    gdt_load(&gdt_descriptor);
}