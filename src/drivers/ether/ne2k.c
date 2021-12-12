#include <drivers/ne2k.h>
#include <string/string.h>
#include <kernel/ports.h>

void init_ne2k(uint64_t ioaddr)
{
    /* refer to https://wiki.osdev.org/Ne2000 for a more extensive
     * explanation */

    /* NE2000 or NE2k is a network card specification,
     * this is a simple implementation to initializate it and get
     * the MAC address of the device */

    /* reset the board, this is done by doing a read
	 * followed by a write to the reset port */
    port_byte_out(ioaddr + 0x1F, port_byte_in(ioaddr + 0x1F));

    while ((port_byte_in(ioaddr + 0x07) & 0x80) == 0)
        ; /* wait the reset to complete */

    port_byte_out(ioaddr + 0x07, 0xFF);

    port_byte_out(ioaddr + 0x00, (1 << 5) | 1);
    port_byte_out(ioaddr + 0x0E, 0x49);
    port_byte_out(ioaddr + 0x0A, 0);
    port_byte_out(ioaddr + 0x0B, 0);
    port_byte_out(ioaddr + 0x0F, 0);
    port_byte_out(ioaddr + 0x07, 0xFF);
    port_byte_out(ioaddr + 0x0C, 0x20);
    port_byte_out(ioaddr + 0x0D, 0x02);
    port_byte_out(ioaddr + 0x0A, 32);
    port_byte_out(ioaddr + 0x0B, 0);
    port_byte_out(ioaddr + 0x08, 0);
    port_byte_out(ioaddr + 0x09, 0);
    port_byte_out(ioaddr + 0x00, 0x0A);

    uint8_t mac[6];
    for (uint8_t i = 0; i < 6; i++)
        mac[i] = port_byte_in(ioaddr + 0x10);

    kprintf("[NE2k] ioaddr = 0x%x, MAC = ", ioaddr);

    for (uint8_t i = 0; i < 6; i++)
        kprintf("%1x%s", mac[i], (i < 5) ? ":" : " ");

    /* since this driver just gets the MAC address (for now), 
     * print this warning */
    kprintf("warning: network is not up\n");
}