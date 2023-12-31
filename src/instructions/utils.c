#include <malloc.h>
#include "utils.h"
#include "stdio.h"

char extract_byte(binary_stream_t *data)
{
    char res;
    bs_next_reset(data, 8, &res);
    return res;
}

int extract_w(binary_stream_t *data, params_t *params)
{
    return bs_next_reset(data, 1, &params->w);
}

int extract_d(binary_stream_t *data, params_t *params)
{
    return bs_next_reset(data, 1, &params->d);
}

int extract_reg(binary_stream_t *data, params_t *params)
{
    return bs_next_reset(data, 3, &params->reg);
}

int extract_mod(binary_stream_t *data, params_t *params)
{
    return bs_next_reset(data, 2, &params->mod);
}

int extract_rm(binary_stream_t *data, params_t *params)
{
    return bs_next_reset(data, 3, &params->rm);
}

int extract_mod_reg_rm(binary_stream_t *data, params_t *params)
{
    if (extract_mod(data, params) != 0)
    {
        return -1;
    }

    if (extract_reg(data, params) != 0)
    {
        return -1;
    }

    if (extract_rm(data, params) != 0)
    {
        return -1;
    }

    return 0;
}

int extract_w_mod_reg_rm(binary_stream_t *data, params_t *params)
{
    if (extract_w(data, params) != 0)
    {
        return -1;
    }

    return extract_mod_reg_rm(data, params);
}

int extract_dw_mod_reg_rm(binary_stream_t *data, params_t *params)
{
    if (extract_d(data, params) != 0)
    {
        return -1;
    }

    return extract_w_mod_reg_rm(data, params);
}

char *get_reg(char w, char val)
{
    return w ? get_word_reg(val) : get_short_reg(val);
}

char *get_word_reg(char val)
{
    switch (val)
    {
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

char *get_short_reg(char val)
{
    switch (val)
    {
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

char *get_rm(binary_stream_t *data, params_t *params)
{

    if (params->mod == 0b11)
    {
        char *reg = get_reg(params->w, params->rm);
        if (reg == NULL)
            return NULL;
        char *res = malloc(3);
        snprintf(res, 3, "%s", reg);
        return res;
    }

    short disp = 0;
    if (params->mod == 0b00 && params->rm != 0b110)
    {
        disp = 0;
    }
    else if (params->mod == 0b01)
    {
        unsigned char val = extract_byte(data);
        if (val & 0x80)
        {
            disp = val | 0xff00;
        }
        else
        {
            disp = val;
        }
    }
    else
    { // params->mod == 0b10
        unsigned char low = extract_byte(data);
        disp = (extract_byte(data) << 8) + low;
    }

    char *effective_address = malloc(20);
    char *format;
    switch (params->rm)
    {
        case 0b000:
            if (disp > 0)
                format = "[bx+si+%x]";
            else if (disp < 0)
                format = "[bx+si-%x]";
            else
                format = "[bx+si]";
            break;
        case 0b001:
            if (disp > 0)
                format = "[bx+di+%x]";
            else if (disp < 0)
                format = "[bx+di-%x]";
            else
                format = "[bx+di]";
            break;
        case 0b010:
            if (disp > 0)
                format = "[bp+si+%x]";
            else if (disp < 0)
                format = "[bp+si-%x]";
            else
                format = "[bp+si]";
            break;
        case 0b011:
            if (disp > 0)
                format = "[bp+di+%x]";
            else if (disp < 0)
                format = "[bp+di-%x]";
            else
                format = "[bp+di]";
            break;
        case 0b100:
            if (disp > 0)
                format = "[si+%x]";
            else if (disp < 0)
                format = "[si-%x]";
            else
                format = "[si]";
            break;
        case 0b101:
            if (disp > 0)
                format = "[di+%x]";
            else if (disp < 0)
                format = "[di-%x]";
            else
                format = "[di]";
            break;
        case 0b110:
            if (params->mod == 0b00)
            {
                format = "[%04x]";
            }
            else
            {
                if (disp > 0)
                    format = "[bp+%x]";
                else if (disp < 0)
                    format = "[bp-%x]";
                else
                    format = "[bp]";
            }
            break;
        case 0b111:
            if (disp > 0)
                format = "[bx+%x]";
            else if (disp < 0)
                format = "[bx-%x]";
            else
                format = "[bx]";
            break;
        default:
            return NULL;
    }
    params->disp = disp;
    if (disp < 0)
    {
        disp = (disp ^ 0xffff) + 1;
    }
    if (disp != 0 || (params->mod == 0b00 && params->rm == 0b110))
        snprintf(effective_address, 20, format, disp);
    else
        snprintf(effective_address, 20, format);

    return effective_address;
}

char *format_dw_rm_to_reg(char *val, binary_stream_t *data, params_t *params)
{
    if (extract_d(data, params) != 0)
        return NULL;
    return format_w_rm_to_reg_d(val, data, params);
}

char *format_sized_dw_rm_to_reg(
    char *val, binary_stream_t *data, params_t *params)
{
    if (extract_dw_mod_reg_rm(data, params) != 0)
    {
        return NULL;
    }
    char *reg = get_reg(params->w, params->reg);

    char *rm_value = get_rm(data, params);

    char *instruction = malloc(50);
    char *format;
    if (params->w)
    {
        format = "%s %s, %s";
    }
    else
    {
        format = "%s byte %s, %s";
    }
    if (params->d)
    {
        snprintf(instruction, 50, format, val, reg, rm_value);
    }
    else
    {
        snprintf(instruction, 50, format, val, rm_value, reg);
    }
    free(rm_value);
    return instruction;
}

char *format_w_rm_to_reg(char *val, binary_stream_t *data, params_t *params)
{
    if (extract_w_mod_reg_rm(data, params) != 0)
    {
        return NULL;
    }
    char *reg = get_reg(params->w, params->reg);

    char *rm_value = get_rm(data, params);

    char *instruction = malloc(50);
    snprintf(instruction, 50, "%s %s, %s", val, reg, rm_value);
    free(rm_value);
    return instruction;
}

char *format_w_rm_to_reg_d(char *val, binary_stream_t *data, params_t *params)
{

    if (extract_w_mod_reg_rm(data, params) != 0)
    {
        return NULL;
    }
    char *reg = get_reg(params->w, params->reg);

    char *rm_value = get_rm(data, params);

    char *instruction = malloc(50);
    if (params->d)
    {
        snprintf(instruction, 50, "%s %s, %s", val, reg, rm_value);
    }
    else
    {
        snprintf(instruction, 50, "%s %s, %s", val, rm_value, reg);
    }
    free(rm_value);
    return instruction;
}

char *format_rm_to_reg(char *val, binary_stream_t *data, params_t *params)
{
    if (extract_mod_reg_rm(data, params) != 0)
    {
        return NULL;
    }

    char *reg = get_reg(0b1, params->reg);

    char *rm_value = get_rm(data, params);

    char *instruction = malloc(50);
    snprintf(instruction, 50, "%s %s, %s", val, reg, rm_value);
    free(rm_value);
    return instruction;
}

short extract_data(binary_stream_t *data, params_t *params)
{
    unsigned char low = extract_byte(data);
    unsigned short val = low;
    if (params->w)
    {
        params->data = (extract_byte(data) << 8) + val;
        return params->data;
    }
    else
    {
        params->data_low = low;
        return (short)low;
    }
}

short extract_data_sw(binary_stream_t *data, params_t *params)
{
    unsigned char low = extract_byte(data);
    unsigned short val;
    if (params->w)
    {
        if (params->d)
        {
            if (low & 0x80)
            {
                val = low | 0xff00;
            }
            else
            {
                val = low;
            }
        }
        else
        {
            unsigned char high = extract_byte(data);
            val = (high << 8) + low;
        }
    }
    else
    {
        val = low;
    }
    return val;
}

char *format_byte_displacement(
    char *val, binary_stream_t *data, params_t *params)
{
    char disp = extract_byte(data);
    params->low_disp = disp;
    unsigned short effective_address
        = data->current_address + data->instruction_buffer_len;
    if (disp < 0)
    {
        disp = (disp ^ 0xff) + 1;
        effective_address -= disp;
    }
    else
    {
        effective_address += disp;
    }
    params->data = effective_address;
    char *res = malloc(50);
    snprintf(res, 50, "%s %04x", val, effective_address);
    return res;
}

char *format_word_displacement(
    char *val, binary_stream_t *data, params_t *params)
{
    unsigned char disp_low = extract_byte(data);

    short disp = (extract_byte(data) << 8) | disp_low;
    params->disp = disp;
    short effective_address
        = data->current_address + data->instruction_buffer_len;

    if (disp < 0)
    {
        disp = (disp ^ 0xffff) + 1;
        effective_address -= disp;
    }
    else
    {
        effective_address += disp;
    }
    params->data = effective_address;

    char *res = malloc(50);
    snprintf(res, 50, "%s %04x", val, effective_address);
    return res;
}

char *format_reg(char *val, binary_stream_t *data, params_t *params)
{
    if (extract_reg(data, params) != 0)
    {
        return NULL;
    }

    char *reg_value = get_reg(0b1, params->reg);
    char *res = malloc(10);
    snprintf(res, 10, "%s %s", val, reg_value);
    return res;
}

char *format_reg_to_acc(char *val, binary_stream_t *data, params_t *params)
{
    if (extract_reg(data, params) != 0)
    {
        return NULL;
    }

    char *reg_value = get_reg(0b1, params->reg);
    char *res = malloc(20);
    snprintf(res, 20, "%s %s, ax", val, reg_value);
    return res;
}

char *format_immediate_from_acc(
    char *val, binary_stream_t *data, params_t *params)
{
    if (extract_w(data, params) != 0)
    {
        return NULL;
    }
    char *res = malloc(50);
    unsigned short extracted = extract_data(data, params);
    char *reg = get_reg(params->w, 0b000);

    snprintf(res, 50, "%s %s, %04x", val, reg, extracted);
    return res;
}

char *format_w_immediate_to_rm(
    char *val, binary_stream_t *data, params_t *params)
{
    if (extract_w_mod_reg_rm(data, params) != 0)
    {
        return NULL;
    }
    char *rm_value = get_rm(data, params);
    unsigned short extracted = extract_data(data, params);

    char *format;
    if (params->w)
    {
        format = "%s %s, %04x";
    }
    else
    {
        format = "%s byte %s, %04x";
    }

    char *res = malloc(50);
    snprintf(res, 50, format, val, rm_value, extracted);
    free(rm_value);
    return res;
}

char *format_w_mem_to_acc(char *val, binary_stream_t *data, params_t *params)
{
    if (extract_w(data, params) != 0)
    {
        return NULL;
    }

    unsigned char low = extract_byte(data);
    unsigned char high = extract_byte(data);
    unsigned short addr = (high << 8) + low;

    char *res = malloc(20);
    snprintf(res, 20, "%s ax, [%04x]", val, addr);
    return res;
}
