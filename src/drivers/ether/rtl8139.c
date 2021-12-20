#include <drivers/rtl8139.h>
#include <string/string.h>
#include <kernel/ports.h>

void init_rtl8139(uint64_t ioaddr)
{
    /* refer to https://wiki.osdev.org/RTL8139 for a more extensive
     * explanation */

    /* the registers used for the RTL8139 are
     *
     *  Offset (from IO base)    Size    Name
     *  0x00                     6       MAC0-5
     *  0x08                     8       MAR0-7
     *  0x30                     4       RBSTART
     *  0x37                     1       CMD
     *  0x3C                     2       IMR
     *  0x3E                     2       ISR
     *  
     *  0x44                             RCR
     *  0x52                             CONFIG_1
     */

    port_byte_out(ioaddr + 0x52, 0x00);            /* turn on the device                          */
    port_byte_out(ioaddr + 0x37, 0x10);            /* reset the device                            */
    while (port_byte_in(ioaddr + 0x37) & (1 << 4)) /* wait till RST bit is clear (reset finished) */
        ;

    /* let's send the chip a pointer to a memory location to use as the receive
     * buffer, it is recommended to allocate 8K + 16 bytes + 1500 bytes */
    uint8_t *rx_buffer = 0; /* NOTE kmalloc is needed */
    port_byte_out(ioaddr + 0x30, (uintptr_t)rx_buffer);

    /* now, we need to set the chip to accept TOK and ROK interrupts, that is,
     * transmit and receive */
    port_byte_out(ioaddr + 0x3C, 0b101);

    /* (https://wiki.osdev.org/RTL8139#Configuring_receive_buffer_.28RCR.29) 
     * tell the chip to accept packets
     *  - 0xf sets the first 4 bits (rules AB, AM, APM, AAP)
     *  - the seventh bit sets the WRAP (enabling buffer overflow)
     *    this is why we needed to add 1500 extra bytes to the buffer
     */
    port_byte_out(ioaddr + 0x44, 0xf | (1 << 7));

    /* finally, we need to enable the receive (RX) and transmitter (TX) functions
     * set the RE and TE bits */
    port_byte_out(ioaddr + 0x37, 0b1100);

    uint8_t mac[6];
    for (uint8_t i = 0; i < 6; i++)
        mac[i] = port_byte_in(ioaddr + i); /* see the MAC0-5 offsets */

    kprintf("[RTL] ioaddr = 0x%x, MAC = ", ioaddr);

    for (uint8_t i = 0; i < 6; i++)
        kprintf("%1x%s", mac[i], (i < 5) ? ":" : " ");

    /* since this driver just gets the MAC address (for now), 
     * print this warning */
    kprintf("warning: network is not up\n");
}