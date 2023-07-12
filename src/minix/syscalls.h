#ifndef EMULATOR_MINIX_SYSCALLS_H
#define EMULATOR_MINIX_SYSCALLS_H

#include "../models/emulator.h"
#include "env.h"

short syscall_exit(emulator_t *emulator);
short syscall_write(emulator_t *emulator);

#endif // !EMULATOR_MINIX_SYSCALLS_H
