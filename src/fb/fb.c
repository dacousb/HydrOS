#include <stddef.h>

#include <fb/fb.h>
#include <fb/font.h>

uint32_t *fb;
uint16_t fb_height, fb_width;

int print_x = 0, print_y = 0;

uint32_t default_fg = 0xffffff, default_bg = 0x000000; /* white; black */

void init_fb(struct stivale2_struct_tag_framebuffer *fb_tag)
{
    fb = (uint32_t *)fb_tag->framebuffer_addr;
    fb_width = fb_tag->framebuffer_width;
    fb_height = fb_tag->framebuffer_height;
}

void scroll()
{
    for (size_t i = 0; i < (fb_width * fb_height); i++)
        fb[i] = (fb + fb_width * FONT_HEIGHT)[i];
}

void set_pixel(int x, int y, uint32_t color)
{
    /* plot a pixel in the framebuffer */
    fb[y * fb_width + x] = color;
}

void put_literal_char(int x, int y, char c, uint32_t fg_color, uint32_t bg_color)
{
    for (int ly = 0; ly < FONT_HEIGHT; ly++)
    {
        for (int lx = 0; lx < FONT_WIDTH; lx++)
        {
            uint8_t pixel = _tvga_c4__8x14[(c * FONT_HEIGHT) + ly];
            if ((pixel >> lx) & 1)
                fb[x + ((FONT_WIDTH - 1) - lx) + ((y + ly) * fb_width)] = fg_color;
            else
                fb[x + ((FONT_WIDTH - 1) - lx) + ((y + ly) * fb_width)] = bg_color;
        }
    }
}

void putchar(char c)
{
    switch (c)
    {
    case '\n': /* new line */
        if (print_y + FONT_HEIGHT*2 > fb_height)
        {
            print_x = 0;
            scroll();
        }
        else
        {
            print_x = 0;
            print_y += FONT_HEIGHT;
        }
        break;
    case '\r': /* carriage return */
        print_x = 0;
        break;
    case '\b': /* back (del) */
        print_x -= FONT_WIDTH;
        putchar(' ');
        print_x -= FONT_WIDTH;
        break;
    default: /* any other character */
        if (print_x + FONT_WIDTH > fb_width)
        {
            print_x = 0;
            print_y += FONT_HEIGHT;
        }
        if (print_y + FONT_HEIGHT > fb_height)
        {
            scroll();
            print_y -= FONT_HEIGHT;
        }
        put_literal_char(print_x, print_y, c, default_fg, default_bg);
        print_x += FONT_WIDTH;
    }
}

void print(const char *s)
{
    for (size_t i = 0; s[i]; i++)
        putchar(s[i]);
}