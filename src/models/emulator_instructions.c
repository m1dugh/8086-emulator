#include <err.h>
#include "../instructions/instructions.h"
#include "../minix/env.h"
#include "../minix/syscalls.h"
#include "emulator.h"
#include "memory_segment.h"

instruction_t *find_4_len_instruction(
    unsigned char instruction, binary_stream_t *stream)
{
    switch (instruction)
    {
        case 0b1011:
            return mov_immediate_to_reg(stream);
    }
    return NULL;
}

instruction_t *find_5_len_instruction(
    unsigned char instruction, binary_stream_t *stream)
{
    switch (instruction)
    {
        /*case 0b01000:
            return inc_reg(stream);*/
        case 0b01001:
            return dec_reg(stream);
        case 0b01010:
            return push_reg(stream);
        case 0b01011:
            return pop_reg(stream);
            /*case 0b10010:
              return xchg_reg(stream);*/
    }
    return NULL;
}

instruction_t *find_6_len_instruction(
    unsigned char instruction, binary_stream_t *stream)
{
    switch (instruction)
    {
        case 0b000000:
            return add_rm_with_reg(stream);
        case 0b000010:
            return or_rm_reg(stream);
        /*case 0b000100:
            return adc_rm_with_reg(stream);
        case 0b000110:
            return ssb_rm_with_reg(stream);
        case 0b001000:
            return and_rm_reg(stream);
        case 0b001010:
            return sub_rm_with_reg(stream);*/
        case 0b001100:
            return xor_rm_reg(stream);
            /*case 0b001110:
                return cmp_rm_reg(stream);*/
        case 0b100000:
            return cmp_immediate_rm(stream);
        case 0b100010:
            return mov_rm_to_reg(stream);
        case 0b110100:
            return shift_left(stream);
    }
    return NULL;
}

instruction_t *find_7_len_instruction(
    unsigned char instruction, binary_stream_t *stream)
{
    switch (instruction)
    {
        /*case 0b0000010:
            return add_immediate_to_acc(stream);
        case 0b0000110:
            return or_immediate_acc(stream);
        case 0b0010110:
            return sub_immediate_to_acc(stream);
        case 0b0011110:
            return cmp_immediate_acc(stream);
        case 0b1000010:
            return test_rm_reg(stream);
        case 0b1000011:
            return xchg_rm_with_reg(stream);
        case 0b1010000:
            return mov_mem_to_acc(stream);
        case 0b1010100:
            return test_immediate_acc(stream);*/
        case 0b1100011:
            return mov_immediate_to_rm(stream);
        /*case 0b1110010:
            return in_fixed_port(stream);
        case 0b1110110:
            return in_var_port(stream);
        case 0b1111001:
            return rep_string(stream);*/
        case 0b1111011:
            return test_immediate_rm(stream);
        case 0b1111111:
            return inc_rm(stream);
    }
    // return get_string_instruction(instruction, stream);
    return NULL;
}

instruction_t *find_8_len_instruction(
    unsigned char instruction, binary_stream_t *stream)
{
    switch (instruction)
    {
        /*case 0b01110010:
            return jb(stream);
        case 0b01111000:
            return js(stream);*/
        case 0b01110011:
            return jnb(stream);
        case 0b01110100:
            return je(stream);
        case 0b01110101:
            return jne(stream);
        /*case 0b01110110:
            return jbe(stream);
        case 0b01110111:
            return jnbe(stream);*/
        case 0b01111100:
            return jl(stream);
        case 0b01111101:
            return jnl_instruction(stream);
        /*case 0b01111110:
            return jle(stream);
        case 0b01111111:
            return jnle(stream);*/
        case 0b10001101:
            return lea(stream);
        /*case 0b10001111:
            return pop_rm(stream);
        case 0b10011000:
            return cbw();
        case 0b10011001:
            return cwd();
        case 0b11001100:
            return interrupt();*/
        case 0b11001101:
            return interrupt_with_code(stream);
        /*case 0b11100000:
            return loopnz(stream);
        case 0b11100001:
            return loopz(stream);
        case 0b11100010:
            return loop(stream);*/
        case 0b11101000:
            return call_direct_seg(stream);
        case 0b11101001:
            return jmp_direct_seg(stream);
        case 0b11101011:
            return jmp_direct_seg_short(stream);
        case 0b11110100:
            return hlt();
        /*case 0b11001010:
        case 0b11000010:
            return ret_data(stream);*/
        // case 0b11001011:
        case 0b11000011:
            return ret_seg(stream);
            /*case 0b11111100:
                return cld();
            case 0b11111101:
                return std_instruction();
            */
    }
    return NULL;
}

instruction_t *next_instruction(binary_stream_t *stream)
{
    bs_flush_buffer(stream);
    char instruction;
    instruction_t *res;

    if (bs_next_reset(stream, 4, &instruction) != 0)
        return NULL;
    if ((res = find_4_len_instruction(instruction, stream)) != NULL)
        return res;

    if (bs_next(stream, 1, &instruction) != 0)
        return NULL;
    if ((res = find_5_len_instruction(instruction, stream)) != NULL)
        return res;

    if (bs_next(stream, 1, &instruction) != 0)
        return NULL;
    if ((res = find_6_len_instruction(instruction, stream)) != NULL)
        return res;

    if (bs_next(stream, 1, &instruction) != 0)
        return NULL;
    if ((res = find_7_len_instruction(instruction, stream)) != NULL)
        return res;

    if (bs_next(stream, 1, &instruction) != 0)
        return NULL;
    if ((res = find_8_len_instruction(instruction, stream)) != NULL)
        return res;

    return NULL;
}

instruction_t *emulator_load_instruction(emulator_t *emulator)
{
    unsigned short address = emulator->processor->ip;
    instruction_t *instruction = code_seg_get(emulator->code, address);
    if (instruction != NULL)
        return instruction;

    bs_goto(emulator->stream, address, emulator->header.a_hdrlen);

    instruction = next_instruction(emulator->stream);
    if (instruction == NULL)
        return NULL;
    code_seg_set(emulator->code, address, instruction);
    return instruction;
}

void emulator_syscall(emulator_t *emulator)
{
    unsigned short opcode
        = emulator_stack_get(emulator, emulator->processor->bx + 2);

    unsigned short res;
    switch (opcode)
    {
        case SYSCALL_EXIT:
            res = syscall_exit(emulator);
            break;
        case SYSCALL_WRITE:
            res = syscall_write(emulator);
            break;
        default:
            errx(-1, "Syscall not found: %04x", opcode);
    }
    emulator->processor->ax = 0;
}
