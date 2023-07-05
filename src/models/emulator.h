#ifndef EMULATOR_MODELS_EMULATOR_H
#define EMULATOR_MODELS_EMULATOR_H

#include "../utils/trie.h"
#include "instruction.h"
#include "memory_segment.h"
#include "processor.h"

typedef struct emulator_t
{
    processor_t *processor;
    unsigned short top_address;
    memory_segment_t *environment;
    memory_segment_t *args;
    memory_segment_t *stack;
    trie_t *heap;
    memory_segment_t *bss;
    memory_segment_t *data;
    trie_t *text;
    unsigned char text_locked;
} emulator_t;

emulator_t *emulator_new(size_t memory_size);
void emulator_free(emulator_t *);

void emulator_prepare(emulator_t *, vector_t *environment, vector_t *args);

unsigned short emulator_push_data(emulator_t *, unsigned char);

unsigned short emulator_push_bss(emulator_t *, unsigned char);

unsigned short emulator_push_environment(emulator_t *, char *str);
unsigned short emulator_push_args(emulator_t *, char *str);

void emulator_stack_push(emulator_t *, unsigned short value);
unsigned short emulator_stack_pop(emulator_t *);

unsigned char emulator_get_reg_byte(emulator_t *, char reg);
void emulator_set_reg_byte(emulator_t *, char reg, unsigned char);

unsigned short emulator_get_reg(emulator_t *, char reg);
void emulator_set_reg(emulator_t *, char reg, unsigned short);

unsigned short emulator_get_effective_addr(emulator_t *, params_t params);

unsigned char emulator_get_mem_byte(
    emulator_t *emulator, unsigned short address);
void emulator_set_mem_byte(
    emulator_t *emulator, unsigned short address, unsigned char value);

unsigned short emulator_get_mem_word(
    emulator_t *emulator, unsigned short address);
void emulator_set_mem_word(
    emulator_t *emulator, unsigned short address, unsigned short value);

unsigned char emulator_get_rm_byte(emulator_t *, params_t params);
void emulator_set_rm_byte(emulator_t *, params_t params, unsigned char);

unsigned short emulator_get_rm(emulator_t *, params_t params);
void emulator_set_rm(emulator_t *, params_t params, unsigned short);

#endif // !EMULATOR_MODELS_EMULATOR_H
