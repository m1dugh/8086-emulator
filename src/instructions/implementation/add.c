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
        proc->flags.o = get_overflow_plus(reg_val, rm_val);
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
        proc->flags.c = get_carry_byte(res);
        proc->flags.s = get_sign_byte(res);
        proc->flags.o = get_overflow_plus_byte(reg_val, rm_val);
        proc->flags.z = get_zero_byte(res);
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

void add_immediate_rm_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    if (params.w)
    {
        unsigned short rm_val = emulator_get_rm(emulator, params);
        unsigned short value = params.data;
        unsigned int res = rm_val + value;
        proc->flags.a = (value & 0x8) && (rm_val & 0x8);
        proc->flags.c = get_carry(res);
        proc->flags.s = get_sign(res);
        proc->flags.o = get_overflow_plus(value, rm_val);
        proc->flags.z = get_zero(res);
        emulator_set_rm(emulator, params, res);
    }
    else
    {
        unsigned char rm_val = emulator_get_rm_byte(emulator, params);
        unsigned char value = params.data_low;
        unsigned short res = value + rm_val;
        proc->flags.a = (value & 0x8) && (rm_val & 0x8);
        proc->flags.c = get_carry_byte(res);
        proc->flags.s = get_sign_byte(res);
        proc->flags.o = get_overflow_plus_byte(value, rm_val);
        proc->flags.z = get_zero_byte(res);
        emulator_set_rm_byte(emulator, params, res);
    }
}
