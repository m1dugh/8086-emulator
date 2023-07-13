#include "or.h"
#include "utils.h"

void or_rm_reg_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    proc->flags.o = 0;
    proc->flags.c = 0;
    if (params.w)
    {
        unsigned short v1 = emulator_get_reg(emulator, params.reg);
        unsigned short v2 = emulator_get_rm(emulator, params);
        unsigned short res = v1 | v2;
        proc->flags.s = get_sign(res);
        proc->flags.z = get_zero(res);
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
        unsigned char v1 = emulator_get_reg_byte(emulator, params.reg);
        unsigned char v2 = emulator_get_rm_byte(emulator, params);
        unsigned char res = v1 | v2;
        proc->flags.s = get_sign_byte(res);
        proc->flags.z = get_zero_byte(res);
        proc->flags.p = get_parity_byte(res);
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

void or_immediate_rm_exec(emulator_t *emulator, params_t params)
{

    processor_t *proc = emulator->processor;
    proc->flags.o = 0;
    proc->flags.c = 0;
    if (params.w)
    {
        unsigned short value = emulator_get_rm(emulator, params);
        unsigned short res = params.data | value;
        proc->flags.s = get_sign(res);
        proc->flags.z = get_zero(res);
        proc->flags.p = get_parity(res);
        emulator_set_rm(emulator, params, res);
    }
    else
    {
        unsigned char value = emulator_get_rm_byte(emulator, params);
        unsigned char res = params.data | value;
        proc->flags.s = get_sign_byte(res);
        proc->flags.z = get_zero_byte(res);
        proc->flags.p = get_parity_byte(res);
        emulator_set_rm_byte(emulator, params, res);
    }
}
