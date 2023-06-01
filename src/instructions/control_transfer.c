#include <malloc.h>
#include <stdio.h>
#include "control_transfer.h"
#include "utils.h"

char *jnb(binary_stream_t *data)
{
    return format_byte_displacement("jnb", data);
}
char *js(binary_stream_t *data)
{
    return format_byte_displacement("js", data);
}

char *jb(binary_stream_t *data)
{
    return format_byte_displacement("jb", data);
}

char *jne(binary_stream_t *data)
{
    return format_byte_displacement("jne", data);
}

char *jnl_instruction(binary_stream_t *data)
{
    return format_byte_displacement("jnl", data);
}

char *jnle(binary_stream_t *data)
{
    return format_byte_displacement("jnle", data);
}

char *jnbe(binary_stream_t *data)
{
    return format_byte_displacement("jnbe", data);
}

char *jbe(binary_stream_t *data)
{
    return format_byte_displacement("jbe", data);
}

char *je(binary_stream_t *data)
{
    return format_byte_displacement("je", data);
}

char *jl(binary_stream_t *data)
{
    return format_byte_displacement("jl", data);
}

char *jle(binary_stream_t *data)
{
    return format_byte_displacement("jle", data);
}

char *call_direct_seg(binary_stream_t *data)
{
    return format_word_displacement("call", data);
}

char *jmp_direct_seg(binary_stream_t *data)
{
    return format_word_displacement("jmp", data);
}

char *jmp_direct_seg_short(binary_stream_t *data)
{
    return format_byte_displacement("jmp short", data);
}

char *interrupt()
{
    char *res = malloc(4);
    snprintf(res, 4, "int");
    return res;
}

char *interrupt_with_code(binary_stream_t *data)
{
    unsigned char opcode = extract_byte(data);
    char *res = malloc(10);
    snprintf(res, 10, "int %x", opcode);

    return res;
}

char *ret_data(binary_stream_t *data)
{
    unsigned char low = extract_byte(data);
    unsigned char high = extract_byte(data);
    unsigned short val = (high << 8) + low;

    char *res = malloc(20);
    snprintf(res, 20, "ret %x", val);
    return res;
}
char *ret()
{
    char *res = malloc(4);
    snprintf(res, 4, "ret");
    return res;
}

char *loop(binary_stream_t *data)
{
    return format_byte_displacement("loop", data);
}
char *loopz(binary_stream_t *data)
{
    return format_byte_displacement("loopz", data);
}
char *loopnz(binary_stream_t *data)
{
    return format_byte_displacement("loopnz", data);
}
