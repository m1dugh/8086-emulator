#ifndef EMULATOR_INSTRUCTIONS_IMPLEMENTATION_TEST_H
#define EMULATOR_INSTRUCTIONS_IMPLEMENTATION_TEST_H

#include "../../models/emulator.h"
#include "utils.h"

void test_immediate_rm_exec(emulator_t *emulator, params_t params);
void test_rm_reg_exec(emulator_t *emulator, params_t params);

#endif // !EMULATOR_INSTRUCTIONS_IMPLEMENTATION_TEST_H
