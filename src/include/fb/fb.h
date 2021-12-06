#pragma once
#include <stivale2.h>

void init_fb(struct stivale2_struct_tag_framebuffer *fb_tag);
void set_pixel(int x, int y, uint32_t color);
void put_literal_char(int x, int y, char c, uint32_t fg_color, uint32_t bg_color);
void putchar(char c);
void print(const char *s);