#include "call.h"

void call_direct_seg_exec(emulator_t *emulator, params_t params)
{
    emulator_stack_push(emulator, emulator->processor->ip);
    emulator->processor->ip = params.data;
}

void call_indirect_seg_exec(emulator_t *emulator, params_t params)
{
    emulator_stack_push(emulator, emulator->processor->ip);
    unsigned short value = emulator_get_rm(emulator, params);
    emulator->processor->ip = value;
}
