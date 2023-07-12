#include <err.h>
#include "call.h"
#include "../../env.h"

void call_direct_seg_exec(emulator_t *emulator, params_t params)
{
    emulator_stack_push(emulator, emulator->processor->ip);
    emulator->processor->ip = params.data;
    // emulator->processor->segment_selector = NO_SELECTOR;
}

void call_indirect_seg_exec(emulator_t *emulator, params_t params)
{
    emulator_stack_push(emulator, emulator->processor->ip);
    unsigned short value = emulator_get_rm(emulator, params);
    emulator->processor->ip = value;
    // emulator->processor->segment_selector = NO_SELECTOR;
}

void ret_seg_exec(emulator_t *emulator, params_t params)
{
    params.data = 0;
    ret_seg_immediate_exec(emulator, params);
    // emulator->processor->segment_selector = NO_SELECTOR;
}

void ret_seg_immediate_exec(emulator_t *emulator, params_t params)
{
    unsigned short released = params.data;
    if (released % 2 != 0)
    {
        errx(-1, "ret: cannot release odd number of stack values");
    }
    for (unsigned short i = 0; i < released; i += 2)
    {
        emulator_stack_pop(emulator);
    }
    unsigned short ip = emulator_stack_pop(emulator);
    emulator->processor->ip = ip;
    // emulator->processor->segment_selector = NO_SELECTOR;
}
