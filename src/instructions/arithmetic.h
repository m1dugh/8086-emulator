#ifndef INSTRUCTIONS_ARITHMETIC_H
#define INSTRUCTIONS_ARITHMETIC_H

#include "../utils/binary_stream.h"

// ADD
char *add_rm_with_reg(binary_stream_t *data);
char *add_immediate_to_rm(binary_stream_t *data);
char *add_immediate_to_acc(binary_stream_t *data);

// ADC
char *adc_rm_with_reg(binary_stream_t *data);
char *adc_immediate_to_rm(binary_stream_t *data);
char *adc_immediate_to_acc(binary_stream_t *data);

// SUB
char *sub_immediate_to_acc(binary_stream_t *data);

// SSB
char *ssb_rm_with_reg(binary_stream_t *data);

// INC
char *inc_rm(binary_stream_t *data);
char *inc_reg(binary_stream_t *data);

// DEC
char *dec_rm(binary_stream_t *data);
char *dec_reg(binary_stream_t *data);

// NEG
char *neg(binary_stream_t *data);

// CMP
char *cmp_rm_reg(binary_stream_t *data);
char *cmp_immediate_rm(binary_stream_t *data);
char *cmp_immeditate_acc(binary_stream_t *data);

char *cbw();
char *cwd();

#endif // !INSTRUCTIONS_ARITHMETIC_H
