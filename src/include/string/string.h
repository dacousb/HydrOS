#pragma once

#include <stddef.h>

void kprintf(const char *fmt, ...);
char *itoa(char base, long long i);
int strcmp(char *s1, char *s2);
int strncmp(char *s1, char *s2, int n);
void *memset(void *buffptr, int value, size_t size);