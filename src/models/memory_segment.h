#ifndef EMULATOR_MODELS_MEMORY_SEGMENT_H
#define EMULATOR_MODELS_MEMORY_SEGMENT_H

#include "../utils/trie.h"
#include "../utils/vector.h"
#include "instruction.h"

#define DIRECTION_DOWN (unsigned char)1
#define DIRECTION_UP (unsigned char)0

typedef struct
{
    /// The bottom address of the memory segment
    unsigned short base_address;

    /// The vector containing all the data
    vector_t *value;
} memory_segment_t;

memory_segment_t *mem_seg_new(unsigned short base_address);
void mem_seg_free(memory_segment_t *mem);
unsigned int mem_seg_high_addr(memory_segment_t *mem);
unsigned int mem_seg_low_addr(memory_segment_t *mem);

unsigned char mem_seg_get(memory_segment_t *mem, unsigned short address);
unsigned short mem_seg_get_word(memory_segment_t *mem, unsigned short address);

/// if ret is < 0, the address is not valid
/// otherwise, the address is an unsigned short in lower part of the return
int mem_set_get_relative_address(memory_segment_t *mem, unsigned int address);

void mem_seg_set(
    memory_segment_t *mem, unsigned short address, unsigned char value);
unsigned short mem_seg_push(memory_segment_t *mem, unsigned char data);
unsigned short mem_seg_push_str(memory_segment_t *mem, char *str);

void mem_seg_set_word(
    memory_segment_t *mem, unsigned short address, unsigned short value);

void mem_seg_display(memory_segment_t *mem, char *name);
int mem_seg_empty(memory_segment_t *mem);

typedef struct
{
    /// The bottom address of the memory segment
    unsigned short base_address;

    trie_t *instructions;
    size_t max_size;
} code_segment_t;

code_segment_t *code_seg_new(unsigned short base_address);
void code_seg_free(code_segment_t *instructions);

void *code_seg_get(code_segment_t *instructions, unsigned short address);
int code_seg_set(code_segment_t *instructions, unsigned short address,
    instruction_t *instruction);

unsigned short code_seg_last_address(code_segment_t *instructions);

typedef void (*code_seg_cb_t)(
    instruction_t *instruction, unsigned long address, void *additionals);

void code_seg_for_each(
    code_segment_t *instructions, code_seg_cb_t function, void *additionals);

#endif // !EMULATOR_MODELS_MEMORY_SEGMENT_H
