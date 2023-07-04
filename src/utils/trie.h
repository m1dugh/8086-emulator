#ifndef UTILS_TRIE_H
#define UTILS_TRIE_H

#include <stdlib.h>
#include "vector.h"

#define ADDRESS_SIZE 2

typedef struct trie_t trie_t;

struct trie_t
{
    short prefix;
    vector_t *children;
    void *data;
};

trie_t *trie_new();
void trie_free(trie_t *);

void *trie_get(trie_t *, unsigned long address);
void trie_set(trie_t *, unsigned long address, void *data);

unsigned long trie_last_address(trie_t *);
unsigned long trie_first_address(trie_t *);

typedef void (*trie_function_t)(
    void *data, unsigned long address, void *additionals);

void trie_for_each(trie_t *, trie_function_t function, void *additionals);

#endif /* !UTILS_TRIE_H */
