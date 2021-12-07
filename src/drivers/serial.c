#include <drivers/serial.h>
#include <kernel/ports.h>

int init_serial()
{
    /* the COM1 port will be our base port              */
    /* we can add an offset (+n) to access other ports, */
    /* each port is mapped to a register                */

    /* refer to https://wiki.osdev.org/Serial_Ports for a better explanation */

    port_byte_out(COM1 + 1, 0x00); /* +1 is the interrupt register, disable interrupts          */
    port_byte_out(COM1 + 3, 0x80); /* +3 is the line control register, holds a bit called DLAB, */
    port_byte_out(COM1 + 0, 0x03); /* ^  it changes how +1 and +0 (COM1) work                   */
    port_byte_out(COM1 + 1, 0x00);
    port_byte_out(COM1 + 3, 0x03);
    port_byte_out(COM1 + 2, 0xC7);
    port_byte_out(COM1 + 4, 0x0B);
    port_byte_out(COM1 + 4, 0x1E);
    port_byte_out(COM1 + 0, 0xAE); /* write an arbitrary value to the data register (+0) */

    if (port_byte_in(COM1 + 0) != 0xAE) /* test if it worked */
        return 1;                       /* if it is not what we wrote (0xAE) */

    port_byte_out(COM1 + 4, 0x0F);
    return 0;
}

int serial_received()
{
    return port_byte_in(COM1 + 5) & 1;
}

char read_serial()
{
    while (serial_received() == 0)
        ;
    return port_byte_in(COM1);
}

int is_transmit_empty()
{
    return port_byte_in(COM1 + 5) & 0x20;
}

void write_serial(char c)
{
    while (is_transmit_empty() == 0)
        ;
    port_byte_out(COM1, c);
}

void print_serial(const char *s)
{
    for (int i = 0; s[i]; i++)
        write_serial(s[i]);
}