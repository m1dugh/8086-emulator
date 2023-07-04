#include <err.h>
#include <malloc.h>
#include <stdio.h>
#include "models.h"
#include "env.h"
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

void stack_push(stack_t *stack, unsigned short value)
{
    vector_append(stack, TO_VOID_PTR(value));
}

unsigned short stack_pop(stack_t *stack)
{
    return TO_ADDR(vector_pop(stack));
}

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
    emulator->processor->sp--;
    vector_append(emulator->stack->value, TO_VOID_PTR(value));
}

unsigned short emulator_stack_pop(emulator_t *emulator)
{
    emulator->processor->sp++;
    return stack_pop(emulator->stack->value);
}

unsigned short emulator_push_data(emulator_t *emulator, unsigned char value)
{
    emulator_text_ensure_lock(emulator);
    return mem_seg_push(emulator->data, value);
}

unsigned short emulator_push_environment(
    emulator_t *emulator, unsigned char value)
{
    return mem_seg_push(emulator->environment, value);
}

unsigned short emulator_push_args(emulator_t *emulator, unsigned char value)
{
    emulator_environment_ensure_lock(emulator);
    return mem_seg_push(emulator->args, value);
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

memory_segment_t *mem_seg_new(
    unsigned short base_address, unsigned char direction)
{
    memory_segment_t *res = malloc(sizeof(memory_segment_t));
    if (res == NULL)
        errx(-1, "malloc error: could not allocate memory for memory_segment");
    if ((res->value = vector_new()) == NULL)
        errx(-1, "malloc error: could not allocate memory for vector for "
                 "memory_segment");

    res->direction = direction;
    res->base_address = base_address;
    res->locked = 0;
    return res;
}

void mem_seg_free(memory_segment_t *mem)
{
    vector_free(mem->value);
    free(mem);
}

unsigned short mem_seg_high_addr(memory_segment_t *mem)
{
    unsigned short len = (unsigned short)vector_len(mem->value);
    if (mem->direction == DIRECTION_UP)
    {
        return mem->base_address + len - 1;
    }
    else
    {
        return mem->base_address;
    }
}

unsigned short mem_seg_low_addr(memory_segment_t *mem)
{
    unsigned short len = (unsigned short)vector_len(mem->value);
    if (mem->direction == DIRECTION_UP || len == 0)
    {
        return mem->base_address;
    }
    else
    {
        return mem->base_address - len + 1;
    }
}

unsigned char mem_seg_get(memory_segment_t *mem, unsigned short address)
{
    if (address >= vector_len(mem->value))
        errx(-1, "memory_segment: cannot access element at address 0x%02x",
            address);

    return TO_BYTE(vector_get(mem->value, (unsigned long)address));
}

unsigned char mem_seg_get_abs(memory_segment_t *mem, unsigned short address)
{
    unsigned short high = mem_seg_high_addr(mem);
    unsigned short low = mem_seg_low_addr(mem);
    if (address < low || address > high)
        errx(-1,
            "memory_segment: cannot access element at address 0x%02x, should "
            "be between 0x%02x and 0x%02x",
            address, low, high);

    unsigned short effective_address;
    if (mem->direction == DIRECTION_UP)
    {
        effective_address = address - low;
    }
    else
    {
        effective_address = high - address;
    }

    return mem_seg_get(mem, effective_address);
}

void mem_seg_set_abs(
    memory_segment_t *mem, unsigned short address, unsigned char value)
{
    unsigned short high = mem_seg_high_addr(mem);
    unsigned short low = mem_seg_low_addr(mem);
    if (address < low || address > high)
        errx(-1,
            "memory_segment: cannot access element at address 0x%02x, should "
            "be between 0x%02x and 0x%02x",
            address, low, high);

    unsigned short effective_address;
    if (mem->direction == DIRECTION_UP)
    {
        effective_address = address + 1 - low;
    }
    else
    {
        effective_address = high - address;
    }
    mem_seg_set(mem, effective_address, value);
}
void mem_seg_set(
    memory_segment_t *mem, unsigned short address, unsigned char value)
{
    if (address >= vector_len(mem->value))
        errx(-1, "memory_segment: cannot access element at address 0x%02x",
            address);

    vector_set(mem->value, (unsigned long)address, TO_VOID_PTR(value));
}

unsigned short mem_seg_push(memory_segment_t *mem, unsigned char data)
{
    if (mem->locked)
    {
        errx(-1, "cannot push data after memory segment has been locked");
    }
    unsigned short address;
    if (mem->direction == DIRECTION_UP)
    {
        address = mem->base_address + vector_len(mem->value) - 1;
    }
    else
    {
        address = mem->base_address - vector_len(mem->value) + 1;
    }
    vector_append(mem->value, TO_VOID_PTR(data));
    return address;
}

int mem_seg_empty(memory_segment_t *mem)
{
    return vector_len(mem->value) == 0;
}

#define DISPLAY_COLUMN_SIZE 16

void mem_seg_display(memory_segment_t *mem)
{
    if (mem_seg_empty(mem))
        return;

    unsigned short address;
    unsigned short low_address = mem_seg_low_addr(mem);
    unsigned short high_address = mem_seg_high_addr(mem);

    if (mem->direction == DIRECTION_UP)
    {
        address = low_address;
    }
    else
    {
        address = high_address;
    }

    char buffer[DISPLAY_COLUMN_SIZE + 1];
    buffer[DISPLAY_COLUMN_SIZE] = 0;
    while (address >= low_address && address <= high_address)
    {
        printf("%04x:", address);
        for (size_t j = 0; j < DISPLAY_COLUMN_SIZE; j++)
        {
            if (address >= low_address && address <= high_address)
            {
                unsigned char value = mem_seg_get_abs(mem, address);
                printf(" %02x", value);
                if (value > 0x14 && value < 0x7f)
                {
                    buffer[j] = value;
                }
                else
                {
                    buffer[j] = '.';
                }
                if (mem->direction == DIRECTION_UP)
                    address++;
                else
                    address--;
            }
            else
            {
                printf("   ");
                buffer[j] = '.';
            }
        }
        printf("\t|%s|\n", buffer);
    }
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

unsigned short emulator_get_rm(emulator_t *, params_t params);
void emulator_set_rm(emulator_t *, params_t params, unsigned short);

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
    else
    {
        errx(-1, "could not extract data at address 0x%04x", address);
    }
}
