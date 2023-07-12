#include <err.h>
#include <malloc.h>
#include <stdlib.h>
#include "vector.h"

int byte_vector_can_grow(byte_vector_t *vector)
{
    return vector->max_size == 0 || vector->cap < vector->max_size;
}

int byte_vector_grow(byte_vector_t *vector)
{
    if (!byte_vector_can_grow(vector))
        return -1;
    size_t new_cap = vector->cap * 2;
    vector->values = realloc(vector->values, new_cap * sizeof(unsigned char));
    if (vector->values == NULL)
        errx(-1, "malloc error: Could not byte_vector_grow vector capacity");
    vector->cap = new_cap;
    return 0;
}

int byte_vector_grow_if_needed(byte_vector_t *vector)
{
    if (vector->len >= vector->cap)
        return byte_vector_grow(vector);

    return 0;
}

byte_vector_t *byte_vector_new_sized(size_t max_size)
{
    byte_vector_t *res = malloc(sizeof(byte_vector_t));
    if (res == NULL)
        return NULL;

    res->values = calloc(VECTOR_INITIAL_CAPACITY, sizeof(unsigned char));
    if (res->values == NULL)
    {
        free(res);
        return NULL;
    }
    res->len = 0;
    res->cap = VECTOR_INITIAL_CAPACITY;
    res->max_size = max_size;
    return res;
}

byte_vector_t *byte_vector_new()
{
    return byte_vector_new_sized(0);
}

void byte_vector_free(byte_vector_t *vector)
{
    free(vector->values);
    free(vector);
}

int byte_vector_set(byte_vector_t *vector, size_t index, unsigned char data)
{
    if (index >= vector->len)
    {
        return -1;
    }
    vector->values[index] = data;
    return 0;
}

unsigned char byte_vector_get(byte_vector_t *vector, size_t index)
{
    if (index >= vector->len)
    {
        return 0;
    }
    return vector->values[index];
}

int byte_vector_push(byte_vector_t *vector, unsigned char data)
{
    int res;
    if ((res = byte_vector_grow_if_needed(vector)) != 0)
        return res;

    vector->values[vector->len] = data;
    vector->len++;
    return 0;
}

int byte_vector_insert(byte_vector_t *vector, size_t index, unsigned char data)
{
    int res;
    if ((res = byte_vector_grow_if_needed(vector)) != 0)
        return res;

    for (size_t i = vector->len; i > index; i--)
        vector->values[i] = vector->values[i - 1];
    vector->values[index] = data;
    vector->len++;
    return 0;
}

size_t byte_vector_len(byte_vector_t *vector)
{
    return vector->len;
}

unsigned char byte_vector_pop(byte_vector_t *vector)
{
    if (vector->len <= 0)
    {
        return 0;
    }
    vector->len--;
    return vector->values[vector->len];
}

unsigned char byte_vector_remove(byte_vector_t *vector, size_t index)
{
    if (index >= vector->len)
        return 0;

    unsigned char res = vector->values[index];

    for (size_t i = index; i < vector->len - 1; i++)
    {
        vector->values[i] = vector->values[i + 1];
    }
    vector->len--;
    return res;
}
