#include "neg.h"
#include "utils.h"

void neg_immediate_rm_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    if (params.w)
    {
        unsigned short value = emulator_get_rm(emulator, params);
        unsigned short res = 0 - value;
        proc->flags.c = value != 0;
        proc->flags.o = get_overflow_minus(0, value);
        proc->flags.s = get_sign(res);
        proc->flags.z = get_zero(res);
        proc->flags.p = get_parity(res);
        // TODO: implement auxiliary flag
        // proc->flags.a = /* something */;
        emulator_set_rm(emulator, params, res);
    }
    else
    {
        unsigned char value = emulator_get_rm_byte(emulator, params);
        unsigned char res = 0 - value;
        proc->flags.c = value != 0;
        proc->flags.o = get_overflow_minus_byte(0, value);
        proc->flags.s = get_sign_byte(res);
        proc->flags.z = get_zero_byte(res);
        proc->flags.p = get_parity_byte(res);
        // TODO: implement auxiliary flag
        // proc->flags.a = /* something */;
        emulator_set_rm_byte(emulator, params, res);
    }
}
