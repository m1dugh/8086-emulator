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

unsigned char get_carry(unsigned int value)
{
    return (value & 0x10000) > 0;
}

unsigned char get_sign(unsigned short value)
{
    return (value & 0x8000) > 0;
}

unsigned char get_zero(unsigned short value)
{
    return (value & 0xFFFF) == 0;
}

unsigned char get_carry_byte(unsigned short value)
{
    return (value & 0x100) > 0;
}

unsigned char get_parity_byte(unsigned char value)
{
    return get_parity((unsigned short)value);
}

unsigned char get_sign_byte(unsigned char value)
{
    return (value & 0x80) > 0;
}

unsigned char get_zero_byte(unsigned char value)
{
    return (value & 0xFF) == 0;
}
