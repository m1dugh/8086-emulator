#ifndef EMULATOR_MODELS_PROCESSOR_H
#define EMULATOR_MODELS_PROCESSOR_H

#include <stdlib.h>
#include "../env.h"

#define DS_SELECTOR 0b000
#define CS_SELECTOR 0b001
#define ES_SELECTOR 0b010
#define SS_SELECTOR 0b011
#define NO_SELECTOR 0b100

typedef struct
{
    union
    {
        struct
        {
            unsigned char al;
            unsigned char ah;
        };

        unsigned short ax;
    };

    union
    {
        struct
        {
            unsigned char cl;
            unsigned char ch;
        };

        unsigned short cx;
    };

    union
    {
        struct
        {
            unsigned char dl;
            unsigned char dh;
        };

        unsigned short dx;
    };

    union
    {
        struct
        {
            unsigned char bl;
            unsigned char bh;
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
        };
    } flags;

    unsigned short es;
    unsigned short cs;
    unsigned short ss;
    unsigned short ds;

    unsigned short ip;

    unsigned int segment_selector : 3;
} processor_t;

processor_t *processor_new();
int processor_set_byte_value(processor_t *, char key, unsigned char value);
int processor_set_value(processor_t *, char key, unsigned short value);
int processor_set_segment(processor_t *, char key, unsigned short value);
void processor_free(processor_t *);

unsigned short processor_segment_addr(processor_t *processor);

char *processor_display(processor_t *processor);

#endif // !EMULATOR_MODELS_PROCESSOR_H
