#include <malloc.h>
#include "processor.h"
#include "../env.h"

#define PROC_REG_SWITCH(key, reg_name) \
    case key:                          \
        proc->reg_name = value;        \
        break;

processor_t *processor_new()
{
    processor_t *res = calloc(1, sizeof(processor_t));
    res->ds = DATA_SEGMENT;
    res->cs = CODE_SEGMENT;
    res->ss = STACK_SEGMENT;
    res->es = EXTRA_SEGMENT;
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
