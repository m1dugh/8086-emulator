#include "utils.h"
#include "stdio.h"
#include <malloc.h>

int extract_mod_reg_rm(binary_stream_t *data, struct params_t *params) {
    struct params_t res;
    if(bs_next_reset(data, 1, &res.d) != 0) {
        return -1;
    }

    if(bs_next_reset(data, 1, &res.w) != 0) {
        return -1;
    }

    if(bs_next_reset(data, 2, &res.mod) != 0) {
        return -1;
    }

    if(bs_next_reset(data, 3, &res.reg) != 0) {
        return -1;
    }

    if(bs_next_reset(data, 3, &res.rm) != 0) {
        return -1;
    }

    *params = res;
    return 0;
}


char *get_reg(char w, char val) {
    return w ? get_word_reg(val) : get_short_reg(val);
}

char *get_word_reg(char val) {
    switch (val) {
        case 0b000:
            return "ax";
        case 0b001:
            return "cx";
        case 0b010:
            return "dx";
        case 0b011:
            return "bx";
        case 0b100:
            return "sp";
        case 0b101:
            return "bp";
        case 0b110:
            return "si";
        case 0b111:
            return "di";
        default:
            return NULL;
    };
}

char *get_short_reg(char val) {
    switch (val) {
        case 0b000:
            return "al";
        case 0b001:
            return "cl";
        case 0b010:
            return "dl";
        case 0b011:
            return "bl";
        case 0b100:
            return "ah";
        case 0b101:
            return "ch";
        case 0b110:
            return "dh";
        case 0b111:
            return "bh";
        default:
            return NULL;
    };
}

char *get_rm(binary_stream_t *data, char w, char mod, char rm) {
    if(mod == 0b11) {
        char *reg = get_reg(w, rm);
        if (reg == NULL)
            return NULL;
        char *res = malloc(3);
        snprintf(res, 2, "%s", reg);
        return res;
    }

    short disp;
    if(mod == 0b00) {
        disp = 0;
    }
    else if(mod == 0b01) {
        char val;
        GET_DATA(val, data, 8);
        disp = val;
        if(val & 0x8) {
            disp |= 0xff00;
        }
    } else { // mod == 0b10
        char val;
        GET_DATA(val, data, 8);
        disp = val;
        GET_DATA(val, data, 8);
        disp |= (val << 8);
    }

    char *effective_address = malloc(20);
    switch(rm) {
        case 0b000:
            snprintf(effective_address, 20, disp > 0 ? "[bx+si+%d]" : "[bx+si%d]", disp);
            break;
        case 0b001:
            snprintf(effective_address, 20, disp > 0 ? "[bx+di+%d]" : "[bx+di%d]", disp);
            break;
        case 0b010:
            snprintf(effective_address, 20, disp > 0 ? "[bp+si+%d]" : "[bp+si%d]", disp);
            break;
        case 0b011:
            snprintf(effective_address, 20, disp > 0 ? "[bp+di+%d]" : "[bp+di%d]", disp);
            break;
        case 0b100:
            snprintf(effective_address, 20, disp > 0 ? "[si+%d]" : "[si%d]", disp);
            break;
        case 0b101:
            snprintf(effective_address, 20, disp > 0 ? "[di+%d]" : "[di%d]", disp);
            break;
        case 0b110:
            snprintf(effective_address, 20, disp > 0 ? "[bp+%d]" : "[bp%d]", disp);
            break;
        case 0b111:
            snprintf(effective_address, 20, disp > 0 ? "[bx+%d]" : "[bx%d]", disp);
            break;
        default:
            return NULL;
    }

    return effective_address;
}
