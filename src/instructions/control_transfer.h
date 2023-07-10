#ifndef INSTRUCTIONS_CONTROL_TRANSFER_H
#define INSTRUCTIONS_CONTROL_TRANSFER_H

#include "../models/emulator.h"
#include "../utils/binary_stream.h"
#include "implementation/control_transfer.h"

// JUMP
instruction_t *jmp_direct_seg(binary_stream_t *data);
instruction_t *jmp_direct_seg_short(binary_stream_t *data);
instruction_t *jnb(binary_stream_t *data);
char *jbe(binary_stream_t *data);
char *jnl_instruction(binary_stream_t *data);
instruction_t *jne(binary_stream_t *data);
char *jnle(binary_stream_t *data);
char *jnbe(binary_stream_t *data);
instruction_t *je(binary_stream_t *data);
instruction_t *jl(binary_stream_t *data);
char *jle(binary_stream_t *data);
char *jb(binary_stream_t *data);
char *js(binary_stream_t *data);

// LOOP
char *loop(binary_stream_t *data);
char *loopz(binary_stream_t *data);
char *loopnz(binary_stream_t *data);

// CALL
instruction_t *call_direct_seg(binary_stream_t *data);

// INT
char *interrupt();
char *interrupt_with_code(binary_stream_t *data);

// RET
char *ret_data(binary_stream_t *data);
char *ret();

#endif /* !INSTRUCTIONS_CONTROL_TRANSFER_H */
