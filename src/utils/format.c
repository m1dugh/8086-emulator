#include <stdio.h>
#include "format.h"

#define LEFT_PAD 14

void print_byte(char c)
{
    char mask = 0x80;
    for (int i = 0; i < 8; i++, mask >>= 1)
    {
        if (mask & c)
        {
            printf("1");
        }
        else
        {
            printf("0");
        }
    }
}
void print_hex(char c)
{
    printf("%x", (c & 0xf0) >> 4);
    printf("%x", (c & 0xf));
}

int printf_instruction(unsigned short address, unsigned char *buffer,
    size_t buffer_len, char *instruction, char *additionals)
{
    size_t offset = 0;
    printf("%04x:", address);
    for (size_t i = 0; i < buffer_len; i++)
    {
        print_hex(buffer[i]);
        offset += 2;
    }
    if (instruction == NULL)
    {
        return -1;
    }
    for (; offset < LEFT_PAD; offset++)
    {
        printf(" ");
    }
    printf("%s", instruction);
    if (additionals != NULL)
        printf(" %s", additionals);
    printf("\n");

    return 0;
}
