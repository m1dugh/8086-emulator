#include "inc.h"
#include "utils.h"

void inc_rm_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    if (params.w)
    {
        unsigned short value = emulator_get_rm(emulator, params);
        unsigned short res = value + 1;
        proc->flags.s = get_sign(res);
        proc->flags.z = get_zero(res);
        proc->flags.p = get_parity(res);
        // TODO: implement flags for auxiliary
        // proc->flags.a = ;
        proc->flags.o = get_overflow_plus(value, 1);
        emulator_set_rm(emulator, params, res);
    }
    else
    {
        unsigned char value = emulator_get_rm_byte(emulator, params);
        unsigned char res = value + 1;
        proc->flags.s = get_sign_byte(res);
        proc->flags.z = get_zero_byte(res);
        proc->flags.p = get_parity_byte(res);
        // TODO: implement flags for auxiliary
        // proc->flags.a = ;
        proc->flags.o = get_overflow_plus_byte(value, 1);
        emulator_set_rm_byte(emulator, params, res);
    }
}

void inc_reg_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    unsigned short value = emulator_get_reg(emulator, params.reg);
    unsigned short res = value + 1;
    proc->flags.s = get_sign(res);
    proc->flags.z = get_zero(res);
    proc->flags.p = get_parity(res);
    // TODO: implement flags for auxiliary
    // proc->flags.a = ;
    proc->flags.o = get_overflow_plus(value, 1);
    emulator_set_reg(emulator, params.reg, res);
}
