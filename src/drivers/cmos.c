#include <stdint.h>

#include <string/string.h>
#include <kernel/ports.h>
#include <drivers/cmos.h>

#define CMOS_CONFIG 0x70
#define CMOS_DATA 0x71

/* we will use two ports to interact with the CMOS
 * 0x70, to select the register (each address of the CMOS is called a register)
 * 0x71, to read what we selected
 */

uint8_t second, minute, hour, day, month, year;

uint8_t cmos_register(int reg)
{
    port_byte_out(CMOS_CONFIG, reg); /* select the desired register */
    return port_byte_in(CMOS_DATA);  /* read its contents           */
}

uint8_t update_flag()
{
    port_byte_out(CMOS_CONFIG, 0x0A);          /* select the Status Register A */
    return port_byte_in(CMOS_DATA) & (1 << 7); /* return its update flag       */
}

void read_cmos()
{
    /* from https://wiki.osdev.org/CMOS
     *
     * Register  Contents            Range
     * 0x00      Seconds             0–59
     * 0x02      Minutes             0–59
     * 0x04      Hours               0–23 in 24-hour mode, 
     *                               1–12 in 12-hour mode, highest bit set if pm
     * 0x06      Weekday             1–7, where Sunday = 1
     * 0x07      Day of Month        1–31
     * 0x08      Month               1–12
     * 0x09      Year                0–99
     * 0x32      Century (maybe)     19–20?
     * 0x0A      Status Register A
     * 0x0B      Status Register B
     * 
     * Status Register A (bit 7 is the update flag, we need it to
     *                    be clear before reading the clock)
     * 
     * Status Register B (value = 2 (010), 24/12 hour format
     *                    value = 4 (100), binary/BCD format)
     * 
     * in BCD, 23 seconds is 0x23 and not 0x17
     */

    while (update_flag())
        ;

    second = cmos_register(0x00);
    minute = cmos_register(0x02);
    hour = cmos_register(0x04);
    day = cmos_register(0x07);
    month = cmos_register(0x08);
    year = cmos_register(0x09);

    uint8_t cmos_reg_b = cmos_register(0x0B);

    /* if the values are in BCD format, convert them into normal ones */
    /* ((val / 16) * 10 + (val & 0xF)) is the way of doing this       */
    if (!(cmos_reg_b & 0b100)) /* remember that AND will ignore everything except that bit */
    {
        second = ((second / 16) * 10) + (second & 0x0F);
        minute = ((minute / 16) * 10) + (minute & 0x0F);
        hour = ((((hour & 0x70) / 16) * 10) + (hour & 0x0F)) | (hour & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = ((month / 16) * 10) + (month & 0x0F);
        year = ((year / 16) * 10) + (year & 0x0F);
    }

    /* in case the format is 12 hour */
    if (!(cmos_reg_b & 0x02) && (hour & 0x80))
        hour = ((hour & 0x7F) + 12) % 24;
}

void print_cmos()
{
    read_cmos();
    kprintf("[DATE] %u-%u-%u %1u:%1u:%1u\n",
            day, month, year,
            hour, minute, second);
}