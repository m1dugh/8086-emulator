#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emulator.h"
#include "../env.h"
#include "memory_segment.h"
#include "processor.h"

emulator_t *emulator_new(FILE *file)
{
    emulator_t *res = malloc(sizeof(emulator_t));
    if (res == NULL)
        errx(-1, "malloc error: could not allocate emulator struct");

    res->verbose = 1;
    res->file = file;

    if ((res->processor = processor_new()) == NULL)
        errx(-1, "malloc error: could not allocate processor struct");

    processor_t *proc = res->processor;

    if ((res->code = code_seg_new(proc->cs)) == NULL)
        errx(-1,
            "malloc error: could not allocate trie struct for instructions");

    if ((res->data = mem_seg_new(proc->ds)) == NULL)
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
    mem_seg_free(emulator->extra);
    bs_free(emulator->stream);
    free(emulator);
}

void emulator_stack_push(emulator_t *emulator, unsigned short value)
{
#if BIG_ENDIAN
    emulator->stack[--emulator->processor->sp] = value & 0xFF;
    emulator->stack[--emulator->processor->sp] = (value & 0xFF00) >> 8;
#else
    emulator->stack[--emulator->processor->sp] = (value & 0xFF00) >> 8;
    emulator->stack[--emulator->processor->sp] = value & 0xFF;
#endif
}

unsigned short emulator_stack_pop(emulator_t *emulator)
{
#if BIG_ENDIAN
    unsigned char high = emulator->stack[emulator->processor->sp++];
    unsigned char low = emulator->stack[emulator->processor->sp++];
#else
    unsigned char low = emulator->stack[emulator->processor->sp++];
    unsigned char high = emulator->stack[emulator->processor->sp++];
#endif
    return (high << 8) + low;
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
    emulator_t *emulator, char **environment, int argc, char **argv)
{
    emulator->processor->bp = 0;
    emulator->processor->sp = 0;

    size_t combined_len = 0;
    for (char **env = environment; *env; env++)
    {
        combined_len += strlen(*env) + 1;
    }
    for (int i = 0; i < argc; i++)
    {
        combined_len += strlen(argv[i]) + 1;
    }

    if (combined_len % 2 != 0)
        emulator->stack[--emulator->processor->sp] = 0;

    vector_t *env_ptrs = vector_new();
    for (; *environment; environment++)
    {
        char *env = *environment;
        size_t len = strlen(env);
        for (size_t i = len + 1; i > 0; i--)
        {
            emulator->stack[--emulator->processor->sp] = env[i - 1];
        }
        vector_push(env_ptrs, TO_VOID_PTR(emulator->processor->sp));
    }

    vector_t *arg_ptrs = vector_new();
    for (int i = argc; i > 0; i--)
    {
        char *arg = argv[i - 1];
        size_t len = strlen(arg);
        if (!len)
            continue;

        for (size_t j = len + 1; j > 0; j--)
        {
            emulator->stack[--emulator->processor->sp] = arg[j - 1];
        }
        vector_push(arg_ptrs, TO_VOID_PTR(emulator->processor->sp));
    }
    emulator_stack_push(emulator, 0);
    for (size_t i = 0; i < vector_len(env_ptrs); i++)
    {
        emulator_stack_push(emulator, TO_ADDR(vector_get(env_ptrs, i)));
    }

    emulator_stack_push(emulator, 0);
    for (size_t i = 0; i < vector_len(arg_ptrs); i++)
    {
        emulator_stack_push(emulator, TO_ADDR(vector_get(arg_ptrs, i)));
    }

    emulator_stack_push(emulator, (unsigned short)argc & 0xFFFF);

    vector_free(env_ptrs);
    vector_free(arg_ptrs);
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
    processor_t *proc = emulator->processor;
    unsigned short overriden_segment;
    unsigned short default_segment;
    unsigned char override_segment = 1;

    switch (emulator->processor->segment_selector)
    {
        case SS_SELECTOR:
            overriden_segment = emulator->processor->ss;
            break;
        case DS_SELECTOR:
            overriden_segment = emulator->processor->ds;
            break;
        case ES_SELECTOR:
            overriden_segment = emulator->processor->es;
            break;
        case CS_SELECTOR:
            overriden_segment = emulator->processor->cs;
            break;
        default:
            override_segment = 0;
    }

    switch (params.rm)
    {
        case 0b000:
            effective_address = proc->bx + proc->si + disp;
            default_segment = emulator->processor->ds;
            break;
        case 0b001:
            effective_address = proc->bx + proc->di + disp;
            default_segment = emulator->processor->ds;
            break;
        case 0b010:
            effective_address = proc->bp + proc->si + disp;
            default_segment = emulator->processor->ds;
            break;
        case 0b011:
            effective_address = proc->bp + proc->di + disp;
            default_segment = emulator->processor->ss;
            break;
        case 0b100:
            effective_address = proc->si + disp;
            default_segment = emulator->processor->ds;
            break;
        case 0b101:
            effective_address = proc->di + disp;
            default_segment = emulator->processor->ds;
            break;
        case 0b110:
            if (params.mod == 0b00)
            {
                effective_address = (unsigned short)disp;
                override_segment = 0;
                default_segment = emulator->processor->ds;
            }
            else
            {
                effective_address = proc->bp + disp;
                default_segment = emulator->processor->ss;
            }
            break;
        case 0b111:
            effective_address = proc->bx + disp;
            default_segment = emulator->processor->ds;
            break;
        default:
            errx(-1, "invalid value for rm: %x", params.rm);
    };

    unsigned int physical_addr;
    if (override_segment)
    {
        physical_addr = (overriden_segment << 4) + effective_address;
    }
    else
    {
        physical_addr = (default_segment << 4) + effective_address;
    }
    return physical_addr;
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
                 = emulator_stack_get_relative_address(emulator, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        return emulator_stack_get_byte(emulator, ea);
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
                 = emulator_stack_get_relative_address(emulator, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        return emulator_stack_get(emulator, ea);
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
                 = emulator_stack_get_relative_address(emulator, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        emulator_stack_set_byte(emulator, ea, value);
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
                 = emulator_stack_get_relative_address(emulator, address))
             > 0)
    {
        unsigned short ea = extracted_address & 0xFFFF;
        emulator_stack_set(emulator, ea, value);
    }
    else
    {
        errx(-1, "could not extract data at address %05x", address);
    }
}

#define DISPLAY_COLUMN_SIZE 16

void emulator_stack_display(emulator_t *emulator)
{
    processor_t *proc = emulator->processor;

    printf("===== START OF STACK =====\n");
    char buffer[DISPLAY_COLUMN_SIZE + 1];
    buffer[DISPLAY_COLUMN_SIZE] = 0;
    for (unsigned short address = proc->sp; address != proc->bp;)
    {
        printf("%04x:", address);
        for (size_t i = 0; i < DISPLAY_COLUMN_SIZE; i++)
        {
            if (address != proc->bp)
            {
                unsigned char value
                    = emulator_stack_get_byte(emulator, address);
                printf(" %02x", value);
                if (value > 0x14 && value < 0x7f)
                {
                    buffer[i] = value;
                }
                else
                {
                    buffer[i] = '.';
                }
                address++;
            }
            else
            {
                printf("   ");
                buffer[i] = '.';
            }
        }
        printf("\t|%s|\n", buffer);
    }
    printf("====== END OF STACK ======\n");
}

int emulator_stack_get_relative_address(
    emulator_t *emulator, unsigned int address)
{
    if (address > ((unsigned int)(emulator->processor->ss << 4) + MAX_ADDRESS)
        || address < ((unsigned int)(emulator->processor->ss << 4)
                      + emulator->processor->sp))
    {
        return -1;
    }
    return address - (emulator->processor->ss << 4);
}

unsigned char emulator_stack_get_byte(
    emulator_t *emulator, unsigned short address)
{
    if (address < emulator->processor->sp)
        errx(-1, "could not access stack at address %04x, as sp=%04x", address,
            emulator->processor->sp);
    return emulator->stack[address];
}

unsigned short emulator_stack_get(emulator_t *emulator, unsigned short address)
{
#if BIG_ENDIAN
    unsigned char low = emulator_stack_get_byte(emulator, address + 1);
    unsigned char high = emulator_stack_get_byte(emulator, address);
#else
    unsigned char high = emulator_stack_get_byte(emulator, address + 1);
    unsigned char low = emulator_stack_get_byte(emulator, address);
#endif
    return (high << 8) + low;
}

void emulator_stack_set_byte(
    emulator_t *emulator, unsigned short address, unsigned char value)
{
    emulator->stack[address] = value;
}

void emulator_stack_set(
    emulator_t *emulator, unsigned short address, unsigned short value)
{
#if BIG_ENDIAN
    emulator_stack_set_byte(emulator, address, (value & 0xFF00) >> 8);
    emulator_stack_set_byte(emulator, address + 1, value & 0xFF);
#else
    emulator_stack_set_byte(emulator, address, value & 0xFF);
    emulator_stack_set_byte(emulator, address + 1, (value & 0xFF00) >> 8);
#endif
}

unsigned long to_long(unsigned char *buffer)
{
    unsigned long res = 0;
    for (unsigned char i = 4; i > 0; i--)
        res = (res << 8) | buffer[i - 1];
    return res;
}

unsigned short to_short(unsigned char *buffer)
{
    unsigned short res = buffer[1];
    res = (res << 8) | buffer[0];
    return res;
}

int emulator_load_header(emulator_t *emulator)
{
    FILE *stream = emulator->file;
    struct exec_header *header = &emulator->header;
    char c;
    if ((c = fgetc(stream)) == -1)
    {
        return -1;
    }
    header->a_magic[0] = (unsigned char)c;
    if ((c = fgetc(stream)) == -1)
    {
        return -1;
    }
    header->a_magic[1] = (unsigned char)c;

    if ((c = fgetc(stream)) == -1)
    {
        return -1;
    }
    header->a_flags = (unsigned char)c;

    if ((c = fgetc(stream)) == -1)
    {
        return -1;
    }
    header->a_cpu = (unsigned char)c;

    if ((c = fgetc(stream)) == -1)
    {
        return -1;
    }
    header->a_hdrlen = (unsigned char)c;

    unsigned char effective_len = header->a_hdrlen - 5;
    unsigned char *buffer = (unsigned char *)malloc(effective_len);
    for (unsigned char i = 0; i < effective_len; i++)
    {
        if ((c = fgetc(stream)) == -1)
        {
            return -1;
        }
        buffer[i] = (unsigned char)c;
    }

    size_t current_index = 0;
    header->a_unused = (unsigned char)buffer[current_index++];
    header->a_version = to_short(buffer + current_index);
    current_index += 2;
    header->a_text = to_long(buffer + current_index);
    current_index += 4;

    header->a_data = to_long(buffer + current_index);
    current_index += 4;
    header->a_bss = to_long(buffer + current_index);
    current_index += 4;
    header->a_entry = to_long(buffer + current_index);
    current_index += 4;
    header->a_total = to_long(buffer + current_index);
    current_index += 4;
    header->a_syms = to_long(buffer + current_index);
    current_index += 4;

    // TODO: implement full header.

    free(buffer);

    emulator->stream = bs_new(emulator->file, emulator->header.a_text);
    return 0;
}

void emulator_load_data(emulator_t *emulator)
{
    struct exec_header header = emulator->header;
    fseek(emulator->file, header.a_hdrlen + header.a_text, SEEK_SET);
    for (unsigned long i = 0; i < header.a_data; i++)
    {
        unsigned char val = fgetc(emulator->file);
        emulator_push_data(emulator, val);
    }
}

void *emulator_data_addr(emulator_t *emulator, unsigned short address)
{
    return &emulator->data->value->values[address];
}
