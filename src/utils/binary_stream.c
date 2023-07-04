#include <malloc.h>
#include <stdio.h>
#include "binary_stream.h"

int push_buffer(binary_stream_t *str, char c)
{
    if (str->instruction_buffer_len == str->_instruction_buffer_cap)
    {
        str->_instruction_buffer_cap *= 2;
        str->instruction_buffer
            = realloc(str->instruction_buffer, str->_instruction_buffer_cap);
        if (str->instruction_buffer == NULL)
            return -1;
    }
    str->instruction_buffer[str->instruction_buffer_len++] = c;
    return 0;
}

char read_bit(binary_stream_t *str, char *res)
{
    if (str->_buffer_index > 7)
    {
        if (bs_finished(str))
            return -1;
        str->_buffer = fgetc(str->_stream);
        push_buffer(str, str->_buffer);
        str->_buffer_index = 1;
        *res = (0b10000000 & str->_buffer) >> 7;
    }
    else
    {
        char mask = 0b10000000 >> str->_buffer_index;
        *res = (mask & str->_buffer) >> (7 - str->_buffer_index);
        str->_buffer_index++;
    }
    return 0;
}

binary_stream_t *bs_new(FILE *stream, long len)
{
    binary_stream_t *res = malloc(sizeof(binary_stream_t));
    if (res == NULL)
        return NULL;
    res->_instruction_buffer_cap = 2;
    res->instruction_buffer
        = malloc(sizeof(char) * res->_instruction_buffer_cap);
    if (res->instruction_buffer == NULL)
    {
        free(res);
        return NULL;
    }
    res->_stream = stream;
    res->len = len;
    res->instruction_buffer_len = 0;
    res->current_address = 0;
    res->_buffer_index = 8;
    res->current_address = 0;
    res->_stack = NULL;
    return res;
}

int bs_next(binary_stream_t *stream, int size, char *res)
{
    for (int i = 0; i < size; i++)
    {
        char bit = 0;
        if (read_bit(stream, &bit) == 0)
        {
            *res = (*res << 1) | bit;
        }
        else
        {
            return -1;
        }
    }

    return 0;
}

int bs_next_reset(binary_stream_t *stream, int size, char *res)
{
    *res = 0;
    return bs_next(stream, size, res);
}

void bs_free(binary_stream_t *stream)
{
    free(stream->instruction_buffer);
    free(stream);
}

void bs_flush_buffer(binary_stream_t *stream)
{
    stream->current_address += stream->instruction_buffer_len;
    stream->instruction_buffer_len = 0;
}

size_t bs_read_bytes(binary_stream_t *stream)
{
    return stream->current_address;
}

int bs_finished(binary_stream_t *stream)
{
    return stream->current_address + 1 >= stream->len;
}

struct bs_stack *bs_stack_push(
    struct bs_stack *stack, size_t cursor, char byte_position)
{
    struct bs_stack *res = malloc(sizeof(struct bs_stack));
    if (res == NULL)
        return NULL;
    res->byte_position = byte_position;
    res->cursor_pos = cursor;
    res->last = stack;
    return res;
}
struct bs_stack *bs_stack_pop(
    struct bs_stack *stack, size_t *cursor, char *byte_position)
{
    if (stack == NULL)
        return NULL;

    *cursor = stack->cursor_pos;
    *byte_position = stack->byte_position;

    struct bs_stack *res = stack->last;
    free(stack);
    return res;
}

void bs_save(binary_stream_t *stream)
{
    size_t cursor = ftell(stream->_stream);
    stream->_stack
        = bs_stack_push(stream->_stack, cursor, stream->_buffer_index);
}
int bs_rollback(binary_stream_t *stream)
{
    if (stream->_stack == NULL)
        return -1;

    size_t cursor;
    stream->_stack
        = bs_stack_pop(stream->_stack, &cursor, &stream->_buffer_index);

    fseek(stream->_stream, cursor, SEEK_SET);
    stream->instruction_buffer_len = 0;

    return 0;
}

int bs_unsave(binary_stream_t *stream)
{
    if (stream->_stack == NULL)
        return -1;
    size_t cursor;
    stream->_stack
        = bs_stack_pop(stream->_stack, &cursor, &stream->_buffer_index);

    return 0;
}
