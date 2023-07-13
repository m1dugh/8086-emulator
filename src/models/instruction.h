#ifndef EMULATOR_MODELS_INSTRUCTION_H
#define EMULATOR_MODELS_INSTRUCTION_H

#include <stdlib.h>

typedef struct emulator_t emulator_t;

typedef struct
{
    char d;
    char w;
    char mod;
    char reg;
    char rm;
    union
    {
        struct
        {
            unsigned char : 8;
            char low_disp;
        };
        short disp;
    };

    union
    {
        struct
        {
            unsigned char : 8;
            unsigned char data_low;
        };
        unsigned short data;
    };
} params_t;

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

#endif // !EMULATOR_MODELS_INSTRUCTION_H
