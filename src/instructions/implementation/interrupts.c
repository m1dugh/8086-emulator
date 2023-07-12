#include "interrupts.h"

void interrupt_with_code_exec(emulator_t *emulator, params_t params)
{
    emulator_syscall(emulator);
}
