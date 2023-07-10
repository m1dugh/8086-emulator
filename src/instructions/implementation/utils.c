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

unsigned char get_overflow_plus_byte(unsigned char v1, unsigned char v2)
{
    unsigned short res = v1 + v2;
    return ((v1 & 0x80) == (v2 & 0x80)) && ((v1 & 0x80) != (res & 0x80));
}

unsigned char get_overflow_minus_byte(unsigned char v1, unsigned char v2)
{
    v2 = (v2 ^ 0xFF) + 1;
    return get_overflow_plus_byte(v1, v2);
}

unsigned char get_overflow_plus(unsigned short v1, unsigned short v2)
{
    unsigned int res = v1 + v2;
    return ((v1 & 0x8000) == (v2 & 0x8000))
           && ((v2 & 0x8000) != (res & 0x8000));
}
unsigned char get_overflow_minus(unsigned short v1, unsigned short v2)
{
    v2 = (v2 ^ 0xFFFF) + 1;
    return get_overflow_plus(v1, v2);
}
