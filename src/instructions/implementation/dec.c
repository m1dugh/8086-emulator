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

void dec_rm_exec(emulator_t *emulator, params_t params)
{
    processor_t *proc = emulator->processor;
    if (params.w)
    {
        unsigned short value = emulator_get_rm(emulator, params);
        unsigned short res = value - 1;

        proc->flags.s = get_sign(res);
        proc->flags.z = get_zero(res);
        proc->flags.p = get_parity(res);
        // TODO: implement flags for auxiliary
        // proc->flags.a = ;
        proc->flags.o = get_overflow_minus(value, 1);

        emulator_set_rm(emulator, params, res);
    }
    else
    {
        unsigned char value = emulator_get_rm_byte(emulator, params);
        unsigned char res = value - 1;

        proc->flags.s = get_sign_byte(res);
        proc->flags.z = get_zero_byte(res);
        proc->flags.p = get_parity_byte(res);
        // TODO: implement flags for auxiliary
        // proc->flags.a = ;
        proc->flags.o = get_overflow_minus_byte(value, 1);

        emulator_set_rm_byte(emulator, params, res);
    }
}
