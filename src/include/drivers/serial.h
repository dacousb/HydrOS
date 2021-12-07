#pragma once

#define COM1 0x3F8

int init_serial();
void print_serial(const char *s);