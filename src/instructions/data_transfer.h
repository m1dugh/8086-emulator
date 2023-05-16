#ifndef INSTRUCTIONS_DATA_TRANSFER_H
#define INSTRUCTIONS_DATA_TRANSFER_H

#include "../utils/binary_stream.h"

// MOV
char *mov_rm_to_reg(binary_stream_t *data);
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
char *push_rm(binary_stream_t *stream);
char *push_reg(binary_stream_t *stream);
char *push_seg_reg(binary_stream_t *stream);

// LEA
char *lea(binary_stream_t *stream);

#endif // !INSTRUCTIONS_DATA_TRANSFER_H
