#ifndef EMULATOR_MODELS_H
#define EMULATOR_MODELS_H

#include <stdlib.h>
#include "env.h"
#include "utils/trie.h"
#include "utils/vector.h"

typedef struct
{
    union
    {
        struct
        {
#ifdef BIG_ENDIAN
            unsigned char ah;
            unsigned char al;
#else
            unsigned char al;
            unsigned char ah;
#endif
        };

        unsigned short ax;
    };

    union
    {
        struct
        {
#ifdef BIG_ENDIAN
            unsigned char ch;
            unsigned char cl;
#else
            unsigned char cl;
            unsigned char ch;
#endif
        };

        unsigned short cx;
    };

    union
    {
        struct
        {
#ifdef BIG_ENDIAN
            unsigned char dh;
            unsigned char dl;
#else
            unsigned char dl;
            unsigned char dh;
#endif
        };

        unsigned short dx;
    };

    union
    {
        struct
        {
#ifdef BIG_ENDIAN
            unsigned char bh;
            unsigned char bl;
#else
            unsigned char bl;
            unsigned char bh;
#endif
        };

        unsigned short bx;
    };

    unsigned short sp;
    unsigned short bp;
    unsigned short si;
    unsigned short di;

    union
    {
        unsigned short value;
        struct
        {
#ifdef BIG_ENDIAN
            // HIGH
            unsigned char : 4;
            unsigned char o : 1;
            unsigned char d : 1;
            unsigned char i : 1;
            unsigned char t : 1;

            // LOW
            unsigned char s : 1;
            unsigned char z : 1;
            unsigned char : 1;
            unsigned char a : 1;
            unsigned char : 1;
            unsigned char p : 1;
            unsigned char : 1;
            unsigned char c : 1;
#else
            // LOW
            unsigned char s : 1;
            unsigned char z : 1;
            unsigned char : 1;
            unsigned char a : 1;
            unsigned char : 1;
            unsigned char p : 1;
            unsigned char : 1;
            unsigned char c : 1;

            // HIGH
            unsigned char : 4;
            unsigned char o : 1;
            unsigned char d : 1;
            unsigned char i : 1;
            unsigned char t : 1;
#endif
        };
    } flags;

    unsigned short es;
    unsigned short cs;
    unsigned short ss;
    unsigned short ds;

    unsigned short ip;
} processor_t;

processor_t *processor_new();
int processor_set_byte_value(processor_t *, char key, unsigned char value);
int processor_set_value(processor_t *, char key, unsigned short value);
int processor_set_segment(processor_t *, char key, unsigned short value);
void processor_free(processor_t *);

char *processor_display(processor_t *processor);

typedef vector_t stack_t;

stack_t *stack_new();
void stack_free(stack_t *);
void stack_push(stack_t *, unsigned short value);
unsigned short stack_pop(stack_t *);

typedef struct
{
    char d;
    char w;
    char mod;
    char reg;
    char rm;
    char data_high;
    char data_low;
    union
    {
        struct
        {
            char low_disp;
            unsigned char : 8;
        };
        short disp;
    };
} params_t;

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
unsigned char mem_seg_get(memory_segment_t *mem, unsigned short address);

void mem_seg_set_abs(
    memory_segment_t *mem, unsigned short address, unsigned char value);
void mem_seg_set(
    memory_segment_t *mem, unsigned short address, unsigned char value);
unsigned short mem_seg_push(memory_segment_t *mem, unsigned char data);

void mem_seg_display(memory_segment_t *mem);
int mem_seg_empty(memory_segment_t *mem);

typedef struct
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

unsigned short emulator_push_environment(emulator_t *, unsigned char);
unsigned short emulator_push_args(emulator_t *, unsigned char);

void emulator_stack_push(emulator_t *, unsigned short value);
unsigned short emulator_stack_pop(emulator_t *);

unsigned char emulator_get_reg_byte(emulator_t *, char reg);
void emulator_set_reg_byte(emulator_t *, char reg, unsigned char);

unsigned short emulator_get_reg(emulator_t *, char reg);
void emulator_set_reg(emulator_t *, char reg, unsigned short);

unsigned char emulator_get_mem_byte(
    emulator_t *emulator, unsigned short address);
void emulator_set_mem_byte(
    emulator_t *emulator, unsigned short address, unsigned char value);

unsigned char emulator_get_rm_byte(emulator_t *, params_t params);
void emulator_set_rm_byte(emulator_t *, params_t params, unsigned char);

unsigned short emulator_get_rm(emulator_t *, params_t params);
void emulator_set_rm(emulator_t *, params_t params, unsigned short);

typedef void (*instruction_cb_t)(emulator_t *emulator, params_t params);

typedef struct
{
    char *display;
    unsigned char *instruction;
    size_t instruction_len;
    params_t params;
    instruction_cb_t callback;
} instruction_t;

instruction_t *instruction_new(char *display, unsigned char *instruction,
    size_t instruction_len, params_t params, instruction_cb_t callback);
void instruction_free(instruction_t *);

#endif // !EMULATOR_MODELS_H
