#include "shift.h"
#include "utils.h"

void shl_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    if (params.w)
    {
        unsigned short old_value = emulator_get_rm(emulator, params);
        unsigned short res = old_value << 1;
        proc->flags.c = (old_value & 0x8000) > 0;
        proc->flags.z = get_zero(res);
        proc->flags.o = (res & 0x8000) != (old_value & 0x8000);
        proc->flags.p = get_parity(res);
        emulator_set_rm(emulator, params, res);
    }
    else
    {
        unsigned char old_value = emulator_get_rm_byte(emulator, params);
        unsigned char res = old_value << 1;
        proc->flags.c = (old_value & 0x80) > 0;
        proc->flags.z = get_zero_byte(res);
        proc->flags.o = (res & 0x80) != (old_value & 0x80);
        proc->flags.p = get_parity_byte(res);
        emulator_set_rm_byte(emulator, params, res);
    }
}
