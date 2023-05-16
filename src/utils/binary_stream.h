#ifndef UTILS_BS_H
#define UTILS_BS_H

#include <stdio.h>

typedef struct {
    FILE *_stream;  /* The stream to pull the bytes from */
    long len;       /* The max number of bytes to pull from the _stream */
    long _index;    /* The current index in bytes in the stream */
    char _buffer;
    char _buffer_index;
    char *instruction_buffer;
    size_t _instruction_buffer_cap;
    size_t instruction_buffer_len;
} binary_stream_t;

binary_stream_t *bs_new(FILE *stream, long len);
int bs_next(binary_stream_t *, int size, char *res);
int bs_next_reset(binary_stream_t *, int size, char *res);

void bs_free(binary_stream_t *stream);

void bs_flush_buffer(binary_stream_t* stream);

size_t bs_read_bytes(binary_stream_t *stream);

#endif /* UTILS_BS_H */
