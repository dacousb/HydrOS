#include <kernel/panic.h>
#include <string/string.h>
#include <fb/fb.h>

void panic(const char *panic_msg)
{
    set_color(DEFAULT_RD, DEFAULT_BG);
    kprintf("------------- PANIC -------------\n");
    kprintf(panic_msg);
    kprintf("---------------------------------\n");
    for (;;)
        asm("sti\nhlt\n");
}