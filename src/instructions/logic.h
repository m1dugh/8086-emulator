#ifndef INSTRUCTIONS_LOGIC_H
#define INSTRUCTIONS_LOGIC_H

#include "../utils/binary_stream.h"

// NOT
char *invert(binary_stream_t *data);

// SHIFT
char *shift_left(binary_stream_t *data);
char *shift_arith_right(binary_stream_t *data);
char *shift_logic_right(binary_stream_t *data);

// AND
char *and_rm_reg(binary_stream_t *data);
char *and_immediate_rm(binary_stream_t *data);
char *and_immediate_acc(binary_stream_t *data);

// XOR
char *xor_rm_reg(binary_stream_t *data);
char *xor_immediate_rm(binary_stream_t *data);
char *xor_immediate_acc(binary_stream_t *data);

#endif // !INSTRUCTIONS_LOGIC_H
