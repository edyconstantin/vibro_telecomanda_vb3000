#include "fmt.h"

unsigned fmt_num(char *dst, int v, unsigned width)
{
    char tmp[8];
    unsigned i = 0, len = 0;
    int neg = 0;

    if (width > 7) width = 7;

    if (v < 0) { neg = 1; v = -v; }

    do {
        tmp[i++] = (char)('0' + (v % 10));
        v /= 10;
    } while (v && i < 8);

    if (neg && i < 8) tmp[i++] = (char)'-';

    // tmp are cifrele inversate; scriem cu zero-pad
    while (len + i < width) dst[len++] = (char)'0';
    while (i > 0) dst[len++] = tmp[--i];
    dst[len] = (char)0;
    return len;
}
