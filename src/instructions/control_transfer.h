#ifndef INSTRUCTIONS_CONTROL_TRANSFER_H
#define INSTRUCTIONS_CONTROL_TRANSFER_H

#include "../models/emulator.h"
#include "../utils/binary_stream.h"
#include "implementation/control_transfer.h"

// JUMP
instruction_t *jmp_direct_seg(binary_stream_t *data);
instruction_t *jmp_direct_seg_short(binary_stream_t *data);
instruction_t *jnb(binary_stream_t *data);
instruction_t *jbe(binary_stream_t *data);
instruction_t *jnl_instruction(binary_stream_t *data);
instruction_t *jne(binary_stream_t *data);
instruction_t *jnle(binary_stream_t *data);
instruction_t *jnbe(binary_stream_t *data);
instruction_t *je(binary_stream_t *data);
instruction_t *jl(binary_stream_t *data);
instruction_t *jle(binary_stream_t *data);
instruction_t *jb(binary_stream_t *data);
char *js(binary_stream_t *data);

// LOOP
char *loop(binary_stream_t *data);
char *loopz(binary_stream_t *data);
char *loopnz(binary_stream_t *data);

// CALL
instruction_t *call_direct_seg(binary_stream_t *data);

// INT
char *interrupt();
instruction_t *interrupt_with_code(binary_stream_t *data);

// RET
instruction_t *ret_seg_data(binary_stream_t *data);
instruction_t *ret_seg(binary_stream_t *data);

#endif /* !INSTRUCTIONS_CONTROL_TRANSFER_H */
