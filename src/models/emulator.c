#include <err.h>
#include "emulator.h"
#include "../env.h"
#include "memory_segment.h"

unsigned short emulator_next_address(memory_segment_t *mem)
{
    if (mem->direction == DIRECTION_UP)
    {
        unsigned short last_address = mem_seg_high_addr(mem);
        if (!mem_seg_empty(mem))
            last_address++;
        return last_address;
    }
    else
    {
        unsigned short last_address = mem_seg_low_addr(mem);
        if (!mem_seg_empty(mem))
            last_address--;
        return last_address;
    }
}

void emulator_text_ensure_lock(emulator_t *emulator)
{
    if (!emulator->text_locked)
    {
        emulator->text_locked = 1;
        unsigned short last_address = trie_last_address(emulator->text);
        instruction_t *instruction = trie_get(emulator->text, last_address);
        emulator->data->base_address
            = last_address + instruction->instruction_len + 1;
    }
}

void emulator_environment_ensure_lock(emulator_t *emulator)
{
    if (!emulator->environment->locked)
    {
        emulator->environment->locked = 1;
        emulator->args->base_address
            = emulator_next_address(emulator->environment);
    }
}

void emulator_args_ensure_lock(emulator_t *emulator)
{
    if (!emulator->args->locked)
    {
        emulator->args->locked = 1;
        emulator->stack->base_address = emulator_next_address(emulator->args);
    }
}

void emulator_data_ensure_lock(emulator_t *emulator)
{
    emulator_text_ensure_lock(emulator);
    if (!emulator->data->locked)
    {
        emulator->data->locked = 1;
        emulator->bss->base_address = emulator_next_address(emulator->data);
    }
}

void emulator_bss_ensure_lock(emulator_t *emulator)
{
    emulator_data_ensure_lock(emulator);
    if (!emulator->bss->locked)
    {
        emulator->bss->locked = 1;
    }
}

emulator_t *emulator_new(size_t memory_size)
{
    emulator_t *res = malloc(sizeof(emulator_t));
    if (res == NULL)
        errx(-1, "malloc error: could not allocate emulator struct");

    if ((res->processor = processor_new()) == NULL)
        errx(-1, "malloc error: could not allocate processor struct");

    if ((res->text = trie_new()) == NULL)
        errx(-1,
            "malloc error: could not allocate trie struct for instructions");
    res->text_locked = 0;

    if ((res->data = mem_seg_new(0, DIRECTION_UP)) == NULL)
        errx(-1, "malloc error: could not allocate memory_segment struct for "
                 "memory");
    if ((res->bss = mem_seg_new(0, DIRECTION_UP)) == NULL)
        errx(-1, "malloc error: could not allocate memory_segment struct for "
                 "memory");
    if ((res->environment = mem_seg_new(memory_size - 1, DIRECTION_DOWN))
        == NULL)
        errx(-1, "malloc error: could not allocate memory_segment struct for "
                 "memory");
    if ((res->args = mem_seg_new(memory_size - 1, DIRECTION_DOWN)) == NULL)
        errx(-1, "malloc error: could not allocate memory_segment struct for "
                 "memory");
    if ((res->stack = mem_seg_new(memory_size - 1, DIRECTION_DOWN)) == NULL)
        errx(-1, "malloc error: could not allocate memory_segment struct for "
                 "memory");

    return res;
}

void instruction_free_cb(
    instruction_t *instruction, unsigned long _address, void *_additional)
{
    UNUSED(_address);
    UNUSED(_additional);
    instruction_free(instruction);
}

void emulator_free(emulator_t *emulator)
{
    processor_free(emulator->processor);
    trie_for_each(emulator->text, (trie_function_t)instruction_free_cb, NULL);
    trie_free(emulator->text);
    mem_seg_free(emulator->data);
    mem_seg_free(emulator->environment);
    mem_seg_free(emulator->args);
    mem_seg_free(emulator->stack);
    mem_seg_free(emulator->bss);
    free(emulator);
}

void emulator_stack_push(emulator_t *emulator, unsigned short value)
{
    vector_append(emulator->stack->value, TO_VOID_PTR(value));
    emulator->processor->sp--;
}

unsigned short emulator_stack_pop(emulator_t *emulator)
{
    emulator->processor->sp++;
    return TO_ADDR(vector_pop(emulator->stack->value));
}

unsigned short emulator_push_data(emulator_t *emulator, unsigned char value)
{
    emulator_text_ensure_lock(emulator);
    return mem_seg_push(emulator->data, value);
}

unsigned short emulator_push_environment(emulator_t *emulator, char *str)
{
    return mem_seg_push_str(emulator->environment, str);
}

unsigned short emulator_push_args(emulator_t *emulator, char *str)
{
    emulator_environment_ensure_lock(emulator);
    return mem_seg_push_str(emulator->args, str);
}

unsigned short emulator_push_bss(emulator_t *emulator, unsigned char value)
{
    emulator_data_ensure_lock(emulator);
    return mem_seg_push(emulator->bss, value);
}

void emulator_prepare(
    emulator_t *emulator, vector_t *environment, vector_t *args)
{
    emulator_bss_ensure_lock(emulator);
    emulator_args_ensure_lock(emulator);

    emulator->processor->bp = emulator_next_address(emulator->args);
    emulator->processor->sp = emulator->processor->bp;

    emulator_stack_push(emulator, 0);

    // Fixes broken offset for stack pointer
    emulator->processor->sp++;

    for (size_t i = vector_len(environment); i > 0; i--)
    {
        emulator_stack_push(emulator, TO_ADDR(vector_get(environment, i - 1)));
    }

    emulator_stack_push(emulator, 0);
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

unsigned char emulator_get_rm_byte(emulator_t *emulator, params_t params)
{
    if (params.mod == 0b11)
    {
        return emulator_get_reg_byte(emulator, params.rm);
    }

    short disp = params.disp;
    unsigned short effective_address;
    processor_t *proc = emulator->processor;
    switch (params.rm)
    {
        case 0b000:
            effective_address = proc->bx + proc->si + disp;
            break;
        case 0b001:
            effective_address = proc->bx + proc->di + disp;
            break;
        case 0b010:
            effective_address = proc->bp + proc->si + disp;
            break;
        case 0b011:
            effective_address = proc->bp + proc->di + disp;
            break;
        case 0b100:
            effective_address = proc->si + disp;
            break;
        case 0b101:
            effective_address = proc->di + disp;
            break;
        case 0b110:
            if (params.mod == 0b00)
            {
                effective_address = (unsigned short)disp;
            }
            else
            {
                effective_address = proc->bp + disp;
            }
            break;
        case 0b111:
            effective_address = proc->bx + disp;
            break;
        default:
            errx(-1, "invalid value for rm: %x", params.rm);
    };

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

    short disp = params.disp;
    unsigned short effective_address;
    processor_t *proc = emulator->processor;
    switch (params.rm)
    {
        case 0b000:
            effective_address = proc->bx + proc->si + disp;
            break;
        case 0b001:
            effective_address = proc->bx + proc->di + disp;
            break;
        case 0b010:
            effective_address = proc->bp + proc->si + disp;
            break;
        case 0b011:
            effective_address = proc->bp + proc->di + disp;
            break;
        case 0b100:
            effective_address = proc->si + disp;
            break;
        case 0b101:
            effective_address = proc->di + disp;
            break;
        case 0b110:
            if (params.mod == 0b00)
            {
                effective_address = (unsigned short)disp;
            }
            else
            {
                effective_address = proc->bp + disp;
            }
            break;
        case 0b111:
            effective_address = proc->bx + disp;
            break;
        default:
            errx(-1, "invalid value for rm: %x", params.rm);
    };

    emulator_set_mem_byte(emulator, effective_address, value);
}

unsigned short emulator_get_rm(emulator_t *emulator, params_t params)
{
    if (params.mod == 0b11)
    {
        return emulator_get_reg(emulator, params.rm);
    }

    short disp = params.disp;
    unsigned short effective_address;
    processor_t *proc = emulator->processor;
    switch (params.rm)
    {
        case 0b000:
            effective_address = proc->bx + proc->si + disp;
            break;
        case 0b001:
            effective_address = proc->bx + proc->di + disp;
            break;
        case 0b010:
            effective_address = proc->bp + proc->si + disp;
            break;
        case 0b011:
            effective_address = proc->bp + proc->di + disp;
            break;
        case 0b100:
            effective_address = proc->si + disp;
            break;
        case 0b101:
            effective_address = proc->di + disp;
            break;
        case 0b110:
            if (params.mod == 0b00)
            {
                effective_address = (unsigned short)disp;
            }
            else
            {
                effective_address = proc->bp + disp;
            }
            break;
        case 0b111:
            effective_address = proc->bx + disp;
            break;
        default:
            errx(-1, "invalid value for rm: %x", params.rm);
    };

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

    short disp = params.disp;
    unsigned short effective_address;
    processor_t *proc = emulator->processor;
    switch (params.rm)
    {
        case 0b000:
            effective_address = proc->bx + proc->si + disp;
            break;
        case 0b001:
            effective_address = proc->bx + proc->di + disp;
            break;
        case 0b010:
            effective_address = proc->bp + proc->si + disp;
            break;
        case 0b011:
            effective_address = proc->bp + proc->di + disp;
            break;
        case 0b100:
            effective_address = proc->si + disp;
            break;
        case 0b101:
            effective_address = proc->di + disp;
            break;
        case 0b110:
            if (params.mod == 0b00)
            {
                effective_address = (unsigned short)disp;
            }
            else
            {
                effective_address = proc->bp + disp;
            }
            break;
        case 0b111:
            effective_address = proc->bx + disp;
            break;
        default:
            errx(-1, "invalid value for rm: %x", params.rm);
    };

    emulator_set_mem_word(emulator, effective_address, value);
}

unsigned char emulator_get_mem_byte(
    emulator_t *emulator, unsigned short address)
{
    unsigned char res = 0;

    if (address <= mem_seg_high_addr(emulator->bss)
        && address >= mem_seg_low_addr(emulator->bss))
    {
        res = mem_seg_get_abs(emulator->bss, address);
    }
    else if (address <= mem_seg_high_addr(emulator->data)
             && address >= mem_seg_low_addr(emulator->data))
    {
        res = mem_seg_get_abs(emulator->data, address);
    }
    else if (address <= mem_seg_high_addr(emulator->environment)
             && address >= mem_seg_low_addr(emulator->environment))
    {
        res = mem_seg_get_abs(emulator->environment, address);
    }
    else if (address <= mem_seg_high_addr(emulator->stack)
             && address >= mem_seg_low_addr(emulator->stack))
    {
        res = mem_seg_get_abs(emulator->stack, address);
    }
    else
    {
        errx(-1, "could not extract data at address 0x%04x", address);
    }
    return res;
}

unsigned short emulator_get_mem_word(
    emulator_t *emulator, unsigned short address)
{
    unsigned short res = 0;

    if (address <= mem_seg_high_addr(emulator->bss)
        && address >= mem_seg_low_addr(emulator->bss))
    {
        res = mem_seg_get_abs_word(emulator->bss, address);
    }
    else if (address <= mem_seg_high_addr(emulator->data)
             && address >= mem_seg_low_addr(emulator->data))
    {
        res = mem_seg_get_abs_word(emulator->data, address);
    }
    else if (address <= mem_seg_high_addr(emulator->environment)
             && address >= mem_seg_low_addr(emulator->environment))
    {
        res = mem_seg_get_abs_word(emulator->environment, address);
    }
    else if (address <= mem_seg_high_addr(emulator->stack)
             && address >= mem_seg_low_addr(emulator->stack))
    {
        res = mem_seg_get_abs_word(emulator->stack, address);
    }

    else
    {
        errx(-1, "could not extract data at address 0x%04x", address);
    }
    return res;
}

void emulator_set_mem_byte(
    emulator_t *emulator, unsigned short address, unsigned char value)
{
    if (address <= mem_seg_high_addr(emulator->bss)
        && address >= mem_seg_low_addr(emulator->bss))
    {
        mem_seg_set_abs(emulator->bss, address, value);
    }
    else if (address <= mem_seg_high_addr(emulator->data)
             && address >= mem_seg_low_addr(emulator->data))
    {
        mem_seg_set_abs(emulator->data, address, value);
    }
    else if (address <= mem_seg_high_addr(emulator->environment)
             && address >= mem_seg_low_addr(emulator->environment))
    {
        mem_seg_set_abs(emulator->environment, address, value);
    }
    else if (address <= mem_seg_high_addr(emulator->stack)
             && address >= mem_seg_low_addr(emulator->stack))
    {
        mem_seg_set_abs(emulator->stack, address, value);
    }
    else
    {
        errx(-1, "could not extract data at address 0x%04x", address);
    }
}

void emulator_set_mem_word(
    emulator_t *emulator, unsigned short address, unsigned short value)
{
    if (address <= mem_seg_high_addr(emulator->bss)
        && address >= mem_seg_low_addr(emulator->bss))
    {
        mem_seg_set_abs_word(emulator->bss, address, value);
    }
    else if (address <= mem_seg_high_addr(emulator->data)
             && address >= mem_seg_low_addr(emulator->data))
    {
        mem_seg_set_abs_word(emulator->data, address, value);
    }
    else if (address <= mem_seg_high_addr(emulator->environment)
             && address >= mem_seg_low_addr(emulator->environment))
    {
        mem_seg_set_abs_word(emulator->environment, address, value);
    }
    else if (address <= mem_seg_high_addr(emulator->stack)
             && address >= mem_seg_low_addr(emulator->stack))
    {
        mem_seg_set_abs_word(emulator->stack, address, value);
    }
    else
    {
        errx(-1, "could not extract data at address 0x%04x", address);
    }
}
