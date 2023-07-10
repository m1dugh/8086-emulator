#ifndef EMULATOR_INSTRUCTIONS_IMPLEMENTATION_JUMP_H
#define EMULATOR_INSTRUCTIONS_IMPLEMENTATION_JUMP_H

#include "../../models/emulator.h"

void jnb_exec(emulator_t *emulator, params_t params);
void jne_exec(emulator_t *emulator, params_t params);
void jmp_exec(emulator_t *emulator, params_t params);
void jl_exec(emulator_t *emulator, params_t params);
void je_exec(emulator_t *emulator, params_t params);

#endif // !EMULATOR_INSTRUCTIONS_IMPLEMENTATION_JUMP_H
