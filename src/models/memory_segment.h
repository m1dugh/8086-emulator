#ifndef EMULATOR_MODELS_MEMORY_SEGMENT_H
#define EMULATOR_MODELS_MEMORY_SEGMENT_H

#include "../utils/vector.h"

#define DIRECTION_DOWN (unsigned char)1
#define DIRECTION_UP (unsigned char)0

typedef struct
{
    unsigned short base_address;
    unsigned char direction;
    vector_t *value;
    char locked;
} memory_segment_t;

memory_segment_t *mem_seg_new(
    unsigned short base_address, unsigned char direction);
void mem_seg_free(memory_segment_t *mem);
unsigned short mem_seg_high_addr(memory_segment_t *mem);
unsigned short mem_seg_low_addr(memory_segment_t *mem);

unsigned char mem_seg_get_abs(memory_segment_t *mem, unsigned short address);
unsigned short mem_seg_get_abs_word(
    memory_segment_t *mem, unsigned short address);

unsigned char mem_seg_get(memory_segment_t *mem, unsigned short address);
unsigned short mem_seg_get_word(memory_segment_t *mem, unsigned short address);

void mem_seg_set_abs(
    memory_segment_t *mem, unsigned short address, unsigned char value);
void mem_seg_set(
    memory_segment_t *mem, unsigned short address, unsigned char value);
unsigned short mem_seg_push(memory_segment_t *mem, unsigned char data);

void mem_seg_set_abs_word(
    memory_segment_t *mem, unsigned short address, unsigned short value);
void mem_seg_set_word(
    memory_segment_t *mem, unsigned short address, unsigned short value);

void mem_seg_display(memory_segment_t *mem);
int mem_seg_empty(memory_segment_t *mem);

#endif // !EMULATOR_MODELS_MEMORY_SEGMENT_H
