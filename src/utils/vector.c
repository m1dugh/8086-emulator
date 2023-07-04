#include <err.h>
#include <malloc.h>
#include <stdlib.h>
#include "vector.h"

int grow(vector_t *vector)
{
    size_t new_cap = vector->cap * 2;
    vector->values = realloc(vector->values, new_cap * sizeof(void *));
    if (vector->values == NULL)
    {
        return -1;
    }
    vector->cap = new_cap;
    return 0;
}

void grow_if_needed(vector_t *vector)
{
    if (vector->len >= vector->cap)
    {
        int res = grow(vector);
        if (res != 0)
        {
            errx(res, "malloc error: Could not grow vector capacity");
        }
    }
}

vector_t *vector_new()
{
    vector_t *res = malloc(sizeof(vector_t));
    if (res == NULL)
        return NULL;

    res->values = calloc(VECTOR_INITIAL_CAPACITY, sizeof(void *));
    if (res->values == NULL)
    {
        free(res);
        return NULL;
    }
    res->len = 0;
    res->cap = VECTOR_INITIAL_CAPACITY;
    return res;
}

void vector_free(vector_t *vector)
{
    free(vector->values);
    free(vector);
}

int vector_set(vector_t *vector, size_t index, void *data)
{
    if (index >= vector->len)
    {
        return -1;
    }
    vector->values[index] = data;
    return 0;
}

void *vector_get(vector_t *vector, size_t index)
{
    if (index >= vector->len)
    {
        return NULL;
    }
    return vector->values[index];
}

void vector_append(vector_t *vector, void *data)
{
    grow_if_needed(vector);
    vector->values[vector->len] = data;
    vector->len++;
}

void vector_insert(vector_t *vector, size_t index, void *data)
{
    grow_if_needed(vector);
    for (size_t i = vector->len; i > index; i--)
    {
        vector->values[i] = vector->values[i - 1];
    }
    vector->values[index] = data;
    vector->len++;
}

size_t vector_len(vector_t *vector)
{
    return vector->len;
}

void *vector_pop(vector_t *vector)
{
    if (vector->len <= 0)
    {
        return NULL;
    }
    vector->len--;
    return vector->values[vector->len];
}

void *vector_remove(vector_t *vector, size_t index)
{
    if (index >= vector->len)
        return NULL;

    void *res = vector->values[index];

    for (size_t i = index; i < vector->len - 1; i++)
    {
        vector->values[i] = vector->values[i + 1];
    }
    vector->len--;
    return res;
}
