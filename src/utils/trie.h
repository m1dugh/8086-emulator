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

#endif /* !UTILS_TRIE_H */
