#ifndef EMULATOR_INSTRUCTIONS_IMPLEMENTATION_CALL_H
#define EMULATOR_INSTRUCTIONS_IMPLEMENTATION_CALL_H

#include "../../models/emulator.h"

void call_direct_seg_exec(emulator_t *emulator, params_t params);

void call_indirect_seg_exec(emulator_t *emulator, params_t params);

#endif // !EMULATOR_INSTRUCTIONS_IMPLEMENTATION_CALL_H
