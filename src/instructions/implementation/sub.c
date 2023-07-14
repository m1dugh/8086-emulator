#include "sub.h"
#include "utils.h"

void sub_immediate_rm_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    if (params.w)
    {
        unsigned short value = params.data;
        unsigned short rm_val = emulator_get_rm(emulator, params);
        unsigned int res = rm_val - value;
        proc->flags.o = get_overflow_minus(rm_val, value);
        proc->flags.s = get_sign(res);
        proc->flags.z = get_zero(res);
        // TODO: implement a flag
        // proc->flags.a = ;
        proc->flags.p = get_parity(res);
        proc->flags.c = get_carry(res);
        emulator_set_rm(emulator, params, (unsigned short)res);
    }
    else
    {
        unsigned char value = params.data_low;
        unsigned char rm_val = emulator_get_rm_byte(emulator, params);
        unsigned short res = rm_val - value;
        proc->flags.o = get_overflow_minus_byte(rm_val, value);
        proc->flags.s = get_sign_byte(res);
        proc->flags.z = get_zero_byte(res);
        // TODO: implement a flag
        // proc->flags.a = ;
        proc->flags.p = get_parity_byte(res);
        proc->flags.c = get_carry_byte(res);
        emulator_set_rm_byte(emulator, params, (unsigned short)res);
    }
}

void sub_rm_reg_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    if (params.w)
    {
        unsigned short reg_val = emulator_get_reg(emulator, params.reg);
        unsigned short rm_val = emulator_get_rm(emulator, params);
        unsigned int res;
        if (params.d)
        {
            res = reg_val - rm_val;
            emulator_set_reg(emulator, params.reg, (unsigned short)res);
            proc->flags.o = get_overflow_minus(reg_val, rm_val);
        }
        else
        {
            res = rm_val - reg_val;
            emulator_set_rm(emulator, params, (unsigned short)res);
            proc->flags.o = get_overflow_minus(rm_val, reg_val);
        }
        proc->flags.s = get_sign(res);
        proc->flags.z = get_zero(res);
        // TODO: implement a flag
        // proc->flags.a = ;
        proc->flags.p = get_parity(res);
        proc->flags.c = get_carry(res);
    }
    else
    {
        unsigned char reg_val = emulator_get_reg_byte(emulator, params.reg);
        unsigned char rm_val = emulator_get_rm_byte(emulator, params);
        unsigned int res;
        if (params.d)
        {
            res = reg_val - rm_val;
            emulator_set_reg_byte(emulator, params.reg, (unsigned short)res);
            proc->flags.o = get_overflow_minus_byte(reg_val, rm_val);
        }
        else
        {
            res = rm_val - reg_val;
            emulator_set_rm_byte(emulator, params, (unsigned short)res);
            proc->flags.o = get_overflow_minus_byte(rm_val, reg_val);
        }
        proc->flags.s = get_sign_byte(res);
        proc->flags.z = get_zero_byte(res);
        // TODO: implement a flag
        // proc->flags.a = ;
        proc->flags.p = get_parity_byte(res);
        proc->flags.c = get_carry_byte(res);
    }
}
