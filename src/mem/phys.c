#include <mem/phys.h>
#include <string/string.h>
#include <stdint.h>

/* refer to the official stivale2 specification memory map section to get more information
 * https://github.com/stivale/stivale/blob/master/STIVALE2.md#memory-map-structure-tag */

void init_phys(struct stivale2_struct_tag_memmap *mem_tag)
{
    uint64_t total_memory = 0;
    uint64_t available_memory = 0;

    /* the memory map tag contains
     * - entries,  count of memory map entries
     * - memmap[], array of memory map entries
     * 
     * and each memory entry
     * - base,   physical address of base of the memory section
     * - length, length of the section
     * - type,   keeping in mind the following constants
     * 
     *    USABLE                 = 1,
     *    RESERVED               = 2,
     *    ACPI_RECLAIMABLE       = 3,
     *    BOOTLOADER_RECLAIMABLE = 0x1000,
     *    KERNEL_AND_MODULES     = 0x1001,
     * 
     * also, from the stivale specification
     * - usable and bootloader reclaimable entries are guaranteed to be 4096 byte
     *   aligned for both base and length
     * - usable and bootloader reclaimable entries are guaranteed not to overlap
     *   with any other entry
     */

    for (uint8_t i = 0; i < mem_tag->entries; i++)
    {
        struct stivale2_mmap_entry mem_entry = mem_tag->memmap[i];
        total_memory += mem_entry.length;
        if (mem_entry.type == STIVALE2_MMAP_USABLE)
            available_memory += mem_entry.length;
    }

    kprintf("[PHYS] %u/%uMB\n", available_memory / (1024 * 1024), total_memory / (1024 * 1024));
}