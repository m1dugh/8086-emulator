#ifndef UTILS_VECTOR_H
#define UTILS_VECTOR_H

#include <stdlib.h>

#define VECTOR_INITIAL_CAPACITY 1

typedef struct
{
    size_t len;
    size_t cap;
    void **values;
    size_t max_size;
} vector_t;

vector_t *vector_new();
vector_t *vector_new_sized(size_t max_size);
void vector_free(vector_t *);

size_t vector_len(vector_t *);

void *vector_get(vector_t *, size_t);
int vector_set(vector_t *, size_t, void *);

int vector_push(vector_t *, void *);
void *vector_pop(vector_t *);

int vector_insert(vector_t *, size_t, void *);
void *vector_remove(vector_t *, size_t index);

typedef struct
{
    size_t len;
    size_t cap;
    unsigned char *values;
    size_t max_size;
} byte_vector_t;

byte_vector_t *byte_vector_new();
byte_vector_t *byte_vector_new_sized(size_t max_size);
void byte_vector_free(byte_vector_t *);

size_t byte_vector_len(byte_vector_t *);

unsigned char byte_vector_get(byte_vector_t *vector, size_t);
int byte_vector_set(byte_vector_t *vector, size_t, unsigned char value);

int byte_vector_push(byte_vector_t *, unsigned char value);
unsigned char byte_vector_pop(byte_vector_t *vector);

int byte_vector_insert(byte_vector_t *, size_t, unsigned char);
unsigned char byte_vector_remove(byte_vector_t *, size_t index);

#endif /* !UTILS_VECTOR_H */
