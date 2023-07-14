#ifndef EMULATOR_INSTRUCTIONS_IMPLEMENTATION_SUB_H
#define EMULATOR_INSTRUCTIONS_IMPLEMENTATION_SUB_H

#include "../../models/emulator.h"

void sub_immediate_rm_exec(emulator_t *emulator, params_t params);
void sub_rm_reg_exec(emulator_t *emulator, params_t params);
void sub_immediate_acc_exec(emulator_t *emulator, params_t params);

#endif // !EMULATOR_INSTRUCTIONS_IMPLEMENTATION_SUB_H
