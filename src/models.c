#include <err.h>
#include <malloc.h>
#include <stdio.h>
#include "models.h"
#include "utils/trie.h"
#include "utils/vector.h"

#define PROC_REG_SWITCH(key, reg_name) \
    case key:                          \
        proc->reg_name = value;        \
        break;

processor_t *processor_new()
{
    processor_t *res = calloc(1, sizeof(processor_t));
    res->sp = MAX_ADDRESS;
    return res;
}

// 45 corresponds to the number of bytes to print
// 8 registers printed on 4 bytes each and flags register
// space separated and an additional null byte
#define PROCESSOR_DISPLAY_LEN 45

char *processor_display(processor_t *processor)
{
    char *res = calloc(PROCESSOR_DISPLAY_LEN, sizeof(char));
    snprintf(res, PROCESSOR_DISPLAY_LEN,
        "%04x %04x %04x %04x %04x %04x %04x %04x %c%c%c%c", processor->ax,
        processor->bx, processor->cx, processor->dx, processor->sp,
        processor->bp, processor->si, processor->di, '?',
        processor->flags.s ? 'S' : '-', processor->flags.z ? 'Z' : '-',
        processor->flags.c ? 'C' : '-');

    return res;
}

int processor_set_byte_value(processor_t *proc, char key, unsigned char value)
{
    switch (key)
    {
        PROC_REG_SWITCH(0b000, al)
        PROC_REG_SWITCH(0b001, cl)
        PROC_REG_SWITCH(0b010, dl)
        PROC_REG_SWITCH(0b011, bl)
        PROC_REG_SWITCH(0b100, ah)
        PROC_REG_SWITCH(0b101, ch)
        PROC_REG_SWITCH(0b110, dh)
        PROC_REG_SWITCH(0b111, bh)
        default:
            return -1;
    }
    return 0;
}

int processor_set_value(processor_t *proc, char key, unsigned short value)
{
    switch (key)
    {
        PROC_REG_SWITCH(0b000, ax)
        PROC_REG_SWITCH(0b001, cx)
        PROC_REG_SWITCH(0b010, dx)
        PROC_REG_SWITCH(0b011, bx)
        PROC_REG_SWITCH(0b100, sp)
        PROC_REG_SWITCH(0b101, bp)
        PROC_REG_SWITCH(0b110, si)
        PROC_REG_SWITCH(0b111, di)
        default:
            return -1;
    }
    return 0;
}
int processor_set_segment(processor_t *proc, char key, unsigned short value)
{
    switch (key)
    {
        PROC_REG_SWITCH(0b00, es)
        PROC_REG_SWITCH(0b01, cs)
        PROC_REG_SWITCH(0b10, ss)
        PROC_REG_SWITCH(0b11, ds)
        default:
            return -1;
    }
    return 0;
}

void processor_free(processor_t *proc)
{
    if (proc == NULL)
        return;
    free(proc);
}

stack_t *stack_new()
{
    return vector_new();
}

void stack_free(stack_t *stack)
{
    vector_free(stack);
}

void stack_push(stack_t *stack, void *data)
{
    vector_append(stack, data);
}

void *stack_pop(stack_t *stack)
{
    return vector_pop(stack);
}

instruction_t *instruction_new(char *display, unsigned char *instruction,
    size_t instruction_len, struct params_t params, instruction_cb_t callback)
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

emulator_t *emulator_new(size_t data_size)
{
    emulator_t *res = malloc(sizeof(emulator_t));
    if (res == NULL)
        errx(-1, "malloc error: could not allocate emulator struct");

    if ((res->processor = processor_new()) == NULL)
        errx(-1, "malloc error: could not allocate processor struct");

    if ((res->stack = stack_new()) == NULL)
        errx(-1, "malloc error: could not allocate stack struct");

    if ((res->instructions = trie_new()) == NULL)
        errx(-1,
            "malloc error: could not allocate trie struct for instructions");

    res->data_size = data_size;
    if ((res->data = calloc(data_size, sizeof(unsigned char))) == NULL)
        errx(-1, "malloc error: could not allocate trie struct for memory");

    return res;
}

void instruction_free_cb(
    instruction_t *instruction, unsigned long _address, void *_additional)
{
    instruction_free(instruction);
}

void emulator_free(emulator_t *emulator)
{
    processor_free(emulator->processor);
    trie_for_each(
        emulator->instructions, (trie_function_t)instruction_free_cb, NULL);
    trie_free(emulator->instructions);
    free(emulator->data);
    stack_free(emulator->stack);
    free(emulator);
}
