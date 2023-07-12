#include <malloc.h>
#include <stdio.h>
#include "control_transfer.h"
#include "implementation/call.h"
#include "implementation/interrupts.h"
#include "implementation/jump.h"
#include "utils.h"

instruction_t *jnb(binary_stream_t *data)
{
    params_t params;
    char *display = format_byte_displacement("jnb", data, &params);
    RET_INSTRUCTION(display, data, params, jnb_exec);
}

char *js(binary_stream_t *data)
{
    params_t params;
    return format_byte_displacement("js", data, &params);
}

char *jb(binary_stream_t *data)
{
    params_t params;
    return format_byte_displacement("jb", data, &params);
}

instruction_t *jne(binary_stream_t *data)
{
    params_t params;
    char *display = format_byte_displacement("jne", data, &params);
    RET_INSTRUCTION(display, data, params, jne_exec);
}

instruction_t *jnl_instruction(binary_stream_t *data)
{
    params_t params;
    char *display = format_byte_displacement("jnl", data, &params);
    RET_INSTRUCTION(display, data, params, jnl_exec);
}

char *jnle(binary_stream_t *data)
{
    params_t params;
    return format_byte_displacement("jnle", data, &params);
}

char *jnbe(binary_stream_t *data)
{
    params_t params;
    return format_byte_displacement("jnbe", data, &params);
}

char *jbe(binary_stream_t *data)
{
    params_t params;
    return format_byte_displacement("jbe", data, &params);
}

instruction_t *je(binary_stream_t *data)
{
    params_t params;
    char *display = format_byte_displacement("je", data, &params);
    RET_INSTRUCTION(display, data, params, je_exec);
}

instruction_t *jl(binary_stream_t *data)
{
    params_t params;
    char *display = format_byte_displacement("jl", data, &params);
    RET_INSTRUCTION(display, data, params, jl_exec);
}

char *jle(binary_stream_t *data)
{
    params_t params;
    return format_byte_displacement("jle", data, &params);
}

instruction_t *call_direct_seg(binary_stream_t *data)
{
    params_t params;
    char *display = format_word_displacement("call", data, &params);
    RET_INSTRUCTION(display, data, params, call_direct_seg_exec);
}

instruction_t *jmp_direct_seg(binary_stream_t *data)
{
    params_t params;
    char *display = format_word_displacement("jmp", data, &params);
    RET_INSTRUCTION(display, data, params, jmp_exec);
}

instruction_t *jmp_direct_seg_short(binary_stream_t *data)
{
    params_t params;
    char *display = format_byte_displacement("jmp short", data, &params);
    RET_INSTRUCTION(display, data, params, jmp_exec);
}

char *interrupt()
{
    char *res = malloc(4);
    snprintf(res, 4, "int");
    return res;
}

instruction_t *interrupt_with_code(binary_stream_t *data)
{
    unsigned char opcode = extract_byte(data);
    params_t params = {.data_low = opcode};
    char *res = malloc(10);
    snprintf(res, 10, "int %x", opcode);

    RET_INSTRUCTION(res, data, params, interrupt_with_code_exec);
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

instruction_t *ret_seg(binary_stream_t *data)
{
    char *res = malloc(4);
    snprintf(res, 4, "ret");
    params_t params = {.data = 0};
    RET_INSTRUCTION(res, data, params, ret_seg_exec);
}

char *loop(binary_stream_t *data)
{
    params_t params;
    return format_byte_displacement("loop", data, &params);
}
char *loopz(binary_stream_t *data)
{
    params_t params;
    return format_byte_displacement("loopz", data, &params);
}
char *loopnz(binary_stream_t *data)
{
    params_t params;
    return format_byte_displacement("loopnz", data, &params);
}
