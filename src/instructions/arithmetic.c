#include "arithmetic.h"
#include "utils.h"
#include <malloc.h>
#include <stdio.h>

char *add_rm_with_reg(binary_stream_t *data) {
    return format_dw_rm_to_reg("add", data);
}

char *cmp_rm_reg(binary_stream_t *data) {
    return format_sized_dw_rm_to_reg("cmp", data);
}

char *cmp_immediate_rm(binary_stream_t *data) {
    struct params_t params;
    if(extract_dw_mod_reg_rm(data, &params) != 0) {
        return NULL;
    }

    char *rm_value = get_rm(data, params.w, params.mod, params.rm);
    unsigned short extracted = extract_data_sw(data, &params);
    char *instruction;
    int signed_val = 1;
    switch (params.reg) {
        case 0b000:
            instruction = "add";
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
            break;
        case 0b110:
            instruction = "xor";
            signed_val = 0;
            break;
        case 0b111:
            if (params.w) {
                instruction = "cmp";
            } else {
                instruction = "cmp byte";
            }
            break;
        default:
            return NULL;
    }

    char *res = malloc(50);
    if(signed_val && extracted & 0x8000) {
        short val = (extracted ^ 0xffff) + 1;
        snprintf(res, 50, "%s %s, -%x", instruction, rm_value, val);
    } else {
        snprintf(res, 50, "%s %s, %04x", instruction, rm_value, extracted);
    }
    free(rm_value);

    return res;
}

char *inc_rm(binary_stream_t *data) {
    struct params_t params;
    if(extract_w_mod_reg_rm(data, &params) != 0) {
        return NULL;
    }
    char *rm_value = get_rm(data, params.w, params.mod, params.rm);
    char *res = malloc(50);
    switch(params.reg) {
        case 0b000:
            snprintf(res, 50, "inc %s", rm_value);
            break;
        case 0b001:
            snprintf(res, 50, "dec %s", rm_value);
            break;
        case 0b010:
            snprintf(res, 50, "call %s", rm_value);
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
            break;
        default:
            return NULL;
    };

    free(rm_value);
    return res;
}

char *inc_reg(binary_stream_t *data) {
    return format_reg("inc", data);
}

char *dec_reg(binary_stream_t *data) {
    return format_reg("dec", data);
}

char *ssb_rm_with_reg(binary_stream_t *data) {
    return format_dw_rm_to_reg("ssb", data);
}

char *sub_rm_with_reg(binary_stream_t *data) {
    return format_dw_rm_to_reg("sub", data);
}

char *cbw() {
    char *res = malloc(4);
    snprintf(res, 4, "cbw");
    return res;
}

char *cwd() {
    char *res = malloc(4);
    snprintf(res, 4, "cwd");
    return res;
}

char *sub_immediate_to_acc(binary_stream_t *data) {
    return format_immediate_from_acc("sub", data);
}

char *cmp_immediate_acc(binary_stream_t *data) {
    return format_immediate_from_acc("cmp", data);
}
