#include <stdint.h>

uint16_t
satsum(uint16_t x, uint16_t y)
{
    uint16_t max = (~(uint16_t)0);
    return ((max - x) < y) ? max: x + y;
}

