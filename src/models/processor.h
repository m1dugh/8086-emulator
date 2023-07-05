#ifndef EMULATOR_MODELS_PROCESSOR_H
#define EMULATOR_MODELS_PROCESSOR_H

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

#endif // !EMULATOR_MODELS_PROCESSOR_H
