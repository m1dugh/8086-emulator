#ifndef EMULATOR_ENV_H
#define EMULATOR_ENV_H

#include <stdlib.h>

#define UNUSED(x) (void)(x)

#define MAX_ADDRESS 0xffff
#define MEM_SIZE MAX_ADDRESS + 1
#define STACK_SIZE MEM_SIZE
#define PROCESSOR_HEADER_LONG \
    " AX   BX   CX   DX   SP   BP   SI   DI   CS   SS   DS   ES  FLAGS"

#define PROCESSOR_HEADER " AX   BX   CX   DX   SP   BP   SI   DI  FLAGS"

#define TO_ADDR(val) (unsigned short)(unsigned long)val
#define TO_BYTE(val) (unsigned char)(unsigned long)val
#define TO_VOID_PTR(val) (void *)(unsigned long)val

#define EXTRA_SEGMENT 0x7000
#define CODE_SEGMENT 0x3000
#define STACK_SEGMENT 0x5000
#define DATA_SEGMENT 0x2000

#define AX 0b000
#define CX 0b001
#define DX 0b010
#define BX 0b011
#define SP 0b100
#define BP 0b101
#define SI 0b110
#define DI 0b111

#define AL 0b000
#define CL 0b001
#define DL 0b010
#define BL 0b011
#define AH 0b100
#define CH 0b101
#define DH 0b110
#define BH 0b111

#define ES 0b00
#define CS 0b01
#define SS 0b10
#define DS 0b11

#endif // !EMULATOR_ENV_H
