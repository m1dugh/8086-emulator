#include <err.h>
#include "dec.h"
#include "utils.h"

void dec_reg_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    unsigned short value;
    switch (params.reg)
    {
        case AX:
            value = --proc->ax;
            break;
        case BX:
            value = --proc->bx;
            break;
        case CX:
            value = --proc->cx;
            break;
        case DX:
            value = --proc->dx;
            break;
        case DI:
            value = --proc->di;
            break;
        case SI:
            value = --proc->si;
            break;
        case SP:
            value = --proc->sp;
            break;
        case BP:
            value = --proc->bp;
            break;
        default:
            errx(-1, "register not found: %01x", params.reg);
    }

    proc->flags.s = get_sign(value);
    proc->flags.z = get_zero(value);
    proc->flags.p = get_parity(value);
    // TODO: implement flags for auxiliary
    // proc->flags.a = ;
    proc->flags.o = get_overflow_minus(value + 1, 1);
}
