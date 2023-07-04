#ifndef EMULATOR_MODELS_H
#define EMULATOR_MODELS_H
#include <stdlib.h>
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

typedef vector_t stack_t;

stack_t *stack_new();
void stack_free(stack_t *);
void stack_push(stack_t *, void *);
void *stack_pop(stack_t *);

struct params_t
{
    char d;
    char w;
    char mod;
    char reg;
    char rm;
    char data_high;
    char data_low;
};

typedef struct
{
    char *display;
    struct params_t params;
} instruction_t;

typedef struct
{
    size_t size;
    unsigned short *value;
} memory_seg_t;

memory_seg_t *memory_new();

#endif // !EMULATOR_MODELS_H
