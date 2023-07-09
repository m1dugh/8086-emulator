#include "call.h"

void call_direct_seg_exec(emulator_t *emulator, params_t params)
{
    emulator->processor->ip = params.data;
}
