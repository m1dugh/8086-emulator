#include "interrupts.h"

void interrupt_with_code_exec(emulator_t *emulator, params_t params)
{
    // TODO: implement proper interrupt based on code
    UNUSED(params);
    emulator_syscall(emulator);
}
