#include "push.h"

void push_reg_exec(emulator_t *emulator, params_t params)
{
    unsigned short value = emulator_get_reg(emulator, params.reg);
    emulator_stack_push(emulator, value);
}
