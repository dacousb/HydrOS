#include <kernel/panic.h>
#include <string/string.h>

void panic(const char *panic_msg)
{
    kprintf("------------- PANIC -------------\n");
    kprintf(panic_msg);
    kprintf("---------------------------------\n");
    for (;;)
        asm("sti\nhlt\n");
}