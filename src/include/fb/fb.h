#pragma once

#include <stivale2.h>

#define DEFAULT_FG 0xFFFFFFFF
#define DEFAULT_BG 0x22282A36
#define DEFAULT_GR 0x555AE67D
#define DEFAULT_RD 0xFFFF5555

void init_fb(struct stivale2_struct_tag_framebuffer *fb_tag);
void set_pixel(int x, int y, uint32_t color);
void put_literal_char(int x, int y, char c, uint32_t fg_color, uint32_t bg_color);
void set_color(uint32_t fg, uint32_t bg);
void putchar(char c);
void print(const char *s);