#include <stdint.h>

#include <drivers/kb.h>
#include <fb/fb.h>
#include <kernel/irq.h>
#include <kernel/ports.h>

#define SCANCODE_MAX 57

char scancodes[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9',
    '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'};

static void keyboard_callback()
{
    uint8_t scancode = port_byte_in(KEYBOARD_DATA_PORT);

    if (scancode > SCANCODE_MAX)
        return;

    putchar(scancodes[scancode]);
}

void init_kb()
{
    irq_set_handler(IRQ1, keyboard_callback);
}