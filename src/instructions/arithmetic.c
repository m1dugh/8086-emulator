#include "arithmetic.h"
#include "utils.h"
#include <malloc.h>
#include <stdio.h>

char *add_rm_with_reg(binary_stream_t *data) {
    return format_dw_rm_to_reg("add", data);
}

char *cmp_rm_reg(binary_stream_t *data) {
    return format_dw_rm_to_reg("cmp", data);
}

char *cmp_immediate_rm(binary_stream_t *data) {
    struct params_t params;
    if(extract_dw_mod_reg_rm(data, &params) != 0) {
        return NULL;
    }

    short val = extract_data_sw(data, &params);
    char *instruction;
    switch (params.reg) {
        case 0b000:
            instruction = "add";
            break;
        case 0b010:
            instruction = "adc";
            break;
        case 0b101:
            instruction = "sub";
            break;
        case 0b011:
            instruction = "ssb";
            break;
        case 0b111:
            instruction = "cmp";
            break;
        default:
            return NULL;
    }

    char *res = malloc(50);
    char *rm_value = get_rm(data, params.w, params.mod, params.rm);
    snprintf(res, 50, "%s %s, %04x", instruction, rm_value, val);

    return res;
}
