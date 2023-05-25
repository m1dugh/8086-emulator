#ifndef INSTRUCTIONS_CONTROL_TRANSFER_H
#define INSTRUCTIONS_CONTROL_TRANSFER_H

#include "../utils/binary_stream.h"

// JUMP
char *jmp_direct_seg(binary_stream_t *data);
char *jmp_direct_seg_short(binary_stream_t *data);
char *jnb(binary_stream_t *data);
char *jbe(binary_stream_t *data);
char *jnl(binary_stream_t *data);
char *jne(binary_stream_t *data);
char *jnle(binary_stream_t *data);
char *jnbe(binary_stream_t *data);
char *je(binary_stream_t *data);
char *jl(binary_stream_t *data);
char *jle(binary_stream_t *data);
char *jb(binary_stream_t *data);

// LOOP
char *loop(binary_stream_t *data);
char *loopz(binary_stream_t *data);
char *loopnz(binary_stream_t *data);

// CALL
char *call_direct_seg(binary_stream_t *data);

// INT
char *interrupt();
char *interrupt_with_code(binary_stream_t *data);

// RET
char *ret_data(binary_stream_t *data);
char *ret();

#endif /* !INSTRUCTIONS_CONTROL_TRANSFER_H */
