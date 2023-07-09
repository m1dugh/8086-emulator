#include <malloc.h>
#include <stdio.h>
#include "implementation/processor_control.h"
#include "processor_control.h"
#include "utils.h"

#define CREATE_STRING(format)

instruction_t *hlt()
{
    char *res = malloc(4);
    snprintf(res, 4, "hlt");
    params_t params = {};
    unsigned char *instruction_buffer = malloc(1);
    instruction_buffer[0] = 0b11110100;
    return instruction_new(res, instruction_buffer, 1, params, hlt_exec);
}

char *cld()
{
    char *res = malloc(4);
    snprintf(res, 4, "cld");
    return res;
}

char *std_instruction()
{
    char *res = malloc(4);
    snprintf(res, 4, "std");
    return res;
}
