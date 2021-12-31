#include <kernel/gdt.h>

/* the GDT that we will load (see init_gdt() and __gdt.asm) */
gdt_t GDT = {
    {0, 0, 0, 0x00, 0x00 << 4, 0}, /* gdt_entry null;        */
    {0, 0, 0, 0x9A, 0x02 << 4, 0}, /* gdt_entry kernel_code; */
    {0, 0, 0, 0x92, 0x00 << 4, 0}, /* gdt_entry kernel_data; */
    {0, 0, 0, 0xFA, 0x02 << 4, 0}, /* gdt_entry user_code;   */
    {0, 0, 0, 0xF2, 0x00 << 4, 0}  /* gdt_entry user_data;   */
};
/* refer to https://wiki.osdev.org/Global_Descriptor_Table#Segment_Descriptor
 * for a better diagram of the segment descriptor and a general explanation
 *
 * GDT.access is a byte with the following structure
 *
 *         | Pr | Privl | S | Ex | DC | RW | Ac |
 *         | 7  | 6 | 5 | 4 | 3  | 2  | 1  | 0  |
 * 
 * 0x9A    | 1  | 0 | 0 | 1 | 1  | 0  | 1  | 0  |
 * 0x92    | 1  | 0 | 0 | 1 | 0  | 0  | 1  | 0  | 
 * 0xFA    | 1  | 1 | 1 | 1 | 1  | 0  | 1  | 0  |
 * 0xF2    | 1  | 1 | 1 | 1 | 0  | 0  | 1  | 0  |
 * 
 * where Privl is the privilege level, 00 is 0, thus, ring 0
 * 11 is 3, thus, ring 3 (less privilege)
 * 
 * where Ex, if set to 1, defines a code segment, if set to 0, defines a data segment
 * 
 * GDT.(limit_)flags (also granularity) has four usable bits, because it forms
 * part of a byte that shares with GDT.limit(_flags), where limit is the high_limit
 * (compared to GDT.low_limit)
 *          ___________________ _______________
 *         |_______flags_______|___high limit__|
 *         | Gr | Sz | L | Res |               |
 *         | 7  | 6  | 5 |  4  | 3 | 2 | 1 | 0 |
 * 
 * 0x02<<4 | 0  | 0  | 1 |  0  | 0 | 0 | 0 | 0 |
 * 
 * where L is the Long-mode flag (64-bit code segment)
 */
static gdt_descriptor_t gdt_descriptor;

void init_gdt()
{
    gdt_descriptor.size = sizeof(gdt_t) - 1;
    gdt_descriptor.base = (uint64_t)&GDT; /* the base address of our GDT */
    gdt_load(&gdt_descriptor);
}