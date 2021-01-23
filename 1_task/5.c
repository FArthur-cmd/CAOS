#include <stdint.h>

extern int
check_int(uint32_t u32)
{
    // все числа до 2^24 переходят в float
    // дальше происходит уменьшение количества точных отображений
    // до 2^25 уже каждое второе сохраняется, до 26 степени каждое 4 и тд.
    uint64_t two_in_twenty_four_degree = 1<<24;
    uint32_t start_mask = 0x80000000; // mask = 0b10000000000000000000000000000000
    uint32_t ending_mask = 0xFF; // mask = 11111111
    if (u32 <= two_in_twenty_four_degree)
    {
        return 1;
    }
    while (ending_mask != 0)
    {
        if ((start_mask & u32) != 0)
        {
            return (u32 & ending_mask) == 0;
        } else
        {
            start_mask >>= 1;
            ending_mask >>= 1;
        }
    }
    return 0;
}
