#include "instruction.h"

instruction_t *instruction_new(char *display, unsigned char *instruction,
    size_t instruction_len, params_t params, instruction_cb_t callback)
{
    instruction_t *res = malloc(sizeof(instruction_t));
    if (res == NULL)
        return NULL;

    res->display = display;
    res->callback = callback;
    res->instruction_len = instruction_len;
    res->params = params;
    res->instruction = instruction;

    return res;
}

void instruction_free(instruction_t *instruction)
{
    free(instruction->display);
    free(instruction->instruction);
    free(instruction);
}
