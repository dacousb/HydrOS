#include <stdint.h>

#include <kernel/timer.h>
#include <kernel/ports.h>
#include <kernel/irq.h>

uint32_t tick = 0;

static void timer_callback()
{
    tick++;
}

void init_timer(int freq)
{
    irq_set_handler(IRQ0, timer_callback);

    int divisor = 1193180 / freq;

    port_byte_out(0x43, 0x36);
    port_byte_out(0x40, divisor & 0xff);
    port_byte_out(0x40, divisor >> 8);
}