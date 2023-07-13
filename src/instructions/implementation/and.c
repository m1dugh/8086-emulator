#include "and.h"
#include "utils.h"

void and_immediate_rm_exec(emulator_t *emulator, params_t params)
{

    processor_t *proc = emulator->processor;
    proc->flags.o = 0;
    proc->flags.c = 0;
    if (params.w)
    {
        unsigned short value = emulator_get_rm(emulator, params);
        unsigned short res = params.data & value;
        proc->flags.s = get_sign(res);
        proc->flags.z = get_zero(res);
        proc->flags.p = get_parity(res);
        emulator_set_rm(emulator, params, res);
    }
    else
    {
        unsigned char value = emulator_get_rm_byte(emulator, params);
        unsigned char res = params.data & value;
        proc->flags.s = get_sign_byte(res);
        proc->flags.z = get_zero_byte(res);
        proc->flags.p = get_parity_byte(res);
        emulator_set_rm_byte(emulator, params, res);
    }
}
