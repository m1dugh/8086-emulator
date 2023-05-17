#ifndef INSTRUCTIONS_UTILS_H
#define INSTRUCTIONS_UTILS_H

#include "../utils/binary_stream.h"

#define GET_DATA(val, data, size)   if(bs_next_reset(data, size, &val) != 0) {\
                                        return NULL; \
                                    }
 

#define GET_MOD(mod, data) GET_DATA(mod, data, 2)
#define GET_RM_VALUE(reg, data) GET_DATA(mod, data, 3)

#define GET_BIT(val, data) GET_DATA(val, data, 1)

struct params_t {
    char d;
    char w;
    char mod;
    char reg;
    char rm;
    char data_high;
    char data_low;
};

int extract_w(binary_stream_t *data, struct params_t *params);
int extract_d(binary_stream_t *data, struct params_t *params);
int extract_reg(binary_stream_t *data, struct params_t *params);
int extract_rm(binary_stream_t *data, struct params_t *params);
int extract_mod(binary_stream_t *data, struct params_t *params);

int extract_mod_reg_rm(binary_stream_t *data, struct params_t *params);
int extract_w_mod_reg_rm(binary_stream_t *data, struct params_t *params);
int extract_dw_mod_reg_rm(binary_stream_t *data, struct params_t *params);

short extract_data(binary_stream_t *data, struct params_t *params);
short extract_data_sw(binary_stream_t *data, struct params_t *params);

char *get_reg(char w, char val);

char *get_short_reg(char val);
char *get_word_reg(char val);

char *get_rm(binary_stream_t *data, char w, char mod, char val);

char *format_dw_rm_to_reg(char *val, binary_stream_t *data);

char *format_w_rm_to_reg(char *val, binary_stream_t *data);

char *format_rm_to_reg(char *val, binary_stream_t *data);

char *format_byte_displacement(char *val, binary_stream_t *data);
char *format_word_displacement(char *val, binary_stream_t *data);
char *format_reg(char *val, binary_stream_t *data);

#endif // !INSTRUCTIONS_UTILS_H
