#include "xor.h"
#include "utils.h"

void xor_rm_reg_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    proc->flags.o = 0;
    proc->flags.c = 0;
    if (params.w)
    {
        unsigned short rm = emulator_get_rm(emulator, params);
        unsigned short reg = emulator_get_reg(emulator, params.reg);
        unsigned short res = rm ^ reg;
        proc->flags.z = res == 0;
        proc->flags.s = (res & 0x8000) > 0;
        proc->flags.p = get_parity(res);
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
        unsigned char rm = emulator_get_rm_byte(emulator, params);
        unsigned char reg = emulator_get_reg_byte(emulator, params.reg);
        unsigned char res = rm ^ reg;
        proc->flags.z = res == 0;
        proc->flags.s = (res & 0x80) > 0;
        proc->flags.p = get_parity(res);
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
