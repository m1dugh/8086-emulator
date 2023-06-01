#include <err.h>
#include <malloc.h>
#include <stdio.h>
#include "instructions/arithmetic.h"
#include "instructions/data_transfer.h"
#include "instructions/instructions.h"
#include "instructions/logic.h"
#include "instructions/processor_control.h"
#include "instructions/string_manipulation.h"
#include "utils/binary_stream.h"
#include "utils/format.h"

struct exec_header
{
    unsigned char a_magic[2]; /* magic number */
    unsigned char a_flags;    /* flags */
    unsigned char a_cpu;      /* cpu id */
    unsigned char a_hdrlen;   /* length of header */
    unsigned char a_unused;   /* reserved */
    unsigned short a_version; /* version stamp (not used) */
    long a_text;              /* The length of the text segment */
    long a_data;              /* The length of the data segment */
    long a_bss;               /* The length of the bss segment */
    long a_entry;             /* The entry point */
    long a_total;             /* The total memory allocated */
    long a_syms;              /* size of symbol table */

    /* LONG VERSION */
    long a_trsize; /* text relocation size */
    long a_drsize; /* data relocation size */
    long a_tbase;  /* text relocation base */
    long a_dbase;  /* data relocation base */
};

long to_long(char *buffer)
{
    long res = buffer[3];
    for (int i = 2; i >= 0; i--)
        res = (res << 8) + buffer[i];

    return res;
}

short to_short(char *buffer)
{
    short res = buffer[1];
    res = (res << 1) + buffer[0];
    return res;
}

int read_header(FILE *stream, struct exec_header *header)
{
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
    char *buffer = (char *)malloc(effective_len);
    for (unsigned char i = 0; i < effective_len; i++)
    {
        if ((c = fgetc(stream)) == -1)
        {
            return -1;
        }
        buffer[i] = c;
    }

    size_t current_index = 0;
    header->a_unused = (unsigned char)buffer[current_index++];
    header->a_version = to_short(buffer + current_index);
    current_index += 2;
    header->a_text = to_long(buffer + current_index);
    current_index += 4;
    // TODO: implement full header.

    free(buffer);
    fseek(stream, header->a_hdrlen, SEEK_SET);
    return 0;
}

char *find_4_len_instruction(
    unsigned char instruction, binary_stream_t *stream)
{
    switch (instruction)
    {
        case 0b1011:
            return mov_immediate_to_reg(stream);
    }
    return NULL;
}

char *find_5_len_instruction(
    unsigned char instruction, binary_stream_t *stream)
{
    switch (instruction)
    {
        case 0b01000:
            return inc_reg(stream);
        case 0b01001:
            return dec_reg(stream);
        case 0b01010:
            return push_reg(stream);
        case 0b01011:
            return pop_reg(stream);
        case 0b10010:
            return xchg_reg(stream);
    }
    return NULL;
}

char *find_6_len_instruction(
    unsigned char instruction, binary_stream_t *stream)
{
    switch (instruction)
    {
        case 0b000000:
            return add_rm_with_reg(stream);
        case 0b000010:
            return or_rm_reg(stream);
        case 0b000100:
            return adc_rm_with_reg(stream);
        case 0b000110:
            return ssb_rm_with_reg(stream);
        case 0b001000:
            return and_rm_reg(stream);
        case 0b001010:
            return sub_rm_with_reg(stream);
        case 0b001100:
            return xor_rm_reg(stream);
        case 0b001110:
            return cmp_rm_reg(stream);
        case 0b100000:
            return cmp_immediate_rm(stream);
        case 0b100010:
            return mov_rm_to_reg(stream);
        case 0b110100:
            return shift_left(stream);
    }
    return NULL;
}

char *find_7_len_instruction(
    unsigned char instruction, binary_stream_t *stream)
{
    switch (instruction)
    {
        case 0b0000010:
            return add_immediate_to_acc(stream);
        case 0b0000110:
            return or_immediate_acc(stream);
        case 0b0010110:
            return sub_immediate_to_acc(stream);
        case 0b0011110:
            return cmp_immediate_acc(stream);
        case 0b1000010:
            return test_rm_reg(stream);
        case 0b1000011:
            return xchg_rm_with_reg(stream);
        case 0b1010000:
            return mov_mem_to_acc(stream);
        case 0b1010100:
            return test_immediate_acc(stream);
        case 0b1100011:
            return mov_immediate_to_rm(stream);
        case 0b1110010:
            return in_fixed_port(stream);
        case 0b1110110:
            return in_var_port(stream);
        case 0b1111001:
            return rep_string(stream);
        case 0b1111011:
            return test_immediate_rm(stream);
        case 0b1111111:
            return inc_rm(stream);
    }
    return get_string_instruction(instruction, stream);
}

char *find_8_len_instruction(
    unsigned char instruction, binary_stream_t *stream)
{
    switch (instruction)
    {
        case 0b01110010:
            return jb(stream);
        case 0b01111000:
            return js(stream);
        case 0b01110011:
            return jnb(stream);
        case 0b01110100:
            return je(stream);
        case 0b01110101:
            return jne(stream);
        case 0b01110110:
            return jbe(stream);
        case 0b01110111:
            return jnbe(stream);
        case 0b01111100:
            return jl(stream);
        case 0b01111101:
            return jnl(stream);
        case 0b01111110:
            return jle(stream);
        case 0b01111111:
            return jnle(stream);
        case 0b10001101:
            return lea(stream);
        case 0b10001111:
            return pop_rm(stream);
        case 0b10011000:
            return cbw();
        case 0b10011001:
            return cwd();
        case 0b11001100:
            return interrupt();
        case 0b11001101:
            return interrupt_with_code(stream);
        case 0b11100000:
            return loopnz(stream);
        case 0b11100001:
            return loopz(stream);
        case 0b11100010:
            return loop(stream);
        case 0b11101000:
            return call_direct_seg(stream);
        case 0b11101001:
            return jmp_direct_seg(stream);
        case 0b11101011:
            return jmp_direct_seg_short(stream);
        case 0b11110100:
            return hlt();
        case 0b11001010:
        case 0b11000010:
            return ret_data(stream);
        case 0b11001011:
        case 0b11000011:
            return ret();
        case 0b11111100:
            return cld();
        case 0b11111101:
            return std_instruction();
    }
    return NULL;
}

char *next_instruction(binary_stream_t *stream)
{
    bs_flush_buffer(stream);
    char instruction;
    char *res;

    if (bs_next_reset(stream, 4, &instruction) != 0)
        return NULL;
    if ((res = find_4_len_instruction(instruction, stream)) != NULL)
        return res;

    if (bs_next(stream, 1, &instruction) != 0)
        return NULL;
    if ((res = find_5_len_instruction(instruction, stream)) != NULL)
        return res;

    if (bs_next(stream, 1, &instruction) != 0)
        return NULL;
    if ((res = find_6_len_instruction(instruction, stream)) != NULL)
        return res;

    if (bs_next(stream, 1, &instruction) != 0)
        return NULL;
    if ((res = find_7_len_instruction(instruction, stream)) != NULL)
        return res;

    if (bs_next(stream, 1, &instruction) != 0)
        return NULL;
    if ((res = find_8_len_instruction(instruction, stream)) != NULL)
        return res;

    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        errx(-1, "Usage: %s <file>", argv[0]);
    }

    FILE *f = fopen(argv[1], "rb");
    if (f == NULL)
    {
        errx(-1, "Could not open file %s", argv[0]);
    }
    struct exec_header header;
    if (read_header(f, &header) != 0)
    {
        errx(-1, "Could not read header.");
    }

    printf("text length: %04lx\ntext start address: %04x\ntext end address: "
           "%04lx\n",
        header.a_text, header.a_hdrlen,
        ((long)header.a_hdrlen) + header.a_text);

    binary_stream_t *stream = bs_new(f, header.a_text);

    while (!bs_finished(stream))
    {
        char *res = next_instruction(stream);
        if (printf_instruction(stream->current_address,
                stream->instruction_buffer, stream->instruction_buffer_len,
                res)
                != 0
            && !bs_finished(stream))
        {
            fflush(stdout);
            errx(-1, "Instruction not found");
        }
        free(res);
    }

    printf("===== END OF .TEXT SECTION =====\n");

    bs_free(stream);

    fclose(f);
    return 0;
}
