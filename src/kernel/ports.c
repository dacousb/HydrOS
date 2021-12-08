#include <kernel/ports.h>

/* <in>  functions receive a value from an I/O location */
/* <out> functions send a value to an I/O location      */

unsigned char port_byte_in(unsigned short port)
{
    unsigned char result;
    asm("in %%dx, %%al"
        : "=a"(result)
        : "d"(port));
    return result;
}

void port_byte_out(unsigned short port, unsigned char data)
{
    asm("out %%al, %%dx"
        :
        : "a"(data), "d"(port));
}

unsigned short port_word_in(unsigned short port)
{
    unsigned short result;
    asm("in %%dx, %%ax"
        : "=a"(result)
        : "d"(port));
    return result;
}

void port_word_out(unsigned short port, unsigned short data)
{
    asm("out %%ax, %%dx"
        :
        : "a"(data), "d"(port));
}

uint32_t port_long_in(uint32_t port)
{
    uint32_t result;
    asm("inl %1, %0"
        : "=a"(result)
        : "d"(port));
    return result;
}

void port_long_out(uint32_t port, uint32_t data)
{
    asm("outl %0, %1"
        :
        : "a"(data), "d"(port));
}