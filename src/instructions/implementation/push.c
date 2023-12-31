#include "push.h"

void push_reg_exec(emulator_t *emulator, params_t params)
{
    unsigned short value = emulator_get_reg(emulator, params.reg);
    emulator_stack_push(emulator, value);
}

void push_rm_exec(emulator_t *emulator, params_t params)
{
    unsigned short value = emulator_get_rm(emulator, params);
    emulator_stack_push(emulator, value);
}
