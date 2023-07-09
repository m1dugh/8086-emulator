#include "jump.h"

void jump_exec(emulator_t *emulator, params_t params, unsigned char condition)
{
    if (condition)
        emulator->processor->ip = params.data;
}

void jnb_exec(emulator_t *emulator, params_t params)
{
    jump_exec(emulator, params, emulator->processor->flags.c == 0);
}

void jne_exec(emulator_t *emulator, params_t params)
{
    jump_exec(emulator, params, emulator->processor->flags.z == 0);
}
