#include <malloc.h>
#include <string.h>
#include "logic.h"
#include "implementation/logic.h"
#include "implementation/test.h"
#include "utils.h"

instruction_t *xor_rm_reg(binary_stream_t *data)
{
    params_t params;
    if (extract_dw_mod_reg_rm(data, &params) != 0)
    {
        return NULL;
    }
    char *reg = get_reg(params.w, params.reg);

    char *rm_value = get_rm(data, &params);

    char *instruction = malloc(50);
    if (params.d)
    {
        snprintf(instruction, 50, "xor %s, %s", reg, rm_value);
    }
    else
    {
        snprintf(instruction, 50, "xor %s, %s", rm_value, reg);
    }
    free(rm_value);
    RET_INSTRUCTION(instruction, data, params, xor_rm_reg_exec);
}

char *test_rm_reg(binary_stream_t *data)
{
    params_t params;
    return format_w_rm_to_reg("test", data, &params);
}

instruction_t *test_immediate_rm(binary_stream_t *data)
{
    params_t params;
    if (extract_w_mod_reg_rm(data, &params) != 0)
    {
        return NULL;
    }

    char *res = malloc(50);
    char *rm_value = get_rm(data, &params);
    instruction_cb_t cb = NULL;
    switch (params.reg)
    {
        case 0b000:
            if (params.w || params.mod == 0b11)
                snprintf(res, 50, "test %s, %04x", rm_value,
                    extract_data(data, &params));
            else
                snprintf(res, 50, "test byte %s, %04x", rm_value,
                    extract_data(data, &params));
            cb = test_immediate_rm_exec;
            break;
        case 0b010:
            snprintf(res, 50, "not %s", rm_value);
            break;
        case 0b011:
            snprintf(res, 50, "neg %s", rm_value);
            break;
        case 0b100:
            snprintf(res, 50, "mul %s", rm_value);
            break;
        case 0b101:
            snprintf(res, 50, "imul %s", rm_value);
            break;
        case 0b110:
            snprintf(res, 50, "div %s", rm_value);
            break;
        case 0b111:
            snprintf(res, 50, "idiv %s", rm_value);
            break;
        default:
            return NULL;
    }

    free(rm_value);
    RET_INSTRUCTION(res, data, params, cb);
}

instruction_t *shift_left(binary_stream_t *data)
{
    params_t params;
    if (extract_dw_mod_reg_rm(data, &params) != 0)
    {
        return NULL;
    }

    char *instruction;
    char *rm_value = get_rm(data, &params);
    instruction_cb_t cb = NULL;
    switch (params.reg)
    {
        case 0b000:
            instruction = "rol";
            break;
        case 0b001:
            instruction = "ror";
            break;
        case 0b010:
            instruction = "rcl";
            break;
        case 0b011:
            instruction = "rcr";
            break;
        case 0b100:
            instruction = "shl";
            cb = shl_exec;
            break;
        case 0b101:
            instruction = "shr";
            break;
        case 0b111:
            instruction = "sar";
            break;
        default:
            return NULL;
    }
    char *res = malloc(50);
    if (params.d == 0)
    {
        snprintf(res, 50, "%s %s, 1", instruction, rm_value);
    }
    else
    {
        snprintf(res, 50, "%s %s, cl", instruction, rm_value);
    }
    free(rm_value);
    RET_INSTRUCTION(res, data, params, cb);
}

char *and_immediate_acc(binary_stream_t *data)
{
    params_t params;
    return format_immediate_from_acc("and", data, &params);
}

char *and_rm_reg(binary_stream_t *data)
{
    params_t params;
    return format_dw_rm_to_reg("and", data, &params);
}

char *or_immediate_acc(binary_stream_t *data)
{
    params_t params;
    return format_immediate_from_acc("or", data, &params);
}

char *or_rm_reg(binary_stream_t *data)
{
    params_t params;
    return format_dw_rm_to_reg("or", data, &params);
}

char *test_immediate_acc(binary_stream_t *data)
{
    params_t params;
    return format_immediate_from_acc("test", data, &params);
}
