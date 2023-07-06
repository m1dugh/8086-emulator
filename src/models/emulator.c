#include <err.h>
#include "emulator.h"
#include "../env.h"
#include "memory_segment.h"

emulator_t *emulator_new()
{
    emulator_t *res = malloc(sizeof(emulator_t));
    if (res == NULL)
        errx(-1, "malloc error: could not allocate emulator struct");

    if ((res->processor = processor_new()) == NULL)
        errx(-1, "malloc error: could not allocate processor struct");

    processor_t *proc = res->processor;

    if ((res->code = code_seg_new(proc->cs)) == NULL)
        errx(-1,
            "malloc error: could not allocate trie struct for instructions");

    if ((res->data = mem_seg_new(proc->ds)) == NULL)
        errx(-1, "malloc error: could not allocate memory_segment struct for "
                 "memory");
    if ((res->stack = mem_seg_new(proc->ss)) == NULL)
        errx(-1, "malloc error: could not allocate memory_segment struct for "
                 "memory");

    if ((res->extra = mem_seg_new(proc->es)) == NULL)
        errx(-1, "malloc error: could not allocate memory_segment struct for "
                 "memory");

    return res;
}

void emulator_free(emulator_t *emulator)
{
    processor_free(emulator->processor);
    code_seg_free(emulator->code);
    mem_seg_free(emulator->data);
    mem_seg_free(emulator->stack);
    mem_seg_free(emulator->extra);
    free(emulator);
}

#include <stdio.h>
void emulator_stack_push(emulator_t *emulator, unsigned short value)
{
    stack_push(emulator->stack, value);
    emulator->processor->sp -= sizeof(value);
}

unsigned short emulator_stack_pop(emulator_t *emulator)
{
    unsigned short val = stack_pop(emulator->stack);
    emulator->processor->sp += sizeof(val);
    return val;
}

unsigned short emulator_push_data(emulator_t *emulator, unsigned char value)
{
    return mem_seg_push(emulator->data, value);
}

unsigned short emulator_push_data_str(emulator_t *emulator, char *str)
{
    return mem_seg_push_str(emulator->data, str);
}

unsigned short emulator_push_args(emulator_t *emulator, char *str)
{
    return mem_seg_push_str(emulator->data, str);
}

void emulator_prepare(
    emulator_t *emulator, vector_t *environment, vector_t *args)
{

    emulator->processor->bp = emulator->processor->ss << 4;
    emulator->processor->sp = emulator->processor->bp;

    emulator_stack_push(emulator, 0);
    for (size_t i = vector_len(environment); i > 0; i--)
    {
        emulator_stack_push(emulator, TO_ADDR(vector_get(environment, i - 1)));
    }

    // emulator_stack_push(emulator, 0);
    for (size_t i = vector_len(args); i > 0; i--)
    {
        emulator_stack_push(emulator, TO_ADDR(vector_get(args, i - 1)));
    }
    emulator_stack_push(emulator, vector_len(args));
}

unsigned char emulator_get_reg_byte(emulator_t *emulator, char reg)
{
    processor_t *proc = emulator->processor;
    switch (reg)
    {
        case 0b000:
            return proc->al;
        case 0b001:
            return proc->cl;
        case 0b010:
            return proc->dl;
        case 0b011:
            return proc->bl;
        case 0b100:
            return proc->ah;
        case 0b101:
            return proc->ch;
        case 0b110:
            return proc->dh;
        case 0b111:
            return proc->bh;
        default:
            errx(-1, "unknow register 0x%1x", reg & 0b111);
    };
}

void emulator_set_reg_byte(emulator_t *emulator, char reg, unsigned char value)
{
    processor_t *proc = emulator->processor;
    switch (reg)
    {
        case 0b000:
            proc->al = value;
            break;
        case 0b001:
            proc->cl = value;
            break;
        case 0b010:
            proc->dl = value;
            break;
        case 0b011:
            proc->bl = value;
            break;
        case 0b100:
            proc->ah = value;
            break;
        case 0b101:
            proc->ch = value;
            break;
        case 0b110:
            proc->dh = value;
            break;
        case 0b111:
            proc->bh = value;
            break;
        default:
            errx(-1, "unknow register 0x%1x", reg & 0b111);
    };
}

unsigned short emulator_get_reg(emulator_t *emulator, char reg)
{
    processor_t *proc = emulator->processor;
    switch (reg)
    {
        case 0b000:
            return proc->ax;
        case 0b001:
            return proc->cx;
        case 0b010:
            return proc->dx;
        case 0b011:
            return proc->bx;
        case 0b100:
            return proc->sp;
        case 0b101:
            return proc->bp;
        case 0b110:
            return proc->si;
        case 0b111:
            return proc->di;
        default:
            errx(-1, "unknow register 0x%1x", reg & 0b111);
    };
}

void emulator_set_reg(emulator_t *emulator, char reg, unsigned short value)
{
    processor_t *proc = emulator->processor;
    switch (reg)
    {
        case 0b000:
            proc->ax = value;
            break;
        case 0b001:
            proc->cx = value;
            break;
        case 0b010:
            proc->dx = value;
            break;
        case 0b011:
            proc->bx = value;
            break;
        case 0b100:
            proc->sp = value;
            break;
        case 0b101:
            proc->bp = value;
            break;
        case 0b110:
            proc->si = value;
            break;
        case 0b111:
            proc->di = value;
            break;
        default:
            errx(-1, "unknow register 0x%1x", reg & 0b111);
    };
}

unsigned int emulator_get_physical_addr(emulator_t *emulator, params_t params)
{
    short disp = params.disp;
    unsigned short effective_address;
    unsigned short segment_addr;
    processor_t *proc = emulator->processor;
    switch (params.rm)
    {
        case 0b000:
            effective_address = proc->bx + proc->si + disp;
            // DATA segment
            segment_addr = emulator->processor->ds;
            break;
        case 0b001:
            effective_address = proc->bx + proc->di + disp;
            // DATA segment
            segment_addr = emulator->processor->ds;
            break;
        case 0b010:
            effective_address = proc->bp + proc->si + disp;
            // STACK segment
            segment_addr = emulator->processor->ss;
            break;
        case 0b011:
            effective_address = proc->bp + proc->di + disp;
            // STACK segment
            segment_addr = emulator->processor->ss;
            break;
        case 0b100:
            effective_address = proc->si + disp;
            // DATA segment
            segment_addr = emulator->processor->ds;
            break;
        case 0b101:
            effective_address = proc->di + disp;
            // DATA segment
            segment_addr = emulator->processor->ds;
            break;
        case 0b110:
            if (params.mod == 0b00)
            {
                effective_address = (unsigned short)disp;
                // DATA segment
                segment_addr = emulator->processor->ds;
            }
            else
            {
                effective_address = proc->bp + disp;
                // STACK segment
                segment_addr = emulator->processor->ss;
            }
            break;
        case 0b111:
            effective_address = proc->bx + disp;
            // DATA segment
            segment_addr = emulator->processor->ds;
            break;
        default:
            errx(-1, "invalid value for rm: %x", params.rm);
    };
    return (segment_addr << 4) + effective_address;
}

unsigned char emulator_get_rm_byte(emulator_t *emulator, params_t params)
{
    if (params.mod == 0b11)
    {
        return emulator_get_reg_byte(emulator, params.rm);
    }

    unsigned int effective_address
        = emulator_get_physical_addr(emulator, params);
    return emulator_get_mem_byte(emulator, effective_address);
}

void emulator_set_rm_byte(
    emulator_t *emulator, params_t params, unsigned char value)
{
    if (params.mod == 0b11)
    {
        emulator_set_reg_byte(emulator, params.rm, value);
        return;
    }

    unsigned int effective_address
        = emulator_get_physical_addr(emulator, params);
    emulator_set_mem_byte(emulator, effective_address, value);
}

unsigned short emulator_get_rm(emulator_t *emulator, params_t params)
{
    if (params.mod == 0b11)
    {
        return emulator_get_reg(emulator, params.rm);
    }

    unsigned int effective_address
        = emulator_get_physical_addr(emulator, params);
    return emulator_get_mem_word(emulator, effective_address);
}

void emulator_set_rm(
    emulator_t *emulator, params_t params, unsigned short value)
{
    if (params.mod == 0b11)
    {
        emulator_set_reg(emulator, params.rm, value);
        return;
    }

    unsigned int effective_address
        = emulator_get_physical_addr(emulator, params);

    emulator_set_mem_word(emulator, effective_address, value);
}

unsigned char emulator_get_mem_byte(emulator_t *emulator, unsigned int address)
{
    int extracted_address;
    if ((extracted_address
            = mem_set_get_relative_address(emulator->extra, address))
        > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        return mem_seg_get(emulator->extra, ea);
    }
    else if ((extracted_address
                 = mem_set_get_relative_address(emulator->data, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        return mem_seg_get(emulator->data, ea);
    }
    else if ((extracted_address
                 = stack_get_relative_address(emulator->stack, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        return mem_seg_get(emulator->stack, ea);
    }
    else
    {
        errx(-1, "could not extract data at address %05x", address);
    }
}

unsigned short emulator_get_mem_word(
    emulator_t *emulator, unsigned int address)
{
    int extracted_address;
    if ((extracted_address
            = mem_set_get_relative_address(emulator->extra, address))
        > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        return mem_seg_get_word(emulator->extra, ea);
    }
    else if ((extracted_address
                 = mem_set_get_relative_address(emulator->data, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        return mem_seg_get_word(emulator->data, ea);
    }
    else if ((extracted_address
                 = stack_get_relative_address(emulator->stack, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        return mem_seg_get_word(emulator->stack, ea);
    }
    else
    {
        errx(-1, "could not extract data at address %05x", address);
    }
}

void emulator_set_mem_byte(
    emulator_t *emulator, unsigned int address, unsigned char value)
{
    int extracted_address;
    if ((extracted_address
            = mem_set_get_relative_address(emulator->extra, address))
        > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        mem_seg_set(emulator->extra, ea, value);
    }
    else if ((extracted_address
                 = mem_set_get_relative_address(emulator->data, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        mem_seg_set(emulator->data, ea, value);
    }
    else if ((extracted_address
                 = stack_get_relative_address(emulator->stack, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        mem_seg_set(emulator->stack, ea, value);
    }
    else
    {
        errx(-1, "could not extract data at address %05x", address);
    }
}

void emulator_set_mem_word(
    emulator_t *emulator, unsigned int address, unsigned short value)
{
    int extracted_address;
    if ((extracted_address
            = mem_set_get_relative_address(emulator->extra, address))
        > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        mem_seg_set_word(emulator->extra, ea, value);
    }
    else if ((extracted_address
                 = mem_set_get_relative_address(emulator->data, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        mem_seg_set_word(emulator->data, ea, value);
    }
    else if ((extracted_address
                 = stack_get_relative_address(emulator->stack, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        mem_seg_set_word(emulator->stack, ea, value);
    }
    else
    {
        errx(-1, "could not extract data at address %05x", address);
    }
}
