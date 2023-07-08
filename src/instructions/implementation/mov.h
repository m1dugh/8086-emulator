#ifndef EMULATOR_INSTRUCTIONS_IMPLEMENTATION_MOV_H
#define EMULATOR_INSTRUCTIONS_IMPLEMENTATION_MOV_H

#include "../../models/emulator.h"

void mov_rm_reg_exec(emulator_t *emulator, params_t params);
void mov_immediate_to_reg_exec(emulator_t *emulator, params_t params);

#endif // !EMULATOR_INSTRUCTIONS_IMPLEMENTATION_MOV_H
