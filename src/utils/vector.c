#include <err.h>
#include <malloc.h>
#include <stdlib.h>
#include "vector.h"

int can_grow(vector_t *vector)
{
    return vector->max_size == 0 || vector->cap < vector->max_size;
}

int grow(vector_t *vector)
{
    if (!can_grow(vector))
        return -1;
    size_t new_cap = vector->cap * 2;
    vector->values = realloc(vector->values, new_cap * sizeof(void *));
    if (vector->values == NULL)
        errx(-1, "malloc error: Could not grow vector capacity");
    vector->cap = new_cap;
    return 0;
}

int grow_if_needed(vector_t *vector)
{
    if (vector->len >= vector->cap)
        return grow(vector);

    return 0;
}

vector_t *vector_new_sized(size_t max_size)
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
    res->max_size = max_size;
    return res;
}

vector_t *vector_new()
{
    return vector_new_sized(0);
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

int vector_push(vector_t *vector, void *data)
{
    int res;
    if ((res = grow_if_needed(vector)) != 0)
        return res;

    vector->values[vector->len] = data;
    vector->len++;
    return 0;
}

int vector_insert(vector_t *vector, size_t index, void *data)
{
    int res;
    if ((res = grow_if_needed(vector)) != 0)
        return res;

    for (size_t i = vector->len; i > index; i--)
        vector->values[i] = vector->values[i - 1];
    vector->values[index] = data;
    vector->len++;
    return 0;
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
