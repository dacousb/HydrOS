#include <stdarg.h>
#include <stddef.h>

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

void kprintf(const char *fmt, ...)
{ /* this function is a variadic function, it accepts a variable number of arguments */
    va_list lst;
    va_start(lst, fmt); /* fmt is the previous argument to the ones we want (...) */
    /* NOTE for some reason, this does not work with llvm, it generates garbage */

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
            case 'u':
                print(itoa('u', va_arg(lst, int)));
                break;
            case 'x':
                print(itoa('x', va_arg(lst, int)));
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