#ifndef EMULATOR_ENV_H
#define EMULATOR_ENV_H

#include <stdlib.h>

#define UNUSED(x) (void)(x)

#define MAX_ADDRESS 0xffff
#define MEM_SIZE MAX_ADDRESS + 1
#define PROCESSOR_HEADER " AX   BX   CX   DX   SP   BP   SI   DI  FLAGS"

#define TO_ADDR(val) (unsigned short)(unsigned long)val
#define TO_BYTE(val) (unsigned char)(unsigned long)val
#define TO_VOID_PTR(val) (void *)(unsigned long)val

#define EXTRA_SEGMENT 0x7000
#define CODE_SEGMENT 0x3000
#define STACK_SEGMENT 0x4fff
#define DATA_SEGMENT 0x2000

#endif // !EMULATOR_ENV_H
