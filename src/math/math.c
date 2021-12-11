#include <stdint.h>

#include <math/math.h>

int pow(int a, int b)
{
    for (uint8_t i = 1; i < b; i++)
        a *= a;
    return a;
}