#ifndef EMULATOR_INSTRUCTIONS_IMPLEMENTATION_OR_H
#define EMULATOR_INSTRUCTIONS_IMPLEMENTATION_OR_H

#include "../../models/emulator.h"

void or_rm_reg_exec(emulator_t *emulator, params_t params);
void or_immediate_rm_exec(emulator_t *emulator, params_t params);

#endif // !EMULATOR_INSTRUCTIONS_IMPLEMENTATION_OR_H
