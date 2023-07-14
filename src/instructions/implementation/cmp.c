#include "cmp.h"
#include "utils.h"

void cmp_immediate_rm_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    if (params.w)
    {
        unsigned short val1 = emulator_get_rm(emulator, params);
        unsigned short val2 = params.data;
        unsigned int res = val1 - val2;
        proc->flags.c = get_carry(res);
        proc->flags.o = get_overflow_minus(val1, val2);
        proc->flags.s = get_sign(res);
        proc->flags.z = get_zero(res);
        // TODO: implement auxiliary flag
        // proc->flags.a = ;
        proc->flags.p = get_parity(res);
    }
    else
    {
        unsigned char val1 = emulator_get_rm_byte(emulator, params);
        unsigned char val2 = params.data_low;
        unsigned short res = val1 - val2;
        proc->flags.c = get_carry_byte(res);
        proc->flags.o = get_overflow_minus_byte(val1, val2);
        proc->flags.s = get_sign_byte(res);
        proc->flags.z = get_zero_byte(res);
        // TODO: implement auxiliary flag
        // proc->flags.a = ;
        proc->flags.p = get_parity_byte(res);
    }
}

void cmp_rm_reg_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    if (params.w)
    {
        unsigned short rm_val = emulator_get_rm(emulator, params);
        unsigned short reg_val = emulator_get_reg(emulator, params.reg);
        unsigned int res;

        if (params.d)
        {
            res = reg_val - rm_val;
            proc->flags.o = get_overflow_minus(reg_val, rm_val);
        }
        else
        {
            res = rm_val - reg_val;
            proc->flags.o = get_overflow_minus(rm_val, reg_val);
        }

        proc->flags.c = get_carry(res);
        proc->flags.s = get_sign(res);
        proc->flags.z = get_zero(res);
        // TODO: implement auxiliary flag
        // proc->flags.a = ;
        proc->flags.p = get_parity(res);
    }
    else
    {
        unsigned char rm_val = emulator_get_rm_byte(emulator, params);
        unsigned char reg_val = emulator_get_reg_byte(emulator, params.reg);
        unsigned short res;
        if (params.d)
        {
            res = reg_val - rm_val;
            proc->flags.o = get_overflow_minus_byte(reg_val, rm_val);
        }
        else
        {
            res = rm_val - reg_val;
            proc->flags.o = get_overflow_minus_byte(rm_val, reg_val);
        }
        proc->flags.c = get_carry_byte(res);
        proc->flags.s = get_sign_byte(res);
        proc->flags.z = get_zero_byte(res);
        // TODO: implement auxiliary flag
        // proc->flags.a = ;
        proc->flags.p = get_parity_byte(res);
    }
}

void cmp_immediate_acc_exec(emulator_t *emulator, params_t params)
{
    params.mod = 0b11;
    params.d = 0;
    if (params.w)
        params.rm = AX;
    else
        params.rm = AL;
    cmp_immediate_rm_exec(emulator, params);
}
