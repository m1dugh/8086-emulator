#include <err.h>
#include <stdio.h>
#include "memory_segment.h"
#include "../env.h"

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

unsigned short mem_seg_get_word(memory_segment_t *mem, unsigned short address)
{
    unsigned short next_address;
    if (mem->direction == DIRECTION_UP)
    {
        next_address = address + 1;
    }
    else
    {
        next_address = address - 1;
    }
    unsigned char first_val = mem_seg_get(mem, address);
    unsigned char second_val = mem_seg_get(mem, next_address);
#if BIG_ENDIAN
    unsigned short res = (first_val << 8) | second_val;
#else
    unsigned short res = (second_val << 8) | first_val;
#endif
    return res;
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

unsigned short mem_seg_get_abs_word(
    memory_segment_t *mem, unsigned short address)
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

    return mem_seg_get_word(mem, effective_address);
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

void mem_seg_set_abs_word(
    memory_segment_t *mem, unsigned short address, unsigned short value)
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
    mem_seg_set_word(mem, effective_address, value);
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
