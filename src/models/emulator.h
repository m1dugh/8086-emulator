#ifndef EMULATOR_MODELS_EMULATOR_H
#define EMULATOR_MODELS_EMULATOR_H

#include "../utils/trie.h"
#include "instruction.h"
#include "memory_segment.h"
#include "processor.h"

typedef struct emulator_t
{
    processor_t *processor;
    memory_segment_t *extra;
    stack_t *stack;
    code_segment_t *code;
    memory_segment_t *data;
} emulator_t;

emulator_t *emulator_new();
void emulator_free(emulator_t *);

void emulator_prepare(emulator_t *, vector_t *environment, vector_t *args);

unsigned short emulator_push_data(emulator_t *, unsigned char);

unsigned short emulator_push_bss(emulator_t *, unsigned char);

unsigned short emulator_push_data_str(emulator_t *, char *str);

void emulator_stack_push(emulator_t *, unsigned short value);
unsigned short emulator_stack_pop(emulator_t *);

unsigned char emulator_get_reg_byte(emulator_t *, char reg);
void emulator_set_reg_byte(emulator_t *, char reg, unsigned char);

unsigned short emulator_get_reg(emulator_t *, char reg);
void emulator_set_reg(emulator_t *, char reg, unsigned short);

unsigned int emulator_get_physical_addr(emulator_t *, params_t params);

unsigned char emulator_get_mem_byte(
    emulator_t *emulator, unsigned int address);
void emulator_set_mem_byte(
    emulator_t *emulator, unsigned int address, unsigned char value);

unsigned short emulator_get_mem_word(
    emulator_t *emulator, unsigned int address);
void emulator_set_mem_word(
    emulator_t *emulator, unsigned int address, unsigned short value);

unsigned char emulator_get_rm_byte(emulator_t *, params_t params);
void emulator_set_rm_byte(emulator_t *, params_t params, unsigned char);

unsigned short emulator_get_rm(emulator_t *, params_t params);
void emulator_set_rm(emulator_t *, params_t params, unsigned short);

#endif // !EMULATOR_MODELS_EMULATOR_H
