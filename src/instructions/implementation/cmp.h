#ifndef EMULATOR_INSTRUCTIONS_IMPLEMENTATION_H
#define EMULATOR_INSTRUCTIONS_IMPLEMENTATION_H
#include "../../models/emulator.h"

void cmp_immediate_rm_exec(emulator_t *emulator, params_t params);
void cmp_rm_reg_exec(emulator_t *emulator, params_t params);
void cmp_immediate_acc_exec(emulator_t *emulator, params_t params);

#endif // !EMULATOR_INSTRUCTIONS_IMPLEMENTATION_H
