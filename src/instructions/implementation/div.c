#include "div.h"

void div_exec(emulator_t *emulator, params_t params)
{
    if (params.w)
    {
        unsigned short low = emulator_get_reg(emulator, AX);
        unsigned short high = emulator_get_reg(emulator, DX);
        unsigned int val = (high << 16) + low;
        unsigned short divisor = emulator_get_rm(emulator, params);
        unsigned short remainder = val % divisor;
        unsigned short quotient = val / divisor;
        emulator_set_reg(emulator, AX, quotient);
        emulator_set_reg(emulator, DX, remainder);
    }
    else
    {
        unsigned short val = emulator_get_reg(emulator, AX);
        unsigned char divisor = emulator_get_rm(emulator, params);
        unsigned char remainder = val % divisor;
        unsigned char quotient = val / divisor;
        emulator_set_reg_byte(emulator, AL, quotient);
        emulator_set_reg_byte(emulator, AH, remainder);
    }
}
