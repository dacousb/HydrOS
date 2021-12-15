#include <mem/phys.h>
#include <string/string.h>
#include <stdint.h>

/* refer to the official stivale2 specification memory map section to get more information
 * https://github.com/stivale/stivale/blob/master/STIVALE2.md#memory-map-structure-tag */

/* the physical memory manager task is simple, it needs to split our available physical memory
 * into chunks, or pages (each one of 4096 bytes)
 *
 * the manager must keep an eye on these pages, and know if they are being used or free, so
 * when a program or the kernel asks for some memory, the manager can give a pointer to
 * a free and usable area (that is, a classic malloc function)
 * 
 * there are several methods of doing this, we are going to use a bitmap
 */

/* each byte of the bitmap can hold information about eight different pages, that is, each bit
 * holds information for one page
 *
 * some notes for the following macros
 *  - we want to set page 9, 9/8 [truncated] is 1, so the information of page 9 will be in byte 1
 *  - we do 9 % 8, getting 1, that means that the information for the page 9 will be in byte 1, bit 1
 *  - keep in mind that page 0 is the first one, and page 9 the tenth
 *                                
 *                                  0000 0000, 0100 0000
 */

#define SET_PAGE(page) phys_bitmap[page / 8] |= (1 << (page % 8))
#define CLEAR_PAGE(page) phys_bitmap[page / 8] &= ~(1 << (page % 8))
#define CHECK_PAGE(page) phys_bitmap[page / 8] & (1 << (page % 8))

static uint64_t highest_page = 0;
static uint8_t *phys_bitmap = {0};

void phys_alloc_page(void *addr)
{
    SET_PAGE((uint64_t)addr / PAGE_SIZE);
}

void phys_free_page(void *addr)
{
    CLEAR_PAGE((uint64_t)addr / PAGE_SIZE);
}

void phys_alloc_multi(void *addr, uint64_t n)
{
    for (uint64_t i = 0; i < n; i++)
        phys_alloc_page(addr + i * PAGE_SIZE);
}

void phys_free_multi(void *addr, uint64_t n)
{
    for (uint64_t i = 0; i < n; i++)
        phys_free_page(addr + i * PAGE_SIZE);
}

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