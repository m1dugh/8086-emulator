#include "mov.h"
#include "utils.h"

void mov_rm_reg_exec(emulator_t *emulator, params_t params)
{
    if (params.w)
    {
        if (params.d)
        {
            unsigned short value = emulator_get_rm(emulator, params);
            emulator_set_reg(emulator, params.reg, value);
        }
        else
        {
            unsigned short value = emulator_get_reg(emulator, params.reg);
            emulator_set_rm(emulator, params, value);
        }
    }
    else
    {
        if (params.d)
        {
            unsigned short value = emulator_get_rm_byte(emulator, params);
            emulator_set_reg_byte(emulator, params.reg, value);
        }
        else
        {
            unsigned short value = emulator_get_reg_byte(emulator, params.reg);
            emulator_set_rm_byte(emulator, params, value);
        }
    }
}
