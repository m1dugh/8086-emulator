#include "convert.h"

void cbw_exec(emulator_t *emulator, params_t params)
{
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
