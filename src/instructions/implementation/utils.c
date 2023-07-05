#include "utils.h"

unsigned char get_parity(unsigned short value)
{
    unsigned short count = 0;
    for (; value > 0; value >>= 1)
    {
        count += value & 0x1;
    }
    return count % 2 == 0;
}
