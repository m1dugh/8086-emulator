#include "utils.h"
#include "stdio.h"
#include <malloc.h>

int extract_w(binary_stream_t *data, struct params_t *params) {
    return bs_next_reset(data, 1, &params->w);
}

int extract_d(binary_stream_t *data, struct params_t *params) {
    return bs_next_reset(data, 1, &params->d);
}

int extract_reg(binary_stream_t *data, struct params_t *params) {
    return bs_next_reset(data, 3, &params->reg);
}

int extract_mod(binary_stream_t *data, struct params_t *params) {
    return bs_next_reset(data, 2, &params->mod); 
}

int extract_rm(binary_stream_t *data, struct params_t *params) {
    return bs_next_reset(data, 3, &params->rm);
}

int extract_mod_reg_rm(binary_stream_t *data, struct params_t *params) {
    if(extract_mod(data, params) != 0) {
        return -1;
    }

    if(extract_reg(data, params) != 0) {
        return -1;
    }

    if(extract_rm(data, params) != 0) {
        return -1;
    }

    return 0;
}

int extract_w_mod_reg_rm(binary_stream_t *data, struct params_t *params) {
    if(extract_w(data, params) != 0) {
        return -1;
    }

    return extract_mod_reg_rm(data, params);
}

int extract_dw_mod_reg_rm(binary_stream_t *data, struct params_t *params) {
    if(extract_d(data, params) != 0) {
        return -1;
    }

    return extract_w_mod_reg_rm(data, params);
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
        snprintf(res, 3, "%s", reg);
        return res;
    }

    short disp;
    if(mod == 0b00 && rm != 0b110) {
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
    char *format;
    switch(rm) {
        case 0b000:
            if (disp > 0)
                format = "[bx+si+%x]";
            else if (disp < 0)
                format = "[bx+si%x]";
            else
                format = "[bx+si]";
            break;
        case 0b001:
            if (disp > 0)
                format = "[bx+di+%x]";
            else if (disp < 0)
                format = "[bx+di%x]";
            else
                format = "[bx+di]";
            break;
        case 0b010:
            if (disp > 0)
                format = "[bp+si+%x]";
            else if (disp < 0)
                format = "[bp+si%x]";
            else
                format = "[bp+si]";
            break;
        case 0b011:
            if (disp > 0)
                format = "[bp+di+%x]";
            else if (disp < 0)
                format = "[bp+di%x]";
            else
                format = "[bp+di]";
            break;
        case 0b100:
            if (disp > 0)
                format = "[si+%x]";
            else if (disp < 0)
                format = "[si%x]";
            else
                format = "[si]";
            break;
        case 0b101:
            if (disp > 0)
                format = "[di+%x]";
            else if (disp < 0)
                format = "[di%x]";
            else
                format = "[di]";
            break;
        case 0b110:
            if(mod == 0b00) {
                format = "[%04x]";
            } else {
                if (disp > 0)
                    format = "[bp+%x]";
                else if (disp < 0)
                    format = "[bp%x]";
                else
                    format = "[bp]";
            }
            break;
        case 0b111:
            if (disp > 0)
                format = "[bx+%x]";
            else if (disp < 0)
                format = "[bx%x]";
            else
                format = "[bx]";
            break;
        default:
            return NULL;
    }
    if (disp != 0)
        snprintf(effective_address, 20, format, disp);
    else
        snprintf(effective_address, 20, format);

    return effective_address;
}

char *format_dw_rm_to_reg(char *val, binary_stream_t *data) {
    struct params_t params;
    if(extract_dw_mod_reg_rm(data, &params) != 0) {
        return NULL;
    }
    char *reg = get_reg(params.w, params.reg);

    char *rm_value = get_rm(data, params.w, params.mod, params.rm);

    char *instruction = malloc(50);
    if (params.d) {
        snprintf(instruction, 50, "%s %s, %s", val, reg, rm_value);
    } else {
        snprintf(instruction, 50, "%s %s, %s", val, rm_value, reg);
    }
    free(rm_value);
    return instruction;
}

char *format_w_rm_to_reg(char *val, binary_stream_t *data) {
    struct params_t params;
    if(extract_w_mod_reg_rm(data, &params) != 0) {
        return NULL;
    }
    char *reg = get_reg(params.w, params.reg);

    char *rm_value = get_rm(data, params.w, params.mod, params.rm);

    char *instruction = malloc(50);
    snprintf(instruction, 50, "%s %s, %s", val, reg, rm_value);
    return instruction;
}

char *format_rm_to_reg(char *val, binary_stream_t *data) {
    struct params_t params;
    if(extract_mod_reg_rm(data, &params) != 0) {
        return NULL;
    }

    char *reg = get_reg(0b1, params.reg);

    char *rm_value = get_rm(data, 0b1, params.mod, params.rm);

    char *instruction = malloc(50);
    snprintf(instruction, 50, "%s %s, %s", val, reg, rm_value);

    free(rm_value);
    return instruction;
}

char extract_byte(binary_stream_t *data) {
    char res;
    bs_next_reset(data, 8, &res);
    return res;
}

short extract_data(binary_stream_t *data, struct params_t *params) {
    short val = extract_byte(data);
    if(params->w) {
        val = (extract_byte(data) << 8) + val;
    }
    return val;
}

short extract_data_sw(binary_stream_t *data, struct params_t *params) {
    short val = extract_byte(data);
    if(params->w) {
        if (params->d) {
            if (val & 0x80) {
                val |= 0xff00;
            }
        } else {
            val = (extract_byte(data) << 8) + val;
        }
    }
    return val;
}

char *format_byte_displacement(char *val, binary_stream_t *data) {
    char disp = extract_byte(data);
    short effective_address = disp + data->current_address + data->instruction_buffer_len;
    char *res = malloc(50);
    snprintf(res, 50, "%s %04x", val, effective_address);
    return res;
}

char *format_word_displacement(char *val, binary_stream_t *data) {
    short disp = extract_byte(data);
    disp += extract_byte(data) << 8;
    
    short effective_address = disp + data->current_address + data->instruction_buffer_len;
    char *res = malloc(50);
    snprintf(res, 50, "%s %04x", val, effective_address);
    return res;
}
