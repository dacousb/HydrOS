#include <mem/kheap.h>
#include <mem/phys.h>
#include <string/string.h>

/* for now, we will use a watermark allocator, we will keep track
 * of what we have allocated and how much memory we got left
 *
 *              | allocated | free memory        |  
 *                          ^free_base   free_top^ 
 * 
 * each time free_base is incremented by one, we are working with one page,
 * that is 4096 bytes 
 */
uint64_t free_base = 0; /* free_base works as an offset of phys_bitmap */
uint64_t free_top = 0;

void *kmalloc(size_t size)
{
    while (CHECK_PAGE(free_base)) /* while we do not find free pages */
    {
        free_base++;
        free_top++;
    }
    while (CHECK_PAGE(free_top) == 0) /* while we find free pages */
        free_top++;
    free_base += (size + (4096 - 1)) / 4096; /* a pretty good round up method for divisions */
    return (void *)(free_base * PAGE_SIZE);
}