#include "pop.h"

void pop_reg_exec(emulator_t *emulator, params_t params)
{
    unsigned short value = emulator_stack_pop(emulator);
    emulator_set_reg(emulator, params.reg, value);
}
