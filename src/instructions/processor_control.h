#ifndef INSTRUCTIONS_PROCESSOR_CONTROL_H
#define INSTRUCTIONS_PROCESSOR_CONTROL_H

#include "../models/emulator.h"
#include "implementation/processor_control.h"

instruction_t *hlt();
char *cld();
char *std_instruction();

#endif // !INSTRUCTIONS_PROCESSOR_CONTROL_H
