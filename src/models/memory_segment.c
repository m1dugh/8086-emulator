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
    if ((res->value = vector_new_sized(MEM_SIZE)) == NULL)
        errx(-1, "malloc error: could not allocate memory for vector for "
                 "memory_segment");

    res->base_address = base_address;
    return res;
}

void mem_seg_free(memory_segment_t *mem)
{
    vector_free(mem->value);
    free(mem);
}

unsigned int mem_seg_high_addr(memory_segment_t *mem)
{
    unsigned short len = (unsigned short)vector_len(mem->value);
    return (mem->base_address << 4) + len;
}

unsigned int mem_seg_low_addr(memory_segment_t *mem)
{
    return mem->base_address << 4;
}

unsigned char mem_seg_get(memory_segment_t *mem, unsigned short address)
{
    if (address >= vector_len(mem->value))
        errx(-1, "memory_segment: cannot access element at address 0x%02x",
            address);

    return TO_BYTE(vector_get(mem->value, (unsigned long)address));
}

unsigned short mem_seg_get_word(memory_segment_t *mem, unsigned short address)
{
    unsigned char first_val = mem_seg_get(mem, address);
    unsigned char second_val = mem_seg_get(mem, address + 1);
#if BIG_ENDIAN
    unsigned short res = (first_val << 8) | second_val;
#else
    unsigned short res = (second_val << 8) | first_val;
#endif
    return res;
}

void mem_seg_set(
    memory_segment_t *mem, unsigned short address, unsigned char value)
{
    if (address >= vector_len(mem->value))
        errx(-1, "memory_segment: cannot access element at address 0x%02x",
            address);

    vector_set(mem->value, (unsigned long)address, TO_VOID_PTR(value));
}

void mem_seg_set_word(
    memory_segment_t *mem, unsigned short address, unsigned short value)
{
    if (address >= vector_len(mem->value))
        errx(-1, "memory_segment: cannot access element at address 0x%02x",
            address);

#if BIG_ENDIAN
    mem_seg_set(mem, address, (value & 0xff00) >> 8);
    mem_seg_set(mem, address + 1, value & 0xff);
#else
    mem_seg_set(mem, address, value & 0xff);
    mem_seg_set(mem, address + 1, (value & 0xff00) >> 8);
#endif
}

unsigned short mem_seg_push(memory_segment_t *mem, unsigned char data)
{
    unsigned short address = vector_len(mem->value);
    vector_push(mem->value, TO_VOID_PTR(data));
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
    return vector_len(mem->value) == 0;
}

#define DISPLAY_COLUMN_SIZE 16

void mem_seg_display(memory_segment_t *mem)
{
    if (mem_seg_empty(mem))
        return;

    printf("===== START OF MEMORY SEGMENT =====\n");
    char buffer[DISPLAY_COLUMN_SIZE + 1];
    buffer[DISPLAY_COLUMN_SIZE] = 0;
    for (size_t i = 0; i < vector_len(mem->value);)
    {
        unsigned short address = i;
        printf("%04x:", address);
        for (size_t j = 0; j < DISPLAY_COLUMN_SIZE; j++, i++)
        {
            if (address < vector_len(mem->value))
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
    printf("====== END OF MEMORY SEGMENT ======\n");
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

void *code_seg_get(code_segment_t *instructions, unsigned short address)
{
    return trie_get(instructions->instructions, (unsigned long)address);
}

int code_seg_set(code_segment_t *instructions, unsigned short address,
    instruction_t *instruction)
{
    if (!instruction_addr_valid(instructions, instruction, address))
        return -1;

    trie_set(instructions->instructions, (unsigned long)address, instruction);

    return 0;
}

void stack_push(stack_t *stack, unsigned short data)
{
#if BIG_ENDIAN
    mem_seg_push(stack, (data & 0xff00) >> 8);
    mem_seg_push(stack, data & 0xff);
#else
    mem_seg_push(mem, value & 0xff);
    mem_seg_push(mem, (value & 0xff00) >> 8);
#endif
}

unsigned short stack_pop(stack_t *stack)
{
    unsigned char val_1 = TO_BYTE(vector_pop(stack->value));
    unsigned char val_2 = TO_BYTE(vector_pop(stack->value));
#if BIG_ENDIAN
    return val_1 << 8 | val_2;
#else
    return val_2 << 8 | val_1;
#endif
}

unsigned int mem_seg_get_low_addr(memory_segment_t *mem)
{
    return mem->base_address << 4;
}

unsigned int mem_seg_get_high_addr(memory_segment_t *mem)
{
    return (mem->base_address << 4) + (unsigned short)vector_len(mem->value);
}

unsigned int stack_get_top(stack_t *stack)
{
    return stack->base_address << 4;
}
unsigned int stack_get_bottom(stack_t *stack)
{
    return (stack->base_address << 4)
           - (unsigned short)vector_len(stack->value);
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

int stack_get_relative_address(stack_t *stack, unsigned int address)
{
    unsigned int high = stack_get_top(stack);
    unsigned int low = stack_get_bottom(stack);
    if (address > high || address < low)
        return -1;

    unsigned short relative_addr = address - low;
    return ((int)0x0) | relative_addr;
}
