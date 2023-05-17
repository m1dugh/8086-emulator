#ifndef INSTRUCTIONS_CONTROL_TRANSFER_H
#define INSTRUCTIONS_CONTROL_TRANSFER_H

#include "../utils/binary_stream.h"

// JUMP
char *jmp_direct_seg(binary_stream_t *data);
char *jnb(binary_stream_t *data);
char *jne(binary_stream_t *data);
char *jl(binary_stream_t *data);

// CALL
char *call_direct_seg(binary_stream_t *data);


#endif /* !INSTRUCTIONS_CONTROL_TRANSFER_H */
