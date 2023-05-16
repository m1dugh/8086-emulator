#include "binary_stream.h"
#include <malloc.h>

int push_buffer(binary_stream_t *str, char c) {
    if (str->instruction_buffer_len == str->_instruction_buffer_cap) {
        str->_instruction_buffer_cap *= 2;
        str->instruction_buffer = realloc(str->instruction_buffer, str->_instruction_buffer_cap);
        if (str->instruction_buffer == NULL)
            return -1;
    }
    str->instruction_buffer[str->instruction_buffer_len++] = c;
    return 0;
}

char read_bit(binary_stream_t *str, char *res) {
    if(str->_buffer_index > 7) {
        if(str->_index >= str->len)
            return -1;
        str->_buffer = fgetc(str->_stream);
        if (str->_buffer == -1) {
            return -1;
        }
        push_buffer(str, str->_buffer);
        str->_buffer_index = 1;
        *res = (0b10000000 & str->_buffer) >> 7;
    } else {
        char mask = 0b10000000 >> str->_buffer_index;
        *res = (mask & str->_buffer) >> (7 - str->_buffer_index);
        str->_buffer_index++;
    }
    return 0;
}

binary_stream_t *bs_new(FILE *stream, long len) {
    binary_stream_t *res = malloc(sizeof(binary_stream_t));
    if(res == NULL)
        return NULL;
    res->_instruction_buffer_cap = 2;
    res->instruction_buffer = malloc(sizeof(char) * res->_instruction_buffer_cap);
    if(res->instruction_buffer == NULL) {
        free(res);
        return NULL;
    }
    res->_stream = stream;
    res->len = len;
    res->instruction_buffer_len = 0;
    res->_index = 0;
    res->_buffer_index = 8;
    return res;
}

int bs_next(binary_stream_t *stream, int size, char *res) {
    for(int i = 0; i < size; i++) {
        char bit = 0;
        if (read_bit(stream, &bit) == 0) {
            *res = (*res << 1) | bit;
        } else {
            return -1;
        }
    }

    return 0;
}

int bs_next_reset(binary_stream_t *stream, int size, char *res) {
    *res = 0;
    return bs_next(stream, size, res);
}

void bs_free(binary_stream_t *stream) {
    free(stream->instruction_buffer);
    free(stream);
}

void bs_flush_buffer(binary_stream_t *stream) {
    stream->instruction_buffer_len = 0;
}

size_t bs_read_bytes(binary_stream_t *stream) {
    return stream->_index;
}

int bs_finished(binary_stream_t *stream) {
    return stream->_index >= stream->len;
}
