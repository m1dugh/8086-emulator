#include <err.h>
#include <stdio.h>
#include <string.h>
#include "memory_segment.h"
#include "../env.h"
#include "instruction.h"

memory_segment_t *mem_seg_new(unsigned short base_address)
{
    memory_segment_t *res = malloc(sizeof(memory_segment_t));
    if (res == NULL)
        errx(-1, "malloc error: could not allocate memory for memory_segment");
    if ((res->value = byte_vector_new_sized(MEM_SIZE)) == NULL)
        errx(-1, "malloc error: could not allocate memory for vector for "
                 "memory_segment");

    res->base_address = base_address;
    return res;
}

void mem_seg_free(memory_segment_t *mem)
{
    byte_vector_free(mem->value);
    free(mem);
}

unsigned int mem_seg_high_addr(memory_segment_t *mem)
{
    unsigned short len = (unsigned short)byte_vector_len(mem->value);
    return (mem->base_address << 4) + len;
}

unsigned int mem_seg_low_addr(memory_segment_t *mem)
{
    return mem->base_address << 4;
}

unsigned char mem_seg_get(memory_segment_t *mem, unsigned short address)
{
    if (address >= byte_vector_len(mem->value))
    {
        errx(-1, "memory_segment: cannot access element at address %04x",
            address);
    }

    return byte_vector_get(mem->value, (unsigned long)address);
}

unsigned short mem_seg_get_word(memory_segment_t *mem, unsigned short address)
{
    unsigned char first_val = mem_seg_get(mem, address);
    unsigned char second_val = mem_seg_get(mem, address + 1);
    unsigned short res = (second_val << 8) | first_val;
    return res;
}

void mem_seg_set(
    memory_segment_t *mem, unsigned short address, unsigned char value)
{
    if (address >= byte_vector_len(mem->value))
    {
        errx(
            -1, "memory_segment: cannot set element at address %04x", address);
    }

    byte_vector_set(mem->value, (unsigned long)address, value);
}

void mem_seg_set_word(
    memory_segment_t *mem, unsigned short address, unsigned short value)
{
    mem_seg_set(mem, address, value & 0xff);
    mem_seg_set(mem, address + 1, (value & 0xff00) >> 8);
}

unsigned short mem_seg_push(memory_segment_t *mem, unsigned char data)
{
    unsigned short address = byte_vector_len(mem->value);
    byte_vector_push(mem->value, data);
    return address;
}

unsigned short mem_seg_push_str(memory_segment_t *mem, char *str)
{
    if (*str)
    {
        unsigned short res = mem_seg_push(mem, *str++);
        for (; *str; str++)
        {
            mem_seg_push(mem, *str);
        }
        mem_seg_push(mem, 0);
        return res;
    }
    else
    {
        return mem_seg_push(mem, 0);
    }
}

int mem_seg_empty(memory_segment_t *mem)
{
    return byte_vector_len(mem->value) == 0;
}

#define DISPLAY_COLUMN_SIZE 16

void mem_seg_display(memory_segment_t *mem, char *name)
{
    if (mem_seg_empty(mem))
        return;

    printf("===== START OF %s SEGMENT =====\n", name);
    char buffer[DISPLAY_COLUMN_SIZE + 1];
    buffer[DISPLAY_COLUMN_SIZE] = 0;
    for (size_t i = 0; i < byte_vector_len(mem->value);)
    {
        unsigned short address = i;
        printf("%04x:", address);
        for (size_t j = 0; j < DISPLAY_COLUMN_SIZE; j++, i++)
        {
            if (address < byte_vector_len(mem->value))
            {
                unsigned char value = mem_seg_get(mem, address);
                printf(" %02x", value);
                if (value > 0x14 && value < 0x7f)
                {
                    buffer[j] = value;
                }
                else
                {
                    buffer[j] = '.';
                }
                address++;
            }
            else
            {
                printf("   ");
                buffer[j] = '.';
            }
        }
        printf("\t|%s|\n", buffer);
    }
    printf("====== END OF %s SEGMENT ======\n", name);
}

code_segment_t *code_seg_new(unsigned short base_address)
{
    code_segment_t *res = malloc(sizeof(code_segment_t));
    if (res == NULL)
        errx(-1, "malloc error: could not allocate memory for code_segment");
    if ((res->instructions = trie_new()) == NULL)
        errx(-1, "malloc error: could not allocate memory for trie for "
                 "code_segment");

    res->base_address = base_address;
    return res;
}

int instruction_addr_valid(code_segment_t *instructions,
    instruction_t *instruction, unsigned short address)
{
    unsigned short end_address = address + instruction->instruction_len;
    return end_address <= instructions->max_size;
}

void _code_seg_free_cb(
    instruction_t *instruction, unsigned long address, void *additionals)
{
    UNUSED(address);
    UNUSED(additionals);
    instruction_free(instruction);
}

void code_seg_free(code_segment_t *instructions)
{
    code_seg_for_each(instructions, _code_seg_free_cb, NULL);
    trie_free(instructions->instructions);
    free(instructions);
}

void code_seg_for_each(
    code_segment_t *instructions, code_seg_cb_t function, void *additionals)
{
    trie_for_each(
        instructions->instructions, (trie_function_t)function, additionals);
}

instruction_t *code_seg_get(
    code_segment_t *instructions, unsigned short address)
{
    return (instruction_t *)trie_get(
        instructions->instructions, (unsigned long)address);
}

int code_seg_set(code_segment_t *instructions, unsigned short address,
    instruction_t *instruction)
{
    if (!instruction_addr_valid(instructions, instruction, address))
        return -1;

    trie_set(instructions->instructions, (unsigned long)address, instruction);

    return 0;
}

unsigned int mem_seg_get_low_addr(memory_segment_t *mem)
{
    return mem->base_address << 4;
}

unsigned int mem_seg_get_high_addr(memory_segment_t *mem)
{
    return (mem->base_address << 4)
           + (unsigned short)byte_vector_len(mem->value);
}

int mem_set_get_relative_address(memory_segment_t *mem, unsigned int address)
{
    unsigned int high = mem_seg_get_high_addr(mem);
    unsigned int low = mem_seg_get_low_addr(mem);
    if (address > high || address < low)
        return -1;

    unsigned short relative_addr = address - low;
    return ((int)0x0) | relative_addr;
}

int mem_seg_expand(memory_segment_t *mem, unsigned short size)
{
    return byte_vector_expand(mem->value, (unsigned short)size);
}

unsigned short mem_seg_size(memory_segment_t *mem)
{
    return byte_vector_len(mem->value);
}
