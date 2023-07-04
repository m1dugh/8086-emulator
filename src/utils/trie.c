#include <malloc.h>
#include "trie.h"
#include "err.h"
#include "vector.h"

trie_t *trie_new()
{
    trie_t *res = malloc(sizeof(trie_t));
    if (res == NULL)
        return NULL;

    res->prefix = -1;
    res->children = vector_new();
    if (res->children == NULL)
    {
        free(res);
        return NULL;
    }
    res->data = NULL;

    return res;
}

void trie_free(trie_t *tree)
{
    for (size_t i = 0; i < vector_len(tree->children); i++)
    {
        trie_free(vector_get(tree->children, i));
    }
    vector_free(tree->children);
    if (tree->data != NULL)
    {
        free(tree->data);
    }

    free(tree);
}

unsigned char get_prefix(trie_t *tree)
{
    return (unsigned char)tree->prefix & 0xFF;
}

int find_insert_index(trie_t *tree, unsigned char prefix, size_t *index)
{
    size_t start = 0;
    size_t end = vector_len(tree->children);
    size_t middle = 0;
    while (start < end)
    {
        middle = (end - start) / 2;
        trie_t *child = vector_get(tree->children, middle);
        unsigned char child_prefix = get_prefix(child);
        if (child_prefix == prefix)
        {
            *index = middle;
            return 0;
        }
        else if (child_prefix < prefix)
        {
            start = middle + 1;
        }
        else
        {
            end = middle;
        }
    }
    *index = middle;
    return -1;
}

trie_t *find_create_child(trie_t *tree, unsigned char prefix)
{
    size_t index;
    if (find_insert_index(tree, prefix, &index) == 0)
    {
        return vector_get(tree->children, index);
    }
    trie_t *child = trie_new();
    if (child == NULL)
    {
        errx(-1, "malloc error: Could not allocate memory for trie creation");
    }
    child->prefix = (short)(prefix | 0x0000);
    vector_insert(tree->children, index, child);
    return child;
}

trie_t *find_create_path(trie_t *root, unsigned long address)
{
    trie_t *current = root;
    for (size_t i = 0; i < ADDRESS_SIZE; i++)
    {
        unsigned char prefix = address & 0xFF;
        current = find_create_child(current, prefix);
        address >>= 8;
    }
    return current;
}

void *trie_get(trie_t *tree, unsigned long address)
{
    trie_t *child = find_create_path(tree, address);
    return child->data;
}

void trie_set(trie_t *tree, unsigned long address, void *data)
{
    trie_t *child = find_create_path(tree, address);
    child->data = data;
}
