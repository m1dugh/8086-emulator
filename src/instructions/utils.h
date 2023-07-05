#ifndef INSTRUCTIONS_UTILS_H
#define INSTRUCTIONS_UTILS_H

#include <string.h>
#include "../models/emulator.h"
#include "../utils/binary_stream.h"

#define GET_DATA(val, data, size)             \
    if (bs_next_reset(data, size, &val) != 0) \
    {                                         \
        return NULL;                          \
    }

#define GET_MOD(mod, data) GET_DATA(mod, data, 2)
#define GET_RM_VALUE(reg, data) GET_DATA(mod, data, 3)

#define GET_BIT(val, data) GET_DATA(val, data, 1)

#define RET_INSTRUCTION(display, data, params, cb)                     \
    unsigned char *instruction_buffer_copy                             \
        = calloc(data->instruction_buffer_len, sizeof(unsigned char)); \
    memcpy(instruction_buffer_copy, data->instruction_buffer,          \
        data->instruction_buffer_len);                                 \
    return instruction_new(display, instruction_buffer_copy,           \
        data->instruction_buffer_len, params, cb);

int extract_w(binary_stream_t *data, params_t *params);
int extract_d(binary_stream_t *data, params_t *params);
int extract_reg(binary_stream_t *data, params_t *params);
int extract_rm(binary_stream_t *data, params_t *params);
int extract_mod(binary_stream_t *data, params_t *params);

int extract_mod_reg_rm(binary_stream_t *data, params_t *params);
int extract_w_mod_reg_rm(binary_stream_t *data, params_t *params);
int extract_dw_mod_reg_rm(binary_stream_t *data, params_t *params);

short extract_data(binary_stream_t *data, params_t *params);
char extract_byte(binary_stream_t *data);
short extract_data_sw(binary_stream_t *data, params_t *params);

char *get_reg(char w, char val);

char *get_short_reg(char val);
char *get_word_reg(char val);

char *get_rm(binary_stream_t *data, params_t *params);

char *format_dw_rm_to_reg(char *val, binary_stream_t *data, params_t *params);
char *format_sized_dw_rm_to_reg(
    char *val, binary_stream_t *data, params_t *params);

char *format_w_rm_to_reg(char *val, binary_stream_t *data, params_t *params);
char *format_w_rm_to_reg_d(char *val, binary_stream_t *data, params_t *params);

char *format_rm_to_reg(char *val, binary_stream_t *data, params_t *params);
char *format_immediate_from_acc(
    char *val, binary_stream_t *data, params_t *params);
char *format_w_immediate_to_rm(
    char *val, binary_stream_t *data, params_t *params);

char *format_byte_displacement(
    char *val, binary_stream_t *data, params_t *params);
char *format_word_displacement(
    char *val, binary_stream_t *data, params_t *params);
char *format_reg(char *val, binary_stream_t *data, params_t *params);
char *format_reg_to_acc(char *val, binary_stream_t *data, params_t *params);

char *format_w_mem_to_acc(char *val, binary_stream_t *data, params_t *params);

#endif // !INSTRUCTIONS_UTILS_H
