#include <malloc.h>
#include <stdio.h>
#include "arithmetic.h"
#include "implementation/add.h"
#include "implementation/call.h"
#include "implementation/cmp.h"
#include "implementation/dec.h"
#include "implementation/push.h"
#include "implementation/sub.h"
#include "utils.h"

instruction_t *add_rm_with_reg(binary_stream_t *data)
{
    params_t params;
    char *display = format_dw_rm_to_reg("add", data, &params);
    RET_INSTRUCTION(display, data, params, add_rm_reg_exec);
}

char *add_immediate_to_acc(binary_stream_t *data)
{
    params_t params;
    return format_immediate_from_acc("add", data, &params);
}

char *cmp_rm_reg(binary_stream_t *data)
{
    params_t params;
    return format_sized_dw_rm_to_reg("cmp", data, &params);
}

instruction_t *cmp_immediate_rm(binary_stream_t *data)
{
    params_t params;
    if (extract_dw_mod_reg_rm(data, &params) != 0)
    {
        return NULL;
    }

    char *rm_value = get_rm(data, &params);
    unsigned short extracted = extract_data_sw(data, &params);
    params.data = extracted;
    char *instruction;
    int signed_val = 1;
    instruction_cb_t cb = NULL;
    switch (params.reg)
    {
        case 0b000:
            instruction = "add";
            cb = add_immediate_rm_exec;
            break;
        case 0b001:
            instruction = "or";
            signed_val = 0;
            break;
        case 0b010:
            instruction = "adc";
            break;
        case 0b011:
            instruction = "ssb";
            break;
        case 0b100:
            instruction = "and";
            signed_val = 0;
            break;
        case 0b101:
            instruction = "sub";
            cb = sub_immediate_rm_exec;
            break;
        case 0b110:
            instruction = "xor";
            signed_val = 0;
            break;
        case 0b111:
            if (params.w)
            {
                instruction = "cmp";
            }
            else
            {
                instruction = "cmp byte";
            }
            cb = cmp_immediate_rm_exec;
            break;
        default:
            return NULL;
    }

    char *res = malloc(50);
    if (signed_val && extracted & 0x8000)
    {
        short val = (extracted ^ 0xffff) + 1;
        snprintf(res, 50, "%s %s, -%x", instruction, rm_value, val);
    }
    else
    {
        snprintf(res, 50, "%s %s, %04x", instruction, rm_value, extracted);
    }
    free(rm_value);
    RET_INSTRUCTION(res, data, params, cb);
}

instruction_t *inc_rm(binary_stream_t *data)
{
    params_t params;
    if (extract_w_mod_reg_rm(data, &params) != 0)
    {
        return NULL;
    }
    char *rm_value = get_rm(data, &params);
    char *res = malloc(50);
    instruction_cb_t cb = NULL;
    switch (params.reg)
    {
        case 0b000:
            snprintf(res, 50, "inc %s", rm_value);
            cb = inc_rm_exec;
            break;
        case 0b001:
            snprintf(res, 50, "dec %s", rm_value);
            break;
        case 0b010:
            snprintf(res, 50, "call %s", rm_value);
            cb = call_indirect_seg_exec;
            break;
        case 0b011:
            // TODO: indirect intersegment
            snprintf(res, 50, "call %s", rm_value);
            break;
        case 0b100:
            snprintf(res, 50, "jmp %s", rm_value);
            break;
        case 0b101:
            // TODO: indirect intersegment
            snprintf(res, 50, "jmp %s", rm_value);
            break;
        case 0b110:
            snprintf(res, 50, "push %s", rm_value);
            cb = push_rm_exec;
            break;
        default:
            return NULL;
    };

    free(rm_value);

    RET_INSTRUCTION(res, data, params, cb);
}

char *inc_reg(binary_stream_t *data)
{
    params_t params;
    return format_reg("inc", data, &params);
}

instruction_t *dec_reg(binary_stream_t *data)
{
    params_t params;
    char *display = format_reg("dec", data, &params);
    RET_INSTRUCTION(display, data, params, dec_reg_exec);
}

char *ssb_rm_with_reg(binary_stream_t *data)
{
    params_t params;
    return format_dw_rm_to_reg("ssb", data, &params);
}

char *sub_rm_with_reg(binary_stream_t *data)
{
    params_t params;
    return format_dw_rm_to_reg("sub", data, &params);
}

char *cbw()
{
    char *res = malloc(4);
    snprintf(res, 4, "cbw");
    return res;
}

char *cwd()
{
    char *res = malloc(4);
    snprintf(res, 4, "cwd");
    return res;
}

char *sub_immediate_to_acc(binary_stream_t *data)
{
    params_t params;
    return format_immediate_from_acc("sub", data, &params);
}

char *cmp_immediate_acc(binary_stream_t *data)
{
    params_t params;
    return format_immediate_from_acc("cmp", data, &params);
}

char *adc_rm_with_reg(binary_stream_t *data)
{
    params_t params;
    return format_dw_rm_to_reg("adc", data, &params);
}
