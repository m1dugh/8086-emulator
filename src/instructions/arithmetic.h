#ifndef INSTRUCTIONS_ARITHMETIC_H
#define INSTRUCTIONS_ARITHMETIC_H

#include "../models/instruction.h"
#include "../utils/binary_stream.h"
#include "implementation/arithmetic.h"
#include "implementation/call.h"
#include "utils.h"

// ADD
instruction_t *add_rm_with_reg(binary_stream_t *data);
char *add_immediate_to_rm(binary_stream_t *data);
char *add_immediate_to_acc(binary_stream_t *data);

// ADC
char *adc_rm_with_reg(binary_stream_t *data);
char *adc_immediate_to_rm(binary_stream_t *data);
char *adc_immediate_to_acc(binary_stream_t *data);

// SUB
instruction_t *sub_rm_with_reg(binary_stream_t *data);
instruction_t *sub_immediate_to_acc(binary_stream_t *data);

// SSB
char *ssb_rm_with_reg(binary_stream_t *data);

// INC
instruction_t *inc_rm(binary_stream_t *data);
instruction_t *inc_reg(binary_stream_t *data);

// DEC
char *dec_rm(binary_stream_t *data);
instruction_t *dec_reg(binary_stream_t *data);

// NEG
char *neg(binary_stream_t *data);

// CMP
instruction_t *cmp_rm_reg(binary_stream_t *data);
instruction_t *cmp_immediate_rm(binary_stream_t *data);
instruction_t *cmp_immediate_acc(binary_stream_t *data);

instruction_t *cbw(binary_stream_t *data);
instruction_t *cwd(binary_stream_t *data);

#endif // !INSTRUCTIONS_ARITHMETIC_H
