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

#define SET_PAGE(page) (phys_bitmap[page / 8] |= (1 << (page % 8)))
#define CLEAR_PAGE(page) (phys_bitmap[page / 8] &= ~(1 << (page % 8)))

uint8_t *phys_bitmap = {0};

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

    /* the problem with the bitmap approach is that we don't really know how many
     * bytes we will need to store our bitmap, and since we didn't initialize the physical
     * memory, we cannot allocate it
     * 
     * a solution would be loading the bitmap into some available area, in theory, the entries must
     * be sorted by base address, lowest to highest, the stivale protocol says that
     *          
     *       [the entries are guaranteed to be sorted by base address, lowest to highest]
     * 
     * thus, the bitmap size in bytes can be calculated with
     * 
     *                  highest address / pages / 8 pages per bitmap byte
     */
    uint32_t bitmap_bytes = 0;

    for (uint8_t i = 0; i < mem_tag->entries; i++)
    {
        struct stivale2_mmap_entry mem_entry = mem_tag->memmap[i];
        total_memory += mem_entry.length;
        if (mem_entry.type == STIVALE2_MMAP_USABLE)
        {
            kprintf("[PHYS] usable, from 0x%lx to 0x%lx\n", mem_entry.base, mem_entry.length);
            available_memory += mem_entry.length;
            bitmap_bytes = (mem_entry.base + mem_entry.length) / PAGE_SIZE / 8;
        }
    }

    for (uint8_t i = 0; i < mem_tag->entries; i++)
    {
        struct stivale2_mmap_entry *mem_entry = &mem_tag->memmap[i];
        /* if we find an entry were we can store our bitmap, we will
         * manually allocate it as following */
        if (mem_entry->type == STIVALE2_MMAP_USABLE && mem_entry->length > bitmap_bytes)
        {
            phys_bitmap = (uint8_t *)mem_entry->base;

            /* if the bitmap size is less than one page, it will cause problems later, since
             * our physical functions will truncate its value, for example, if the bitmap
             * size is 2048 bytes, that is, 0.5 pages, it will be treated as 0 pages */
            uint64_t bitmap_bytes_up = ((bitmap_bytes + (PAGE_SIZE - 1)) / PAGE_SIZE);
            bitmap_bytes_up *= PAGE_SIZE; /* this always rounds up */

            mem_entry->base += bitmap_bytes_up;
            mem_entry->length -= bitmap_bytes_up;
            break;
        }
    }

    /* now that we got the bitmap correctly initialized, we can set all the pages contained
     * in STIVALE2_MMAP_USABLE entries as free
     * 
     * we need to keep in mind that between entries, there is memory that is not usable,
     * so, before doing anything, let's mark everything as used (the for loop will
     * take care of setting the usable areas as free)
     */

    memset(phys_bitmap, 0xFF, bitmap_bytes); /* 0xFF is equivalent to one entire byte set */
    for (uint8_t i = 0; i < mem_tag->entries; i++)
    {
        struct stivale2_mmap_entry mem_entry = mem_tag->memmap[i];
        if (mem_entry.type == STIVALE2_MMAP_USABLE)
            phys_free_multi((void *)mem_entry.base, mem_entry.length / PAGE_SIZE);
    }

    kprintf("[PHYS] %uMB available, %uMB total\n", available_memory / (1024 * 1024), total_memory / (1024 * 1024));
}