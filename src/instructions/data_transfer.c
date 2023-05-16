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
