#ifndef UTILS_BS_H
#define UTILS_BS_H

#include <stdio.h>

struct stack {
    struct stack *last;
    size_t cursor_pos;
    char byte_position;
};

struct stack *stack_push(struct stack* stack, size_t cursor, char byte_position);
struct stack *stack_pop(struct stack* stack, size_t *cursor, char *byte_position);

typedef struct {
    FILE *_stream;  /* The stream to pull the bytes from */
    long len;       /* The max number of bytes to pull from the _stream */
    char _buffer;
    char _buffer_index;
    char *instruction_buffer;
    size_t _instruction_buffer_cap;
    size_t instruction_buffer_len;
    short current_address;
    struct stack *_stack;
} binary_stream_t;

binary_stream_t *bs_new(FILE *stream, long len);
int bs_next(binary_stream_t *, int size, char *res);
int bs_next_reset(binary_stream_t *, int size, char *res);

void bs_save(binary_stream_t *stream);
int bs_rollback(binary_stream_t *stream);
int bs_unsave(binary_stream_t *stream);
void bs_free(binary_stream_t *stream);

void bs_flush_buffer(binary_stream_t* stream);

size_t bs_read_bytes(binary_stream_t *stream);

int bs_finished(binary_stream_t *stream);

#endif /* UTILS_BS_H */
