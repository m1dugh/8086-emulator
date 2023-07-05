#include <malloc.h>
#include <stdio.h>
#include "data_transfer.h"
#include "implementation/data_transfer.h"
#include "implementation/lea.h"
#include "implementation/mov.h"
#include "utils.h"

instruction_t *mov_rm_to_reg(binary_stream_t *data)
{
    params_t params;
    char *display = format_dw_rm_to_reg("mov", data, &params);
    RET_INSTRUCTION(display, data, params, mov_rm_reg_exec);
}

char *mov_immediate_to_rm(binary_stream_t *data)
{
    params_t params;
    return format_w_immediate_to_rm("mov", data, &params);
}

char *mov_immediate_to_reg(binary_stream_t *data)
{
    params_t params;
    if (extract_w(data, &params) != 0)
        return NULL;
    if (extract_reg(data, &params) != 0)
        return NULL;
    short val = extract_data(data, &params);
    char *reg_value = get_reg(params.w, params.reg);
    char *res = malloc(50);

    snprintf(res, 50, "mov %s, %04x", reg_value, val & 0xffff);
    return res;
}

char *mov_mem_to_acc(binary_stream_t *data)
{
    params_t params;
    return format_w_mem_to_acc("mov", data, &params);
}

instruction_t *lea(binary_stream_t *data)
{
    params_t params;
    char *display = format_rm_to_reg("lea", data, &params);
    RET_INSTRUCTION(display, data, params, lea_exec);
}

char *push_reg(binary_stream_t *data)
{
    params_t params;
    return format_reg("push", data, &params);
}

char *pop_rm(binary_stream_t *data)
{
    params_t params;
    if (extract_mod_reg_rm(data, &params) != 0)
    {
        return NULL;
    }
    if (params.reg != 0b000)
        return NULL;

    char *rm_value = get_rm(data, &params);
    char *res = malloc(50);
    snprintf(res, 50, "pop %s", rm_value);
    free(rm_value);
    return res;
}

char *pop_reg(binary_stream_t *data)
{
    params_t params;
    return format_reg("pop", data, &params);
}

char *in_out_port(char *val, binary_stream_t *data, int fixed)
{
    params_t params;
    if (extract_w(data, &params) != 0)
    {
        return NULL;
    }

    char *reg = get_reg(params.w, 0b000);
    char *res = malloc(16);
    if (!fixed)
    {
        snprintf(res, 16, "%s %s, %s", val, reg, get_reg(1, 0b010));
    }
    else
    {
        unsigned char port = extract_byte(data) & 0xff;
        snprintf(res, 16, "%s %s, %x", val, reg, port);
    }
    return res;
}

char *in_fixed_port(binary_stream_t *data)
{
    return in_out_port("in", data, 1);
}

char *in_var_port(binary_stream_t *data)
{
    return in_out_port("in", data, 0);
}

char *out_fixed_port(binary_stream_t *data)
{
    return in_out_port("out", data, 1);
}

char *out_var_port(binary_stream_t *data)
{
    return in_out_port("out", data, 0);
}

char *xchg_rm_with_reg(binary_stream_t *data)
{
    return format_w_rm_to_reg_d("xchg", data, 0);
}
char *xchg_reg(binary_stream_t *data)
{
    params_t params;
    return format_reg_to_acc("xchg", data, &params);
}
