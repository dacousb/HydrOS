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

    int divisor = 1193182 / freq; /* PIT clock signal */

    /* Programmable interval timer */

    /* PIT I/O locations (ports) are
     * 0x40 data
     * 0x41 data
     * 0x42 data
     * 0x43 command register (write only)
     */

    /* 0x36 is the result of 0x00 | 0x06 | 0x30 | 0x00
     *
     * 0x00 use binary counter values
     * 0x06 square wave mode
     * 0x30 lobyte/hibyte
     * 0x00 no counter flags
     * 
     * refer to https://wiki.osdev.org/PIT#Read_Back_Status_Byte
     * for the entire table and its respective binary values
     */

    port_byte_out(0x43, 0x36); /* set the command byte */
    port_byte_out(0x40, divisor & 0xff);
    port_byte_out(0x40, divisor >> 8);
}