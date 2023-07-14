#include "convert.h"

void cbw_exec(emulator_t *emulator, params_t params)
{
    UNUSED(params);
    unsigned char low = emulator_get_reg_byte(emulator, AL);
    unsigned short res;
    if (low & 0x80)
    {
        res = low | 0xff00;
    }
    else
    {
        res = low;
    }
    emulator_set_reg(emulator, AX, res);
}

void cwd_exec(emulator_t *emulator, params_t params)
{
    UNUSED(params);
    unsigned short val = emulator_get_reg(emulator, AX);
    if (val & 0x8000)
    {
        emulator_set_reg(emulator, DX, 0xffff);
    }
    else
    {
        emulator_set_reg(emulator, DX, 0);
    }
}
