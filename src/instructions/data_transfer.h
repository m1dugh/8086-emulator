#ifndef INSTRUCTIONS_DATA_TRANSFER_H
#define INSTRUCTIONS_DATA_TRANSFER_H

#include "../models/emulator.h"
#include "../utils/binary_stream.h"

// MOV
instruction_t *mov_rm_to_reg(binary_stream_t *data);
char *mov_immediate_to_rm(binary_stream_t *data);
char *mov_immediate_to_reg(binary_stream_t *data);
char *mov_mem_to_acc(binary_stream_t *data);
char *mov_acc_to_mem(binary_stream_t *data);
char *mov_rm_to_seg_reg(binary_stream_t *data);
char *mov_seg_reg_to_rm(binary_stream_t *data);

// POP
char *pop_rm(binary_stream_t *stream);
char *pop_reg(binary_stream_t *stream);
char *pop_seg_reg(binary_stream_t *stream);

// PUSH
char *push_reg(binary_stream_t *stream);
char *push_seg_reg(binary_stream_t *stream);

// LEA
char *lea(binary_stream_t *stream);

// IN/OUT
char *in_fixed_port(binary_stream_t *stream);
char *in_var_port(binary_stream_t *stream);
char *out_fixed_port(binary_stream_t *data);
char *out_var_port(binary_stream_t *data);

// XCHG
char *xchg_rm_with_reg(binary_stream_t *data);
char *xchg_reg(binary_stream_t *data);

#endif // !INSTRUCTIONS_DATA_TRANSFER_H
