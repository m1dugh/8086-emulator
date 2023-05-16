#include "data_transfer.h"
#include "utils.h"
#include <malloc.h>
#include <stdio.h>

char *mov_rm_to_reg(binary_stream_t *data) {
    struct params_t params;
    if(extract_mod_reg_rm(data, &params) != 0) {
        return NULL;
    }
    char *reg = get_reg(params.w, params.reg);

    char *rm_value = get_rm(data, params.w, params.mod, params.rm);

    char *instruction = malloc(50);
    if (params.d) {
        snprintf(instruction, 50, "mov %s, %s", reg, rm_value);
    } else {
        snprintf(instruction, 50, "mov %s, %s", rm_value, reg);
    }
    free(rm_value);
    return instruction;
}
