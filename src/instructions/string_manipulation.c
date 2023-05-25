#include <malloc.h>
#include <stdio.h>
#include "string_manipulation.h"
#include "utils.h"

char *get_string_instruction(char instruction, binary_stream_t *data)
{

    char *format;

    switch (instruction)
    {
        case 0b1010010:
            format = "movs%c";
            break;
        case 0b1010011:
            format = "cmps%c";
            break;
        case 0b1010101:
            format = "stos%c";
            break;
        case 0b1010110:
            format = "lods%c";
            break;
        case 0b1010111:
            format = "scas%c";
            break;
        default:
            return NULL;
    };

    struct params_t params;
    if (extract_w(data, &params) != 0)
        return NULL;

    char *res = malloc(6);
    snprintf(res, 6, format, params.w ? 'w' : 'b');
    return res;
}

char *rep_string(binary_stream_t *data)
{
    struct params_t params;
    if (extract_w(data, &params) != 0)
    {
        return NULL;
    }

    char instruction;
    if (bs_next_reset(data, 7, &instruction) != 0)
    {
        return NULL;
    }

    char *next_ins = get_string_instruction(instruction, data);
    if (next_ins == NULL)
        return NULL;

    char *res = malloc(20);
    snprintf(res, 20, "rep %s", next_ins);
    free(next_ins);

    return res;
}
