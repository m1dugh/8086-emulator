#include "data_transfer.h"
#include "utils.h"
#include <malloc.h>
#include <stdio.h>

char *mov_rm_to_reg(binary_stream_t *data) {
    return format_dw_rm_to_reg("mov", data);
}

char *mov_immediate_to_rm(binary_stream_t *data) {
    struct params_t params;
    if(extract_w_mod_reg_rm(data, &params) != 0) {
        return NULL;
    }
    short val = extract_data(data, &params);

    char *rm_value = get_rm(data, params.mod, params.w, params.rm);

    char *res = malloc(50);
    snprintf(res, 50, "mov %s, %04x", rm_value, val);
    return res;
}

char *mov_immediate_to_reg(binary_stream_t *data) {
    struct params_t params;
    if(extract_w(data, &params) != 0)
        return NULL;
    if(extract_reg(data, &params) != 0)
        return NULL;
    short val = extract_data(data, &params);

    char *reg_value = get_reg(params.w, params.reg);

    char *res = malloc(50);
    snprintf(res, 50, "mov %s, %04x", reg_value, val);
    return res;
}

char *lea(binary_stream_t *data) {
    return format_rm_to_reg("lea", data);
}

char *push_rm(binary_stream_t *data) {
    struct params_t params;
    if(extract_mod_reg_rm(data, &params) != 0) {
        return NULL;
    }
    char *instruction;
    switch(params.reg) {
        case 0b010:
            instruction = "call";
            break;
        case 0b011:
            // TODO: indirect intersegment
            instruction = "call";
            break;
        case 0b100:
            instruction = "jmp";
            break;
        case 0b101:
            // TODO: indirect intersegment
            instruction = "jmp";
            break;
        case 0b110:
            instruction = "push";
            break;
        default:
            return NULL;
    };

    char *rm_value = get_rm(data, params.w, params.mod, params.rm);
    char *res = malloc(50);
    snprintf(res, 50, "%s %s", instruction, rm_value);
    free(rm_value);
    return res;
}

char *push_reg(binary_stream_t *data) {
    struct params_t params;
    if(extract_reg(data, &params) != 0) {
        return NULL;
    }

    char *reg_value = get_reg(0b1, params.reg);
    char *res = malloc(10);
    snprintf(res, 10, "push %s", reg_value);
    return res;
}
