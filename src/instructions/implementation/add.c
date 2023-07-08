#include "add.h"
#include "utils.h"

void add_rm_reg_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    if (params.w)
    {
        unsigned short reg_val = emulator_get_reg(emulator, params.reg);
        unsigned short rm_val = emulator_get_rm(emulator, params);
        unsigned int res = reg_val + rm_val;
        proc->flags.a = (reg_val & 0x8) && (rm_val & 0x8);
        proc->flags.c = get_carry(res);
        proc->flags.s = get_sign(res);
        proc->flags.o = ((reg_val & 0x8000) == (rm_val & 0x8000))
                        && ((reg_val & 0x8000) != (res & 0x8000));
        proc->flags.z = get_zero(res);
        if (params.d)
        {
            emulator_set_reg(emulator, params.reg, res);
        }
        else
        {
            emulator_set_rm(emulator, params, res);
        }
    }
    else
    {
        unsigned char reg_val = emulator_get_reg_byte(emulator, params.reg);
        unsigned char rm_val = emulator_get_rm_byte(emulator, params);
        unsigned short res = reg_val + rm_val;
        proc->flags.a = (reg_val & 0x8) && (rm_val & 0x8);
        proc->flags.c = (res & 0x100) > 0;
        proc->flags.s = ((res & 0x80) >> (sizeof(unsigned char) - 1)) > 0;
        proc->flags.o = ((reg_val & 0x80) == (rm_val & 0x80))
                        && ((reg_val & 0x80) != (res & 0x80));
        proc->flags.z = (res & 0xFF) == 0;
        if (params.d)
        {
            emulator_set_reg_byte(emulator, params.reg, res);
        }
        else
        {
            emulator_set_rm_byte(emulator, params, res);
        }
    }
}
