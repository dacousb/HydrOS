#include <stdarg.h>

#include <math/math.h>
#include <string/string.h>
#include <fb/fb.h>

char *itoa(char base, long long i)
{
    char negative = 0;
    if (i < 0)
    {
        negative = 1;
        i = -i;
    }

    if (base == 'u')
        base = 10;
    else if (base == 'x')
        base = 16;

    static char hex[] = "0123456789ABCDEF";
    static char buffer[50];
    char *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do
    {
        *--ptr = hex[i % base];
        i /= base;
    } while (i != 0);

    if (negative)
        *--ptr = '-';

    return ptr;
}

int strcmp(char *s1, char *s2)
{
    while (*s1)
    {
        if (*s1 != *s2)
            return 1;
        ++s1;
        ++s2;
    }
    return 0;
}

int strncmp(char *s1, char *s2, int n)
{
    while (*s1 && n)
    {
        if (*s1 != *s2)
            return 1;
        ++s1;
        ++s2;
        n--;
    }
    return 0;
}

void kprintf(const char *fmt, ...)
{ /* this function is a variadic function, it accepts a variable number of arguments */
    va_list lst;
    va_start(lst, fmt); /* fmt is the previous argument to the ones we want (...) */
    /* NOTE for some reason, this does not work with llvm, it generates garbage */
    int leading_zero;

    for (size_t i = 0; fmt[i]; i++)
    {
        if (fmt[i] == '%')
        {
            i++;
            switch (fmt[i])
            {
            case 's':
                print(va_arg(lst, char *));
                break;
            case 'l':
                i++;
                if (fmt[i] == 'u' || fmt[i] == 'x')
                    print(itoa(fmt[i], va_arg(lst, long)));
                break;
            case 'u':
                print(itoa('u', va_arg(lst, int)));
                break;
            case 'x':
                print(itoa('x', va_arg(lst, int)));
                break;
            case '0' ... '9':
                leading_zero = fmt[i] - '0';
                i++;
                if (fmt[i] == 'u' || fmt[i] == 'x')
                {
                    int tmp = va_arg(lst, int);
                    if (tmp < pow((fmt[i] == 'u') ? 10 : 16, leading_zero))
                        putchar('0');
                    print(itoa(fmt[i], tmp));
                }
                break;
            case '%':
                putchar(fmt[i]);
                break;
            }
        }
        else
            putchar(fmt[i]);
    }

    va_end(lst);
}

void *memset(void *buffptr, int value, size_t size)
{
    uint8_t *buff = (uint8_t *)buffptr;
    for (size_t i = 0; i < size; i++)
        buff[i] = (uint8_t)value;
    return buffptr;
}