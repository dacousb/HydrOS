#include <stdarg.h>
#include <stddef.h>

#include <string/string.h>
#include <fb/fb.h>

void kprintf(const char *fmt, ...)
{ /* this function is a variadic function, it accepts a variable number of arguments */
    va_list lst;
    va_start(lst, fmt); /* fmt is the previous argument to the ones we want (...) */
    /* NOTE for some reason, this does not work with llvm, it generates garbage */

    for (size_t i = 0; fmt[i]; i++)
    {
        if (fmt[i] == '%' && i++)
        {
            switch (fmt[i])
            {
            case 's':
                print(va_arg(lst, char *));
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